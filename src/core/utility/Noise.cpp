//
// Created by user on 6/29/20.
//

#include "Noise.h"
#include "FastNoise.h"


Noise::Noise(int seed, int octaves, float lacunarity, float frequency) {
    this->seed = seed;
    noiser = new FastNoise(seed);
    noiser->SetFractalOctaves(octaves);
    noiser->SetFractalLacunarity(lacunarity);
    noiser->SetFrequency(frequency);
}

Noise::~Noise() {
    delete noiser;
}

float Noise::simplex(float x, float y, float z) {
    return noiser->GetSimplex(x, y, z);
}

float Noise::simplex(float x, float y) {
    return noiser->GetSimplex(x, y);
}

float Noise::fractal(float x, float y, float z) {
    return noiser->GetPerlinFractal(x, y, z);
}

float Noise::fractal(float x, float y) {
    return noiser->GetPerlinFractal(x, y);
}

float Noise::perlin(float x, float y, float z) {
    return noiser->GetPerlin(x, y, z);
}

float Noise::perlin(float x, float y) {
    return noiser->GetPerlin(x, y);
}






