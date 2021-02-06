#pragma once

class FrameController {
    long previous_frame, current_frame, minimum_frame_duration;
public:
    FrameController(long refresh_rate);
    void wait_next_frame();
    long previous_frame_length();
};