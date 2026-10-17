#include "Rendering/ZBuffer.h"
#include <cmath>
#include <limits>


ZBuffer::ZBuffer(int width, int height) : width(width), height(height), depthBuffer(width * height)
{
    Clear();
}

void ZBuffer::Clear()
{
    std::fill(depthBuffer.begin(), depthBuffer.end(), std::numeric_limits<float>::infinity());
}

bool ZBuffer::TestAndApply(int x, int y, float depth)
{
    if ( x < 0 || x >= width || y < 0 || y >= height )
        return false;
    
    if (!std::isfinite(depth))
        return false;
    
    const int index = y * width + x;
    if (depth < depthBuffer[index])
    {
        depthBuffer[index] = depth;
        return true;
    }
    return false;
}
