#pragma once

#include<DxLib.h>

// @brief 回転を表現するためのクォータニオンクラス
class Quaternion
{
public:

	// クォータニオン成分関連
	double w_;	// スカラー成分
	double x_;	// X成分
	double y_;	// Y成分
	double z_;	// Z成分

	// @brief デフォルトコンストラクタ
	Quaternion(void);

	// @brief オイラー角から初期化
	// @param radian 各軸の回転角
	Quaternion(const VECTOR& _radian);

	// @brief 各成分から初期化
	// @param w スカラー成分
	// @param x X成分
	// @param y Y成分
	// @param z Z成分
	Quaternion(double _scalarW, double _vectorX, double _vectorY, double _vectorZ);

	// @brief デストラクタ
	~Quaternion(void) = default;

	// @brief オイラー角からクォータニオン生成
	// @param radian 各軸回転
	// @return 生成されたクォータニオン
	static Quaternion CreateEuler(const VECTOR& _radian);

	// @brief XYZ角からクォータニオン生成
	// @param radianX X回転
	// @param radianY Y回転
	// @param radianZ Z回転
	// @return 生成されたクォータニオン
	static Quaternion CreateEuler(double _radianX, double _radianY, double _radianZ);

	// @brief クォータニオン合成
	// @param quaternion1 第1クォータニオン
	// @param quaternion2 第2クォータニオン
	// @return 合成結果
	static Quaternion Multiply(const Quaternion& _quaternion1, const Quaternion& _quaternion2);

	// @brief 現在の回転と合成
	// @param quaternion 合成対象
	// @return 合成結果
	Quaternion Multiply(const Quaternion& _quaternion) const;

	// @brief 軸回転クォータニオン生成
	// @param radian 回転角
	// @param axis 回転軸
	// @return 回転クォータニオン
	static Quaternion CreateAngleAxis(double _radian, VECTOR _axis);

	// @brief ベクトルを回転（静的）
	// @param quaternion 回転
	// @param axis 対象ベクトル
	// @return 回転後ベクトル
	static VECTOR RotatePosition(const Quaternion& _quaternion, VECTOR _axis);

	// @brief ベクトルを回転（メンバ）
	// @param position 対象ベクトル
	// @return 回転後ベクトル
	VECTOR RotatePosition(VECTOR _position) const;

	// @brief オイラー角変換（静的）
	// @param quaternion 対象クォータニオン
	// @return オイラー角
	static VECTOR ConvertToEuler(const Quaternion& _quaternion);

	// @brief オイラー角変換（メンバ）
	// @return オイラー角
	VECTOR ConvertToEuler(void) const;

	// @brief 行列変換（静的）
	// @param quaternion 対象クォータニオン
	// @return 回転行列
	static MATRIX ConvertToMatrix(const Quaternion& _quaternion);

	// @brief 行列変換（メンバ）
	// @return 回転行列
	MATRIX ConvertToMatrix(void) const;

	// @brief 方向から回転生成
	// @param direction 前方向
	// @return 回転クォータニオン
	static Quaternion CreateLookRotation(VECTOR _direction);

	// @brief 方向＋上方向から回転生成
	// @param direction 前方向
	// @param upDirection 上方向
	// @return 回転クォータニオン
	static Quaternion CreateLookRotation(VECTOR _direction, VECTOR _upDirection);

	// @brief 行列から回転抽出
	// @param matrix 行列
	// @return 回転クォータニオン
	static Quaternion GetRotation(MATRIX _matrix);

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
	// @param quaternion1 クォータニオン1
	// @param quaternion2 クォータニオン2
	// @return 内積値
	static double CalculateDot(const Quaternion& _quaternion1, const Quaternion& _quaternion2);

	// @brief 内積計算（メンバ）
	// @param target 比較対象
	// @return 内積値
	double CalculateDot(const Quaternion& _target) const;

	// @brief 正規化（静的）
	// @param quaternion 対象
	// @return 正規化結果
	static Quaternion Normalize(const Quaternion& _quaternion);

	// @brief 正規化（非破壊）
	// @return 正規化結果
	Quaternion GetNormalized(void) const;

	// @brief 正規化（破壊）
	void Normalize(void);

	// @brief 逆クォータニオン
	// @return 逆回転
	Quaternion CalculateInverse(void) const;

	/// @brief 球面線形補間
	/// @param fromQuaternion 開始
	/// @param toQuaternion 終了
	/// @param ratio 補間率
	/// @return 補間結果
	static Quaternion CalculateSlerp(Quaternion _fromQuaternion,
		Quaternion _toQuaternion, double _ratio);

	/// @brief 方向差から回転生成
	/// @param fromDirection 開始方向
	/// @param toDirection 目標方向
	/// @return 回転クォータニオン
	static Quaternion CreateFromToRotation(VECTOR _fromDirection, VECTOR _toDirection);

	// @brief 最大角制限付き補間
	// @param fromQuaternion 現在
	// @param toQuaternion 目標
	// @param maxDegreesDelta 最大角度
	// @return 補間結果
	static Quaternion RotateTowards(const Quaternion& _fromQuaternion,
		const Quaternion& _toQuaternion, float _maxDegreesDelta);

	// @brief 角度差取得
	// @param quaternion1 クォータニオン1
	// @param quaternion2 クォータニオン2
	// @return 角度差
	static double CalculateAngle(const Quaternion& _quaternion1, const Quaternion& _quaternion2);

	// @brief 制限なし補間
	// @param startQuaternion 開始
	// @param endQuaternion 終了
	// @param ratio 補間率
	// @return 補間結果
	static Quaternion CalculateSlerpUnclamped(Quaternion _startQuaternion,
		Quaternion _endQuaternion, float _ratio);

	// @brief 単位クォータニオン
	// @return 回転なし
	static Quaternion GetIdentity(void);

	// @brief 長さ取得
	// @return 長さ
	double CalculateLength(void) const;

	// @brief 長さの2乗
	// @return 長さの2乗
	double CalculateLengthSquared(void) const;

	// @brief x, y, z取得
	// @return ベクトル
	VECTOR GetXYZ(void) const;

	// @brief 角度と軸に分解
	// @param angle 出力角度
	// @param axis 出力軸
	void ConvertToAngleAxis(float* _angle, VECTOR* _axis);

	// @brief スカラー乗算（破壊）
	// @param rightValue 値
	// @return 結果
	Quaternion operator*(float& _rightValue);

	// @brief スカラー乗算（非破壊）
	// @param rightValue 値
	// @return 結果
	const Quaternion operator*(const float& _rightValue);

	// @brief 加算（破壊）
	// @param rightValue 相手
	// @return 結果
	Quaternion operator+(Quaternion& _rightValue);

	// @brief 加算（非破壊）
	// @param rightValue 相手
	// @return 結果
	const Quaternion operator+(const Quaternion& _rightValue);

private:

	// 正規化時の極小値（ゼロ割防止）
	static constexpr float KEPSILON_NORMAL_SQRT = 1e-15F;

	// @brief 指定方向を回転
	// @param direction 入力方向
	// @return 回転後
	VECTOR GetDirection(VECTOR _direction) const;

};
