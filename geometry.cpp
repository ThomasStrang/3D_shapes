#pragma once
#include <memory>
#include <iostream>
#include <math.h>
#include "geometry.h"

Matrix4x4 Matrix4x4::operator *(Matrix4x4 other) {
	return Matrix4x4(
		*this * other.i,
		*this * other.j,
		*this * other.k,
		*this * other.m
	);
}
Matrix4x4 Matrix4x4::normalise() {
	return Matrix4x4(i.normalise(), j.normalise(), k.normalise(), m.normalise());
}

Matrix4x1 Matrix4x4::operator *(Matrix4x1 v) {
	return (i * v.x) + (j * v.y) + (k * v.z) + (m * v.w);
}
//if this is run and w!=0 and d!=1 then the resulting w will be >0
Matrix4x1 Matrix4x1::operator * (double d) {
	return Matrix4x1(
		x * d,
		y * d,
		z * d,
		w * d
	);
}

Matrix4x1 Matrix4x1::operator + (Matrix4x1 v) {
	return Matrix4x1(
		x + v.x,
		y + v.y,
		z + v.z,
		w + v.w
	);
}

Matrix4x1 Matrix4x1::operator - (Matrix4x1 v) {
	return Matrix4x1(
		x - v.x,
		y - v.y,
		z - v.z,
		w - v.w
	);
}

Matrix4x1 Matrix4x1::operator / (double d) {
	double inv_d = 1.0 / d;
	return Matrix4x1(
		x * inv_d,
		y * inv_d,
		z * inv_d,
		w * inv_d
	);
}

Matrix4x1 Matrix4x1::cross_as_3d(Matrix4x1 o) {
	return Matrix4x1(y * o.z - z * o.y, z * o.x - x * o.z, x * o.y - y * o.x, 1);
}

Matrix4x1 Matrix4x1::normalise() {
	return *this * (1 / (sqrt(x * x + y * y + z * z + w * w)));
}

Matrix4x1 Matrix4x1::normalise_as_3d() {
	auto m = *this * (1 / (sqrt(x * x + y * y + z * z)));
	m.w = 1;
	return m;
}

double Matrix4x1::dot_product_as_3d(Matrix4x1 o) {
	return x * o.x + y * o.y + z * o.z;
}

//returns the application of this matrix to the other
Matrix3x3 Matrix3x3::operator *(Matrix3x3 other) {
	return Matrix3x3(
		*this * other.i,
		*this * other.j,
		*this * other.m
	);
}

Matrix3x1 Matrix3x3::operator *(Matrix3x1 v) {
	return (i * v.x) + (j * v.y) + (m * v.w);
}

Matrix3x1 Matrix3x1::operator * (double d) {
	return Matrix3x1(
		x * d,
		y * d,
		w * d
	);
}

void Matrix3x1::print(std::ostream* cout) {
	*cout << "[" << x << " , " << y << " , " << w << "]";
}

void Matrix3x3::print(std::ostream* cout) {
	*cout << "[";
	i.print(cout);
	*cout << ",";
	j.print(cout);
	*cout << ",";
	m.print(cout);
	*cout << "]";
}
void Matrix4x1::print(std::ostream* cout) {
	*cout << "[" << x << "," << y << "," << z << "," << w << "]";
}

void Matrix4x4::print(std::ostream* cout) {
	*cout << "[";
	i.print(cout);
	*cout << ",";
	j.print(cout);
	*cout << ",";
	k.print(cout);
	*cout << ",";
	m.print(cout);
	*cout << "]";
}

Matrix3x1 Matrix3x1::operator + (Matrix3x1 v) {
	return Matrix3x1(
		x + v.x,
		y + v.y,
		w + v.w
	);
}

Matrix3x1 Matrix3x1::operator - (Matrix3x1 v) {
	return Matrix3x1(
		x - v.x,
		y - v.y,
		w - v.w
	);
}

double Matrix3x1::cross_as_2d(Matrix3x1 o) {
	return x * o.y - y * o.x;
}

double Matrix3x1::homogenous_dot_product(Matrix3x1 o) {
	return x * o.x + y * o.y + w * o.w;
}

Matrix3x1 Matrix3x1::homogenous_cross_product(Matrix3x1 o) {
	return Matrix3x1(y * o.w - w * o.y, w * o.x - x * o.w, x * o.y - y * o.x);
}

Matrix4x4 identityMatrix4x4() {
	return Matrix4x4(
		Matrix4x1(1, 0, 0, 0),
		Matrix4x1(0, 1, 0, 0),
		Matrix4x1(0, 0, 1, 0),
		Matrix4x1(0, 0, 0, 1)
	);
}

Matrix4x4 translation4x1(Matrix4x1 t) {
	return Matrix4x4(
		Matrix4x1(1, 0, 0, 0),
		Matrix4x1(0, 1, 0, 0),
		Matrix4x1(0, 0, 1, 0),
		Matrix4x1(t.x, t.y, t.z, 1)
	);
}
Matrix3x3 translation3x1(Matrix3x1 t) {
	return Matrix3x3(
		Matrix3x1(1, 0, 0),
		Matrix3x1(0, 1, 0),
		Matrix3x1(t.x, t.y, 1)
	);
}

Matrix4x4 rotationRoll(double angle) {
	return Matrix4x4(
		Matrix4x1(cos(angle), -sin(angle), 0, 0),
		Matrix4x1(sin(angle), cos(angle), 0, 0),
		Matrix4x1(0, 0, 1, 0),
		Matrix4x1(0, 0, 0, 1)
	);
}
Matrix4x4 rotationPitch(double angle) {
	return Matrix4x4(
		Matrix4x1(1, 0, 0, 0),
		Matrix4x1(0, cos(angle), -sin(angle), 0),
		Matrix4x1(0, sin(angle), cos(angle), 0),
		Matrix4x1(0, 0, 0, 1)
	);
}
Matrix4x4 rotationYaw(double angle) {
	return Matrix4x4(
		Matrix4x1(cos(angle), 0, sin(angle), 0),
		Matrix4x1(0, 1, 0, 0),
		Matrix4x1(-sin(angle), 0, cos(angle), 0),
		Matrix4x1(0, 0, 0, 1)
	);
}

