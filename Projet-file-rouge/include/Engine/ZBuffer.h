#pragma once

#include <vector>

class ZBuffer
{
private :
    int width, height;
    std::vector <float> depthBuffer;
    
public:
    ZBuffer(int width, int height);
    void Clear();
    bool TestAndApply(int x, int y, float depth);
    
};
