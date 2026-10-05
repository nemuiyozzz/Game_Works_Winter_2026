#include "../Pch.h"
#include "UtilityMath.h"



// 小数値を四捨五入して整数に変換する
int UtilityMath::Round(float v)
{
	return static_cast<int>(roundf(v));
}

// 文字列を指定された区切り文字で分割する
std::vector<std::wstring> UtilityMath::Split(const std::wstring& line, wchar_t delimiter)
{
	std::wistringstream stream(line);
	std::wstring field;
	std::vector<std::wstring> result;

	while (std::getline(stream, field, delimiter))
	{
		result.push_back(field);
	}

	return result;
}

// ラジアンを度に変換する（double）
double UtilityMath::Rad2DegD(double rad)
{
	return rad * (180.0 / DX_PI);
}

// ラジアンを度に変換する（float）
float UtilityMath::Rad2DegF(float rad)
{
	return rad * (180.0f / DX_PI_F);
}

// ラジアンを度に変換して整数にする
int UtilityMath::Rad2DegI(int rad)
{
	return rad * Round(180.0f / DX_PI_F);
}

// 度をラジアンに変換する（double）
double UtilityMath::Deg2RadD(double deg)
{
	return deg * (DX_PI / 180.0);
}

// 度をラジアンに変換する（float）
float UtilityMath::Deg2RadF(float deg)
{
	return deg * (DX_PI_F / 180.0f);
}

// 度をラジアンに変換して整数にする
int UtilityMath::Deg2RadI(int deg)
{
	return deg * Round(DX_PI_F / 180.0f);
}

// 角度を0～360度の範囲に正規化する
double UtilityMath::DegIn360(double deg)
{
	deg = fmod(deg, 360.0);
	if (deg < 0.0f)
	{
		deg += 360.0;
	}

	return deg;
}

// ラジアンを0～2πの範囲に正規化する
double UtilityMath::RadIn2PI(double rad)
{
	rad = fmod(rad, DX_TWO_PI);
	if (rad < 0.0)
	{
		rad += DX_TWO_PI;
	}

	return rad;
}

// ラジアン単位でどちら回りが近いかを判定する（-1=反時計回り、1=時計回り）
int UtilityMath::DirNearAroundRad(float from, float to)
{
	float ret = 1.0f;

	float diff = to - from;

	if (diff >= 0.0f)
	{

		//比較元よりも時計回りに位置する
		if (diff > DX_PI_F)
		{

			//でも、180度以上離れているので、反時計回りの方が近い
			ret = -1.0f;
		}
		else
		{
			//時計回り
			ret = 1.0f;
		}

	}
	else
	{

		//比較元よりも反時計回りに位置する

		if (diff < -DX_PI_F)
		{
			//でも、180度以上離れているので、時計回りの方が近い
			ret = 1.0f;
		}
		else
		{
			//反時計回り
			ret = -1.0f;
		}
	}

	return static_cast<int>(ret);
}

// 度単位でどちら回りが近いかを判定する（-1=反時計回り、1=時計回り）
int UtilityMath::DirNearAroundDeg(float from, float to)
{
	float ret = 1.0f;

	float diff = to - from;

	if (diff >= 0.0f)
	{


		//比較元よりも時計回りに位置する
		if (diff > 180.0f)
		{

			//でも、180度以上離れているので、反時計回りの方が近い
			ret = -1.0f;
		}
		else
		{
			//時計回り
			ret = 1.0f;
		}
	}
	else
	{
		//比較元よりも反時計回りに位置する
		if (diff < -180.0f)
		{
			//でも、180度以上離れているので。時計回りの方が近い
			ret = 1.0f;
		}
		else
		{
			//時計回り
			ret = -1.0f;
		}
	}

	return static_cast<int>(ret);
}

// 整数値を線形補間する
int UtilityMath::Lerp(int start, int end, float t)
{
	//線形補間
	if (t >= 1.0f)
	{
		return end;
	}

	return (start + Round(t * static_cast<float>(end - start)));;
}

