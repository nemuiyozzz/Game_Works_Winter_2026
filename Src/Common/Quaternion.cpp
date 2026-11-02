#include "../Pch.h"
#include "Quaternion.h"

#include "../Engine/Utility/UtilityMath.h"

namespace
{
    // 数値計算用定数関連
    constexpr double VALUE_ZERO_D = 0.0;        // 浮動小数点数（double型）のゼロ
    constexpr double VALUE_ONE_D = 1.0;        // 浮動小数点数（double型）の1.0
    constexpr double VALUE_HALF_D = 0.5;        // 浮動小数点数（double型）の0.5（半分）
    constexpr double VALUE_TWO_D = 2.0;        // 浮動小数点数（double型）の2.0
    constexpr float  VALUE_ZERO_F = 0.0f;       // 浮動小数点数（float型）のゼロ
    constexpr float  VALUE_ONE_F = 1.0f;       // 浮動小数点数（float型）の1.0
    constexpr float  VALUE_TWO_F = 2.0f;       // 浮動小数点数（float型）の2.0
    constexpr float  VALUE_QUARTER_F = 0.25f;   // 浮動小数点数（float型）の0.25（4分の1）

    // 角度および比較用定数関連
    constexpr double ANGLE_LIMIT_DEGREE = 179.9196; // 180度近傍の判定を行うための限界角度（度数法）
    constexpr float  LENGTH_EPSILON = 0.000001f;    // ベクトルの長さなどの極小値（ゼロ除算防止用）
    constexpr float  DENOMINATOR_EPSILON = 0.0001f; // 分母の極小値（ゼロ除算防止用）
    constexpr float  SLERP_THRESHOLD = 0.99f;       // 球面線形補間と線形補間を切り替える閾値

    // 行列のインデックス定数関連
    constexpr int MATRIX_INDEX_0 = 0; // 行列の0番目（X成分関連）
    constexpr int MATRIX_INDEX_1 = 1; // 行列の1番目（Y成分関連）
    constexpr int MATRIX_INDEX_2 = 2; // 行列の2番目（Z成分関連）
    constexpr int MATRIX_INDEX_3 = 3; // 行列の3番目（W成分・平行移動関連）
}

Quaternion::Quaternion(void)
    : w_(VALUE_ONE_D)
    , x_(VALUE_ZERO_D)
    , y_(VALUE_ZERO_D)
    , z_(VALUE_ZERO_D)
{
}

Quaternion::Quaternion(const VECTOR& _radian)
{
    Quaternion quaternion = CreateEuler(_radian.x, _radian.y, _radian.z);
    w_ = quaternion.w_;
    x_ = quaternion.x_;
    y_ = quaternion.y_;
    z_ = quaternion.z_;
}

Quaternion::Quaternion(double _scalarW, double _vectorX, double _vectorY, double _vectorZ)
    : w_(_scalarW)
    , x_(_vectorX)
    , y_(_vectorY)
    , z_(_vectorZ)
{
}

Quaternion Quaternion::CreateEuler(const VECTOR& _radian)
{
    return CreateEuler(_radian.x, _radian.y, _radian.z);
}

Quaternion Quaternion::CreateEuler(double _radianX, double _radianY, double _radianZ)
{
    Quaternion result = Quaternion();

    // 角度を0〜2π範囲に正規化
    _radianX = UtilityMath::RadIn2PI(_radianX);
    _radianY = UtilityMath::RadIn2PI(_radianY);
    _radianZ = UtilityMath::RadIn2PI(_radianZ);

    // オイラー角からクォータニオンへの変換式
    double cosineZ = cos(_radianZ / VALUE_TWO_D);
    double sineZ = sin(_radianZ / VALUE_TWO_D);
    double cosineX = cos(_radianX / VALUE_TWO_D);
    double sineX = sin(_radianX / VALUE_TWO_D);
    double cosineY = cos(_radianY / VALUE_TWO_D);
    double sineY = sin(_radianY / VALUE_TWO_D);

    // ZYXの順で適用するクォータニオン計算
    result.w_ = cosineX * cosineY * cosineZ + sineX * sineY * sineZ;
    result.x_ = sineX * cosineY * cosineZ + cosineX * sineY * sineZ;
    result.y_ = cosineX * sineY * cosineZ - sineX * cosineY * sineZ;
    result.z_ = cosineX * cosineY * sineZ - sineX * sineY * cosineZ;

    return result;
}

