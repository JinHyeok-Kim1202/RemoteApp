#pragma once
#include <vector>
#include <opencv2/opencv.hpp>

class MjpegEncoder {
public:
    int width, height, quality;
    MjpegEncoder(int w, int h, int quality = 50);
    ~MjpegEncoder();
    // ĸó�� BGR24 �̹���(raw_bgr), ���ڵ��� MJPEG ����Ʈ ����(encoded_out)�� ���
    void EncodeFrame(const uint8_t* raw_bgr, std::vector<uint8_t>& encoded_out);
};