// 浮動小数点値を線形補間する
float UtilityMath::Lerp(float start, float end, float t)
{
	//線形補間
	if (t >= 1.0f)
	{
		return end;
	}

	return (start + (t * (end - start)));
}

// 倍精度浮動小数点値を線形補間する
double UtilityMath::Lerp(double start, double end, double t)
{
	//線形補間
	if (t >= 1.0)
	{
		return end;
	}

	return (start + (t * (end - start)));;
}

// 2Dベクトルを線形補間する
Vector2 UtilityMath::Lerp(const Vector2& start, const Vector2& end, float t)
{
	//線形補間
	if (t >= 1.0f)
	{
		return end;
	}

	
	return Vector2(start.x + Round(t * static_cast<float>((end.x - start.x)))
				 , start.y + Round(t * static_cast<float>((end.y - start.y))));
}

// 3Dベクトルを線形補間する
VECTOR UtilityMath::Lerp(const VECTOR& start, const VECTOR& end, float t)
{
	//線形補間
	if (t >= 1.0f)
	{
		return end;
	}

	return VGet(start.x + t * (end.x - start.x)
		, start.y + t * (end.y - start.y)
		, start.z + t * (end.z - start.z));
}

// 度単位で角度を線形補間し、360度を考慮する
double UtilityMath::LerpDeg(double start, double end, double t)
{
	double ret;

	double diff = end - start;

	if (diff < -180.0)
	{
		end += 360.0;

		ret = Lerp(start, end, t);

		if (ret >= 360.0)
		{
			ret -= 360.0;
		}
	}
	else if (diff > 180.0)
	{
		end -= 360.0;

		ret = Lerp(start, end, t);

		if (ret < 0.0)
		{
			ret += 360.0;
		}
	}
	else
	{
		ret = Lerp(start, end, t);
	}

	return ret;
}

// 色（COLOR_F）を線形補間する
COLOR_F UtilityMath::Lerp(const COLOR_F& start, const COLOR_F& end, float t)
{
	//線形補間
	if (t >= 1.0f)
	{
		return end;
	}
	return COLOR_F(start.r + t * (end.r - start.r)
		, start.g + t * (end.g - start.g)
		, start.b + t * (end.b - start.b)
		, start.a + t * (end.a - start.a));
}

// 2Dベクトルの3点間でベジエ補間する
Vector2 UtilityMath::Bezier(const Vector2& p1, const Vector2& p2, const Vector2& p3, float t)
{
	Vector2 a = Lerp(p1, p2, t);
	Vector2 b = Lerp(p2, p3, t);

	return Lerp(a, b, t);
}

// 3Dベクトルの3点間でベジエ補間する
VECTOR UtilityMath::Bezier(const VECTOR& p1, const VECTOR& p2, const VECTOR& p3, float t)
{
	VECTOR a = Lerp(p1, p2, t);
	VECTOR b = Lerp(p2, p3, t);

	return Lerp(a, b, t);
}

// XZ平面上で中心点を軸に回転後の座標を求める
VECTOR UtilityMath::RotXZPos(const VECTOR& centerPos, const VECTOR& radiusPos, float rad)
{
	float x = ((radiusPos.x - centerPos.x) * cosf(rad)) - ((radiusPos.z - centerPos.z) * sinf(rad));
	float z = ((radiusPos.x - centerPos.x) * cosf(rad)) + ((radiusPos.z - centerPos.z) * cosf(rad));

	return VGet(centerPos.x + x, radiusPos.y, centerPos.z + z);
}

// 2Dベクトルの長さを求める
double UtilityMath::Magnitude(const Vector2& v)
{
	return sqrt((v.x * v.x) + (v.y * v.y));
}

