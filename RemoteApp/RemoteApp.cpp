#include "RemoteApp.h"


void udp_connect_thread() {
    // 실제 UDP는 '연결'이 없지만, 예를 들어 수신측에 신호를 주거나, 핑-퐁 등으로 "연결됨"을 감지한다고 가정
    // 실제로는 바로 udpReady = true; 로 동작시켜도 됨
    // 수신측이 응답을 주면 set; 여기서는 2초 후 연결됐다고 가정
    std::this_thread::sleep_for(std::chrono::seconds(2));
    udpReady = true;
}

void capture_thread(DxgiImageCapture* cap, int w, int h) {
    while (!quit) {
        std::vector<uint8_t> frame;
        int width = w, height = h;
        if (cap->DxgiCaptureScreen(frame, width, height)) {
            std::unique_lock<std::mutex> lock(mtxRaw);
            rawFrames.push(std::move(frame));
            lock.unlock();
            cvRaw.notify_one();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(33)); // 30fps
    }
}

void encode_thread(int w, int h, int quality) {
    MjpegEncoder encoder(w, h, quality);
    while (!quit) {
        std::vector<uint8_t> frame;
        {   // 캡처 큐에서 꺼내기
            std::unique_lock<std::mutex> lock(mtxRaw);
            cvRaw.wait(lock, [] { return !rawFrames.empty() || quit; });
            if (quit) break;
            frame = std::move(rawFrames.front());
            rawFrames.pop();
        }
        // BGRA -> BGR
        cv::Mat bgra(h, w, CV_8UC4, frame.data());
        cv::Mat bgr; cv::cvtColor(bgra, bgr, cv::COLOR_BGRA2BGR);

        std::vector<uint8_t> encoded;
        encoder.EncodeFrame(bgr.data, encoded);

        {   // 압축 큐에 저장
            std::unique_lock<std::mutex> lock(mtxEnc);
            encodedFrames.push(std::move(encoded));
        }
        cvEnc.notify_one();
    }
}

void udp_send_thread(UdpSender* udp) {
    // udpReady 될 때까지 대기
    while (!udpReady && !quit)
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    while (!quit) {
        std::vector<uint8_t> encoded;
        {   // 압축 큐에서 꺼내기
            std::unique_lock<std::mutex> lock(mtxEnc);
            cvEnc.wait(lock, [] { return !encodedFrames.empty() || quit; });
            if (quit) break;
            encoded = std::move(encodedFrames.front());
            encodedFrames.pop();
        }
        if (udp->send(encoded))
            std::cout << "UDP 송신 성공 (" << encoded.size() << " bytes)\n";
        else
            std::cout << "UDP 송신 실패\n";
    }
}

int main() {
    DxgiImageCapture dxgiCapture;

    int w = 1920, h = 1080; // 실제 캡처 해상도
    UdpSender udp("192.168.0.2", 12345);
    // 1번: UDP 연결 준비
    std::thread t1(udp_connect_thread);

    // 2번: 캡처 스레드
    std::thread t2(capture_thread, &dxgiCapture, w, h);

    // 3번: 인코딩 스레드
    std::thread t3(encode_thread, w, h, 40);

    // 4번: UDP 전송 스레드
    std::thread t4(udp_send_thread, &udp);

    // 10초 후 종료 예시
    std::this_thread::sleep_for(std::chrono::seconds(10));
    quit = true;
    cvRaw.notify_all();
    cvEnc.notify_all();

    t1.join(); t2.join(); t3.join(); t4.join();
    return 0;
}

