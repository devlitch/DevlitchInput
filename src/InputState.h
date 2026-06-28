#pragma once

struct InputState {
    float leftX = 0.0f;
    float leftY = 0.0f;

    float rightX = 0.0f;
    float rightY = 0.0f;

    float leftTrigger = 0.0f;
    float rightTrigger = 0.0f;

    bool a = false;
    bool b = false;
    bool x = false;
    bool y = false;

    bool lb = false;
    bool rb = false;

    bool back = false;
    bool start = false;

    bool dpadUp = false;
    bool dpadDown = false;
    bool dpadLeft = false;
    bool dpadRight = false;

    bool ls = false;
    bool rs = false;
};