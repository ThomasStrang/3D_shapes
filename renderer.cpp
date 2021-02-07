#include <iostream>
#include <math.h>
#include <chrono>
#include "sdl_window.h"
#include "tessellation.h"
#include "geometry.h"
#include <thread>
#include "camera.h"

#define NUM_THREADS 16

class Renderer {

	struct RasterisationTriangle {
		bool renderable;
		double z;//for now just have 1 z for each triangle
		int lo_x, lo_y, hi_x, hi_y;
		Uint32 pixel_colour;
		Matrix3x1 ab, bc, ca;

		RasterisationTriangle(double z, int lo_x, int hi_x, int lo_y, int hi_y, double pixel_colour, Matrix3x1 ab, Matrix3x1 bc, Matrix3x1 ca) :
			renderable(true), z(z), lo_x(lo_x), hi_x(hi_x), lo_y(lo_y), hi_y(hi_y), pixel_colour(pixel_colour), ab(ab) ,bc(bc), ca(ca) {}

		RasterisationTriangle() : renderable(false) {}//default value for a triangle which we won't bother rendering.

		bool point_within(Matrix3x1 xy) {
			return xy.homogenous_dot_product(ab) >= 0 && xy.homogenous_dot_product(bc) >= 0 && xy.homogenous_dot_product(ca) >= 0;
		}
	};

	Window& window;
	Uint32* pixel_buffer;//technically owned by the window
	Camera* cam;
	Tessellation* tess;

	//variables which are owned by this and can change
	Matrix4x4 cam_transform;
	Matrix4x1 skylight = Matrix4x1(1, 1, 0, 0).normalise();
	Matrix4x1* points_camera_space;
	Matrix3x1* points_screen_space;
	RasterisationTriangle* triangles_for_rasterisation;
	int w, h, num_pixels, num_points, num_triangles;
	double w_div_2, h_div_2, screen_ratio;
	double* z_buffer;
	bool print_debug_info = true;
	

public:

	Renderer(Window& w, Camera* c, Tessellation* t) : 
		window(w), 
		cam(c), 
		tess(t), 
		num_points(0), 
		num_triangles(0), 
		w(0),
		h(0),
		w_div_2(0),
		h_div_2(0),
		screen_ratio(0),
		num_pixels(0),
		points_camera_space(nullptr),
		points_screen_space(nullptr),
		triangles_for_rasterisation(nullptr),
		z_buffer(nullptr),
		pixel_buffer(nullptr) {}

