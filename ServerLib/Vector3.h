#pragma once


	/*---------------------------
			Unity Vector3
	---------------------------*/
struct Vector3
{
	Vector3();
	Vector3(float x, float y, float z);
	Vector3(const Vector3& other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
	}

	Vector3& operator= (const Vector3& other)
	{
		if (this != &other)
		{
			x = other.x;
			y = other.y;
			z = other.z;
		}

		return *this;
	}

	Vector3& operator+= (const Vector3& other)
	{
		if (this != &other)
		{
			x += other.x;
			y += other.y;
			z += other.z;
		}

		return *this;
	}

	Vector3 operator- (const Vector3& other)
	{
		return Vector3(x - other.x, y - other.y, z - other.z);
	}

	Vector3 operator- (const Vector3& other) const
	{
		return Vector3(x - other.x, y - other.y, z - other.z);
	}

	Vector3 operator*(float f)
	{
		return Vector3(x * f, y * f, z * f);
	}

	static Vector3 Zero()
	{
		static const Vector3 zero(0, 0, 0);
		return zero;
	}

	static Vector3 Left()
	{
		static const Vector3 left(-1.0f, 0, 0);
		return left;
	}
	static Vector3 Right()
	{
		static const Vector3 right(1.0f, 0, 0);
		return right;
	}
	static Vector3 Forward()
	{
		static const Vector3 forward(0, 0, 1.0f);
		return forward;
	}
	static Vector3 Back()
	{
		static const Vector3 back(0, 0, -1.0f);
		return back;
	}

	// Distance와 Magnitude는 루트계산 -> 느리다. 정확한 값아니면 사용x
	static float Distance(const Vector3& firstVec, const Vector3& secondVec);
	float Magnitude()
	{
		int powCount = 2;

		return static_cast<float>(sqrt(pow(x, powCount) + pow(y, powCount) + pow(z, powCount)));
	}
	float sqrMagnitude() const { return x * x + y * y + z * z; }

	static float Dot(const Vector3& from, const Vector3& to)
	{
		return from.x * to.x + from.y * to.y + from.z * to.z;
	}

	Vector3 Normalized()
	{
		float magnitude = Magnitude();

		if (magnitude > 1E-05f)
			return Vector3(x / magnitude, y / magnitude, z / magnitude);

		return Zero();
	}

	// 두 벡터 사이의 각도를 반환한다. 방향상관없이 리턴값은 0~180, 0~360을 위해선 SignedAngle을 사용하라.
	static float Angle(Vector3 from, Vector3 to)
	{
		float num = (float)sqrt(from.sqrMagnitude() * to.sqrMagnitude());
		if (num < 1E-15f)
		{
			return 0;
		}

		float num2 = std::clamp(Dot(from, to) / num, -1.0f, 1.0f);
		return (float)acos(num2) * 57.29578f;
	}


	bool operator== (const Vector3& other) const
	{
		return this->x == other.x && this->y == other.y && this->z == other.z;
	}

	bool operator!= (const Vector3& other) const
	{
		return !((*this) == other);
	}

	float x;
	float y;
	float z;
};

const float deg2Rad = 180.0f / 3.141592f;
float NormalizeAngle(float angle);