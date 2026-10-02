#pragma once

#include<string>
#include<vector>
#include<DxLib.h>

#include"../Common/Vector2.h"
#include"../Common/Quaternion.h"

/// <summary>
/// 各種ベクトル・角度・補間などのユーティリティ関数群を提供する静的クラス
/// </summary>
class UtilityMath
{
public:

	// ラジアンから度への変換定数（float
	static constexpr float RAD2DEG = (180 / DX_PI_F);

	// 度からラジアンへの変換定数（float)
	static constexpr float DEG2RAD = (DX_PI_F / 180.0f);

	/// @brief ゼロベクトル(0, 0, 0)
	static constexpr VECTOR VECTOR_ZERO = { 0.0f, 0.0f, 0.0f };
	static constexpr Vector2 VECTOR2_ZERO = { 0, 0 };
	static constexpr Vector2F VECTOR2F_ZERO = { 0.0f, 0.0f };

	/// @brief 単位ベクトル(1, 1, 1)
	static constexpr VECTOR VECTOR_ONE = { 1.0f, 1.0f, 1.0f };
	static constexpr Vector2 VECTOR2_ONE = { 1, 1 };
	static constexpr Vector2F VECTOR2F_ONE = { 1.0f, 1.0f };

	//回転軸

	/// <summary>X軸方向の単位ベクトル</summary>
	static constexpr VECTOR AXIS_X = { 1.0f, 0.0f, 0.0f };

	/// <summary>Y軸方向の単位ベクトル</summary>
	static constexpr VECTOR AXIS_Y = { 0.0f, 1.0f, 0.0f };

	/// <summary>Z軸方向の単位ベクトル</summary>
	static constexpr VECTOR AXIS_Z = { 0.0f, 0.0f, 1.0f };

	//方向

	/// <summary>前方方向 (Z+)</summary>
	static constexpr VECTOR DIR_FORWARD = { 0.0f, 0.0f, 1.0f };

	/// <summary>後方方向 (Z-)</summary>
	static constexpr VECTOR DIR_BACK = { 0.0f, 0.0f, -1.0f };

	/// <summary>右方向 (X+)</summary>
	static constexpr VECTOR DIR_R = { 1.0f, 0.0f, 0.0f };

	/// <summary>左方向 (X-)</summary>
	static constexpr VECTOR DIR_L = { -1.0f, 0.0f, 0.0f };

	/// <summary>上方向 (Y+)</summary>
	static constexpr VECTOR DIR_UP = { 0.0f, 1.0f, 0.0f };

	/// <summary>下方向 (Y-)</summary>
	static constexpr VECTOR DIR_DOWN = { 0.0f, -1.0f, 0.0f };

	/// <summary>浮動小数点の誤差比較用の最小値</summary>
	static constexpr float kEpsilonNormalSqrt = 1e-15F;

	static constexpr float HALF_NUM = 0.5f;

	/// <summary>
	/// 小数を四捨五入して整数に変換する
	/// </summary>
	/// <param name="v">対象の値</param>
	/// <returns>四捨五入された整数</returns>
	static int Round(float v);

	/// <summary>
	/// 文字列を指定文字で分割する
	/// </summary>
	/// <param name="line">分割対象の文字列</param>
	/// <param name="delimiter">区切り文字</param>
	/// <returns>分割された文字列の配列</returns>
	static std::vector < std::wstring> Split(const std::wstring& line, wchar_t delimiter);

	/// <summary>
	/// ラジアンから度（double）へ変換
	/// </summary>
	/// <param name="rad">ラジアン角</param>
	/// <returns>度</returns>
	static double Rad2DegD(double rad);

	/// <summary>
	/// 度からラジアン（float）へ変換
	/// </summary>
	/// <param name="deg">度</param>
	/// <returns>ラジアン角</returns>
	static float Rad2DegF(float rad);

