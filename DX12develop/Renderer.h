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
	//�i�[�ł���f�X�N���v�^�̐��i�f�X�N���v�^�q�[�v���z��ׁ̈A�z�񐔂̎w�肪�K�v�j�B�ʏ탌���_�[�^�[�Q�b�g�̓t���[���o�b�t�@��1�F1�ɂȂ�悤�ɍ쐬����
	static const UINT FrameCount = 2;
	//�f�o�C�X�|�C���^
	ComPtr<ID3D12Device5> m_device;
	//�X���b�v�`�F�[��
	ComPtr<IDXGISwapChain3> m_swapChain;
	//�R�}���h�L���[
	ComPtr<ID3D12CommandQueue> m_commandQueue;
	//���[�v�A�_�v�^���g�����ǂ���
	bool m_use_warp_device = false;
	//�����_�[�^�[�Q�b�g�f�X�N���v�^
	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	//�����_�[�^�[�Q�b�g�p�f�X�N���v�^�̃T�C�Y
	UINT m_rtvDescriptorSize;
	//�e�t���[���ɑ΂��Ẵ����_�[�r���[�^�[�Q�b�g
	ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
	//�R�}���h�A���P�[�^
	ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	//���[�g�V�O�l�`��
	ComPtr<ID3D12RootSignature> m_rootSignature;
	//�p�C�v���C���X�e�[�g
	ComPtr<ID3D12PipelineState> m_pipelineState;
	//�R�}���h���X�g
	ComPtr<ID3D12GraphicsCommandList4> m_commandList;

	// �����I�u�W�F�N�g�B
	//�`�摤�t���[���o�b�t�@�̃C���f�b�N�X
	UINT m_frameIndex;
	HANDLE m_fenceEvent;
	ComPtr<ID3D12Fence> m_fence;
	UINT64 m_fenceValue;
};


