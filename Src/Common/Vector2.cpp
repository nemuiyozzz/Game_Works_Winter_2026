#include "../Pch.h"
#include "Vector2.h"

/* intŒ^Vector2 */

Vector2 Vector2::operator=(const Vector2& vec)
{
	x = vec.x;
	y = vec.y;
	return Vector2{ vec.x, vec.y };
}


Vector2 Vector2::operator+(const Vector2& vec)const
{
	return Vector2{ (x + vec.x), (y + vec.y) };
}

void Vector2::operator+=(const Vector2& vec)
{
	x += vec.x;
	y += vec.y;
}

Vector2 Vector2::operator-(const Vector2& vec)const
{
	return Vector2{ (x - vec.x), (y - vec.y) };
}

void Vector2::operator-=(const Vector2& vec)
{
	x -= vec.x;
	y -= vec.y;
}

Vector2 Vector2::operator*(const Vector2& vec)const
{
	return Vector2{ (x * vec.x), (y * vec.y) };
}

void Vector2::operator*=(const Vector2& vec)
{
	x *= vec.x;
	y *= vec.y;
}
void Vector2::operator*=(int _value)
{
	if (_value == 0)
	{
		OutputDebugString(L"\n”’l‚ª0‚É‚È‚Á‚Ä‚¢‚Ü‚·B\n");
	}
	x *= _value;
	y *= _value;
}
void Vector2::operator*=(float _value)
{
	if (_value == 0.0f)
	{
		OutputDebugString(L"\n”’l‚ª0‚É‚È‚Á‚Ä‚¢‚Ü‚·B\n");
	}
	float xF = static_cast<float>(x);
	float yF = static_cast<float>(y);

	xF *= _value;
	yF *= _value;

	x = static_cast<int>(xF);
	y = static_cast<int>(yF);
}

Vector2 Vector2::operator/(const Vector2& _vec)const
{
	Vector2 ret = _vec;

	// ƒ[ƒœZ‘Îô
	ret.x = ((_vec.x != 0) ? (x / _vec.x) : 0);
	ret.y = ((_vec.y != 0) ? (y / _vec.y) : 0);

	return ret;
}

void Vector2::operator/=(const Vector2& _vec)
{
	// ƒ[ƒœZ‘Îô
	x = ((_vec.x != 0) ? (x / _vec.x) : 0);
	y = ((_vec.y != 0) ? (y / _vec.y) : 0);
}

void Vector2::operator/=(int _value)
{
	if (_value == 0)
	{
		OutputDebugString(L"\n”’l‚ª0‚É‚È‚Á‚Ä‚¢‚Ü‚·B\n");
	}

	// ƒ[ƒœZ‘Îô
	x = ((_value != 0) ? (x / _value) : 0);
	y = ((_value != 0) ? (y / _value) : 0);
}



/* floatŒ^Vector2 */

Vector2F Vector2F::operator=(const Vector2F& vec)
{
	x = vec.x;
	y = vec.y;
	return Vector2F{ vec.x, vec.y };
}


Vector2F Vector2F::operator+(const Vector2F& vec)const
{
	return Vector2F{ (x + vec.x), (y + vec.y) };
}
void Vector2F::operator+=(const Vector2F& vec)
{
	x += vec.x;
	y += vec.y;
}
void Vector2F::operator+=(float _value)
{
	x += _value;
	y += _value;
}

Vector2F Vector2F::operator-(const Vector2F& vec)const
{
	return Vector2F{ (x - vec.x), (y - vec.y) };
}
void Vector2F::operator-=(const Vector2F& vec)
{
	x -= vec.x;
	y -= vec.y;
}
void Vector2F::operator-=(float _value)
{
	x -= _value;
	y -= _value;
}

Vector2F Vector2F::operator*(const Vector2F& vec)const
{
	return Vector2F{ (x * vec.x), (y * vec.y) };
}
void Vector2F::operator*=(const Vector2F& vec)
{
	x *= vec.x;
	y *= vec.y;
}
void Vector2F::operator*=(float _value)
{
	x *= _value;
	y *= _value;
}

Vector2F Vector2F::operator/(const Vector2F& _vec)const
{
	Vector2F ret = _vec;

	// ƒ[ƒœZ‘Îô
	ret.x = ((_vec.x != 0) ? (x / _vec.x) : 0);
	ret.y = ((_vec.y != 0) ? (y / _vec.y) : 0);

	return ret;
}
void Vector2F::operator/=(const Vector2F& _vec)
{
	x /= _vec.x;
	y /= _vec.y;
}
void Vector2F::operator/=(float _value)
{
	// ƒ[ƒœZ‘Îô
	x = ((_value != 0) ? (x / _value) : 0);
	y = ((_value != 0) ? (y / _value) : 0);
}
