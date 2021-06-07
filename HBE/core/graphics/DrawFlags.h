#pragma once
namespace HBE {
    enum {
        DRAW_FLAGS_NONE = 0,
        DRAW_FLAGS_CULL_FACE_BACK = 1,
        DRAW_FLAGS_CULL_FACE_FRONT = 1 << 1,
        DRAW_FLAGS_Z_TEST = 1 << 2,
        DRAW_FLAGS_TRANSPARENT = 1 << 3,
    };
    typedef uint32_t DRAW_FLAGS;
}