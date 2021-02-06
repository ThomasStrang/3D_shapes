#include <iostream>
#include <math.h>
#include <chrono>
#include "sdl_window.h"
#include "tessellation.h"
#include "geometry.h"
#include "camera.h"

struct RasterisationTriangle {
	double z;//for now just have 1 z for each triangle
	int lo_x, lo_y, hi_x, hi_y;
	Matrix4x1 surface_normal;
	double dot_camera;
	double dot_skylight;
	PointIndex points[3];
	RasterisationTriangle(double z, int lo_x, int hi_x, int lo_y, int hi_y, Matrix4x1 surface_normal, double dot_camera, double dot_skylight, PointIndex a, PointIndex b, PointIndex c) :
		z(z), lo_x(lo_x), hi_x(hi_x), lo_y(lo_y), hi_y(hi_y), surface_normal(surface_normal), dot_camera(dot_camera), dot_skylight(dot_skylight) {
		points[0] = a;
		points[1] = b;
		points[2] = c;
	}

	PointIndex operator[](U32 i) const {
		return points[i];
	}
};

class Renderer {

	Window& window;
	Camera* cam;
	Tessellation* tess;

	Matrix4x4 cam_transform;
	Matrix4x1 skylight = Matrix4x1(1, 1.2, 0, 1).normalise();
	Matrix4x1* points_camera_space;
	Matrix3x1* points_screen_space;
	RasterisationTriangle* triangles_for_rasterisation;
	int w, h, num_pixels, num_points, num_triangles;
	double w_div_2, h_div_2, screen_ratio;
	Uint32* pixel_buffer;
	double* z_buffer;
	bool print_debug_info = true;
	

public:

	Renderer(Window& w, Camera* c, Tessellation* t) : window(w), cam(c), tess(t), num_points(0), num_triangles(0), w(0), h(0) {}

	int render() {
		if (!window.is_initialised()) return -1;
		update_member_variables();
		auto time1 = get_time();
		map_points_to_camera_space();
		auto time2 = get_time();
		map_points_to_screen_space();
		auto time3 = get_time();
		map_triangles_to_rasterisation_triangles();
		auto time4 = get_time();
		rasterise_triangles_to_pixel_buffer();
		auto time5 = get_time();
		copy_pixel_buffer_to_window();
		auto time6 = get_time();
		std::cout << "3d transformation: " << time2 - time1 << "\t perspective projection: " << time3 - time2 << "\t triangle computation: " << time4 - time3 << "\t rasterisation: " << time5 - time4 << "\t buffer swap: " << time6 - time5 << "\n";
		std::cout << "frame time: " << time6 - time1 << "\n";
	}

private:

	long long get_time() {
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}

	void update_member_variables() {
		int new_num_triangles = tess->allTriangles().size();
		if (num_points != tess->allPoints().size()) {
			num_points = tess->allPoints().size();
			free(points_camera_space);
			free(points_screen_space);
			points_camera_space = (Matrix4x1*)malloc(num_points * sizeof(Matrix4x1));
			points_screen_space = (Matrix3x1*)malloc(num_points * sizeof(Matrix3x1));
		}
		if (num_triangles != tess->allTriangles().size()) {
			num_triangles = tess->allTriangles().size();
			free(triangles_for_rasterisation);
			triangles_for_rasterisation = (RasterisationTriangle*)malloc(num_triangles * sizeof(RasterisationTriangle));
		}
		if (w != window.get_w() || h != window.get_h()) {
			w = window.get_w();
			h = window.get_h();
			w_div_2 = w * 0.5;
			h_div_2 = h * 0.5;
			screen_ratio = (double)w / (double)h;
			num_pixels = w * h;
			free(pixel_buffer);
			free(z_buffer);
			pixel_buffer = (Uint32*)malloc(num_pixels * sizeof(Uint32));
			z_buffer = (double*)malloc(num_pixels * sizeof(double));
		}
		cam_transform = cam->get_transform();
	}

	Matrix4x1 map_point_to_camera_space(Point p) {
		Matrix4x1 v = cam_transform * Matrix4x1(p.x, p.y, p.z, 1);
		v.y *= -screen_ratio;
		return v;
	}

	Matrix3x1 map_camera_space_to_screen_space(Matrix4x1 v) {
		double inv_z = 1 / v.z;
		auto s = Matrix3x1(v.x * inv_z, v.y * inv_z, 1);
		s.x = (s.x + 1) * w_div_2;
		s.y = (s.y + 1) * h_div_2;
		return s;
	}

