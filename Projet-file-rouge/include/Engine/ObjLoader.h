#pragma once
#include <string>
#include "Mesh.h"

class ObjLoader {
public:
    static Mesh Load(const std::string& filename);
};