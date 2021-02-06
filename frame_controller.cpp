#include "frame_controller.h"
#include <chrono>
#include <thread>
#include <iostream>

long long get_time() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

FrameController::FrameController(long refresh_rate) {
    minimum_frame_duration = (1000 / refresh_rate);
    previous_frame = get_time() + minimum_frame_duration;
    current_frame = previous_frame;
}

void FrameController::wait_next_frame() {
    long time_to_next_frame = (current_frame + minimum_frame_duration) - get_time();
    if (time_to_next_frame>0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(time_to_next_frame));
    }
    previous_frame = current_frame;
    current_frame = get_time();
}

long FrameController::previous_frame_length() {
    return current_frame - previous_frame;
}