	/// <summary>
	/// ラジアンから度に変換（int）
	/// </summary>
	/// <param name="rad">ラジアン値</param>
	/// <returns>度数値</returns>
	static int Rad2DegI(int rad);

	/// <summary>
	/// 度からラジアンに変換（double）
	/// </summary>
	/// <param name="deg">度数値</param>
	/// <returns>ラジアン値</returns>
	static double Deg2RadD(double deg);

	/// <summary>
	/// 度からラジアンに変換（float）
	/// </summary>
	/// <param name="deg">度数値</param>
	/// <returns>ラジアン値</returns>
	static float Deg2RadF(float deg);

	/// <summary>
	/// 度からラジアンに変換（int）
	/// </summary>
	/// <param name="deg">度数値</param>
	/// <returns>ラジアン値</returns>
	static int Deg2RadI(int deg);

	/// <summary>
	/// 角度を0～360に正規化
	/// </summary>
	/// <param name="deg">入力角度（度）</param>
	/// <returns>0～360度の範囲に正規化された角度</returns>
	static double DegIn360(double deg);

	/// <summary>
	/// 角度を0～2πに正規化
	/// </summary>
	/// <param name="rad">入力角度（ラジアン）</param>
	/// <returns>0～2πの範囲に正規化された角度</returns>
	static double RadIn2PI(double rad);

	/// <summary>
	/// 回転が少ない方の方向を判定（ラジアン）。時計回りなら1、反時計回りなら-1を返す。
	/// </summary>
	/// <param name="from">開始角度（ラジアン）</param>
	/// <param name="to">終了角度（ラジアン）</param>
	/// <returns>回転方向（1: 時計回り, -1: 反時計回り）</returns>
	static int DirNearAroundRad(float from, float to);

	/// <summary>
	/// 回転が少ない方の方向を判定（度）。時計回りなら1、反時計回りなら-1を返す。
	/// </summary>
	/// <param name="from">開始角度（度）</param>
	/// <param name="to">終了角度（度）</param>
	/// <returns>回転方向（1: 時計回り, -1: 反時計回り）</returns>
	static int DirNearAroundDeg(float from, float to);

	//線形補間

	/// <summary>
	/// 線形補間（int）
	/// </summary>
	/// <param name="start">開始値</param>
	/// <param name="end">終了値</param>
	/// <param name="t">補間係数（0～1）</param>
	/// <returns>補間後の値</returns>
	static int Lerp(int start, int end, float t);

	/// <summary>
	/// 線形補間（float）
	/// </summary>
	/// <param name="start">開始値</param>
	/// <param name="end">終了値</param>
	/// <param name="t">補間係数（0～1）</param>
	/// <returns>補間後の値</returns>
	static float Lerp(float start, float end, float t);

	/// <summary>
	/// 線形補間（double）
	/// </summary>
	/// <param name="start">開始値</param>
	/// <param name="end">終了値</param>
	/// <param name="t">補間係数（0～1）</param>
	/// <returns>補間後の値</returns>
	static double Lerp(double start, double end, double t);

	/// <summary>
	/// 線形補間（Vector2）
	/// </summary>
	/// <param name="start">開始ベクトル</param>
	/// <param name="end">終了ベクトル</param>
	/// <param name="t">補間係数（0～1）</param>
	/// <returns>補間後のベクトル</returns>
	static Vector2 Lerp(const Vector2& start, const Vector2& end, float t);

	/// <summary>
	/// 線形補間（VECTOR）
	/// </summary>
	/// <param name="start">開始ベクトル</param>
	/// <param name="end">終了ベクトル</param>
	/// <param name="t">補間係数（0～1）</param>
	/// <returns>補間後のベクトル</returns>
	static VECTOR Lerp(const VECTOR& start, const VECTOR& end, float t);

	/// <summary>
	/// 角度の線形補間（度）
	/// </summary>
	/// <param name="start">開始角度（度）</param>
	/// <param name="end">終了角度（度）</param>
	/// <param name="t">補間係数（0～1）</param>
	/// <returns>補間後の角度（度）</returns>
	static double LerpDeg(double start, double end, double t);

