#pragma once
#include "geometry.h"
class Camera {

	Matrix4x4 camera_translation = identityMatrix4x4();//translates world coordinates root at camera
	Matrix4x4 camera_rotation = identityMatrix4x4();
	Matrix4x4 camera_rotation_inv = identityMatrix4x4();

public:
	void move_camera_absolute(Matrix4x1 v);
	void move_camera_relative_to_rotation(Matrix4x1 v);
	void rotate_camera_roll(double angle);
	void rotate_camera_pitch(double angle);
	void rotate_camera_yaw(double angle);
	Matrix4x4 get_translation();
	Matrix4x4 get_rotation();
};