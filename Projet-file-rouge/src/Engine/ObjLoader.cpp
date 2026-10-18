#include "Engine/ObjLoader.h"
#include <iostream>
#include <fstream>
#include <sstream>

Mesh ObjLoader::Load(const std::string& filename) {
    Mesh mesh;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Impossible d'ouvrir le fichier : " << filename << std::endl;
        return mesh;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "v") {
            float x, y, z;
            if (ss >> x >> y >> z) {
                mesh.vertices.push_back(MathsSIMD::Vec3_SIMD(x, y, z));
            }
        }
        else if (type == "f") {
            int v[3] = {0, 0, 0};
            bool valid = true;
            for (int i = 0; i < 3; ++i) {
                std::string token;
                if (ss >> token) {
                    std::stringstream token_stream(token);
                    if (!(token_stream >> v[i])) valid = false;
                } else {
                    valid = false;
                }
            }
            if (valid && v[0] > 0 && v[1] > 0 && v[2] > 0) {
                mesh.faces.push_back(Face{ v[0] - 1, v[1] - 1, v[2] - 1 });
            }
        }
    }
    return mesh;
}