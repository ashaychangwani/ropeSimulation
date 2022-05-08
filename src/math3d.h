/* SIMPLE MATHS LIBRARY
PREPARED BY GIZEM KAYAR FOR COMPUTER GRAPHICS COURSE/NEW YORK UNIVERSITY
YEAR: 2022
*/
#pragma once
class vector3f
{
private:
	float x, y, z;
public:
	//constructor
	vector3f()
	{
		clear();
	}

	vector3f(float xx, float yy, float zz)
	{
		x = xx;
		y = yy;
		z = zz;
	}

	//dectructor
	~vector3f() { }

	//Setters and getters
	float getX();
	float getY();
	float getZ();
	void setX(float _x);
	void setY(float _y);
	void setZ(float _z);

	//Important functions
	void clear();
	void setUp(float a, float b, float c);
	vector3f addition(vector3f v);

	vector3f operator+(vector3f v);
	vector3f operator-(vector3f v);
	void operator +=(vector3f v);
	vector3f operator / (float d);
	vector3f& operator /= (float d);
	vector3f operator * (float d);
	float operator *(vector3f v);
	vector3f& operator= (vector3f v);

	float length();
	float lengthSquare();

	void makeUnit();
	vector3f returnUnit();

	void print();

	vector3f projectTo(vector3f& direction);

	vector3f computeSurfaceNormal(vector3f& v);
};
