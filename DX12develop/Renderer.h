#pragma once

#include "Singleton.h"

#define BUFFER_COUNT (2)

using Microsoft::WRL::ComPtr;

using namespace DirectX;

class Renderer : public Singleton<Renderer>
{
public:
	Renderer(){}
	~Renderer(){}

	void Init();

	void Draw();

	void Update();

	void Uninit();

private:
	//格納できるデスクリプタの数（デスクリプタヒープが配列の為、配列数の指定が必要）。通常レンダーターゲットはフレームバッファと1：1になるように作成する
	static const UINT FrameCount = 2;
	//デバイスポインタ
	ComPtr<ID3D12Device5> m_device;
	//スワップチェーン
	ComPtr<IDXGISwapChain3> m_swapChain;
	//コマンドキュー
	ComPtr<ID3D12CommandQueue> m_commandQueue;
	//ワープアダプタを使うかどうか
	bool m_use_warp_device = false;
	//レンダーターゲットデスクリプタ
	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	//レンダーターゲット用デスクリプタのサイズ
	UINT m_rtvDescriptorSize;

	// 同期オブジェクト。
	//描画側フレームバッファのインデックス
	UINT m_frameIndex;
};


