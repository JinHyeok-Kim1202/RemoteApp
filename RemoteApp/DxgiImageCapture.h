#include <vector>
#include <windows.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <wrl.h>
#pragma once

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

using Microsoft::WRL::ComPtr;

class DxgiImageCapture
{
public:
	DxgiImageCapture() = default;
	~DxgiImageCapture() = default;
	// ĸó�� ȭ���� outFrame�� �����ϰ�, �ػ󵵸� width�� height�� �����մϴ�.
	// �����ϸ� true, �����ϸ� false�� ��ȯ�մϴ�.
	// outFrame�� RGB24 �������� ����˴ϴ�.
	bool DxgiCaptureScreen(std::vector<uint8_t>& outFrame, int& width, int& height);
};

