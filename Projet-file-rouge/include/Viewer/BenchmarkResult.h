#pragma once
#include <string>

struct BenchmarkResult {
    std::string name;      // ex: "BM_Vec4Addition"
    double size;           // ex: 250000 (tiré du paramètre)
    double cyclesPerOp;    // ex: 3.42
    bool selected = true;  // Utilisé plus tard pour cocher/décocher dans ImGui
};