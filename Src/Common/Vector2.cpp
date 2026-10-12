#include "../Pch.h"
#include "Vector2.h"

/* intŒ^Vector2 */

Vector2 Vector2::operator=(const Vector2& vec)
{
	x_ = vec.x_;
	y_ = vec.y_;
	return Vector2{ vec.x_, vec.y_ };
}


Vector2 Vector2::operator+(const Vector2& vec)const
{
	return Vector2{ (x_ + vec.x_), (y_ + vec.y_) };
}

void Vector2::operator+=(const Vector2& vec)
{
	x_ += vec.x_;
	y_ += vec.y_;
}

Vector2 Vector2::operator-(const Vector2& vec)const
{
	return Vector2{ (x_ - vec.x_), (y_ - vec.y_) };
}

void Vector2::operator-=(const Vector2& vec)
{
	x_ -= vec.x_;
	y_ -= vec.y_;
}

Vector2 Vector2::operator*(const Vector2& vec)const
{
	return Vector2{ (x_ * vec.x_), (y_ * vec.y_) };
}

void Vector2::operator*=(const Vector2& vec)
{
	x_ *= vec.x_;
	y_ *= vec.y_;
}
void Vector2::operator*=(int value)
{
	if (value == 0)
	{
		OutputDebugString(L"\n”’l‚ª0‚É‚È‚Á‚Ä‚¢‚Ü‚·B\n");
	}
	x_ *= value;
	y_ *= value;
}
void Vector2::operator*=(float value)
{
	if (value == 0.0f)
	{
		OutputDebugString(L"\n”’l‚ª0‚É‚È‚Á‚Ä‚¢‚Ü‚·B\n");
	}
	float xF = static_cast<float>(x_);
	float yF = static_cast<float>(y_);

	xF *= value;
	yF *= value;

	x_ = static_cast<int>(xF);
	y_ = static_cast<int>(yF);
}

Vector2 Vector2::operator/(const Vector2& vec)const
{
	Vector2 ret = vec;

	// ƒ[ƒœZ‘Îô
	ret.x_ = ((vec.x_ != 0) ? (x_ / vec.x_) : 0);
	ret.y_ = ((vec.y_ != 0) ? (y_ / vec.y_) : 0);

	return ret;
}

void Vector2::operator/=(const Vector2& vec)
{
	// ƒ[ƒœZ‘Îô
	x_ = ((vec.x_ != 0) ? (x_ / vec.x_) : 0);
	y_ = ((vec.y_ != 0) ? (y_ / vec.y_) : 0);
}

void Vector2::operator/=(int value)
{
	if (value == 0)
	{
		OutputDebugString(L"\n”’l‚ª0‚É‚È‚Á‚Ä‚¢‚Ü‚·B\n");
	}

	// ƒ[ƒœZ‘Îô
	x_ = ((value != 0) ? (x_ / value) : 0);
	y_ = ((value != 0) ? (y_ / value) : 0);
}



/* floatŒ^Vector2 */

Vector2F Vector2F::operator=(const Vector2F& vec)
{
	x_ = vec.x_;
	y_ = vec.y_;
	return Vector2F{ vec.x_, vec.y_ };
}


Vector2F Vector2F::operator+(const Vector2F& vec)const
{
	return Vector2F{ (x_ + vec.x_), (y_ + vec.y_) };
}
void Vector2F::operator+=(const Vector2F& vec)
{
	x_ += vec.x_;
	y_ += vec.y_;
}
void Vector2F::operator+=(float value)
{
	x_ += value;
	y_ += value;
}

Vector2F Vector2F::operator-(const Vector2F& vec)const
{
	return Vector2F{ (x_ - vec.x_), (y_ - vec.y_) };
}
void Vector2F::operator-=(const Vector2F& vec)
{
	x_ -= vec.x_;
	y_ -= vec.y_;
}
void Vector2F::operator-=(float value)
{
	x_ -= value;
	y_ -= value;
}

Vector2F Vector2F::operator*(const Vector2F& vec)const
{
	return Vector2F{ (x_ * vec.x_), (y_ * vec.y_) };
}
void Vector2F::operator*=(const Vector2F& vec)
{
	x_ *= vec.x_;
	y_ *= vec.y_;
}
void Vector2F::operator*=(float value)
{
	x_ *= value;
	y_ *= value;
}

Vector2F Vector2F::operator/(const Vector2F& vec)const
{
	Vector2F ret = vec;

	// ƒ[ƒœZ‘Îô
	ret.x_ = ((vec.x_ != 0) ? (x_ / vec.x_) : 0);
	ret.y_ = ((vec.y_ != 0) ? (y_ / vec.y_) : 0);

	return ret;
}
void Vector2F::operator/=(const Vector2F& vec)
{
	x_ /= vec.x_;
	y_ /= vec.y_;
}
void Vector2F::operator/=(float value)
{
	// ƒ[ƒœZ‘Îô
	x_ = ((value != 0) ? (x_ / value) : 0);
	y_ = ((value != 0) ? (y_ / value) : 0);
}
