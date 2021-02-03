#pragma once
#include <memory>

//a matric which is a 2d homogenous vector
struct Matrix3x1 {
	double x, y, w;
	Matrix3x1() {}
	Matrix3x1(double x, double y, double w) : x(x), y(y), w(w) {}
	Matrix3x1 operator * (double d);
	Matrix3x1 operator + (Matrix3x1 v);
	Matrix3x1 operator - (Matrix3x1 v);
	double cross_as_2d(Matrix3x1);
	void print(std::ostream* os);
};

//a matrix which is a 3d homogenous vector
struct Matrix4x1 {
	double x, y, z, w;
	Matrix4x1() : x(), y(), z(), w() {}
	Matrix4x1(double x, double y, double z, double w) : x(x), y(y), z(z), w(w) {}
	Matrix4x1 operator * (double d);
	Matrix4x1 operator + (Matrix4x1 v);
	Matrix4x1 operator - (Matrix4x1 v);
	Matrix4x1 operator / (double d);
	Matrix4x1 cross_as_3d(Matrix4x1 o);
	Matrix4x1 normalise();
	double dot_product_as_3d(Matrix4x1 o);
	void print(std::ostream* os);
};

//the matrix which maps a 3d homogenous vector to another 3d homogenous vector
struct Matrix3x3 {
	Matrix3x1 i;
	Matrix3x1 j;
	Matrix3x1 m;
	Matrix3x3() : i(), j(), m() {}
	Matrix3x3(Matrix3x1 i, Matrix3x1 j, Matrix3x1 m) : i(i), j(j), m(m) {}
	Matrix3x3 operator *(Matrix3x3 other);
	Matrix3x1 operator *(Matrix3x1 v);
	void print(std::ostream* os);
};

//the matrix which maps a 3d homogenous vector to another 3d homogenous vector
struct Matrix4x4 {
	Matrix4x1 i;
	Matrix4x1 j;
	Matrix4x1 k;
	Matrix4x1 m;
	Matrix4x4() : i(), j(), k(), m() {}
	Matrix4x4(Matrix4x1 i, Matrix4x1 j, Matrix4x1 k, Matrix4x1 m) : i(i), j(j), k(k), m(m) {}
	Matrix4x4 operator *(Matrix4x4 other);
	Matrix4x1 operator *(Matrix4x1 v);
	Matrix4x4 normalise();
	void print(std::ostream* os);
};

Matrix4x4 identityMatrix4x4();
Matrix4x4 translation4x1(Matrix4x1 t);
Matrix3x3 translation3x1(Matrix3x1 t);
Matrix4x4 rotationRoll(double angle);
Matrix4x4 rotationPitch(double angle);
Matrix4x4 rotationYaw(double angle);

