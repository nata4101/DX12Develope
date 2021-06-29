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
	//�f�o�b�O ���C���[��L���ɂ��܂� (�O���t�B�b�N �c�[���́u�I�v�V�����@�\�v���K�v�ł�)�B
	//��: �f�o�C�X�̍쐬��Ƀf�o�b�O ���C���[��L���ɂ���ƁA�A�N�e�B�u�ȃf�o�C�X�B
	{
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
			debugController->EnableDebugLayer();

			// Enable additional debug layers.(�ǉ��̃f�o�b�O���C���[��L���ɂ��܂��B)
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
	}
#endif

	//�t�@�N�g��-�����E�E�E�჌�x���Ȃ��̂ɃA�N�Z�X���邽�߂̃f�o�C�X�݂����Ȃ��̂炵��
	hr = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_factry));
	if (FAILED(hr)) {
		MessageBox(NULL, L"�t�@�N�g���[�̍쐬�Ɏ��s���܂���", L"�G���[���b�Z�[�W", MB_OK);
	}

	//���[�v�A�_�v�^�E�E�E�O���{�̎w��炵��
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
			MessageBox(NULL, L"���[�v�A�_�v�^�̍쐬�Ɏ��s���܂���", L"�G���[���b�Z�[�W", MB_OK);
		}
		hr = D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0,IID_PPV_ARGS(&m_device));
		if (FAILED(hr)) {
			MessageBox(NULL, L"DirectX�f�o�C�X�̍쐬�Ɏ��s���܂���", L"�G���[���b�Z�[�W", MB_OK);
		}
	}
	else {
		ComPtr<IDXGIAdapter1> hardwareAdapter;
		GetHardwareAdapter(factory.Get(), &hardwareAdapter);

		ThrowIfFailed(D3D12CreateDevice(hardwareAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&m_device)));
	}*/

	//�f�o�C�X����
	hr = D3D12CreateDevice(tmpAdapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_device));
	if (FAILED(hr)) {
		MessageBox(NULL, L"Direct3D�C���^�[�t�F�[�X�̍쐬�Ɏ��s���܂���", L"�G���[���b�Z�[�W", MB_OK);
	}

	//�R�}���h�A���P�[�^�̍쐬
	hr = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator));

	// �R�}���h���X�g���쐬���܂�
	m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), m_pipelineState.Get(), IID_PPV_ARGS(&m_commandList));

	
	//�R�}���h�L���[�̍쐬
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;	//�L���[�̓��ʂȋ���������ꍇ�ɐݒ�B�f�t�H���g��D3D12_COMMAND_QUEUE_FLAG_NONE
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;	//�R�}���h�L���[�̎�ނ�����D3D12_COMMAND_LIST_TYPE�񋓌^�̒l�B
	hr = m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue));
	if (FAILED(hr)) {
		MessageBox(NULL, L"�R�}���h�L���[�����̎��s", L"�G���[���b�Z�[�W", MB_OK);
	}

	//�X���b�v�`�F�[���쐬
	//�X���b�v�`�F�[���f�X�N�̐ݒ�
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	//�g�p����o�b�t�@�̐�
	swapChainDesc.BufferCount = BUFFER_COUNT;
	//�t���[���o�b�t�@�̕��ƍ����B�ʏ�̓E�B���h�E�̕`��̈�Ɠ����傫����ݒ�
	swapChainDesc.Width = SCREEN_WIDTH;							
	swapChainDesc.Height = SCREEN_HEIGHT;
	//�t���[���o�b�t�@�̃s�N�Z���t�H�[�}�b�g
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	
	//�t���[���o�b�t�@�̎g�p���@���w��
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	
	//��ʂ�؂�ւ�����A�`���ʂɂȂ����t���[���o�b�t�@�̈��������w��
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	//AA�ׂ̈̃T���v�����O�񐔂��w��BAA���g�p���Ȃ��ꍇ�A1���w��B
	swapChainDesc.SampleDesc.Count = 1;
	//�T���v�����O�̕i�����w��BAA���g�p���Ȃ��ꍇ�A0���w��B
	swapChainDesc.SampleDesc.Quality = 0;
	//�t���[���o�b�t�@�ƃE�B���h�E�̃T�C�Y���قȂ�ꍇ�̕\�����@���w��
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;

	ComPtr<IDXGISwapChain1> swapChain;
	//CreateSwapChainForHwnd::�X���b�v�`�F�[���̏o�̓E�B���h�E�ւ�HWND�n���h���Ɋ֘A�t����ꂽ�X���b�v�`�F�[�����쐬���܂��B
	hr = m_factry->CreateSwapChainForHwnd(
		m_commandQueue.Get(),
		GetWindow(),
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain);
	if (FAILED(hr)) {
		MessageBox(NULL, L"�X���b�v�`�F�[�������̎��s", L"�G���[���b�Z�[�W", MB_OK);
	}
	//�t���X�N���[���h�~
	hr = m_factry->MakeWindowAssociation(GetWindow(), DXGI_MWA_NO_ALT_ENTER);
	if (FAILED(hr)) {
		MessageBox(NULL, L"�X���b�v�`�F�[�������̎��s", L"�G���[���b�Z�[�W", MB_OK);
	}
	//SwapChain1��SwapChain3�Ɂ@SwapChain3�𒼐ڐ����ł��Ȃ����ۂ��H
	hr = swapChain.As(&m_swapChain);
	if (FAILED(hr)) {
		MessageBox(NULL, L"�X���b�v�`�F�[�������̎��s", L"�G���[���b�Z�[�W", MB_OK);
	}
	//�`�摤�t���[���o�b�t�@�̃C���f�b�N�X�擾
	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	// �f�B�X�N���v�^�q�[�v���쐬
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	//�i�[�ł���f�X�N���v�^�̐�
	rtvHeapDesc.NumDescriptors = FrameCount;
	//�f�X�N���v�^�q�[�v�̃^�C�v��ݒ�B�g���݂��ɉ�����4�̃^�C�v����ݒ�
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	//D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_NONE��ݒ�B�f�X�N���v�^���w���Ă���f�[�^���V�F�[�_����Q�Ƃ������ꍇ��D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE��ݒ�B
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	hr = m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap));
	//�����_�[�^�[�Q�b�g�p�f�X�N���v�^�̃T�C�Y���擾
	m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	//�����_�[�r���[�^�[�Q�b�g�̍쐬
	//�w���p�[���̍\����
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
	//�e�t���[���̃����_�[�r���[�^�[�Q�b�g�̍쐬
	for (UINT n = 0; n < FrameCount; n++) {
		hr = m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n]));
		if (FAILED(hr)) {
			MessageBox(NULL, L"�����_�[�r���[�^�[�Q�b�g�����̎��s", L"�G���[���b�Z�[�W", MB_OK);
		}
		m_device->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr,rtvHandle);
		rtvHandle.Offset(1, m_rtvDescriptorSize);
	}

	

	//���[�g�V�O�l�`���̍쐬
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	hr = (D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
	hr = (m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));

	// �V�F�[�_�[�̃R���p�C���Ɠǂݍ��݂��܂ރp�C�v���C����Ԃ��쐬
	ComPtr<ID3DBlob> vertexShader;
	ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
	// Enable better shader debugging with the graphics debugging tools.
	// �O���t�B�b�N �f�o�b�O �c�[�����g�p���āA���K�؂ȃV�F�[�_�[ �f�o�b�O��L���ɂ��܂��B
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

	hr = D3DCompileFromFile(L"shaders.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr);
	hr = D3DCompileFromFile(L"shaders.hlsl", nullptr, nullptr, "PSMain", "vs_5_0", compileFlags, 0, &pixelShader, nullptr);

	// ���_���̓��C�A�E�g���`���܂��B
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
		 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0} };

	// �O���t�B�b�N�X �p�C�v���C����ԃI�u�W�F�N�g (PSO) ���L�q���č쐬
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