	~Renderer() {
		free(points_camera_space);
		free(points_screen_space);
		free(triangles_for_rasterisation);
		free(z_buffer);
	}

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
		clear_pixel_buffer();
		auto time5 = get_time();
		rasterise_triangles_to_pixel_buffer();
		auto time6 = get_time();
		std::cout << "3d transformation: " << time2 - time1 << "\t perspective projection: " << time3 - time2 << "\t triangle computation: " << time4 - time3 << "\t buffer clear: " << time5 - time4 << "\t rasterisation: " << time6 - time5 << "\n";
		std::cout << "frame time: " << time6 - time1 << "\n";
		return 0;
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
			free(z_buffer);
			z_buffer = (double*)malloc(num_pixels * sizeof(double));
		}
		pixel_buffer = window.get_pixels();
		cam_transform = cam->get_transform();
	}

	Matrix4x1 map_point_to_camera_space(Point p) {
		Matrix4x1 v = cam_transform * Matrix4x1(p.x, p.y, p.z, 1);
		v.y *= -screen_ratio;
		return v;
	}

	void map_points_to_camera_space() {
		for (int i = 0; i < num_points; i++)
			points_camera_space[i] = map_point_to_camera_space(tess->allPoints()[i]);
	}

	Matrix3x1 map_camera_space_to_screen_space(Matrix4x1 v) {
		double inv_z = 1 / v.z;
		auto s = Matrix3x1(v.x * inv_z, v.y * inv_z, 1);
		s.x = (s.x + 1) * w_div_2;
		s.y = (s.y + 1) * h_div_2;
		return s;
	}

	void map_points_to_screen_space() {
		for (int i = 0; i < num_points; i++)
			points_screen_space[i] = map_camera_space_to_screen_space(points_camera_space[i]);
	}

	Uint32 map_grey_to_uint32(int gr) {
		return (255 << 24) + (gr << 16) + (gr << 8) + gr;

	}

	Uint32 map_rgb_to_uint(int r, int g, int b) {
		return (255 << 24) + (r << 16) + (g << 8) + b;
	}
	
	//const Uint32 debug_colours[4] = { map_rgb_to_uint(255,128,0),map_rgb_to_uint(0,255,128),map_rgb_to_uint(128,0,255),map_rgb_to_uint(0,128,255) };

	RasterisationTriangle map_triangle_to_rasterisation_triangle(Triangle t) {
		if (points_camera_space[t[0]].z <= 0 || points_camera_space[t[1]].z <= 0 || points_camera_space[t[2]].z <= 0) return RasterisationTriangle();
		auto a3d = points_camera_space[t[0]];
		auto b3d = points_camera_space[t[1]];
		auto c3d = points_camera_space[t[2]];
		auto z = (a3d.z + b3d.z + c3d.z) / 3; //for now just have 1 z for each triangle
		auto surface_normal = (b3d - a3d).cross_as_3d(c3d - a3d).normalise_as_3d();
		auto dot_camera = surface_normal.dot_product_as_3d(a3d);
		if (dot_camera < 0) return RasterisationTriangle();
		auto a2d = points_screen_space[t[0]];
		auto b2d = points_screen_space[t[1]];
		auto c2d = points_screen_space[t[2]];
		auto lo_x = std::max((int)std::min({ a2d.x,b2d.x,c2d.x }), 0);
		auto lo_y = std::max((int)std::min({ a2d.y,b2d.y,c2d.y }), 0);
		auto hi_x = std::min((int)std::max({ a2d.x + 1,b2d.x + 1,c2d.x + 1 }), w - 1);
		auto hi_y = std::min((int)std::max({ a2d.y + 1,b2d.y + 1,c2d.y + 1 }), h - 1);
		auto dot_skylight = surface_normal.dot_product_as_3d(skylight);//work out why transforming skylight by camera transform doesn't work
		auto light_level = map_grey_to_uint32((dot_skylight + 1.0) * 127.0);
		return RasterisationTriangle(z, lo_x, hi_x, lo_y, hi_y, light_level, a2d.homogenous_cross_product(b2d), b2d.homogenous_cross_product(c2d), c2d.homogenous_cross_product(a2d));
	}

	void map_triangles_to_rasterisation_triangles() {
		for (int i = 0; i < num_triangles; i++)
			triangles_for_rasterisation[i] = map_triangle_to_rasterisation_triangle(tess->allTriangles()[i]);
	}

	void clear_pixel_buffer() {
		std::fill_n(pixel_buffer, num_pixels, map_rgb_to_uint(60,60,200));
	}

	void rasterise_triangle_to_portion_of_pixel_buffer(RasterisationTriangle t, int offset, int num_threads) {
		auto pixel_colour = t.pixel_colour;
		//temp for seeing different colours of threads
		//pixel_colour = debug_colours[offset];
		int t_offset = t.lo_x % num_threads;
		int offset_diff = offset - t_offset;
		int lo_x = t.lo_x + offset_diff;
		if (offset_diff < 0) lo_x += num_threads;
		for (int x = lo_x; x < t.hi_x; x+=num_threads) {
			for (int y = t.lo_y; y < t.hi_y; y++) {
				int pixel_num = x + (w * y);
				if (t.z < z_buffer[pixel_num]) {
					if (t.point_within(Matrix3x1(x + 0.5, y + 0.5, 1))) {
						pixel_buffer[pixel_num] = pixel_colour;
						z_buffer[pixel_num] = t.z;
					}
				}
			}
		}
	}

	void rasterise_triangles_to_portion_of_pixel_buffer(int offset, int num_threads) {
		for (int i = 0; i < num_triangles; i++)
			if (triangles_for_rasterisation[i].renderable)
				rasterise_triangle_to_portion_of_pixel_buffer(triangles_for_rasterisation[i], offset, num_threads);
	}
	

	void rasterise_triangles_to_pixel_buffer() {
		std::fill_n(z_buffer, num_pixels, (double) 99999.0);
		std::thread threads[NUM_THREADS];
		for (int i = 0; i < NUM_THREADS; i++) {
			threads[i] = std::thread([this,i] {this->rasterise_triangles_to_portion_of_pixel_buffer(i, NUM_THREADS); });
		}
		for (int i = 0; i < NUM_THREADS; i++)
			threads[i].join();
	}
};