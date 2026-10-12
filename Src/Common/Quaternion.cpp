#include "../Pch.h"
#include "Quaternion.h"
#include"../Engine/Utility/UtilityMath.h"

Quaternion::Quaternion(void)
{
	w_ = 1;
	x_ = y_ = z_ = 0;
}

Quaternion::Quaternion(const VECTOR& rad)
{
	Quaternion q = Euler(rad.x, rad.y, rad.z);
	w_ = q.w_;
	x_ = q.x_;
	y_ = q.y_;
	z_ = q.z_;
}

Quaternion::Quaternion(double ww, double wx, double wy, double wz)
{
	w_ = ww;
	x_ = wx;
	y_ = wy;
	z_ = wz;
}

Quaternion Quaternion::Euler(const VECTOR& rad)
{
	return Euler(rad.x, rad.y, rad.z);
}

Quaternion Quaternion::Euler(double radX, double radY, double radZ)
{
	Quaternion ret = Quaternion();

	// 角度を0〜2π範囲に正規化
	radX = UtilityMath::RadIn2PI(radX);
	radY = UtilityMath::RadIn2PI(radY);
	radZ = UtilityMath::RadIn2PI(radZ);

	// オイラー角からクォータニオンへの変換式
	double cosZ = cos(radZ / 2.0f);
	double sinZ = sin(radZ / 2.0f);
	double cosX = cos(radX / 2.0f);
	double sinX = sin(radX / 2.0f);
	double cosY = cos(radY / 2.0f);
	double sinY = sin(radY / 2.0f);

	// ZYXの順で適用するクォータニオン計算
	ret.w_ = cosX * cosY * cosZ + sinX * sinY * sinZ;
	ret.x_ = sinX * cosY * cosZ + cosX * sinY * sinZ;
	ret.y_ = cosX * sinY * cosZ - sinX * cosY * sinZ;
	ret.z_ = cosX * cosY * sinZ - sinX * sinY * cosZ;

	return ret;
}

Quaternion Quaternion::Mult(const Quaternion& q1, const Quaternion& q2)
{
	Quaternion ret = Quaternion();
	double d1, d2, d3, d4;

	// wの計算 
	d1 = q1.w_ * q2.w_;
	d2 = -q1.x_ * q2.x_;
	d3 = -q1.y_ * q2.y_;
	d4 = -q1.z_ * q2.z_;
	ret.w_ = d1 + d2 + d3 + d4;

	// xの計算 
	d1 = q1.w_ * q2.x_;
	d2 = q2.w_ * q1.x_;
	d3 = q1.y_ * q2.z_;
	d4 = -q1.z_ * q2.y_;
	ret.x_ = d1 + d2 + d3 + d4;

	// yの計算
	d1 = q1.w_ * q2.y_;
	d2 = q2.w_ * q1.y_;
	d3 = q1.z_ * q2.x_;
	d4 = -q1.x_ * q2.z_;
	ret.y_ = d1 + d2 + d3 + d4;

	// zの計算
	d1 = q1.w_ * q2.z_;
	d2 = q2.w_ * q1.z_;
	d3 = q1.x_ * q2.y_;
	d4 = -q1.y_ * q2.x_;
	ret.z_ = d1 + d2 + d3 + d4;

	return ret;
}

Quaternion Quaternion::Mult(const Quaternion& q) const
{
	return Mult(Quaternion(w_, x_, y_, z_), q);
}

Quaternion Quaternion::AngleAxis(double rad, VECTOR axis)
{
	Quaternion ret = Quaternion();

	double norm;
	double c, s;

	// 単位クォータニオンで初期化（Unityの仕様に合わせる）
	ret.w_ = 1.0;
	ret.x_ = ret.y_ = ret.z_ = 0.0;

	// 軸ベクトルの正規化
	norm = (double)axis.x * (double)axis.x + (double)axis.y * (double)axis.y + (double)axis.z * (double)axis.z;
	if (norm <= 0.0f)
	{
		return ret; // 軸が無効な場合は単位クォータニオンを返す
	}

	norm = 1.0 / sqrt(norm);
	axis.x = (float)(axis.x * norm);
	axis.y = (float)(axis.y * norm);
	axis.z = (float)(axis.z * norm);

	// 角度の半分のcos, sinを計算
	c = cos(0.5f * rad);
	s = sin(0.5f * rad);

	// クォータニオン生成
	ret.w_ = c;
	ret.x_ = s * axis.x;
	ret.y_ = s * axis.y;
	ret.z_ = s * axis.z;

	return ret;
}

