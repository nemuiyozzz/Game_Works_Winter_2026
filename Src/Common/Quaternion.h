#pragma once

#include<DxLib.h>

// @brief 回転を表現するためのクォータニオンクラス
class Quaternion
{
public:

	// 正規化時の極小値（ゼロ割防止）
	static constexpr float kEpsilonNorMalSqrt = 1e-15F;

	// クォータニオンのスカラー成分（回転量の余弦）
	double w;

	// クォータニオンのX成分（ベクトル部）
	double x;

	// クォータニオンのY成分（ベクトル部）
	double y;

	// クォータニオンのZ成分（ベクトル部）
	double z;

	// @brief デフォルトコンストラクタ（全成分0）
	Quaternion(void);

	// @brief オイラー角から初期化
	// @param rad 各軸の回転角（ラジアン）
	Quaternion(const VECTOR& rad);

	// @brief 各成分から初期化
	// @param w スカラー成分
	// @param x X成分
	// @param y Y成分
	// @param z Z成分
	Quaternion(double w, double x, double y, double z);

	// @brief デストラクタ
	~Quaternion(void) = default;

	// @brief オイラー角からクォータニオン生成
	// @param rad 各軸回転（ラジアン）
	// @return 生成されたクォータニオン
	static Quaternion Euler(const VECTOR& rad);

	// @brief XYZ角からクォータニオン生成
	// @param radX X回転
	// @param radY Y回転
	// @param radZ Z回転
	// @return 生成されたクォータニオン
	static Quaternion Euler(double radX, double radY, double radZ);

	// @brief クォータニオン合成
	// @param q1 第1クォータニオン
	// @param q2 第2クォータニオン
	// @return 合成結果
	static Quaternion Mult(const Quaternion& q1, const Quaternion& q2);

	// @brief 現在の回転と合成
	// @param q 合成対象
	// @return 合成結果
	Quaternion Mult(const Quaternion& q) const;

	// @brief 軸回転クォータニオン生成
	// @param rad 回転角
	// @param axis 回転軸
	// @return 回転クォータニオン
	static Quaternion AngleAxis(double rad, VECTOR axis);

	// @brief ベクトルを回転（静的）
	// @param q 回転
	// @param axis 対象ベクトル
	// @return 回転後ベクトル
	static VECTOR PosAxis(const Quaternion& q, VECTOR axis);

	// @brief ベクトルを回転（メンバ）
	// @param pos 対象ベクトル
	// @return 回転後ベクトル
	VECTOR PosAxis(VECTOR pos) const;

	// @brief オイラー角変換（静的）
	// @param q 対象クォータニオン
	// @return オイラー角
	static VECTOR ToEuler(const Quaternion& q);

	// @brief オイラー角変換（メンバ）
	// @return オイラー角
	VECTOR ToEuler(void) const;

	// @brief 行列変換（静的）
	// @param q 対象クォータニオン
	// @return 回転行列
	static MATRIX ToMatrix(const Quaternion& q);

	// @brief 行列変換（メンバ）
	// @return 回転行列
	MATRIX ToMatrix(void) const;

	// @brief 方向から回転生成
	// @param dir 前方向
	// @return 回転クォータニオン
	static Quaternion LookRotation(VECTOR dir);

	// @brief 方向＋上方向から回転生成
	// @param dir 前方向
	// @param up 上方向
	// @return 回転クォータニオン
	static Quaternion LookRotation(VECTOR dir, VECTOR up);

	// @brief 行列から回転抽出
	// @param mat 行列
	// @return 回転クォータニオン
	static Quaternion GetRotation(MATRIX mat);

	// @brief 前方向取得
	// @return 前方向ベクトル
	VECTOR GetForward(void) const;

	// @brief 後方向取得
	// @return 後方向ベクトル
	VECTOR GetBack(void) const;

	// @brief 右方向取得
	// @return 右方向ベクトル
	VECTOR GetRight(void) const;

	// @brief 左方向取得
	// @return 左方向ベクトル
	VECTOR GetLeft(void) const;

	// @brief 上方向取得
	// @return 上方向ベクトル
	VECTOR GetUp(void) const;

	// @brief 下方向取得
	// @return 下方向ベクトル
	VECTOR GetDown(void) const;

	// @brief 内積計算（静的）
	// @param q1 クォータニオン1
	// @param q2 クォータニオン2
	// @return 内積値
	static double Dot(const Quaternion& q1, const Quaternion& q2);

	// @brief 内積計算（メンバ）
		// @param b 比較対象
		// @return 内積値
	double Dot(const Quaternion& b) const;

	// @brief 正規化（静的）
	// @param q 対象
	// @return 正規化結果
	static Quaternion Normalize(const Quaternion& q);

	// @brief 正規化（非破壊）
	// @return 正規化結果
	Quaternion Normalized(void) const;

	// @brief 正規化（破壊）
	void Normalize(void);

	// @brief 逆クォータニオン
	// @return 逆回転
	Quaternion Inverse(void) const;

	// @brief 球面線形補間
	// @param from 開始
	// @param to 終了
	// @param t 補間率
	// @return 補間結果
	static Quaternion Slerp(Quaternion from, Quaternion to, double t);

	// @brief 方向差から回転生成
	// @param fromDir 開始方向
	// @param toDir 目標方向
	// @return 回転クォータニオン
	static Quaternion FromToRotation(VECTOR fromDir, VECTOR toDir);

	// @brief 最大角制限付き補間
	// @param from 現在
	// @param to 目標
	// @param maxDegreesDelta 最大角度
	// @return 補間結果
	static Quaternion RotateTowards(const Quaternion& from, const Quaternion& to, float maxDegreesDelta);

	// @brief 角度差取得
	// @param q1 クォータニオン1
	// @param q2 クォータニオン2
	// @return 角度差
	static double Angle(const Quaternion& q1, const Quaternion& q2);

	// @brief 制限なし補間
	// @param a 開始
	// @param b 終了
	// @param t 補間率
	// @return 補間結果
	static Quaternion SlerpUnclamped(Quaternion a, Quaternion b, float t);

	// @brief 単位クォータニオン
	// @return 回転なし
	static Quaternion Identity(void);

	// @brief 長さ取得
	// @return 長さ
	double Length(void) const;

	// @brief 長さの2乗
	// @return 長さの2乗
	double LengthSquared(void) const;

	// @brief x, y, z取得
	// @return ベクトル
	VECTOR xyz(void) const;

	// @brief 角度と軸に分解
	// @param angle 出力角度
	// @param axis 出力軸
	void ToAngleAxis(float* angle, VECTOR* axis);

private:

	// @brief 指定方向を回転
	// @param dir 入力方向
	// @return 回転後
	VECTOR GetDir(VECTOR dir) const;

	// @brief スカラー乗算（破壊）
	// @param rhs 値
	// @return 結果
	Quaternion operator*(float& rhs);

	// @brief スカラー乗算（非破壊）
		// @param rhs 値
		// @return 結果
	const Quaternion operator*(const float& rhs);

	// @brief 加算（破壊）
	// @param rhs 相手
	// @return 結果
	Quaternion operator+(Quaternion& rhs);

	// @brief 加算（非破壊）
	// @param rhs 相手
	// @return 結果
	const Quaternion operator+(const Quaternion& rhs);

};
