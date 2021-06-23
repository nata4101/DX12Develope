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

	void CreateSynchronizationObject(void);

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
	//各フレームに対してのレンダービューターゲット
	ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
	//コマンドアロケータ
	ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	//ルートシグネチャ
	ComPtr<ID3D12RootSignature> m_rootSignature;
	//パイプラインステート
	ComPtr<ID3D12PipelineState> m_pipelineState;
	//コマンドリスト
	ComPtr<ID3D12GraphicsCommandList4> m_commandList;

	// 同期オブジェクト。
	//描画側フレームバッファのインデックス
	UINT m_frameIndex;
	HANDLE m_fenceEvent;
	ComPtr<ID3D12Fence> m_fence;
	UINT64 m_fenceValue;
};


