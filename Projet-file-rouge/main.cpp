#include "Engine/DisplayWindow.h"
#include "Engine/Pipeline.h"
#include "Engine/ObjLoader.h"
#include "Engine/Camera.h"
#include "MathLib/MathLib_SIMD/include/Vec4_SIMD.h"
#include <iostream>

const int WIDTH = 800;
const int HEIGHT = 800;

int main() {
    DisplayWindow window(WIDTH, HEIGHT, "Affichage Spot OBJ");
    if (!window.Init()) {
        std::cerr << "Erreur d'initialisation de la fenetre." << std::endl;
        return -1;
    }

    Mesh mesh = ObjLoader::Load("C:/Users/jcrap/Documents/GitHub/ProjectFileRouge/spot.obj");
    if (mesh.vertices.empty()) {
        std::cerr << "Erreur : Maillage vide ou non trouve." << std::endl;
        return -1;
    }
    
    Texture texture; 
    if (!Texture::texture_load("C:/Users/jcrap/Documents/GitHub/ProjectFileRouge/spot_texture.png", &texture))
    {
        std::cout << "Erreur d'initialisation de la texture." << std::endl;
        return 1;
    }
    
    Pipeline pipeline(WIDTH, HEIGHT);
    
    Camera camera(
        MathsSIMD::Vec3_SIMD(0.0f, 0.0f, 3.0f),
        MathsSIMD::Vec3_SIMD(0.0f, 0.0f, 0.0f),
        MathsSIMD::Vec3_SIMD(0.0f, 1.0f, 0.0f)
    );

    while (!window.ShouldClose()) {
        window.PollEvents();
        pipeline.Clear();

        float time = static_cast<float>(window.GetTime());
        MathsSIMD::Matrix4x4_SIMD mvp = camera.GetMVPMatrix(time, WIDTH, HEIGHT);

        for (const auto& face : mesh.faces) {
            const auto& v0 = mesh.vertices[face.v0];
            const auto& v1 = mesh.vertices[face.v1];
            const auto& v2 = mesh.vertices[face.v2];
            
            MathsSIMD::Vec4_SIMD v0_local(_mm_setr_ps(
                MathsSIMD::Vec3_SIMD::GetX(v0.data),
                MathsSIMD::Vec3_SIMD::GetY(v0.data),
                MathsSIMD::Vec3_SIMD::GetZ(v0.data),
                1.0f
            ));
            MathsSIMD::Vec4_SIMD v1_local(_mm_setr_ps(
                MathsSIMD::Vec3_SIMD::GetX(v1.data),
                MathsSIMD::Vec3_SIMD::GetY(v1.data),
                MathsSIMD::Vec3_SIMD::GetZ(v1.data),
                1.0f
            ));
            MathsSIMD::Vec4_SIMD v2_local(_mm_setr_ps(
                MathsSIMD::Vec3_SIMD::GetX(v2.data),
                MathsSIMD::Vec3_SIMD::GetY(v2.data),
                MathsSIMD::Vec3_SIMD::GetZ(v2.data),
                1.0f
            ));

            MathsSIMD::Vec4_SIMD v0_clip = mvp * v0_local;
            MathsSIMD::Vec4_SIMD v1_clip = mvp * v1_local;
            MathsSIMD::Vec4_SIMD v2_clip = mvp * v2_local;
            

            alignas(16) float raw0[4], raw1[4], raw2[4];
            _mm_store_ps(raw0, v0_clip.data);
            _mm_store_ps(raw1, v1_clip.data);
            _mm_store_ps(raw2, v2_clip.data);

            float w0 = raw0[3], w1 = raw1[3], w2 = raw2[3];
            if (w0 == 0.0f || w1 == 0.0f || w2 == 0.0f) continue;

            float x0 = raw0[0] / w0, y0 = raw0[1] / w0, z0 = raw0[2] / w0;
            float x1 = raw1[0] / w1, y1 = raw1[1] / w1, z1 = raw1[2] / w1;
            float x2 = raw2[0] / w2, y2 = raw2[1] / w2, z2 = raw2[2] / w2;

            Point p0 = { static_cast<int>((x0 + 1.0f) * 0.5f * WIDTH),
                         static_cast<int>((1.0f - y0) * 0.5f * HEIGHT) };
            Point p1 = { static_cast<int>((x1 + 1.0f) * 0.5f * WIDTH),
                         static_cast<int>((1.0f - y1) * 0.5f * HEIGHT) };
            Point p2 = { static_cast<int>((x2 + 1.0f) * 0.5f * WIDTH),
                         static_cast<int>((1.0f - y2) * 0.5f * HEIGHT) };

            // Récupération des UV de chaque sommet
            Vec2 uv0 = mesh.uvs[face.vt0];
            Vec2 uv1 = mesh.uvs[face.vt1];
            Vec2 uv2 = mesh.uvs[face.vt2];

            // Inverses de w pour la correction de perspective
            float iw0 = 1.0f / w0;
            float iw1 = 1.0f / w1;
            float iw2 = 1.0f / w2;
            
            if (pipeline.BackCulling(p0, p1, p2)) {
                continue;
            }
            
            pipeline.DrawTriangle(
                p0, p1, p2,
                z0, z1, z2,
                iw0, iw1, iw2,
                uv0, uv1, uv2,
                &texture
            );
        }

        window.RenderBuffer(pipeline.GetColorBuffer());
    }

    return 0;
}