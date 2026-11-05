#pragma once
#include <string>
#include "Mesh.h"
#include "Engine/utils/Filepath.h"


class ObjLoader {
public:
    static Mesh Load(const Filepath& filepath);
};