VECTOR Quaternion::PosAxis(const Quaternion& q, VECTOR pos)
{
	// 位置情報に回転情報を反映させる
	// p' = q * p * q^-1 (pを純四元数として扱う)
	Quaternion tmp = Quaternion();
	tmp = tmp.Mult(q);
	tmp = tmp.Mult(Quaternion(0.0f, pos.x, pos.y, pos.z));
	tmp = tmp.Mult(q.Inverse());

	return { (float)tmp.x_, (float)tmp.y_, (float)tmp.z_ };
}

VECTOR Quaternion::PosAxis(VECTOR pos) const
{
	return PosAxis(Quaternion(w_, x_, y_, z_), pos);
}

VECTOR Quaternion::ToEuler(const Quaternion& q)
{
	VECTOR ret;

	// クォータニオンから回転行列の要素を計算
	double r11 = 2 * (q.x_ * q.z_ + q.w_ * q.y_);
	double r12 = q.w_ * q.w_ - q.x_ * q.x_ - q.y_ * q.y_ + q.z_ * q.z_;
	double r21 = -2 * (q.y_ * q.z_ - q.w_ * q.x_);
	double r31 = 2 * (q.x_ * q.y_ + q.w_ * q.z_);
	double r32 = q.w_ * q.w_ - q.x_ * q.x_ + q.y_ * q.y_ - q.z_ * q.z_;

	// オイラー角（X, Y, Z）を計算
	ret.x = static_cast<float>(asin(r21));          // X軸回転（ピッチ）
	ret.y = static_cast<float>(atan2(r11, r12));    // Y軸回転（ヨー）
	ret.z = static_cast<float>(atan2(r31, r32));    // Z軸回転（ロール）

	return ret;
}

VECTOR Quaternion::ToEuler(void) const
{
	return ToEuler(Quaternion(w_, x_, y_, z_));
}

MATRIX Quaternion::ToMatrix(const Quaternion& q)
{
	MATRIX mat;

	FLOAT4 fq = { (float)q.x_, (float)q.y_, (float)q.z_, (float)q.w_ };

	// クォータニオンの成分から回転行列の要素を計算
	float sx = fq.x * fq.x * 2.0f;
	float sy = fq.y * fq.y * 2.0f;
	float sz = fq.z * fq.z * 2.0f;
	float cx = fq.y * fq.z * 2.0f;
	float cy = fq.x * fq.z * 2.0f;
	float cz = fq.x * fq.y * 2.0f;
	float wx = fq.w * fq.x * 2.0f;
	float wy = fq.w * fq.y * 2.0f;
	float wz = fq.w * fq.z * 2.0f;

	// 4x4回転行列を生成
	mat.m[0][0] = 1.0f - (sy + sz);	mat.m[0][1] = cz + wz;			mat.m[0][2] = cy - wy;			mat.m[0][3] = 0.0f;
	mat.m[1][0] = cz - wz;			mat.m[1][1] = 1.0f - (sx + sz);	mat.m[1][2] = cx + wx;			mat.m[1][3] = 0.0f;
	mat.m[2][0] = cy + wy;			mat.m[2][1] = cx - wx;			mat.m[2][2] = 1.0f - (sx + sy);	mat.m[2][3] = 0.0f;
	mat.m[3][0] = 0.0f;				mat.m[3][1] = 0.0f;				mat.m[3][2] = 0.0f;				mat.m[3][3] = 1.0f;

	return mat;
}

MATRIX Quaternion::ToMatrix(void) const
{
	return ToMatrix(Quaternion(w_, x_, y_, z_));
}

Quaternion Quaternion::LookRotation(VECTOR dir)
{
	VECTOR up = VGet(0.0f, 1.0f, 0.0f ); // デフォルトのアップベクトル（Y軸）
	return LookRotation(dir, up);
}

