layout (binding = 4, set = 0) uniform Frame{
    float time;
    uint index;
    uint sample_count;
    uint max_bounces;
    float scattering_multiplier;
    float density_falloff;
} frame;
struct MaterialData
{
    vec4 albedo;
    vec4 emission;
    float roughness;
};

const float E =  2.71828182845904523536028747135266249;
const float PHI = 1.61803398874989484820459;// Φ = Golden Ratio
const float PI = 3.1415926535897932384626433832795;
const float TWO_PI = 2.0 * PI;