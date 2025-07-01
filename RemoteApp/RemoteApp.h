#pragma once
#include "UdpSender.h"
#include "DxgiImageCapture.h"
#include "Encoder.h"

#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>

std::queue<std::vector<uint8_t>> rawFrames;      // 캡처된 원본 프레임
std::queue<std::vector<uint8_t>> encodedFrames;  // 압축된 프레임
std::mutex mtxRaw, mtxEnc;
std::condition_variable cvRaw, cvEnc;

std::atomic<bool> udpReady(false);
std::atomic<bool> quit(false);

void udp_connect_thread();
void capture_thread(DxgiImageCapture* cap, int w, int h);
void encode_thread(int w, int h, int quality);
void udp_send_thread(UdpSender* udp);