Quaternion Quaternion::LookRotation(VECTOR dir, VECTOR up)
{
	// 方向ベクトルを正規化
	dir = UtilityMath::VNormalize(dir);
	// 右方向ベクトルを計算し正規化（アップベクトルと方向ベクトルの外積）
	VECTOR right = UtilityMath::VNormalize(VCross(up, dir));
	// 正確なアップベクトルを再計算（直交性を保証）
	up = VCross(dir, right);

	// 回転行列の要素を計算
	auto m00 = right.x;
	auto m01 = right.y;
	auto m02 = right.z;
	auto m10 = up.x;
	auto m11 = up.y;
	auto m12 = up.z;
	auto m20 = dir.x;
	auto m21 = dir.y;
	auto m22 = dir.z;

	// 回転行列からクォータニオンへの変換（一般的なアルゴリズム）
	float num8 = (m00 + m11) + m22;
	auto quaternion = Quaternion();
	if (num8 > 0.0f)
	{
		// 対角和が正の場合
		double num = sqrt(num8 + 1.0);
		quaternion.w_ = num * 0.5;
		num = 0.5 / num;
		quaternion.x_ = ((double)m12 - m21) * num;
		quaternion.y_ = ((double)m20 - m02) * num;
		quaternion.z_ = ((double)m01 - m10) * num;
		return quaternion.Normalized();
	}
	if ((m00 >= m11) && (m00 >= m22))
	{
		// m00が最大の場合
		auto num7 = sqrt(((1.0f + m00) - m11) - m22);
		auto num4 = 0.5f / num7;
		quaternion.x_ = 0.5 * num7;
		quaternion.y_ = ((double)m01 + m10) * num4;
		quaternion.z_ = ((double)m02 + m20) * num4;
		quaternion.w_ = ((double)m12 - m21) * num4;
		return quaternion.Normalized();
	}
	if (m11 > m22)
	{
		// m11が最大の場合
		auto num6 = sqrt(((1.0f + m11) - m00) - m22);
		auto num3 = 0.5f / num6;
		quaternion.x_ = ((double)m10 + m01) * num3;
		quaternion.y_ = 0.5 * num6;
		quaternion.z_ = ((double)m21 + m12) * num3;
		quaternion.w_ = ((double)m20 - m02) * num3;
		return quaternion.Normalized();
	}

	// m22が最大の場合
	auto num5 = sqrt(((1.0f + m22) - m00) - m11);
	auto num2 = 0.5f / num5;
	quaternion.x_ = ((double)m20 + m02) * num2;
	quaternion.y_ = ((double)m21 + m12) * num2;
	quaternion.z_ = 0.5 * num5;
	quaternion.w_ = ((double)m01 - m10) * num2;
	return quaternion.Normalized();
}

Quaternion Quaternion::GetRotation(MATRIX mat)
{
	Quaternion ret;

	float s;
	// 行列の対角和+1を計算（クォータニオン変換の一般的なアルゴリズム）
	float tr = mat.m[0][0] + mat.m[1][1] + mat.m[2][2] + 1.0f;
	if (tr >= 1.0f)
	{
		// 対角和が大きい場合（一般的なケース）
		s = 0.5f / sqrtf(tr);
		ret.w_ = 0.25f / s;
		ret.x_ = (mat.m[1][2] - mat.m[2][1]) * s;
		ret.y_ = (mat.m[2][0] - mat.m[0][2]) * s;
		ret.z_ = (mat.m[0][1] - mat.m[1][0]) * s;
	}
	else
	{
		// 対角和が小さい場合は最大対角要素を基準に計算
		float max;
		max = mat.m[1][1] > mat.m[2][2] ? mat.m[1][1] : mat.m[2][2];

		if (max < mat.m[0][0])
		{
			// m00が最大の場合
			s = sqrtf(mat.m[0][0] - (mat.m[1][1] + mat.m[2][2]) + 1.0f);

			float x = s * 0.5f;
			s = 0.5f / s;
			ret.x_ = x;
			ret.y_ = (mat.m[0][1] + mat.m[1][0]) * s;
			ret.z_ = (mat.m[2][0] + mat.m[0][2]) * s;
			ret.w_ = (mat.m[1][2] - mat.m[2][1]) * s;
		}
		else if (max == mat.m[1][1])
		{
			// m11が最大の場合
			s = sqrtf(mat.m[1][1] - (mat.m[2][2] + mat.m[0][0]) + 1.0f);

			float y = s * 0.5f;
			s = 0.5f / s;
			ret.x_ = (mat.m[0][1] + mat.m[1][0]) * s;
			ret.y_ = y;
			ret.z_ = (mat.m[1][2] + mat.m[2][1]) * s;
			ret.w_ = (mat.m[2][0] - mat.m[0][2]) * s;
		}
		else
		{
			// m22が最大の場合
			s = sqrtf(mat.m[2][2] - (mat.m[0][0] + mat.m[1][1]) + 1.0f);

			float z = s * 0.5f;
			s = 0.5f / s;
			ret.x_ = (mat.m[2][0] + mat.m[0][2]) * s;
			ret.y_ = (mat.m[1][2] + mat.m[2][1]) * s;
			ret.z_ = z;
			ret.w_ = (mat.m[0][1] - mat.m[1][0]) * s;
		}
	}

	return ret;
}

