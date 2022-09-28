layout (binding = 4, set = 0) uniform Frame{
    float time;
    uint index;
    uint sample_count;
    uint max_bounces;
} frame;