	/// <summary>
	/// 色の線形補間
	/// </summary>
	/// <param name="start">開始色</param>
	/// <param name="end">終了色</param>
	/// <param name="t">補間係数（0～1）</param>
	/// <returns>補間後の色</returns>
	static COLOR_F Lerp(const COLOR_F& start, const COLOR_F& end, float t);

	/// <summary>
	/// 2Dベジェ曲線（Vector2）での位置計算
	/// </summary>
	/// <param name="p1">開始点</param>
	/// <param name="p2">中間点</param>
	/// <param name="p3">終了点</param>
	/// <param name="t">補間係数（0～1）</param>
	/// <returns>補間後の位置（ベジェ曲線上）</returns>
	static Vector2 Bezier(const Vector2& p1, const Vector2& p2, const Vector2& p3, float t);

	/// <summary>
	/// 3Dベジェ曲線（VECTOR）での位置計算
	/// </summary>
	/// <param name="p1">開始点</param>
	/// <param name="p2">中間点</param>
	/// <param name="p3">終了点</param>
	/// <param name="t">補間係数（0～1）</param>
	/// <returns>補間後の位置（ベジェ曲線上）</returns>
	static VECTOR Bezier(const VECTOR& p1, const VECTOR& p2, const VECTOR& p3, float t);

	/// <summary>
	/// Y軸を中心としたXZ平面での回転座標を求める
	/// </summary>
	/// <param name="centerPos">回転中心位置</param>
	/// <param name="radiusPos">回転させる対象の位置</param>
	/// <param name="rad">回転角度（ラジアン）</param>
	/// <returns>回転後の位置</returns>
	static VECTOR RotXZPos(const VECTOR& centerPos, const VECTOR& radiusPos, float rad);

	/// <summary>
	/// ベクトルの長さ（2D）
	/// </summary>
	/// <param name="v">対象のベクトル（2D）</param>
	/// <returns>ベクトルの長さ</returns>
	static double Magnitude(const Vector2& v);

	/// <summary>
	/// ベクトルの長さ（3D）
	/// </summary>
	/// <param name="v">対象のベクトル（3D）</param>
	/// <returns>ベクトルの長さ</returns>
	static double Magnitude(const VECTOR& v);

	/// <summary>
	/// ベクトルの長さ（3D・float版）
	/// </summary>
	/// <param name="v">対象のベクトル（3D）</param>
	/// <returns>ベクトルの長さ</returns>
	static float MagnitudeF(const VECTOR& v);

	/// <summary>
	/// ベクトルの長さの2乗（2D）
	/// </summary>
	/// <param name="v">対象のベクトル（2D）</param>
	/// <returns>ベクトルの長さの2乗</returns>
	static int SqrMagnitude(const Vector2& v);

	/// <summary>
	/// ベクトルの長さの2乗（3D・float版）
	/// </summary>
	/// <param name="v">対象のベクトル（3D）</param>
	/// <returns>ベクトルの長さの2乗</returns>
	static float SqrMagnitudeF(const VECTOR& v);

	/// <summary>
	/// ベクトルの長さの2乗（3D）
	/// </summary>
	/// <param name="v">対象のベクトル（3D）</param>
	/// <returns>ベクトルの長さの2乗</returns>
	static double SqrMagnitude(const VECTOR& v);

	/// <summary>
	/// 2点間の距離の2乗（3D）
	/// </summary>
	/// <param name="v1">開始ベクトル（3D）</param>
	/// <param name="v2">終了ベクトル（3D）</param>
	/// <returns>2点間の距離の2乗</returns>
	static double SqrMagnitude(const VECTOR& v1, const VECTOR& v2);

	/// <summary>
	/// 2点間の距離（2D）
	/// </summary>
	/// <param name="v1">開始ベクトル（2D）</param>
	/// <param name="v2">終了ベクトル（2D）</param>
	/// <returns>2点間の距離</returns>
	static double Distance(const Vector2& v1, const Vector2& v2);