// 3Dベクトルの長さを求める
double UtilityMath::Magnitude(const VECTOR& v)
{
	return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

// 3Dベクトルの長さをfloatで求める
float UtilityMath::MagnitudeF(const VECTOR& v)
{
	return sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

// 2Dベクトルの長さの2乗を求める
int UtilityMath::SqrMagnitude(const Vector2& v)
{
	return v.x * v.x + v.y * v.y;
}

// 3Dベクトルの長さの2乗をfloatで求める
float UtilityMath::SqrMagnitudeF(const VECTOR& v)
{
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

// 3Dベクトルの長さの2乗を求める
double UtilityMath::SqrMagnitude(const VECTOR& v)
{
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

// 2つの3Dベクトル間の距離の2乗を求める

double UtilityMath::SqrMagnitude(const VECTOR& v1, const VECTOR& v2)
{
	return pow(v2.x - v1.x, 2) + pow(v2.y - v1.y, 2) + pow(v2.z - v1.z, 2);
}

// 2つの2Dベクトル間の距離を求める
double UtilityMath::Distance(const Vector2& v1, const Vector2& v2)
{
	return sqrt(pow(v2.x - v1.x, 2) + pow(v2.y - v1.y, 2));
}

// 2つの3Dベクトル間の距離を求める
double UtilityMath::Distance(const VECTOR& v1, const VECTOR& v2)
{
	return sqrt(pow(v2.x - v1.x, 2) + pow(v2.y - v1.y, 2) + pow(v2.z - v1.z, 2));
}

// 2つの3Dベクトルが等しいかどうかを判定する
bool UtilityMath::Equals(const VECTOR& vec1, const VECTOR& vec2)
{
	if (vec1.x == vec2.x && vec1.y == vec2.y && vec1.z == vec2.z)
	{
		return true;
	}

	return false;
}

/* ベクトルがゼロベクトルか判定 */
bool UtilityMath::EqualsVZero(const VECTOR& vec)
{
	const VECTOR& ZERO = VECTOR_ZERO;
	if (vec.x == ZERO.x && vec.y == ZERO.y && vec.z == ZERO.z)
	{
		return true;
	}

	return false;
}
bool UtilityMath::EqualsVZero(const Vector2& vec)
{
	const Vector2& ZERO = VECTOR2_ZERO;
	if (vec.x == ZERO.x && vec.y == ZERO.y)
	{
		return true;
	}

	return false;
}
bool UtilityMath::EqualsVZero(const Vector2F& vec)
{
	const Vector2F& ZERO = VECTOR2F_ZERO;
	if (vec.x == ZERO.x && vec.y == ZERO.y)
	{
		return true;
	}
	return false;
}

// 2Dベクトルを正規化して3Dベクトルで返す
VECTOR UtilityMath::Normalize(const Vector2& v)
{
	VECTOR ret = VGet(
		static_cast<float>(v.x),
		static_cast<float>(v.y),
		0.0f
	);

	float len = static_cast<float>(Magnitude(v));

	ret.x /= len;
	ret.y /= len;
	ret.z /= len;

	return ret;
}

/* ベクトルを正規化 */
VECTOR UtilityMath::VNormalize(const VECTOR& _vec)
{
	// ゼロ除算対策で元のゼロベクトルを返す
	if (UtilityMath::EqualsVZero(_vec)) { return _vec; }

	return VNorm(_vec);
}
Vector2 UtilityMath::VNormalize(const Vector2& vec)
{
	// ゼロ除算対策で元のゼロベクトルを返す
	if (UtilityMath::EqualsVZero(vec)) { return vec; }

	float mag = sqrtf((vec.x * vec.x) + (vec.y * vec.y));

	return Vector2(static_cast<int>(vec.x / mag), static_cast<int>(vec.y / mag));
}
Vector2F UtilityMath::VNormalize(const Vector2F& _vec)
{
	// ゼロ除算対策で元のゼロベクトルを返す
	if (UtilityMath::EqualsVZero(_vec)) { return _vec; }

	float mag = sqrtf((_vec.x * _vec.x) + (_vec.y * _vec.y));

	return Vector2F(_vec.x / mag, _vec.y / mag);
}

// 2つの3Dベクトルのなす角（度）を求める
double UtilityMath::AngleDeg(const VECTOR& from, const VECTOR& to)
{
	auto fLen = SqrMagnitude(from);
	auto tLen = SqrMagnitude(to);
	auto denominator = sqrt(fLen * tLen);
	if (denominator < kEpsilonNormalSqrt)
	{
		return 0.0f;
	}

	auto dot = VDot(from, to) / denominator;

	if (dot < -1.0f)
	{
		dot = -1.0f;
	}
	if (dot > 1.0f)
	{
		dot = 1.0f;
	}

	return acos(dot) * (180.0 / DX_PI);

}

// 指定方向に線と終点に球体を描画する
void UtilityMath::DrawLineDir(const VECTOR& pos, const VECTOR& dir, int color, float len)
{
	auto nDir = UtilityMath::VNormalize(dir);
	auto sPos = VAdd(pos, VScale(nDir, -len));
	auto ePos = VAdd(pos, VScale(nDir, len));

	DrawLine3D(sPos, ePos, color);
	DrawSphere3D(ePos, 5.0f, 5, color, color, true);
}

// 回転行列を基にXYZ軸方向の線を描画する
void UtilityMath::DrawLineXYZ(const VECTOR& pos, const MATRIX& rot, float len)
{
	VECTOR dir;

	// X
	dir = VTransform(UtilityMath::DIR_R, rot);
	DrawLineDir(pos, dir, 0xff0000, len);

	// Y
	dir = VTransform(UtilityMath::DIR_UP, rot);
	DrawLineDir(pos, dir, 0x00ff00, len);

	// Z
	dir = VTransform(UtilityMath::DIR_FORWARD, rot);
	DrawLineDir(pos, dir, 0x0000ff, len);
}

// クォータニオンを基にXYZ軸方向の線を描画する
void UtilityMath::DrawLineXYZ(const VECTOR& pos, const Quaternion& rot, float len)
{
	VECTOR dir;

	// X
	dir = rot.GetRight();
	DrawLineDir(pos, dir, 0xff0000, len);

	// Y
	dir = rot.GetUp();
	DrawLineDir(pos, dir, 0x00ff00, len);

	// Z
	dir = rot.GetForward();
	DrawLineDir(pos, dir, 0x0000ff, len);

	// W
	dir = VAdd(VNormalize(rot.GetRight()), VNormalize(rot.GetUp()));
	dir = VAdd(dir, VNormalize(rot.GetForward()));
	dir = VNormalize(dir);
	DrawLineDir(pos, dir, 0xffffff, len);
}

//bool UtilityMath::IsTimeOver(float& totalTime, const float& waitTime)
//{
//	//デルタタイム
//	auto delta = SceneManager::GetInstance().GetDeltaTime();
//	totalTime += delta;
//
//	//待機時間を超過しているか判断
//	if (totalTime >= waitTime)
//	{
//		return true;
//	}
//
//	return false;
//}

VECTOR UtilityMath::GetCirclePos(const VECTOR& center, float radius, float angle)
{
	return VGet(center.x + std::cosf(angle) * radius,
		center.y + std::sinf(angle) * radius, center.z);
}

float UtilityMath::RandRangeF(float min, float max)
{
	return min + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (max - min);
}

VECTOR UtilityMath::GetNearestPointOnSegment(const VECTOR& _startPos,
	const VECTOR& _endPos, const VECTOR& _targetPos)
{
	VECTOR segmentVec = VSub(_endPos, _startPos);
	VECTOR toTargetVec = VSub(_targetPos, _startPos);

	float lenSquare = static_cast<float>(SqrMagnitude(segmentVec));

	if (lenSquare < 1e-6)
	{
		return _startPos;
	}

	float segmentRatio = VDot(toTargetVec, segmentVec) / lenSquare;

	if (segmentRatio < 0.0f) { segmentRatio = 0.0f; }
	if (segmentRatio > 1.0f) { segmentRatio = 1.0f; }

	VECTOR nearestPos = VAdd(_startPos, VScale(segmentVec, segmentRatio));

	return nearestPos;
}