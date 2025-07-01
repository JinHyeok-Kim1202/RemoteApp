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
	// 캡처된 화면을 outFrame에 저장하고, 해상도를 width와 height에 저장합니다.
	// 성공하면 true, 실패하면 false를 반환합니다.
	// outFrame은 RGB24 포맷으로 저장됩니다.
	bool DxgiCaptureScreen(std::vector<uint8_t>& outFrame, int& width, int& height);
};

