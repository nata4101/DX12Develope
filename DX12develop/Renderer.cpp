#include "main.h"
#include <dxgi1_4.h>
#include "Renderer.h"

void Renderer::Init()
{
	HRESULT hr;

#if defined(_DEBUG)
	// Enable the debug layer (requires the Graphics Tools "optional feature").
	// NOTE: Enabling the debug layer after device creation will invalidate the
	// active device.
	//デバッグ レイヤーを有効にします (グラフィック ツールの「オプション機能」が必要です)。
	//注: デバイスの作成後にデバッグ レイヤーを有効にすると、アクティブなデバイス。
	{
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
			debugController->EnableDebugLayer();

			// Enable additional debug layers.(追加のデバッグレイヤーを有効にします。)
			//dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
	}
#endif

	//ファクトリ-生成・・・低レベルなものにアクセスするためのデバイスみたいなものらしい
	ComPtr<IDXGIFactory4> factory;
	if (FAILED(hr)) {
		MessageBox(NULL, L"ファクトリーの作成に失敗しました", L"エラーメッセージ", MB_OK);
	}

	//ワープアダプタ・・・よくわからん
	/*if (m_use_warp_device) {
		ComPtr<IDXGIAdapter> warpAdapter;
		hr = factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter));
		if (FAILED(hr)) {
			MessageBox(NULL, L"ワープアダプタの作成に失敗しました", L"エラーメッセージ", MB_OK);
		}
		hr = D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0,IID_PPV_ARGS(&m_device));
		if (FAILED(hr)) {
			MessageBox(NULL, L"DirectXデバイスの作成に失敗しました", L"エラーメッセージ", MB_OK);
		}
	}
	else {
		ComPtr<IDXGIAdapter1> hardwareAdapter;
		GetHardwareAdapter(factory.Get(), &hardwareAdapter);

		ThrowIfFailed(D3D12CreateDevice(hardwareAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&m_device)));
	}*/

	//デバイス生成
	hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_device));
	if (FAILED(hr)) {
		MessageBox(NULL, L"Direct3Dインターフェースの作成に失敗しました", L"エラーメッセージ", MB_OK);
	}

	
	//コマンドキューの作成
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;	//キューの特別な挙動がある場合に設定。デフォルトはD3D12_COMMAND_QUEUE_FLAG_NONE
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;	//コマンドキューの種類を示すD3D12_COMMAND_LIST_TYPE列挙型の値。
	hr = m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue));
	if (FAILED(hr)) {
		MessageBox(NULL, L"コマンドキュー生成の失敗", L"エラーメッセージ", MB_OK);
	}

	//スワップチェーン作成
	//スワップチェーンデスクの設定
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	//使用するバッファの数
	swapChainDesc.BufferCount = BUFFER_COUNT;
	//フレームバッファの幅と高さ。通常はウィンドウの描画領域と同じ大きさを設定
	swapChainDesc.Width = SCREEN_WIDTH;							
	swapChainDesc.Height = SCREEN_HEIGHT;
	//フレームバッファのピクセルフォーマット
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	
	//フレームバッファの使用方法を指定
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	
	//画面を切り替えた後、描画画面になったフレームバッファの扱い方を指定
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	//AAの為のサンプリング回数を指定。AAを使用しない場合、1を指定。
	swapChainDesc.SampleDesc.Count = 1;
	//サンプリングの品質を指定。AAを使用しない場合、0を指定。
	swapChainDesc.SampleDesc.Quality = 0;
	//フレームバッファとウィンドウのサイズが異なる場合の表示方法を指定
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;

	ComPtr<IDXGISwapChain1> swapChain;
	//CreateSwapChainForHwnd::スワップチェーンの出力ウィンドウへのHWNDハンドルに関連付けられたスワップチェーンを作成します。
	hr = factory->CreateSwapChainForHwnd(
		m_commandQueue.Get(),
		GetWindow(),
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain);
	if (FAILED(hr)) {
		MessageBox(NULL, L"スワップチェーン生成の失敗", L"エラーメッセージ", MB_OK);
	}
	//フルスクリーン防止
	hr = factory->MakeWindowAssociation(GetWindow(), DXGI_MWA_NO_ALT_ENTER);
	if (FAILED(hr)) {
		MessageBox(NULL, L"スワップチェーン生成の失敗", L"エラーメッセージ", MB_OK);
	}
	//SwapChain1をSwapChain3に　SwapChain3を直接生成できないっぽい？
	hr = swapChain.As(&m_swapChain);
	if (FAILED(hr)) {
		MessageBox(NULL, L"スワップチェーン生成の失敗", L"エラーメッセージ", MB_OK);
	}
	//描画側フレームバッファのインデックス取得
	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	// 記述子ヒープを作成
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	//格納できるデスクリプタの数
	rtvHeapDesc.NumDescriptors = FrameCount;
	//デスクリプタヒープのタイプを設定。使いみちに応じて4つのタイプから設定
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	//D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_NONEを設定。デスクリプタが指しているデータをシェーダから参照したい場合はD3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLEを設定。
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	hr = m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap));
	//レンダーターゲット用デスクリプタのサイズを取得
	m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	//レンダービューターゲットの作成
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
		m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
}

void Renderer::Draw()
{
}

void Renderer::Update()
{
}

void Renderer::Uninit()
{
}
