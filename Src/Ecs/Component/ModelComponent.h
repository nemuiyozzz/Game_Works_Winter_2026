#pragma once

#include <vector>
#include <DxLib.h>

/// @brief 3Dモデルのハンドル情報を保持する純粋なデータコンポーネント
struct ModelComponent
{
	// モデルのハンドルID
	int modelHandle = -1; 

	// 距離カリング用のフレームごとのキャッシュ情報
	struct FrameInfo
	{
		int frameIndex;         // DxLibのフレーム番号
		VECTOR localCenterPos;  // ローカル空間での中心座標
	};

	// モデルが持つ有効なメッシュフレームのリスト
	std::vector<FrameInfo> frameInfos;

	// モデル読み込み直後に呼ぶ、フレーム情報の自動キャッシュ関数
	void BuildFrameInfos(void)
	{
		frameInfos.clear();

		if (modelHandle == -1) { return; }

		int totalFrameNum = MV1GetFrameNum(modelHandle);

		for (int i = 0; i < totalFrameNum; ++i)
		{
			if (MV1GetFrameTriangleNum(modelHandle, i) > 0)
			{
				VECTOR minPos = MV1GetFrameMinVertexLocalPosition(modelHandle, i);
				VECTOR maxPos = MV1GetFrameMaxVertexLocalPosition(modelHandle, i);

				FrameInfo info;

				info.frameIndex = i;
				info.localCenterPos = VScale(VAdd(minPos, maxPos), 0.5f);
				frameInfos.push_back(info);
			}
		}
	}
};