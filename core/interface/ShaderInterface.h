#pragma once
namespace HBE {
    enum SHADER_STAGE : uint8_t {
        SHADER_STAGE_NONE,
        SHADER_STAGE_COMPUTE,
        SHADER_STAGE_VERTEX,
        SHADER_STAGE_FRAGMENT,
        SHADER_STAGE_GEOMETRY,
        SHADER_STAGE_RAY_GEN,
        SHADER_STAGE_RAY_MISS,
        SHADER_STAGE_CLOSEST_HIT,
        SHADER_STAGE_ANY_HIT,
        SHADER_STAGE_INTERSECTION,
    };

    struct ShaderInfo {
        SHADER_STAGE stage = SHADER_STAGE_NONE;
        std::string path = "";
        std::string preamble = "";
    };
    using ShaderHandle = Handle;
}


