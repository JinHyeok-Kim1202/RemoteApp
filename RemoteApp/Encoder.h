#pragma once
#include <vector>
#include <opencv2/opencv.hpp>

class MjpegEncoder {
public:
    int width, height, quality;
    MjpegEncoder(int w, int h, int quality = 50);
    ~MjpegEncoder();
    // 캡처된 BGR24 이미지(raw_bgr), 인코딩된 MJPEG 바이트 벡터(encoded_out)로 출력
    void EncodeFrame(const uint8_t* raw_bgr, std::vector<uint8_t>& encoded_out);
};