Quaternion Quaternion::Multiply(const Quaternion& _quaternion1, const Quaternion& _quaternion2)
{
    Quaternion result = Quaternion();
    double value1, value2, value3, value4;

    // wの計算 
    value1 = _quaternion1.w_ * _quaternion2.w_;
    value2 = -_quaternion1.x_ * _quaternion2.x_;
    value3 = -_quaternion1.y_ * _quaternion2.y_;
    value4 = -_quaternion1.z_ * _quaternion2.z_;
    result.w_ = value1 + value2 + value3 + value4;

    // xの計算 
    value1 = _quaternion1.w_ * _quaternion2.x_;
    value2 = _quaternion2.w_ * _quaternion1.x_;
    value3 = _quaternion1.y_ * _quaternion2.z_;
    value4 = -_quaternion1.z_ * _quaternion2.y_;
    result.x_ = value1 + value2 + value3 + value4;

    // yの計算
    value1 = _quaternion1.w_ * _quaternion2.y_;
    value2 = _quaternion2.w_ * _quaternion1.y_;
    value3 = _quaternion1.z_ * _quaternion2.x_;
    value4 = -_quaternion1.x_ * _quaternion2.z_;
    result.y_ = value1 + value2 + value3 + value4;

    // zの計算
    value1 = _quaternion1.w_ * _quaternion2.z_;
    value2 = _quaternion2.w_ * _quaternion1.z_;
    value3 = _quaternion1.x_ * _quaternion2.y_;
    value4 = -_quaternion1.y_ * _quaternion2.x_;
    result.z_ = value1 + value2 + value3 + value4;

    return result;
}

Quaternion Quaternion::Multiply(const Quaternion& _quaternion) const
{
    return Multiply(Quaternion(w_, x_, y_, z_), _quaternion);
}

Quaternion Quaternion::CreateAngleAxis(double _radian, VECTOR _axis)
{
    Quaternion result = Quaternion();

    // 単位クォータニオンで初期化
    result.w_ = VALUE_ONE_D;
    result.x_ = result.y_ = result.z_ = VALUE_ZERO_D;

    double squaredNorm = UtilityMath::SqrMagnitude(_axis);

    if (squaredNorm <= VALUE_ZERO_D)
    {
        return result; // 軸が無効な場合は単位クォータニオンを返す
    }

    _axis = UtilityMath::VNormalize(_axis);

    // 角度の半分のcos, sinを計算
    double cosineValue = cos(VALUE_HALF_D * _radian);
    double sineValue = sin(VALUE_HALF_D * _radian);

    // クォータニオン生成
    result.w_ = cosineValue;
    result.x_ = sineValue * _axis.x;
    result.y_ = sineValue * _axis.y;
    result.z_ = sineValue * _axis.z;

    return result;
}

VECTOR Quaternion::RotatePosition(const Quaternion& _quaternion, VECTOR _position)
{
    // 位置情報に回転情報を反映させる
    Quaternion temporary = Quaternion();

    temporary = temporary.Multiply(_quaternion);
    temporary = temporary.Multiply(Quaternion(VALUE_ZERO_F, _position.x,
        _position.y, _position.z));
    temporary = temporary.Multiply(_quaternion.CalculateInverse());

    return { static_cast<float>(temporary.x_),
             static_cast<float>(temporary.y_),
             static_cast<float>(temporary.z_) };
}

VECTOR Quaternion::RotatePosition(VECTOR _position) const
{
    return RotatePosition(Quaternion(w_, x_, y_, z_), _position);
}

