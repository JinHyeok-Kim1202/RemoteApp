#include "Encoder.h"

MjpegEncoder::MjpegEncoder(int w, int h, int quality)
    : width(w), height(h), quality(quality) {
}

MjpegEncoder::~MjpegEncoder() {}

void MjpegEncoder::EncodeFrame(const uint8_t* raw_bgr, std::vector<uint8_t>& encoded_out) {
    // raw_bgr(BGR24, ���ӵ� �޸�)�� cv::Mat�� wrap
    cv::Mat img(height, width, CV_8UC3, (void*)raw_bgr);
    std::vector<int> params = { cv::IMWRITE_JPEG_QUALITY, quality };
    // ".jpg"�� ���ڵ� (MJPEG)
    cv::imencode(".jpg", img, encoded_out, params);
}
