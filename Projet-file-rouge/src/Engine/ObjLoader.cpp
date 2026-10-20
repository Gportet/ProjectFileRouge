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
                mesh.vertices.push_back(MathsSIMD::Vec3_SIMD(x, y, z)); //mal au perf 
            }
        }
        else if (type == "vt") {
            float u, v;
            if (ss >> u >> v) {
                mesh.uvs.push_back(Vec2{ u, v });
            }
        }
        else if (type == "f") {
            Face face;
            bool valid = true;

            for (int i = 0; i < 3; ++i) {
                std::string token;
                if (ss >> token) {
                    std::stringstream token_stream(token);
                    std::string v_str, vt_str;
                    
                    std::getline(token_stream, v_str, '/');
                    std::getline(token_stream, vt_str, '/');

                    if (!v_str.empty()) {
                        int v_idx = std::stoi(v_str) - 1; // OBJ est basé sur 1
                        int vt_idx = !vt_str.empty() ? std::stoi(vt_str) - 1 : 0;

                        if (i == 0) { face.v0 = v_idx; face.vt0 = vt_idx; }
                        else if (i == 1) { face.v1 = v_idx; face.vt1 = vt_idx; }
                        else if (i == 2) { face.v2 = v_idx; face.vt2 = vt_idx; }
                    } else {
                        valid = false;
                    }
                } else {
                    valid = false;
                }
            }

            if (valid) {
                mesh.faces.push_back(face);
            }
        }
    }
    if (!mesh.faces.empty()) {
        mesh.ComputeFaceNormals();
    }

    return mesh;
}