VECTOR Quaternion::ConvertToEuler(const Quaternion& _quaternion)
{
    VECTOR result;

    // クォータニオンから回転行列の要素を計算
    double r11 = VALUE_TWO_D * (_quaternion.x_ * _quaternion.z_ + _quaternion.w_ * _quaternion.y_);
    double r12 = _quaternion.w_ * _quaternion.w_ - _quaternion.x_ * _quaternion.x_ -
        _quaternion.y_ * _quaternion.y_ + _quaternion.z_ * _quaternion.z_;
    double r21 = -VALUE_TWO_D * (_quaternion.y_ * _quaternion.z_ - _quaternion.w_ * _quaternion.x_);
    double r31 = VALUE_TWO_D * (_quaternion.x_ * _quaternion.y_ + _quaternion.w_ * _quaternion.z_);
    double r32 = _quaternion.w_ * _quaternion.w_ - _quaternion.x_ * _quaternion.x_ +
        _quaternion.y_ * _quaternion.y_ - _quaternion.z_ * _quaternion.z_;

    // オイラー角（X, Y, Z）を計算
    result.x = static_cast<float>(asin(r21));       // X軸回転（ピッチ）
    result.y = static_cast<float>(atan2(r11, r12)); // Y軸回転（ヨー）
    result.z = static_cast<float>(atan2(r31, r32)); // Z軸回転（ロール）

    return result;
}

VECTOR Quaternion::ConvertToEuler(void) const
{
    return ConvertToEuler(Quaternion(w_, x_, y_, z_));
}

MATRIX Quaternion::ConvertToMatrix(const Quaternion& _quaternion)
{
    MATRIX matrix;

    FLOAT4 floatQuaternion = {
        static_cast<float>(_quaternion.x_),
        static_cast<float>(_quaternion.y_),
        static_cast<float>(_quaternion.z_),
        static_cast<float>(_quaternion.w_)
    };

    // クォータニオンの成分から回転行列の要素を計算
    float scaleX = floatQuaternion.x * floatQuaternion.x * VALUE_TWO_F;
    float scaleY = floatQuaternion.y * floatQuaternion.y * VALUE_TWO_F;
    float scaleZ = floatQuaternion.z * floatQuaternion.z * VALUE_TWO_F;
    float crossX = floatQuaternion.y * floatQuaternion.z * VALUE_TWO_F;
    float crossY = floatQuaternion.x * floatQuaternion.z * VALUE_TWO_F;
    float crossZ = floatQuaternion.x * floatQuaternion.y * VALUE_TWO_F;
    float weightX = floatQuaternion.w * floatQuaternion.x * VALUE_TWO_F;
    float weightY = floatQuaternion.w * floatQuaternion.y * VALUE_TWO_F;
    float weightZ = floatQuaternion.w * floatQuaternion.z * VALUE_TWO_F;

    // 4x4回転行列を生成
    matrix.m[MATRIX_INDEX_0][MATRIX_INDEX_0] = VALUE_ONE_F - (scaleY + scaleZ);
    matrix.m[MATRIX_INDEX_0][MATRIX_INDEX_1] = crossZ + weightZ;
    matrix.m[MATRIX_INDEX_0][MATRIX_INDEX_2] = crossY - weightY;
    matrix.m[MATRIX_INDEX_0][MATRIX_INDEX_3] = VALUE_ZERO_F;

    matrix.m[MATRIX_INDEX_1][MATRIX_INDEX_0] = crossZ - weightZ;
    matrix.m[MATRIX_INDEX_1][MATRIX_INDEX_1] = VALUE_ONE_F - (scaleX + scaleZ);
    matrix.m[MATRIX_INDEX_1][MATRIX_INDEX_2] = crossX + weightX;
    matrix.m[MATRIX_INDEX_1][MATRIX_INDEX_3] = VALUE_ZERO_F;

    matrix.m[MATRIX_INDEX_2][MATRIX_INDEX_0] = crossY + weightY;
    matrix.m[MATRIX_INDEX_2][MATRIX_INDEX_1] = crossX - weightX;
    matrix.m[MATRIX_INDEX_2][MATRIX_INDEX_2] = VALUE_ONE_F - (scaleX + scaleY);
    matrix.m[MATRIX_INDEX_2][MATRIX_INDEX_3] = VALUE_ZERO_F;

    matrix.m[MATRIX_INDEX_3][MATRIX_INDEX_0] = VALUE_ZERO_F;
    matrix.m[MATRIX_INDEX_3][MATRIX_INDEX_1] = VALUE_ZERO_F;
    matrix.m[MATRIX_INDEX_3][MATRIX_INDEX_2] = VALUE_ZERO_F;
    matrix.m[MATRIX_INDEX_3][MATRIX_INDEX_3] = VALUE_ONE_F;

    return matrix;
}

