#pragma once

/* int型Vector2  */
class Vector2
{
public:

	int x; // X座標
	int y; // Y座標

	// デストラクタ
	~Vector2(void) = default;

	// 代入処理
	Vector2 operator=(const Vector2& _vec);

	// 加算処理
	Vector2 operator+(const Vector2& _vec)const;
	void operator+=(const Vector2& _vec);

	// 減算処理
	Vector2 operator-(const Vector2& _vec)const;
	void operator-=(const Vector2& _vec);

	// 乗算処理
	Vector2 operator*(const Vector2& _vec)const;
	void operator*=(const Vector2& _vec);
	void operator*=(int _value);
	void operator*=(float _value);

	// 除算処理
	Vector2 operator/(const Vector2& _vec)const;
	void operator/=(const Vector2& _vec);
	void operator/=(int _value);
};


/* float型Vector2  */
class Vector2F
{
public:

	float x; // X座標
	float y; // Y座標

	// デストラクタ
	~Vector2F(void) = default;

	// 代入処理
	Vector2F operator=(const Vector2F& _vec);

	// 加算処理
	Vector2F operator+(const Vector2F& _vec)const;
	void operator+=(const Vector2F& _vec);
	void operator+=(float _value);

	// 減算処理
	Vector2F operator-(const Vector2F& _vec)const;
	void operator-=(const Vector2F& _vec);
	void operator-=(float _value);

	// 乗算処理
	Vector2F operator*(const Vector2F& _vec)const;
	void operator*=(const Vector2F& _vec);
	void operator*=(float _value);

	// 除算処理
	Vector2F operator/(const Vector2F& _vec)const;
	void operator/=(const Vector2F& _vec);
	void operator/=(float _value);
};