#include "geometry.h"
#include "camera.h"



void Camera::move_camera_absolute(Matrix4x1 v) {
	camera_translation.m = camera_translation.m - v;
}

void Camera::move_camera_relative_to_rotation(Matrix4x1 v) {
	move_camera_absolute(camera_rotation_inv * v);
}

//rotate on relative "z" axis (i.e. aircraft leaning left)
void Camera::rotate_camera_roll(double angle) {
	camera_rotation = (rotationRoll(angle) * camera_rotation);
	camera_rotation_inv = (camera_rotation_inv * rotationRoll(-angle));
}

//rotate on relative "x" axis (i.e. aircraft leaning backwards)
void Camera::rotate_camera_pitch(double angle) {
	camera_rotation = (rotationPitch(angle) * camera_rotation);
	camera_rotation_inv = (camera_rotation_inv * rotationPitch(-angle));
}
//rotate on relative "y" axis (i.e. aircraft turning left)
void Camera::rotate_camera_yaw(double angle) {
	camera_rotation = (rotationYaw(angle) * camera_rotation);
	camera_rotation_inv = (camera_rotation_inv * rotationYaw(-angle));
}

Matrix4x4 Camera::get_transform() {
	return camera_rotation * camera_translation;
}

Matrix4x4 Camera::get_translation() {
	return camera_translation;
}

Matrix4x4 Camera::get_rotation() {
	return camera_rotation;
}

Matrix4x4 Camera::get_rotation_inv() {
	return camera_rotation_inv;
}