MATRIX Quaternion::ConvertToMatrix(void) const
{
    return ConvertToMatrix(Quaternion(w_, x_, y_, z_));
}

Quaternion Quaternion::CreateLookRotation(VECTOR _direction)
{
    return CreateLookRotation(_direction, UtilityMath::DIR_UP);
}

Quaternion Quaternion::CreateLookRotation(VECTOR _direction, VECTOR _upDirection)
{
    // 方向ベクトルを正規化
    _direction = UtilityMath::VNormalize(_direction);

    // 右方向ベクトルを計算し正規化（アップベクトルと方向ベクトルの外積）
    VECTOR rightDirection = UtilityMath::VNormalize(VCross(_upDirection, _direction));

    // 正確なアップベクトルを再計算（直交性を保証）
    _upDirection = VCross(_direction, rightDirection);

    // 回転行列の要素を計算
    float matrix00 = rightDirection.x;
    float matrix01 = rightDirection.y;
    float matrix02 = rightDirection.z;
    float matrix10 = _upDirection.x;
    float matrix11 = _upDirection.y;
    float matrix12 = _upDirection.z;
    float matrix20 = _direction.x;
    float matrix21 = _direction.y;
    float matrix22 = _direction.z;

    // 回転行列からクォータニオンへの変換
    float traceValue = (matrix00 + matrix11) + matrix22;
    Quaternion resultQuaternion = Quaternion();

    if (traceValue > VALUE_ZERO_F)
    {
        // 対角和が正の場合
        double scaleValue = sqrt(traceValue + VALUE_ONE_D);

        resultQuaternion.w_ = scaleValue * VALUE_HALF_D;
        scaleValue = VALUE_HALF_D / scaleValue;
        resultQuaternion.x_ = (static_cast<double>(matrix12) - matrix21) * scaleValue;
        resultQuaternion.y_ = (static_cast<double>(matrix20) - matrix02) * scaleValue;
        resultQuaternion.z_ = (static_cast<double>(matrix01) - matrix10) * scaleValue;
        return resultQuaternion.GetNormalized();
    }
    if ((matrix00 >= matrix11) && (matrix00 >= matrix22))
    {
        // m00が最大の場合
        double scaleX = sqrt(((VALUE_ONE_F + matrix00) - matrix11) - matrix22);
        double scaleFactor = UtilityMath::HALF_NUM / scaleX;

        resultQuaternion.x_ = VALUE_HALF_D * scaleX;
        resultQuaternion.y_ = (static_cast<double>(matrix01) + matrix10) * scaleFactor;
        resultQuaternion.z_ = (static_cast<double>(matrix02) + matrix20) * scaleFactor;
        resultQuaternion.w_ = (static_cast<double>(matrix12) - matrix21) * scaleFactor;
        return resultQuaternion.GetNormalized();
    }
    if (matrix11 > matrix22)
    {
        // m11が最大の場合
        double scaleY = sqrt(((VALUE_ONE_F + matrix11) - matrix00) - matrix22);
        double scaleFactor = UtilityMath::HALF_NUM / scaleY;

        resultQuaternion.x_ = (static_cast<double>(matrix10) + matrix01) * scaleFactor;
        resultQuaternion.y_ = VALUE_HALF_D * scaleY;
        resultQuaternion.z_ = (static_cast<double>(matrix21) + matrix12) * scaleFactor;
        resultQuaternion.w_ = (static_cast<double>(matrix20) - matrix02) * scaleFactor;
        return resultQuaternion.GetNormalized();
    }

    // m22が最大の場合
    double scaleZ = sqrt(((VALUE_ONE_F + matrix22) - matrix00) - matrix11);
    double scaleFactor = UtilityMath::HALF_NUM / scaleZ;

    resultQuaternion.x_ = (static_cast<double>(matrix20) + matrix02) * scaleFactor;
    resultQuaternion.y_ = (static_cast<double>(matrix21) + matrix12) * scaleFactor;
    resultQuaternion.z_ = VALUE_HALF_D * scaleZ;
    resultQuaternion.w_ = (static_cast<double>(matrix01) - matrix10) * scaleFactor;

    return resultQuaternion.GetNormalized();
}

