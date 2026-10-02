#pragma once

/* intŒ^Vector2  */
class Vector2
{
public:

	int x; // XÀ•W
	int y; // YÀ•W

	// ƒfƒXƒgƒ‰ƒNƒ^
	~Vector2(void) = default;

	// ‘ã“üˆ—
	Vector2 operator=(const Vector2& _vec);

	// ‰ÁZˆ—
	Vector2 operator+(const Vector2& _vec)const;
	void operator+=(const Vector2& _vec);

	// Œ¸Zˆ—
	Vector2 operator-(const Vector2& _vec)const;
	void operator-=(const Vector2& _vec);

	// æZˆ—
	Vector2 operator*(const Vector2& _vec)const;
	void operator*=(const Vector2& _vec);
	void operator*=(int _value);
	void operator*=(float _value);

	// œZˆ—
	Vector2 operator/(const Vector2& _vec)const;
	void operator/=(const Vector2& _vec);
	void operator/=(int _value);
};


/* floatŒ^Vector2  */
class Vector2F
{
public:

	float x; // XÀ•W
	float y; // YÀ•W

	// ƒfƒXƒgƒ‰ƒNƒ^
	~Vector2F(void) = default;

	// ‘ã“üˆ—
	Vector2F operator=(const Vector2F& _vec);

	// ‰ÁZˆ—
	Vector2F operator+(const Vector2F& _vec)const;
	void operator+=(const Vector2F& _vec);
	void operator+=(float _value);

	// Œ¸Zˆ—
	Vector2F operator-(const Vector2F& _vec)const;
	void operator-=(const Vector2F& _vec);
	void operator-=(float _value);

	// æZˆ—
	Vector2F operator*(const Vector2F& _vec)const;
	void operator*=(const Vector2F& _vec);
	void operator*=(float _value);

	// œZˆ—
	Vector2F operator/(const Vector2F& _vec)const;
	void operator/=(const Vector2F& _vec);
	void operator/=(float _value);
};