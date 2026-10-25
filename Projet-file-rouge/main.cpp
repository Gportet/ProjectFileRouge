#include "Engine/DisplayWindow.h"
#include "Engine/Pipeline.h"
#include "Engine/ObjLoader.h"
#include "Engine/Camera.h"
#include "Engine/Light.h"
#include "MathLib/MathLib_SIMD/include/Vec4_SIMD.h"
#include "MathLib/MathLib_SIMD/include/Matrix4x4_SIMD.h"
#include <iostream>
#include "Engine/ParticleGenerator.h"

// Resolution de la fenetre d'affichage


int main() {
    // =========================================================================
    // 1. INITIALISATION DU CONTEXTE ET DU MOTOR DE RENDU
    // =========================================================================
    DisplayWindow window(WIDTH, HEIGHT, "Rasterizer 3D - Smooth Phong & Near Clipping");
    if (!window.Init()) {
        std::cerr << "Erreur : Impossible d'initialiser la fenetre OpenGL/GLFW." << std::endl;
        return -1;
    }

    // Chargement du maillage 3D (.obj)
    Mesh mesh = ObjLoader::Load("C:/ProjectFileRouge/Projet-file-rouge/assets/spot.obj");
    if (mesh.vertices.empty()) {
        std::cerr << "Erreur : Maillage vide ou chemin introuvable." << std::endl;
        return -1;
    }

    // Configuration de la source lumineuse directionnelle
    DirectionalLight sunLight;

    // Chargement de la texture (Albedo)
    Texture texture; 
    if (!Texture::texture_load("C:/ProjectFileRouge/Projet-file-rouge/assets/spot_texture.png", &texture)) {
        std::cerr << "Erreur : Impossible de charger la texture." << std::endl;
        return 1;
    }
    
    // Pipeline de rendu logiciel (Color Buffer & Z-Buffer)
    Pipeline pipeline(WIDTH, HEIGHT);
    
    // Configuration de la camera virtuelle (Position Eye, Target, Up Vector)
    Camera camera(
        MathsSIMD::Vec3_SIMD(0.0f, 0.0f, 3.0f),
        MathsSIMD::Vec3_SIMD(0.0f, 0.0f, 0.0f),
        MathsSIMD::Vec3_SIMD(0.0f, 1.0f, 0.0f)
    );
    
    ParticleGenerator particleGenerator(500);
    double lastFrameTime = glfwGetTime();

    // =========================================================================
    // 2. BOUCLE PRINCIPALE DE RENDU (RENDER LOOP)
    // =========================================================================
    while (!window.ShouldClose()) {
        window.PollEvents();
        pipeline.Clear(); // Reinitialise les buffers de couleur et de profondeur

        //Delta
        double currentFrameTime = glfwGetTime();
        float dt = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;
        
        // Temps ecoule pour l'animation temporelle
        float time = static_cast<float>(window.GetTime());

        // Matrice de transformation MVP (Model-View-Projection)
        MathsSIMD::Matrix4x4_SIMD mvp = camera.GetMVPMatrix(time, WIDTH, HEIGHT);
        
        // Matrice de rotation globale pour l'orientation des normales en World Space
        MathsSIMD::Matrix4x4_SIMD rotModel = MathsSIMD::Matrix4x4_SIMD::RotationY(time * 0.7f);

        // Precalcul du Half-Vector (Blinn-Phong) unique pour toute la frame
        MathsSIMD::Vec3_SIMD halfVector = sunLight.GetHalfVector();

        // ---------------------------------------------------------------------
        // PASSE 1 : TRAITEMENT GEOMETRIQUE ET DECOUPAGE (GEOMETRY PASS)
        // ---------------------------------------------------------------------
        for (size_t fidx = 0; fidx < mesh.faces.size(); ++fidx) {
            const auto& face = mesh.faces[fidx];

            // Récupération des positions locales des sommets du triangle
            const auto& v0_pos = mesh.vertices[face.v0];
            const auto& v1_pos = mesh.vertices[face.v1];
            const auto& v2_pos = mesh.vertices[face.v2];
            
            // Conversion vers le format SIMD (Vec4 avec coordonnée homogene w = 1.0)
            MathsSIMD::Vec4_SIMD v0_local(_mm_setr_ps(
                MathsSIMD::Vec3_SIMD::GetX(v0_pos.data), MathsSIMD::Vec3_SIMD::GetY(v0_pos.data), MathsSIMD::Vec3_SIMD::GetZ(v0_pos.data), 1.0f
            ));
            MathsSIMD::Vec4_SIMD v1_local(_mm_setr_ps(
                MathsSIMD::Vec3_SIMD::GetX(v1_pos.data), MathsSIMD::Vec3_SIMD::GetY(v1_pos.data), MathsSIMD::Vec3_SIMD::GetZ(v1_pos.data), 1.0f
            ));
            MathsSIMD::Vec4_SIMD v2_local(_mm_setr_ps(
                MathsSIMD::Vec3_SIMD::GetX(v2_pos.data), MathsSIMD::Vec3_SIMD::GetY(v2_pos.data), MathsSIMD::Vec3_SIMD::GetZ(v2_pos.data), 1.0f
            ));
            
            // Assemblage des sommets de découpage avec leurs attributs associés
            ClipVertex cv0, cv1, cv2;

            // Transformation en Clip Space (Position avant la division perspective)
            cv0.clipPos = mvp * v0_local;
            cv1.clipPos = mvp * v1_local;
            cv2.clipPos = mvp * v2_local;

            // Transformation des normales de sommets dans l'Espace Monde (World Space)
            cv0.normal = rotModel.TransformDirection(mesh.vertexNormals[face.v0]);
            cv1.normal = rotModel.TransformDirection(mesh.vertexNormals[face.v1]);
            cv2.normal = rotModel.TransformDirection(mesh.vertexNormals[face.v2]);

            // Association des coordonnées de texture (UV)
            cv0.uv = mesh.uvs[face.vt0];
            cv1.uv = mesh.uvs[face.vt1];
            cv2.uv = mesh.uvs[face.vt2];

            // Découpage géométrique contre le plan proche (Near-Plane Clipping : z + w >= 0)
            // Évite la division par zéro et génère 0, 1 ou 2 triangles (Sutherland-Hodgman)
            ClipTriangle clippedTris[2];
            int triCount = Pipeline::ClipTriangleNear(cv0, cv1, cv2, clippedTris);
            
            // -----------------------------------------------------------------
            // PASSE 2 : PROJECTION ET RASTERISATION (RASTERIZATION PASS)
            // -----------------------------------------------------------------
            for (int tIdx = 0; tIdx < triCount; ++tIdx) {
                const auto& tri = clippedTris[tIdx];

                // Extraction des données SIMD alignées en mémoire
                alignas(16) float raw0[4], raw1[4], raw2[4];
                _mm_store_ps(raw0, tri.v0.clipPos.data);
                _mm_store_ps(raw1, tri.v1.clipPos.data);
                _mm_store_ps(raw2, tri.v2.clipPos.data);

                // Récupération de la profondeur de la caméra (w)
                float w0 = raw0[3], w1 = raw1[3], w2 = raw2[3];
                if (w0 <= 0.0001f || w1 <= 0.0001f || w2 <= 0.0001f) continue;

                // Précalcul des inverses de w pour la correction perspective
                float iw0 = 1.0f / w0;
                float iw1 = 1.0f / w1;
                float iw2 = 1.0f / w2;

                // Division perspective (Clip Space -> NDC : Normalized Device Coordinates)
                float x0 = raw0[0] * iw0, y0 = raw0[1] * iw0, z0 = raw0[2] * iw0;
                float x1 = raw1[0] * iw1, y1 = raw1[1] * iw1, z1 = raw1[2] * iw1;
                float x2 = raw2[0] * iw2, y2 = raw2[1] * iw2, z2 = raw2[2] * iw2;

                // Transformation Viewport (NDC [-1, 1] -> Coordonnées Écran Pixels)
                Point p0 = { static_cast<int>((x0 + 1.0f) * 0.5f * WIDTH), static_cast<int>((1.0f - y0) * 0.5f * HEIGHT) };
                Point p1 = { static_cast<int>((x1 + 1.0f) * 0.5f * WIDTH), static_cast<int>((1.0f - y1) * 0.5f * HEIGHT) };
                Point p2 = { static_cast<int>((x2 + 1.0f) * 0.5f * WIDTH), static_cast<int>((1.0f - y2) * 0.5f * HEIGHT) };

                // Élimination des faces arrière (Backface Culling)
                if (pipeline.BackCulling(p0, p1, p2)) {
                    continue;
                }

                // Dessin du triangle avec ombrage de Phong pixel par pixel
                pipeline.SubmitTriangle(
                                p0, p1, p2,
                                z0, z1, z2,
                                iw0, iw1, iw2,
                                tri.v0.uv, tri.v1.uv, tri.v2.uv,
                                tri.v0.normal, tri.v1.normal, tri.v2.normal,
                                &texture, sunLight, halfVector
                            );
            }
        }
        pipeline.StartFramSync();
        
        float centerX = static_cast<float>(WIDTH) * 0.5f;
        float centerY = static_cast<float>(HEIGHT) * 0.5f;

        float offsetX = static_cast<float>((rand() % 300) - 50);
        float offsetY = static_cast<float>((rand() % 300) - 50);

        Vec2 emitterPos{centerX, centerY };
        
        particleGenerator.Update(dt, ParticleType::Snow);
        particleGenerator.Update(dt, ParticleType::firework,emitterPos);
        particleGenerator.Draw(pipeline);
        // Transfert du Framebuffer vers la fenêtre et affichage
        window.RenderBuffer(pipeline.GetColorBuffer());
    }

    return 0;
}