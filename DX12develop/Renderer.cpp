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
	//�f�o�b�O ���C���[��L���ɂ��܂� (�O���t�B�b�N �c�[���́u�I�v�V�����@�\�v���K�v�ł�)�B
	//��: �f�o�C�X�̍쐬��Ƀf�o�b�O ���C���[��L���ɂ���ƁA�A�N�e�B�u�ȃf�o�C�X�B
	{
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
			debugController->EnableDebugLayer();

			// Enable additional debug layers.(�ǉ��̃f�o�b�O���C���[��L���ɂ��܂��B)
			//dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
	}
#endif

	//�t�@�N�g��-�����E�E�E�჌�x���Ȃ��̂ɃA�N�Z�X���邽�߂̃f�o�C�X�݂����Ȃ��̂炵��
	ComPtr<IDXGIFactory4> factory;
	if (FAILED(hr)) {
		MessageBox(NULL, L"�t�@�N�g���[�̍쐬�Ɏ��s���܂���", L"�G���[���b�Z�[�W", MB_OK);
	}

	//���[�v�A�_�v�^�E�E�E�悭�킩���
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
	hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_device));
	if (FAILED(hr)) {
		MessageBox(NULL, L"Direct3D�C���^�[�t�F�[�X�̍쐬�Ɏ��s���܂���", L"�G���[���b�Z�[�W", MB_OK);
	}

	
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
	hr = factory->CreateSwapChainForHwnd(
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
	hr = factory->MakeWindowAssociation(GetWindow(), DXGI_MWA_NO_ALT_ENTER);
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

	// �L�q�q�q�[�v���쐬
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