Quaternion Quaternion::GetRotation(MATRIX _matrix)
{
    Quaternion result;

    float scaleValue;

    // 行列の対角和+1を計算
    float trace = _matrix.m[MATRIX_INDEX_0][MATRIX_INDEX_0] +
        _matrix.m[MATRIX_INDEX_1][MATRIX_INDEX_1] +
        _matrix.m[MATRIX_INDEX_2][MATRIX_INDEX_2] + VALUE_ONE_F;

    if (trace >= VALUE_ONE_F)
    {
        // 対角和が大きい場合
        scaleValue = UtilityMath::HALF_NUM / sqrtf(trace);
        result.w_ = VALUE_QUARTER_F / scaleValue;
        result.x_ = (_matrix.m[MATRIX_INDEX_1][MATRIX_INDEX_2]
            - _matrix.m[MATRIX_INDEX_2][MATRIX_INDEX_1]) * scaleValue;
        result.y_ = (_matrix.m[MATRIX_INDEX_2][MATRIX_INDEX_0]
            - _matrix.m[MATRIX_INDEX_0][MATRIX_INDEX_2]) * scaleValue;
        result.z_ = (_matrix.m[MATRIX_INDEX_0][MATRIX_INDEX_1]
            - _matrix.m[MATRIX_INDEX_1][MATRIX_INDEX_0]) * scaleValue;
    }
    else
    {
        // 対角和が小さい場合は最大対角要素を基準に計算
        float maxValue = _matrix.m[MATRIX_INDEX_1][MATRIX_INDEX_1] >
            _matrix.m[MATRIX_INDEX_2][MATRIX_INDEX_2] ? _matrix.m[MATRIX_INDEX_1][MATRIX_INDEX_1]
            : _matrix.m[MATRIX_INDEX_2][MATRIX_INDEX_2];

        if (maxValue < _matrix.m[MATRIX_INDEX_0][MATRIX_INDEX_0])
        {
            // m00が最大の場合
            scaleValue = sqrtf(_matrix.m[MATRIX_INDEX_0][MATRIX_INDEX_0] -
                (_matrix.m[MATRIX_INDEX_1][MATRIX_INDEX_1]
                    + _matrix.m[MATRIX_INDEX_2][MATRIX_INDEX_2]) + VALUE_ONE_F);

            float calculateX = scaleValue * UtilityMath::HALF_NUM;

            scaleValue = UtilityMath::HALF_NUM / scaleValue;
            result.x_ = calculateX;
            result.y_ = (_matrix.m[MATRIX_INDEX_0][MATRIX_INDEX_1]
                + _matrix.m[MATRIX_INDEX_1][MATRIX_INDEX_0]) * scaleValue;
            result.z_ = (_matrix.m[MATRIX_INDEX_2][MATRIX_INDEX_0]
                + _matrix.m[MATRIX_INDEX_0][MATRIX_INDEX_2]) * scaleValue;
            result.w_ = (_matrix.m[MATRIX_INDEX_1][MATRIX_INDEX_2]
                - _matrix.m[MATRIX_INDEX_2][MATRIX_INDEX_1]) * scaleValue;
        }
        else if (maxValue == _matrix.m[MATRIX_INDEX_1][MATRIX_INDEX_1])
        {
            // m11が最大の場合
            scaleValue = sqrtf(_matrix.m[MATRIX_INDEX_1][MATRIX_INDEX_1] -
                (_matrix.m[MATRIX_INDEX_2][MATRIX_INDEX_2]
                    + _matrix.m[MATRIX_INDEX_0][MATRIX_INDEX_0]) + VALUE_ONE_F);

            float calculateY = scaleValue * UtilityMath::HALF_NUM;

            scaleValue = UtilityMath::HALF_NUM / scaleValue;
            result.x_ = (_matrix.m[MATRIX_INDEX_0][MATRIX_INDEX_1]
                + _matrix.m[MATRIX_INDEX_1][MATRIX_INDEX_0]) * scaleValue;
            result.y_ = calculateY;
            result.z_ = (_matrix.m[MATRIX_INDEX_1][MATRIX_INDEX_2]
                + _matrix.m[MATRIX_INDEX_2][MATRIX_INDEX_1]) * scaleValue;
            result.w_ = (_matrix.m[MATRIX_INDEX_2][MATRIX_INDEX_0]
                - _matrix.m[MATRIX_INDEX_0][MATRIX_INDEX_2]) * scaleValue;
        }
        else
        {
            // m22が最大の場合
            scaleValue = sqrtf(_matrix.m[MATRIX_INDEX_2][MATRIX_INDEX_2] -
                (_matrix.m[MATRIX_INDEX_0][MATRIX_INDEX_0]
                    + _matrix.m[MATRIX_INDEX_1][MATRIX_INDEX_1]) + VALUE_ONE_F);

            float calculateZ = scaleValue * UtilityMath::HALF_NUM;

            scaleValue = UtilityMath::HALF_NUM / scaleValue;
            result.x_ = (_matrix.m[MATRIX_INDEX_2][MATRIX_INDEX_0]
                + _matrix.m[MATRIX_INDEX_0][MATRIX_INDEX_2]) * scaleValue;
            result.y_ = (_matrix.m[MATRIX_INDEX_1][MATRIX_INDEX_2]
                + _matrix.m[MATRIX_INDEX_2][MATRIX_INDEX_1]) * scaleValue;
            result.z_ = calculateZ;
            result.w_ = (_matrix.m[MATRIX_INDEX_0][MATRIX_INDEX_1]
                - _matrix.m[MATRIX_INDEX_1][MATRIX_INDEX_0]) * scaleValue;
        }
    }

    return result;
}

