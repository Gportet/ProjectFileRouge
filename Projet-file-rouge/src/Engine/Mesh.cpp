#include "Engine/Mesh.h"


void Mesh::ComputeFaceNormals() {
    faceNormals.clear();
    faceNormals.reserve(faces.size());

    for (const auto& face : faces) {
        const auto& v0 = vertices[face.v0];
        const auto& v1 = vertices[face.v1];
        const auto& v2 = vertices[face.v2];

        MathsSIMD::Vec3_SIMD e1 = v1 - v0;
        MathsSIMD::Vec3_SIMD e2 = v2 - v0;

        MathsSIMD::Vec3_SIMD normal = e1.Cross(e2).Normalize();
        faceNormals.push_back(normal);
    }
}

void Mesh::ComputeVertexNormals() {
    vertexNormals.assign(vertices.size(), MathsSIMD::Vec3_SIMD(0.0f, 0.0f, 0.0f));
    
    for (size_t i = 0; i < faces.size(); ++i) {
        const auto& face = faces[i];
        const auto& fNorm = faceNormals[i];

        vertexNormals[face.v0] = vertexNormals[face.v0] + fNorm;
        vertexNormals[face.v1] = vertexNormals[face.v1] + fNorm;
        vertexNormals[face.v2] = vertexNormals[face.v2] + fNorm;
    }
    
    for (auto& vn : vertexNormals) {
        if (vn.MagnitudeSquared() > 0.0f) {
            vn = vn.Normalize();
        }
    }
}

