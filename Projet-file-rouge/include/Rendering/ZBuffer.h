#pragma once

#include <cmath>
#include <vector>
#include <limits>

class ZBuffer
{
private :
    int width, height;
    std::vector <float> Zbuffer;
    
public:
    ZBuffer(int width, int height);
    void Clear();
    bool TestAndApply();
    
};
