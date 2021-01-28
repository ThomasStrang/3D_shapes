#pragma once

class FrameController {
    long next_frame;
    long minimum_frame_duration;
public:
    FrameController(long refresh_rate);
    void wait_next_frame();
};