VECTOR Quaternion::GetDirection(VECTOR _direction) const
{
    return RotatePosition(_direction);
}

VECTOR Quaternion::GetForward(void) const
{
    return GetDirection(UtilityMath::DIR_FORWARD);
}

VECTOR Quaternion::GetBack(void) const
{
    return GetDirection(UtilityMath::DIR_BACK);
}

VECTOR Quaternion::GetRight(void) const
{
    return GetDirection(UtilityMath::DIR_R);
}

VECTOR Quaternion::GetLeft(void) const
{
    return GetDirection(UtilityMath::DIR_L);
}

VECTOR Quaternion::GetUp(void) const
{
    return GetDirection(UtilityMath::DIR_UP);
}

VECTOR Quaternion::GetDown(void) const
{
    return GetDirection(UtilityMath::DIR_DOWN);
}

double Quaternion::CalculateDot(const Quaternion& _quaternion1, const Quaternion& _quaternion2)
{
    return (_quaternion1.w_ * _quaternion2.w_ + _quaternion1.x_ * _quaternion2.x_
        + _quaternion1.y_ * _quaternion2.y_ + _quaternion1.z_ * _quaternion2.z_);
}

double Quaternion::CalculateDot(const Quaternion& _target) const
{
    return (w_ * _target.w_ + x_ * _target.x_ + y_ * _target.y_ + z_ * _target.z_);
}

Quaternion Quaternion::Normalize(const Quaternion& _quaternion)
{
    float scaleFactor = VALUE_ONE_F / static_cast<float>(_quaternion.CalculateLength());
    VECTOR scaledVector = VScale(_quaternion.GetXYZ(), scaleFactor);
    Quaternion result = Quaternion(_quaternion.w_ * scaleFactor, scaledVector.x,
        scaledVector.y, scaledVector.z);

    return result;
}

Quaternion Quaternion::GetNormalized(void) const
{
    double magnitude = CalculateLength();
    return Quaternion(w_ / magnitude, x_ / magnitude, y_ / magnitude, z_ / magnitude);
}

void Quaternion::Normalize(void)
{
    double magnitude = CalculateLength();

    w_ /= magnitude;
    x_ /= magnitude;
    y_ /= magnitude;
    z_ /= magnitude;
}

Quaternion Quaternion::CalculateInverse(void) const
{
    double squaredNorm = VALUE_ONE_D / CalculateLengthSquared();
    Quaternion conjugate = Quaternion(w_, -x_, -y_, -z_);

    return Quaternion(conjugate.w_ * squaredNorm, conjugate.x_ * squaredNorm,
        conjugate.y_ * squaredNorm, conjugate.z_ * squaredNorm);
}