VECTOR Quaternion::GetDir(VECTOR dir) const
{
	return PosAxis(dir);
}

VECTOR Quaternion::GetForward(void) const
{
	return GetDir(UtilityMath::DIR_FORWARD);
}

VECTOR Quaternion::GetBack(void) const
{
	return GetDir(UtilityMath::DIR_BACK);
}

VECTOR Quaternion::GetRight(void) const
{
	return GetDir(UtilityMath::DIR_R);
}

VECTOR Quaternion::GetLeft(void) const
{
	return GetDir(UtilityMath::DIR_L);
}

VECTOR Quaternion::GetUp(void) const
{
	return GetDir(UtilityMath::DIR_UP);
}

VECTOR Quaternion::GetDown(void) const
{
	return GetDir(UtilityMath::DIR_DOWN);
}

double Quaternion::Dot(const Quaternion& q1, const Quaternion& q2)
{
	return (q1.w_ * q2.w_ + q1.x_ * q2.x_ + q1.y_ * q2.y_ + q1.z_ * q2.z_);
}

double Quaternion::Dot(const Quaternion& q) const
{
	return (w_ * q.w_ + x_ * q.x_ + y_ * q.y_ + z_ * q.z_);
}

Quaternion Quaternion::Normalize(const Quaternion& q)
{
	float scale = 1.0f / static_cast<float>(q.Length());
	VECTOR v = VScale(q.xyz(), scale);
	Quaternion ret = Quaternion(q.w_ * scale, v.x, v.y, v.z);
	return ret;
}

Quaternion Quaternion::Normalized(void) const
{
	double mag = sqrt(w_ * w_ + x_ * x_ + y_ * y_ + z_ * z_);
	return Quaternion(w_ / mag, x_ / mag, y_ / mag, z_ / mag);
}

void Quaternion::Normalize(void)
{
	double mag = sqrt(w_ * w_ + x_ * x_ + y_ * y_ + z_ * z_);

	w_ /= mag;
	x_ /= mag;
	y_ /= mag;
	z_ /= mag;
}

Quaternion Quaternion::Inverse(void) const
{
	double n = 1.0f / (w_ * w_ + x_ * x_ + y_ * y_ + z_ * z_);
	Quaternion tmp = Quaternion(w_, -x_, -y_, -z_);  // 共役クォータニオン
	return Quaternion(tmp.w_ * n, tmp.x_ * n, tmp.y_ * n, tmp.z_ * n);  // 共役/長さの二乗
}

Quaternion Quaternion::Slerp(Quaternion from, Quaternion to, double t)
{
	if (t > 1) t = 1;
	if (t < 0) t = 0;
	return SlerpUnclamped(from, to, (float)t);
}

inline float SIGN(float x)
{
	return (x >= 0.0f) ? +1.0f : -1.0f;
}

inline float NORM(float a, float b, float c, float d)
{
	return sqrt(a * a + b * b + c * c + d * d);
}

Quaternion Quaternion::FromToRotation(VECTOR fromDir, VECTOR toDir)
{
	// 回転軸を計算（fromDirとtoDirの外積）
	VECTOR axis = VCross(fromDir, toDir);
	double angle = UtilityMath::AngleDeg(fromDir, toDir);

	// 180度近くの回転の場合、別の方法で軸を決定
	if (angle >= 179.9196)
	{
		auto r = VCross(fromDir, UtilityMath::DIR_R);
		axis = VCross(r, fromDir);
		float len = axis.x * axis.x + axis.y * axis.y + axis.z * axis.z;
		if (len < 0.000001f)
		{
			axis = UtilityMath::DIR_UP; // 軸が小さすぎる場合はY軸を使用
		}
	}

	// 軸を正規化し、angle-axis表現からクォータニオンを生成
	axis = UtilityMath::VNormalize(axis);
	return Quaternion::AngleAxis(UtilityMath::Deg2RadD(angle), axis);
}

Quaternion Quaternion::RotateTowards(const Quaternion& from, const Quaternion& to, float maxDegreesDelta)
{
	double num = Quaternion::Angle(from, to);
	if (num == 0.0)
	{
		return to; // 角度が0の場合はtoをそのまま返す
	}

	// 補間比率tを計算（最大角度で制限）
	float t = std::min(1.0f, maxDegreesDelta / (float)num);
	return Quaternion::SlerpUnclamped(from, to, t);
}

