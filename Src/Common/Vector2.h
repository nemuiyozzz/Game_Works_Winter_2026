#pragma once

/* int型Vector2  */
class Vector2
{
public:

	int x_; // X座標
	int y_; // Y座標

	// デストラクタ
	~Vector2(void) = default;

	// 代入処理
	Vector2 operator=(const Vector2& vec);

	// 加算処理
	Vector2 operator+(const Vector2& vec)const;
	void operator+=(const Vector2& vec);

	// 減算処理
	Vector2 operator-(const Vector2& vec)const;
	void operator-=(const Vector2& vec);

	// 乗算処理
	Vector2 operator*(const Vector2& vec)const;
	void operator*=(const Vector2& vec);
	void operator*=(int value);
	void operator*=(float value);

	// 除算処理
	Vector2 operator/(const Vector2& vec)const;
	void operator/=(const Vector2& vec);
	void operator/=(int value);
};


/* float型Vector2  */
class Vector2F
{
public:

	float x_; // X座標
	float y_; // Y座標

	// デストラクタ
	~Vector2F(void) = default;

	// 代入処理
	Vector2F operator=(const Vector2F& vec);

	// 加算処理
	Vector2F operator+(const Vector2F& vec)const;
	void operator+=(const Vector2F& vec);
	void operator+=(float value);

	// 減算処理
	Vector2F operator-(const Vector2F& vec)const;
	void operator-=(const Vector2F& vec);
	void operator-=(float value);

	// 乗算処理
	Vector2F operator*(const Vector2F& vec)const;
	void operator*=(const Vector2F& vec);
	void operator*=(float value);

	// 除算処理
	Vector2F operator/(const Vector2F& vec)const;
	void operator/=(const Vector2F& vec);
	void operator/=(float value);
};