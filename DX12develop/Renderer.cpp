#include "main.h"
#include "Renderer.h"
#include <vector>

void Renderer::Init()
{
	HRESULT hr;

	UINT dxgiFactoryFlags = 0;

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
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
	}
#endif

	//ファクトリ-生成・・・低レベルなものにアクセスするためのデバイスみたいなものらしい
	hr = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_factry));
	if (FAILED(hr)) {
		MessageBox(NULL, L"ファクトリーの作成に失敗しました", L"エラーメッセージ", MB_OK);
	}

	//ワープアダプタ・・・グラボの指定らしい
	std::vector <IDXGIAdapter*> adapters;

	IDXGIAdapter* tmpAdapter = nullptr;

	for (int i = 0; m_factry->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {
		adapters.push_back(tmpAdapter);
	}

	for (auto adpt : adapters) {
		DXGI_ADAPTER_DESC adesc = {};
		adpt->GetDesc(&adesc);

		std::wstring strDesc = adesc.Description;

		if (strDesc.find(L"NVIDIA") != std::string::npos) {
			tmpAdapter = adpt;
			break;
		}
	}
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
	hr = D3D12CreateDevice(tmpAdapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_device));
	if (FAILED(hr)) {
		MessageBox(NULL, L"Direct3Dインターフェースの作成に失敗しました", L"エラーメッセージ", MB_OK);
	}

	//コマンドアロケータの作成
	hr = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator));

	// コマンドリストを作成します
	m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), m_pipelineState.Get(), IID_PPV_ARGS(&m_commandList));

	
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
	hr = m_factry->CreateSwapChainForHwnd(
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
	hr = m_factry->MakeWindowAssociation(GetWindow(), DXGI_MWA_NO_ALT_ENTER);
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

	// ディスクリプタヒープを作成
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
	//ヘルパー内の構造体
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
	//各フレームのレンダービューターゲットの作成
	for (UINT n = 0; n < FrameCount; n++) {
		hr = m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n]));
		if (FAILED(hr)) {
			MessageBox(NULL, L"レンダービューターゲット生成の失敗", L"エラーメッセージ", MB_OK);
		}
		m_device->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr,rtvHandle);
		rtvHandle.Offset(1, m_rtvDescriptorSize);
	}

	

	//ルートシグネチャの作成
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	hr = (D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
	hr = (m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));

	// シェーダーのコンパイルと読み込みを含むパイプライン状態を作成
	ComPtr<ID3DBlob> vertexShader;
	ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
	// Enable better shader debugging with the graphics debugging tools.
	// グラフィック デバッグ ツールを使用して、より適切なシェーダー デバッグを有効にします。
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

	hr = D3DCompileFromFile(L"shaders.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr);
	hr = D3DCompileFromFile(L"shaders.hlsl", nullptr, nullptr, "PSMain", "vs_5_0", compileFlags, 0, &pixelShader, nullptr);

	// 頂点入力レイアウトを定義します。
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
		 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0} };

	// グラフィックス パイプライン状態オブジェクト (PSO) を記述して作成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	psoDesc.pRootSignature = m_rootSignature.Get();
	psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;
	hr = m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState));
  
	



}

void Renderer::CreateSynchronizationObject(void)
{
	m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
	m_fenceValue = 1;

	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (m_fence == nullptr) {
		HRESULT_FROM_WIN32(GetLastError());
	}

	
}

void Renderer::Uninit()
{
}
