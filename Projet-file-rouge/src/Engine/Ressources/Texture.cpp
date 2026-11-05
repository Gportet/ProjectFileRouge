#include "Engine/Ressources/Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <cmath>
#include <iostream>

bool Texture::Load(const Filepath& filepath)
{
    int _width, _height, _channels;

    uint8_t* data = stbi_load(
        filepath.str().c_str(),
        &width,
        &height,
        &_channels,
        4
    );

    if (!data)
        return false;

    path = filepath;
    _width = width;
    _height = height;

    pixels.resize(
        static_cast<size_t>(_width) *
        static_cast<size_t>(_height)
    );

    for (int i = 0; i < _width * _height; ++i)
    {
        const uint8_t r = data[i * 4 + 0];
        const uint8_t g = data[i * 4 + 1];
        const uint8_t b = data[i * 4 + 2];
        const uint8_t a = data[i * 4 + 3];

        pixels[i] =
            (static_cast<uint32_t>(a) << 24) |
            (static_cast<uint32_t>(r) << 16) |
            (static_cast<uint32_t>(g) << 8) |
            static_cast<uint32_t>(b);
    }

    stbi_image_free(data);

    return true;
}

bool Texture::isLoaded() const
{
    return !pixels.empty() &&
       width > 0 &&
       height > 0;
}

uint32_t Texture::SampleNearest(float u, float v) const
{
    if (!isLoaded())
        return 0;

    // Répétition de la texture
    u -= std::floor(u);
    v -= std::floor(v);

    const int x = static_cast<int>(u * width);
    const int y = static_cast<int>((1.0f - v) * height);

    const int clampedX = std::clamp(x, 0, width - 1);
    const int clampedY = std::clamp(y, 0, height - 1);

    return pixels[clampedY * width + clampedX];
}

uint32_t Texture::SampleBilinear(float u, float v) const
{
    if (!isLoaded())
        return 0;

    // Répétition de la texture
    u -= std::floor(u);
    v -= std::floor(v);

    // Coordonnées dans l'image
    const float x = u * width - 0.5f;
    const float y = (1.0f - v) * height - 0.5f;

    const int x0 = static_cast<int>(std::floor(x));
    const int y0 = static_cast<int>(std::floor(y));

    const int x1 = x0 + 1;
    const int y1 = y0 + 1;

    const float tx = x - x0;
    const float ty = y - y0;

    // Répétition aux frontières
    const int ix0 = (x0 % width + width) % width;
    const int ix1 = (x1 % width + width) % width;
    const int iy0 = (y0 % height + height) % height;
    const int iy1 = (y1 % height + height) % height;

    const uint32_t c00 = pixels[iy0 * width + ix0];
    const uint32_t c10 = pixels[iy0 * width + ix1];
    const uint32_t c01 = pixels[iy1 * width + ix0];
    const uint32_t c11 = pixels[iy1 * width + ix1];

    auto interpolate = [](float a, float b, float t)
    {
        return a * (1.0f - t) + b * t;
    };

    uint32_t result = 0;

    for (int shift : {0, 8, 16, 24})
    {
        const float a = (c00 >> shift) & 0xFF;
        const float b = (c10 >> shift) & 0xFF;
        const float c = (c01 >> shift) & 0xFF;
        const float d = (c11 >> shift) & 0xFF;

        const float top = interpolate(a, b, tx);
        const float bottom = interpolate(c, d, tx);

        const uint32_t channel = static_cast<uint32_t>(
            std::clamp(interpolate(top, bottom, ty), 0.0f, 255.0f)
        );

        result |= channel << shift;
    }

    return result;
}

