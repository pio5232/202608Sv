#include "LibraryPch.h"
#include "Vector3.h"

float NormalizeAngle(float angle)
{
	angle = fmod(angle + 180.0f, 360.0f);
	if (angle < 0)
		angle += 360.0f;
	return angle - 180.0f;
}

Vector3::Vector3() : x(0), y(0), z(0)
{
}

Vector3::Vector3(float inX, float inY, float inZ) : x(inX), y(inY), z(inZ)
{
}

float Vector3::Distance(const Vector3& firstVec, const Vector3& secondVec)
{
	float diffX = firstVec.x - secondVec.x;
	float diffY = firstVec.y - secondVec.y;
	float diffZ = firstVec.z - secondVec.z;

	return static_cast<float>(sqrt(diffX * diffX + diffY * diffY + diffZ * diffZ));
}