Quaternion Quaternion::CalculateSlerp(Quaternion _fromQuaternion,
    Quaternion _toQuaternion, double _ratio)
{
    if (_ratio > VALUE_ONE_D)
    {
        _ratio = VALUE_ONE_D;
    }
    if (_ratio < VALUE_ZERO_D)
    {
        _ratio = VALUE_ZERO_D;
    }

    return CalculateSlerpUnclamped(_fromQuaternion, _toQuaternion, static_cast<float>(_ratio));
}

Quaternion Quaternion::CreateFromToRotation(VECTOR _fromDirection, VECTOR _toDirection)
{
    // 回転軸を計算（_fromDirectionと_toDirectionの外積）
    VECTOR axisDirection = VCross(_fromDirection, _toDirection);
    double angleDegree = UtilityMath::AngleDeg(_fromDirection, _toDirection);

    // 180度近くの回転の場合、別の方法で軸を決定
    if (angleDegree >= ANGLE_LIMIT_DEGREE)
    {
        VECTOR rightDirection = VCross(_fromDirection, UtilityMath::DIR_R);
        axisDirection = VCross(rightDirection, _fromDirection);

        float axisLengthSquared = UtilityMath::SqrMagnitudeF(axisDirection);

        if (axisLengthSquared < LENGTH_EPSILON)
        {
            axisDirection = UtilityMath::DIR_UP;
        }
    }

    // 軸を正規化し、angle-axis表現からクォータニオンを生成
    axisDirection = UtilityMath::VNormalize(axisDirection);

    return Quaternion::CreateAngleAxis(UtilityMath::Deg2RadD(angleDegree), axisDirection);
}

Quaternion Quaternion::RotateTowards(const Quaternion& _fromQuaternion,
    const Quaternion& _toQuaternion, float _maxDegreesDelta)
{
    double calculatedAngle = Quaternion::CalculateAngle(_fromQuaternion, _toQuaternion);

    if (calculatedAngle == VALUE_ZERO_D)
    {
        // 角度が0の場合は終了目標をそのまま返す
        return _toQuaternion;
    }

    // 補間比率を計算（最大角度で制限）
    float interpolationRatio = std::min(VALUE_ONE_F, _maxDegreesDelta
        / static_cast<float>(calculatedAngle));

    return Quaternion::CalculateSlerpUnclamped(_fromQuaternion, _toQuaternion, interpolationRatio);
}

double Quaternion::CalculateAngle(const Quaternion& _quaternion1, const Quaternion& _quaternion2)
{
    double cosineValue = Quaternion::CalculateDot(_quaternion1, _quaternion2);
    double arcCosineValue = acos(cosineValue);

    return UtilityMath::Rad2DegD(arcCosineValue);
}

