#pragma once


class FastNoise;
class Noise {
    FastNoise* noiser;
    int seed=0;
public:
    Noise(int seed=0,int octaves=3,float lacunarity=2,float frequency=0.01);
    ~Noise();
    float simplex(float x,float y,float z);
    float simplex(float x,float y);

    float fractal(float x,float y,float z);
    float fractal(float x,float y);

    float perlin(float x,float y,float z);
    float perlin(float x,float y);

    float celular(float x,float y,float z);
    float celular(float x,float y);
};