double Quaternion::Angle(const Quaternion& q1, const Quaternion& q2)
{
	double cos = Quaternion::Dot(q1, q2);
	double ac = acos(cos);
	return ac * (180.0 / DX_PI); // ラジアンから度に変換
}

Quaternion Quaternion::SlerpUnclamped(Quaternion a, Quaternion b, float t)
{
	// いずれかの入力が0の場合はもう一方を返す
	if (a.LengthSquared() == 0.0f)
	{
		if (b.LengthSquared() == 0.0f)
		{
			return Identity();
		}
		return b;
	}
	else if (b.LengthSquared() == 0.0f)
	{
		return a;
	}

	// 内積から回転の角度を判断
	float cosHalfAngle = (float)(a.w_ * b.w_) + VDot(a.xyz(), b.xyz());

	if (cosHalfAngle >= 1.0f || cosHalfAngle <= -1.0f)
	{
		// 角度が0の場合は入力のひとつを返す
		return a;
	}
	else if (cosHalfAngle < 0.0f)
	{
		// 内積が負の場合、-bを使用して最短経路で補間
		b.x_ = b.x_ * -1.0f;
		b.y_ = b.y_ * -1.0f;
		b.z_ = b.z_ * -1.0f;
		b.w_ = -b.w_;
		cosHalfAngle = -cosHalfAngle;
	}

	float blendA;
	float blendB;
	if (cosHalfAngle < 0.99f)
	{
		// 大きな角度の場合は真のSlerp
		float halfAngle = acosf(cosHalfAngle);
		float sinHalfAngle = sinf(halfAngle);
		float oneOverSinHalfAngle = 1.0f / sinHalfAngle;
		blendA = sinf(halfAngle * (1.0f - t)) * oneOverSinHalfAngle;
		blendB = sinf(halfAngle * t) * oneOverSinHalfAngle;
	}
	else
	{
		// 角度が小さい場合は線形補間（Lerp）
		blendA = 1.0f - t;
		blendB = t;
	}

	// 重み付け合成
	VECTOR v = VAdd(VScale(a.xyz(), blendA), VScale(b.xyz(), blendB));
	Quaternion result = Quaternion(blendA * a.w_ + blendB * b.w_, v.x, v.y, v.z);

	// 正規化して返す
	if (result.LengthSquared() > 0.0f)
	{
		return Normalize(result);
	}
	else
	{
		return Identity();
	}
}

Quaternion Quaternion::Identity(void)
{
	return Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
}

double Quaternion::Length(void) const
{
	return sqrt(x_ * x_ + y_ * y_ + z_ * z_ + w_ * w_);
}

double Quaternion::LengthSquared(void) const
{
	return x_ * x_ + y_ * y_ + z_ * z_ + w_ * w_;
}

VECTOR Quaternion::xyz(void) const
{
	return { (float)x_, (float)y_, (float)z_ };
}

void Quaternion::ToAngleAxis(float* angle, VECTOR* axis)
{
	// クォータニオンの正規化
	if (abs(this->w_) > 1.0f)
	{
		this->Normalize();
	}

	// 回転角を計算
	*angle = 2.0f * acosf((float)this->w_); // angle

	// 回転角が極めて小さい場合は0とする
	if (x_ == 0 && y_ == 0 && z_ == 0)
	{
		*angle = 0.0f;
	}

	// 回転軸を計算
	float den = sqrtf(1.0f - (float)(this->w_ * this->w_));
	if (den > 0.0001f)
	{
		// 軸を正規化
		auto v = this->xyz();
		axis->x = v.x / den;
		axis->y = v.y / den;
		axis->z = v.z / den;
	}
	else
	{
		// 角度が0の場合は任意の軸を設定
		// （この場合、回転がないため軸は意味を持たない）
		*axis = { 1.0f, 0.0f, 0.0f };
	}
}

Quaternion Quaternion::operator*(float& f)
{
	return Quaternion(w_ * f, x_ * f, y_ * f, z_ * f);
}

const Quaternion Quaternion::operator*(const float& f)
{
	return Quaternion(w_ * f, x_ * f, y_ * f, z_ * f);
}

Quaternion Quaternion::operator+(Quaternion& rhs)
{
	return Quaternion(w_ + rhs.w_, x_ + rhs.x_, y_ + rhs.y_, z_ + rhs.z_);
}

const Quaternion Quaternion::operator+(const Quaternion& rhs)
{
	return Quaternion(w_ + rhs.w_, x_ + rhs.x_, y_ + rhs.y_, z_ + rhs.z_);
}