	/// <summary>
	/// 2点間の距離（3D）
	/// </summary>
	/// <param name="v1">開始ベクトル（3D）</param>
	/// <param name="v2">終了ベクトル（3D）</param>
	/// <returns>2点間の距離</returns>
	static double Distance(const VECTOR& v1, const VECTOR& v2);

	
	/// @brief 2つのベクトルが等しいか判定
	/// @param _vec1 比較するベクトル１
	/// @param _vec2 比較するベクトル２
	static bool Equals(const VECTOR& vec1, const VECTOR& vec2);

	/// @brief ベクトルがゼロベクトルか判定
	/// @param _vec 対象のベクトル
	static bool EqualsVZero(const VECTOR& vec);
	static bool EqualsVZero(const Vector2& vec);
	static bool EqualsVZero(const Vector2F& vec);


	/// @brief 2Dベクトルを正規化し3Dベクトルに変換
	/// @param _vec 対象の2Dベクトル
	static VECTOR Normalize(const Vector2& vec);

	
	/// @brief ベクトルを正規化
	/// @param _vec 対象の3Dベクトル
	static VECTOR VNormalize(const VECTOR& _vec);
	static Vector2 VNormalize(const Vector2& vec);
	static Vector2F VNormalize(const Vector2F& _vec);

	/// <summary>
	/// 2つのベクトルの間の角度（度）を返す
	/// </summary>
	/// <param name="from">開始ベクトル</param>
	/// <param name="to">終了ベクトル</param>
	/// <returns>ベクトル間の角度（度）</returns>
	static double AngleDeg(const VECTOR& from, const VECTOR& to);

	//描画系

	/// <summary>
	/// 指定位置から方向ベクトルに向けて線を描画する
	/// </summary>
	/// <param name="pos">開始位置</param>
	/// <param name="dir">方向ベクトル</param>
	/// <param name="color">色（DxLibの色コード）</param>
	/// <param name="len">線の長さ（デフォルトは50.0f）</param>
	/// <returns>なし</returns>
	static void DrawLineDir(const VECTOR& pos, const VECTOR& dir, int color, float len = 50.0f);

	/// <summary>
	/// 指定位置から回転行列の軸方向に線を描画する
	/// </summary>
	/// <param name="pos">開始位置</param>
	/// <param name="rot">回転行列</param>
	/// <param name="len">線の長さ（デフォルトは50.0f）</param>
	/// <returns>なし</returns>
	static void DrawLineXYZ(const VECTOR& pos, const MATRIX& rot, float len = 50.0f);

	/// <summary>
	/// 指定位置からクォータニオンの軸方向に線を描画する
	/// </summary>
	/// <param name="pos">開始位置</param>
	/// <param name="rot">クォータニオン回転</param>
	/// <param name="len">線の長さ（デフォルトは50.0f）</param>
	/// <returns>なし</returns>
	static void DrawLineXYZ(const VECTOR& pos, const Quaternion& rot, float len = 50.0f);

	/// @brief 指定した中心点から円周上の位置を計算する
    /// @param center 中心座標
    /// @param radius 半径
    /// @param angle 角度（ラジアン）
    /// @return 円周上の位置座標
	static VECTOR GetCirclePos(const VECTOR& center, float radius, float angle);

	//待機時間
	//static bool IsTimeOver(float& totalTime, const float& waitTime);

	// ランダムな数値を返す(float用)
	static float  RandRangeF(float min, float max);

	/// @brief 線分上の最もターゲットに近い座標を算出
	/// @param startPos 線分の開始点
	/// @param endPos 線分の終了点
	/// @param targetPos ターゲット座標
	/// @return 線分上の最近接座標
	static VECTOR GetNearestPointOnSegment(const VECTOR& _startPos,
		const VECTOR& _endPos, const VECTOR& _targetPos);
};

