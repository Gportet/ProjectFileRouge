#pragma once

#include <vector>

#include "Engine/utils/Filepath.h"

class Texture
{
private:
    Filepath path;
    std::vector<uint32_t> pixels;
    int width = 0;
    int height = 0;

public:
    
    bool Load(const Filepath& path);
    bool isLoaded() const;
    
    uint32_t SampleNearest(float u, float v) const;
    uint32_t SampleBilinear(float u, float v) const;
    
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    const Filepath& getPath() const { return path; };
    



};
