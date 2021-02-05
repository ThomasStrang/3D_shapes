#include <iostream>
#include "sdl_window.h"
#include "tessellation.h"
#include <math.h>
#include "geometry.h"
#include "camera.h"

struct Pixel {
	int x, y;
	Pixel(int x, int y) : x(x), y(y) {}
};

struct RasterisationTriangle {
	double z;//for now just have 1 z for each triangle
	int lo_x, lo_y, hi_x, hi_y;
	Matrix4x1 surface_normal;
	double dot_camera;
	double dot_skylight;
	PointIndex points[3];
	RasterisationTriangle(Triangle t, Matrix4x1* points_camera_space, Matrix3x1* points_screen_space, Matrix4x1 skylight, int w, int h) {
		auto a2d = points_screen_space[t[0]];
		auto b2d = points_screen_space[t[1]];
		auto c2d = points_screen_space[t[2]];
		auto a3d = points_camera_space[t[0]];
		auto b3d = points_camera_space[t[1]];
		auto c3d = points_camera_space[t[2]];

		z = (a3d.z + b3d.z + c3d.z) / 3; //for now just have 1 z for each triangle
		lo_x = std::min({ a2d.x,b2d.x,c2d.x });
		lo_y = std::min({ a2d.y,b2d.y,c2d.y });
		hi_x = std::max({ a2d.x+1,b2d.x+1,c2d.x+1 });
		hi_y = std::max({ a2d.y+1,b2d.y+1,c2d.y+1 });
		//intersect the bounds with the screen
		lo_x = std::max(lo_x, 0);
		hi_x = std::min(hi_x, w - 1);
		lo_y = std::max(lo_y, 0);
		hi_y = std::min(hi_y, h - 1);
		surface_normal = (b3d - a3d).cross_as_3d(c3d - a3d).normalise();
		dot_camera = surface_normal.dot_product_as_3d(a3d);
		dot_skylight = surface_normal.dot_product_as_3d(skylight);
		points[0] = t.ps[0];
		points[1] = t.ps[1];
		points[2] = t.ps[2];
	}

	PointIndex operator[](U32 i) const {
		return points[i];
	}
};

class Renderer {

	Window& window;
	Camera* cam;
	Matrix4x1 skylight = Matrix4x1(1, 1.2, 0, 1).normalise();
	Tessellation* tess;

public:

	Renderer(Window& w, Camera* c, Tessellation* t) : window(w), cam(c), tess(t) {}

	int render() {
		//work out size of window at this point in time
		if (!window.is_initialised()) return -1;
		int w = window.get_w();
		int h = window.get_h();
		double screen_ratio = (double)w / (double)h;
		int num_pixels = w * h;
		std::vector<Point> orig_points = tess->allPoints();
		std::vector<Triangle> orig_tris = tess->allTriangles();
		int num_points = orig_points.size();
		int num_tris = orig_tris.size();

		//make array of 3d points mapped to camera space
		Matrix4x4 cam_transform = cam->get_transform();
		Matrix4x1* points_camera_space = (Matrix4x1*)malloc(num_points * sizeof(Matrix4x1));
		for (int i = 0; i < num_points; i++)
			points_camera_space[i] = map_point_to_camera_space(orig_points[i], cam_transform, screen_ratio);

		//make array of 2d points in screen space
		Matrix3x1* points_screen_space = (Matrix3x1*)malloc(num_points * sizeof(Matrix3x1));
		for (int i = 0; i < num_points; i++)
			points_screen_space[i] = map_camera_space_to_screen_space(points_camera_space[i], w * 0.5, h * 0.5);

		//make array of triangles with z values and bounds etc
		RasterisationTriangle* triangles_for_rasterisation = (RasterisationTriangle*)malloc(num_tris * sizeof(RasterisationTriangle));
		for (int i = 0; i < num_tris; i++) {
			Triangle t = orig_tris[i];
			if (points_camera_space[t[0]].z > 0 && points_camera_space[t[1]].z > 0 &&points_camera_space[t[2]].z > 0)
				triangles_for_rasterisation[i] = RasterisationTriangle(t, points_camera_space, points_screen_space, skylight, w, h);
		}
		//perform rasterisation into an intermediat buffer
		double* z_buffer = (double*)malloc(num_pixels * sizeof(double));
		std::fill_n(z_buffer, num_pixels, 0);
		Uint32* pixel_buffer = (Uint32*)malloc(num_pixels * sizeof(Uint32));
		for (int i = 0; i < num_tris; i++)
			rasterise_triangle_to_pixel_buffer(triangles_for_rasterisation[i],points_screen_space,w,h,z_buffer,pixel_buffer);

		//write pixels to window
		Uint32 default_colour = (255 << 24) + (60 << 16) + (60 << 8) + 200;
		for (int i = 0; i < num_pixels; i++) {
			if (z_buffer[i] != 0) {
				window.write_pixel(i, pixel_buffer[i]);
			}
			else {
				window.write_pixel(i, default_colour);
			}
		}

		free(points_camera_space);
		free(points_screen_space);
		free(triangles_for_rasterisation);
		free(z_buffer);
		free(pixel_buffer);

	}

private:

	Matrix4x1 map_point_to_camera_space(Point p, Matrix4x4 cam_transform, double screen_ratio) {
		Matrix4x1 v = cam_transform * Matrix4x1(p.x, p.y, p.z, 1);
		v.y *= -screen_ratio;
		return v;

	}

	Matrix3x1 map_camera_space_to_screen_space(Matrix4x1 v, double w_div_2, double h_div_2) {
		double inv_z = 1 / v.z;
		auto s = Matrix3x1(v.x * inv_z, v.y * inv_z, 1);
		s.x = (s.x + 1) * w_div_2;
		s.y = (s.y + 1) * h_div_2;
		return s;
	}

	void rasterise_triangle_to_pixel_buffer(RasterisationTriangle t, Matrix3x1* points_screen_space, int w, int h, double* z_buffer, Uint32* pixel_buffer) {
		if (t.dot_camera < 0)  return; // don't render triangles facing away? maybe remove this later.
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
};