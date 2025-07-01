#include "DxgiImageCapture.h"

bool DxgiImageCapture::DxgiCaptureScreen(std::vector<uint8_t>& outFrame, int& width, int& height)
{
    HRESULT hr;
    ComPtr<ID3D11Device> d3dDevice;
    ComPtr<ID3D11DeviceContext> d3dContext;
    D3D_FEATURE_LEVEL featureLevel;
    hr = D3D11CreateDevice(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
        nullptr, 0, D3D11_SDK_VERSION,
        &d3dDevice, &featureLevel, &d3dContext);
    if (FAILED(hr)) return false;

    // DXGI Device
    ComPtr<IDXGIDevice> dxgiDevice;
    d3dDevice.As(&dxgiDevice);

    // DXGI Adapter
    ComPtr<IDXGIAdapter> dxgiAdapter;
    dxgiDevice->GetAdapter(&dxgiAdapter);

    // DXGI Output (Monitor 0)
    ComPtr<IDXGIOutput> dxgiOutput;
    dxgiAdapter->EnumOutputs(0, &dxgiOutput);

    // DXGI Output1
    ComPtr<IDXGIOutput1> dxgiOutput1;
    dxgiOutput.As(&dxgiOutput1);

    // Output Duplication
    ComPtr<IDXGIOutputDuplication> deskDupl;
    hr = dxgiOutput1->DuplicateOutput(d3dDevice.Get(), &deskDupl);
    if (FAILED(hr)) return false;

    // 모니터 해상도
    DXGI_OUTPUT_DESC outDesc;
    dxgiOutput->GetDesc(&outDesc);
    width = outDesc.DesktopCoordinates.right - outDesc.DesktopCoordinates.left;
    height = outDesc.DesktopCoordinates.bottom - outDesc.DesktopCoordinates.top;

    // 1프레임 캡처
    DXGI_OUTDUPL_FRAME_INFO frameInfo;
    ComPtr<IDXGIResource> desktopResource;
    hr = deskDupl->AcquireNextFrame(100, &frameInfo, &desktopResource);
    if (FAILED(hr)) return false;

    // IDXGIResource -> ID3D11Texture2D
    ComPtr<ID3D11Texture2D> acquiredTex;
    desktopResource.As(&acquiredTex);

    // CPU에서 읽을 수 있는 스테이징 텍스처 생성
    D3D11_TEXTURE2D_DESC desc;
    acquiredTex->GetDesc(&desc);
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    desc.Usage = D3D11_USAGE_STAGING;
    desc.BindFlags = 0;
    desc.MiscFlags = 0;
    ComPtr<ID3D11Texture2D> cpuTex;
    d3dDevice->CreateTexture2D(&desc, nullptr, &cpuTex);

    // GPU→CPU 복사
    d3dContext->CopyResource(cpuTex.Get(), acquiredTex.Get());

    // CPU에서 맵핑(메모리 포인터 얻기)
    D3D11_MAPPED_SUBRESOURCE mapped;
    hr = d3dContext->Map(cpuTex.Get(), 0, D3D11_MAP_READ, 0, &mapped);
    if (FAILED(hr)) return false;

    // BGRA 프레임 복사 (행 패딩 주의)
    outFrame.resize(width * height * 4);
    uint8_t* dst = outFrame.data();
    for (int y = 0; y < height; ++y)
        memcpy(dst + y * width * 4, (uint8_t*)mapped.pData + y * mapped.RowPitch, width * 4);

    d3dContext->Unmap(cpuTex.Get(), 0);
    deskDupl->ReleaseFrame(); // 꼭 해제!

    return true;
}