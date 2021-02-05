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


class Renderer {

	Window& window;
	Tessellation* tess;
	Matrix4x4 cube_rotation = identityMatrix4x4();
	Camera* cam;
	Matrix4x1 skylight = Matrix4x1(1, 1.2, 0, 1).normalise();
	bool initialised;
	double screenRatio;
	double w_div_2;
	double h_div_2;
	int w;
	int h;

public:

	Renderer(Window&  w, Camera* c, Tessellation* t) : window(w), cam(c), tess(t),  initialised(false) {}

	bool is_initialised() {
		return initialised;
	}

	int init() {
		if (!window.is_initialised()) return -1;
		w = window.get_w();
		h = window.get_h();
		w_div_2 = w >> 1;
		h_div_2 = h >> 1;
		screenRatio = w_div_2 / h_div_2;
		initialised = 1;
		return 0;
	}

	int render() {
		if (!initialised) return -1;
		for (int x = 0; x < w; x++) {
			for (int y = 0; y < h; y++) {
				window.write_pixel(x, y, x * ((float)255 / w), 0, y * ((float)255 / h));
			}
		}
		auto points = tess->allPoints();
		double* z_buffer = new double[((int)w) * ((int)h)] { 100000.0 };
		for (Triangle t : tess->allTriangles()) {
			renderTriangle(points[t[0]], points[t[1]], points[t[2]], cam->get_transform(), z_buffer);
		}
		delete[] z_buffer;
		return 0;
	}

private:
	
	void renderTriangle(Point a, Point b, Point c, Matrix4x4 cam_transform, double* z_buffer) {
		Matrix4x1 v1 =   (cam_transform * (cube_rotation * Matrix4x1(a.x, a.y, a.z, 1)));
		Matrix4x1 v2 =  (cam_transform * (cube_rotation * Matrix4x1(b.x, b.y, b.z, 1)));
		Matrix4x1 v3 =  (cam_transform * (cube_rotation * Matrix4x1(c.x, c.y, c.z, 1)));
		v1.y *= -screenRatio;
		v2.y *= -screenRatio;
		v3.y *= -screenRatio;
		if (v1.z > 0 && v2.z > 0 && v3.z > 0) {//TODO update this so that if a point is off the screen we clip the triangle into the bits which are on screen.
			rasterise(v1,v2,v3,z_buffer);
		}
	}

	Matrix3x1 map_to_plane(Matrix4x1 v) {
		double inv_z = 1/v.z;
		auto s = Matrix3x1(v.x*inv_z,v.y*inv_z,1);
		s.x = (s.x + 1) * w_div_2;
		s.y = (s.y + 1) * h_div_2;
		return s;
	}

	void rasterise(Matrix4x1 v1, Matrix4x1 v2, Matrix4x1 v3, double* z_buffer) {
		auto surface_normal = (v2 - v1).cross_as_3d(v3 - v1).normalise();
		double dot_with_camera = surface_normal.dot_product_as_3d(v1);//if any are > 0 then all are.
		if (dot_with_camera < 0)  return; // don't render triangles facing away? maybe remove this later.
		double dot_with_skylight = surface_normal.dot_product_as_3d(skylight);//currently light is always in same orientation as the camera I geuss..
		int light_level = (dot_with_skylight + 1.0) * 127.0;
		auto s1 = map_to_plane(v1);
		auto s2 = map_to_plane(v2);
		auto s3 = map_to_plane(v3);
		int lo_x = std::max({ std::min({ (double)w - 1, s1.x,s2.x,s3.x }), 0.0 });
		int lo_y = std::max({ std::min({ (double)h - 1, s1.y,s2.y,s3.y }), 0.0 });
		int hi_x = std::min({ std::max({ 0.0, s1.x+1,s2.x+1,s3.x+1 }), (double)w - 1 });
		int hi_y = std::min({ std::max({ 0.0, s1.y+1,s2.y+1,s3.y+1 }), (double)h - 1 });
		auto s12 = s2 - s1;
		auto s23 = s3 - s2;
		auto s31 = s1 - s3;
		double z = (v1.z + v2.z + v3.z) / 3;
		for (int x = lo_x; x < hi_x; x++) {
			for (int y = lo_y; y < hi_y; y++) {
				if (z > z_buffer[x + (w * y)] && z_buffer[x + (w * y)] != 0)  continue;
				Matrix3x1 xy = Matrix3x1(x+0.5, y+0.5, 1);//add 0.5 to get the "middle" of the pixel, just for my own visualisation in my head.
				if ((xy - s1).cross_as_2d(s12) <= 0 && (xy - s2).cross_as_2d(s23) <= 0 && (xy - s3).cross_as_2d(s31) <= 0) {
					window.write_pixel(x, y, light_level, light_level, light_level);
					z_buffer[x + (w * y)] = z;
				}
			}
		}
	}
};