Quaternion Quaternion::CalculateSlerpUnclamped(Quaternion _startQuaternion, Quaternion _endQuaternion, float _ratio)
{
    // いずれかの入力が0の場合はもう一方を返す
    if (_startQuaternion.CalculateLengthSquared() == VALUE_ZERO_D)
    {
        if (_endQuaternion.CalculateLengthSquared() == VALUE_ZERO_D)
        {
            return GetIdentity();
        }
        return _endQuaternion;
    }
    else if (_endQuaternion.CalculateLengthSquared() == VALUE_ZERO_D)
    {
        return _startQuaternion;
    }

    // 内積から回転の角度を判断
    float cosineHalfAngle = static_cast<float>(_startQuaternion.w_ * _endQuaternion.w_)
        + VDot(_startQuaternion.GetXYZ(), _endQuaternion.GetXYZ());

    if (cosineHalfAngle >= VALUE_ONE_F || cosineHalfAngle <= -VALUE_ONE_F)
    {
        // 角度が0の場合は入力のひとつを返す
        return _startQuaternion;
    }
    else if (cosineHalfAngle < VALUE_ZERO_F)
    {
        // 内積が負の場合、逆回転を使用して最短経路で補間
        _endQuaternion.x_ = _endQuaternion.x_ * -VALUE_ONE_F;
        _endQuaternion.y_ = _endQuaternion.y_ * -VALUE_ONE_F;
        _endQuaternion.z_ = _endQuaternion.z_ * -VALUE_ONE_F;
        _endQuaternion.w_ = -_endQuaternion.w_;
        cosineHalfAngle = -cosineHalfAngle;
    }

    float blendRatioA;
    float blendRatioB;
    if (cosineHalfAngle < SLERP_THRESHOLD)
    {
        // 大きな角度の場合は真のSlerp
        float halfAngle = acosf(cosineHalfAngle);
        float sineHalfAngle = sinf(halfAngle);
        float reciprocalSineHalfAngle = VALUE_ONE_F / sineHalfAngle;
        blendRatioA = sinf(halfAngle * (VALUE_ONE_F - _ratio)) * reciprocalSineHalfAngle;
        blendRatioB = sinf(halfAngle * _ratio) * reciprocalSineHalfAngle;
    }
    else
    {
        // 角度が小さい場合は線形補間（Lerp）
        blendRatioA = VALUE_ONE_F - _ratio;
        blendRatioB = _ratio;
    }

    // 重み付け合成
    VECTOR calculatedVector = VAdd(VScale(_startQuaternion.GetXYZ(), blendRatioA),
        VScale(_endQuaternion.GetXYZ(), blendRatioB));
    Quaternion result = Quaternion(blendRatioA * _startQuaternion.w_ + blendRatioB
        * _endQuaternion.w_, calculatedVector.x, calculatedVector.y, calculatedVector.z);

    // 正規化して返す
    if (result.CalculateLengthSquared() > VALUE_ZERO_D)
    {
        return Normalize(result);
    }
    else
    {
        return GetIdentity();
    }
}

Quaternion Quaternion::GetIdentity(void)
{
    return Quaternion(VALUE_ONE_D, VALUE_ZERO_D, VALUE_ZERO_D, VALUE_ZERO_D);
}

double Quaternion::CalculateLength(void) const
{
    return sqrt(CalculateLengthSquared());
}

double Quaternion::CalculateLengthSquared(void) const
{
    return x_ * x_ + y_ * y_ + z_ * z_ + w_ * w_;
}

VECTOR Quaternion::GetXYZ(void) const
{
    return { static_cast<float>(x_), static_cast<float>(y_), static_cast<float>(z_) };
}

void Quaternion::ConvertToAngleAxis(float* _angle, VECTOR* _axis)
{
    // クォータニオンの正規化
    if (abs(this->w_) > VALUE_ONE_D)
    {
        this->Normalize();
    }

    // 回転角を計算
    *_angle = VALUE_TWO_F * acosf(static_cast<float>(this->w_));

    if (UtilityMath::EqualsVZero(GetXYZ()))
    {
        *_angle = VALUE_ZERO_F;
    }

    // 回転軸を計算
    float denominator = sqrtf(VALUE_ONE_F - static_cast<float>(this->w_ * this->w_));
    if (denominator > DENOMINATOR_EPSILON)
    {
        // 軸を正規化
        VECTOR axisVector = this->GetXYZ();
        _axis->x = axisVector.x / denominator;
        _axis->y = axisVector.y / denominator;
        _axis->z = axisVector.z / denominator;
    }
    else
    {
        // 角度が0の場合は任意の軸（X軸単位ベクトル）を設定
        *_axis = UtilityMath::AXIS_X;
    }
}

Quaternion Quaternion::operator*(float& _rightValue)
{
    return Quaternion(w_ * _rightValue, x_ * _rightValue, y_ * _rightValue, z_ * _rightValue);
}

const Quaternion Quaternion::operator*(const float& _rightValue)
{
    return Quaternion(w_ * _rightValue, x_ * _rightValue, y_ * _rightValue, z_ * _rightValue);
}

Quaternion Quaternion::operator+(Quaternion& _rightValue)
{
    return Quaternion(w_ + _rightValue.w_, x_ + _rightValue.x_,
        y_ + _rightValue.y_, z_ + _rightValue.z_);
}

const Quaternion Quaternion::operator+(const Quaternion& _rightValue)
{
    return Quaternion(w_ + _rightValue.w_, x_ + _rightValue.x_,
        y_ + _rightValue.y_, z_ + _rightValue.z_);
}