#include "math3d.h"
#include <iostream>
using namespace std;

float vector3f::getX() { return x; }

float vector3f::getY() { return y; }

float vector3f::getZ() { return z; }

void vector3f::setX(float _x) { x = _x; }

void vector3f::setY(float _y) { y = _y; }

void vector3f::setZ(float _z) { z = _z; }

void vector3f::clear()
{
	x = y = z = 0.0;
}

void vector3f::setUp(float a, float b, float c)
{
	x = a;
	y = b;
	z = c;
}

vector3f vector3f::addition(vector3f v)
{
	float q, w, r;
	q = x + v.getX();
	w = y + v.getY();
	r = z + v.getZ();

	vector3f s;
	s.setUp(q, w, r);
	return s;
}

vector3f vector3f::operator+(vector3f v)
{
	vector3f newVec;
	newVec.setUp(x + v.getX(), y + v.getY(), z + v.getZ());
	return newVec;
}

vector3f vector3f::operator-(vector3f v)
{
	vector3f newVec;
	newVec.setUp(x - v.getX(), y - v.getY(), z - v.getZ());
	return newVec;
}

void vector3f::operator +=(vector3f v)
{
	x += v.getX();
	y += v.getY();
	z += v.getZ();
}

vector3f vector3f::operator /(float d)
{
	return vector3f(x / d, y / d, z / d);
}

vector3f& vector3f::operator /=(float d)
{
	x /= d;
	y /= d;
	z /= d;
	return *this;
}

vector3f vector3f::operator *(float d)
{
	return vector3f(x * d, y * d, z * d);
}

float vector3f::operator *(vector3f v)
{
	return (x * v.getX() + y * v.getY() + z * v.getZ());
}

vector3f& vector3f::operator= (vector3f v)
{
	x = v.getX();
	y = v.getY();
	z = v.getZ();

	return *this;
}

float vector3f::length()
{
	return sqrtf(x * x + y * y + z * z);
}

float vector3f::lengthSquare()
{
	return x * x + y * y + z * z;
}

void vector3f::makeUnit()
{
	float s = length();
	if (s != 0)
	{
		x /= s;
		y /= s;
		z /= s;
	}

}

vector3f vector3f::returnUnit()
{
	float s = length();
	if (s != 0)
		return vector3f(x / s, y / s, z / s);
	else
		return vector3f(0, 0, 0);
}

void vector3f::print()
{
	cout << "x is " << x << " y is " << y << " z is " << z << "\n";
}

vector3f vector3f::projectTo(vector3f& direction)
{
	float cos = (*this) * direction;
	return vector3f(direction / direction.lengthSquare() * cos);
}

vector3f vector3f::computeSurfaceNormal(vector3f& v)
{
	float x = this->getY() * v.getZ() - this->getZ() * v.getY();
	float y = this->getZ() * v.getX() - this->getX() * v.getZ();
	float z = this->getX() * v.getY() - this->getY() * v.getX();

	return vector3f(x, y, z);
}