	RasterisationTriangle map_triangle_to_rasterisation_triangle(Triangle t) {
		auto a2d = points_screen_space[t[0]];
		auto b2d = points_screen_space[t[1]];
		auto c2d = points_screen_space[t[2]];
		auto a3d = points_camera_space[t[0]];
		auto b3d = points_camera_space[t[1]];
		auto c3d = points_camera_space[t[2]];
		auto z = (a3d.z + b3d.z + c3d.z) / 3; //for now just have 1 z for each triangle
		auto lo_x = std::max((int)std::min({ a2d.x,b2d.x,c2d.x }),0);
		auto lo_y = std::max((int)std::min({ a2d.y,b2d.y,c2d.y }),0);
		auto hi_x = std::min((int)std::max({ a2d.x + 1,b2d.x + 1,c2d.x + 1 }),w-1);
		auto hi_y = std::min((int)std::max({ a2d.y + 1,b2d.y + 1,c2d.y + 1 }),h-1);
		auto surface_normal = (b3d - a3d).cross_as_3d(c3d - a3d).normalise();
		auto dot_camera = surface_normal.dot_product_as_3d(a3d);
		auto dot_skylight = surface_normal.dot_product_as_3d(skylight);
		return RasterisationTriangle(z,lo_x,hi_x,lo_y,hi_y,surface_normal,dot_camera,dot_skylight,t.ps[0],t.ps[1],t.ps[2]);
	}

	void rasterise_triangle_to_pixel_buffer(RasterisationTriangle t) {
		if (t.dot_camera < 0)  return; // don't render triangles facing away? maybe remove this later.
		if (points_camera_space[t[0]].z <= 0 || points_camera_space[t[1]].z <= 0 || points_camera_space[t[2]].z <= 0) return; // don't render triangles behind the screen. TODO change this to render only the renderable bit.
		Uint32 light_level = (t.dot_skylight + 1.0) * 127.0;
		Uint32 pixel_colour = (255 << 24) + (light_level << 16) + (light_level << 8) + light_level;
		auto a = points_screen_space[t[0]];
		auto b = points_screen_space[t[1]];
		auto c = points_screen_space[t[2]];
		auto ab = b - a;
		auto bc = c - b;
		auto ca = a - c;
		for (int x = t.lo_x; x < t.hi_x; x++) {
			for (int y = t.lo_y; y < t.hi_y; y++) {
				int pixel_num = x + (w * y);
				if (t.z > z_buffer[pixel_num] && z_buffer[pixel_num] != 0)  continue;
				Matrix3x1 xy = Matrix3x1(x + 0.5, y + 0.5, 1);//add 0.5 to get the "middle" of the pixel, just for my own visualisation in my head.
				if ((xy - a).cross_as_2d(ab) <= 0 && (xy - b).cross_as_2d(bc) <= 0 && (xy - c).cross_as_2d(ca) <= 0) {
					pixel_buffer[pixel_num] = pixel_colour;
					z_buffer[pixel_num] = t.z;
				}
			}
		}
	}

	void map_points_to_camera_space() {
		for (int i = 0; i < num_points; i++)
			points_camera_space[i] = map_point_to_camera_space(tess->allPoints()[i]);
	}

	void map_points_to_screen_space() {
		for (int i = 0; i < num_points; i++)
			points_screen_space[i] = map_camera_space_to_screen_space(points_camera_space[i]);
	}

	void map_triangles_to_rasterisation_triangles() {
		for (int i = 0; i < num_triangles; i++)
			triangles_for_rasterisation[i] = map_triangle_to_rasterisation_triangle(tess->allTriangles()[i]);
	}

	void rasterise_triangles_to_pixel_buffer() {
		std::fill_n(z_buffer, num_pixels, 0);
		for (int i = 0; i < num_triangles; i++)
			rasterise_triangle_to_pixel_buffer(triangles_for_rasterisation[i]);
	}

	void copy_pixel_buffer_to_window() {
		Uint32 default_colour = (255 << 24) + (60 << 16) + (60 << 8) + 200;
		for (int i = 0; i < num_pixels; i++) {
			if (z_buffer[i] != 0)
				window.write_pixel(i, pixel_buffer[i]);
			else
				window.write_pixel(i, default_colour);
		}
	}
};