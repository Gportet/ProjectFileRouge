#include "ECS/System/RenderSystem.h"

#include "ECS/Component/CameraComponent.h"
#include "ECS/Component/LightComponent.h"
#include "ECS/Component/MeshComponent.h"
#include "ECS/Component/TransformComponent.h"
#include "ECS/ComponentManager.h"
#include "ECS/EntityManager.h"
#include "Engine/Pipeline.h"

namespace ECS
{
    namespace
    {
        DirectionalLight ToDirectionalLight(const LightComponent& l)
        {
            DirectionalLight light;
            light.direction        = l.direction;
            light.ambient          = l.ambient;
            light.specularStrength = l.specularStrength;
            light.viewDir          = l.viewDir;
            return light;
        }

        // Ancienne boucle du main : géométrie, découpage near-plane, projection, culling, envoi au pipeline.
        void SubmitMesh(Pipeline& pipeline, const Mesh& mesh, Texture* texture,
                        const TransformComponent& transform, const CameraComponent& camera,
                        const DirectionalLight& sunLight)
        {
            const int width  = pipeline.GetWidth();
            const int height = pipeline.GetHeight();

            // La même rotation sert à la géométrie (MVP) et aux normales
            const MathsSIMD::Matrix4x4_SIMD mvp = MathsSIMD::Matrix4x4_SIMD::MVP_Matrix(
                transform.GetModelMatrix(), camera.eye, camera.target, camera.up, camera.fov, width, height);
            const MathsSIMD::Matrix4x4_SIMD rotModel = transform.GetRotationMatrix();

            const MathsSIMD::Vec3_SIMD halfVector = sunLight.GetHalfVector();

            for (size_t fidx = 0; fidx < mesh.faces.size(); ++fidx) {
                const auto& face = mesh.faces[fidx];

                const auto& v0_pos = mesh.vertices[face.v0];
                const auto& v1_pos = mesh.vertices[face.v1];
                const auto& v2_pos = mesh.vertices[face.v2];

                MathsSIMD::Vec4_SIMD v0_local(_mm_setr_ps(
                    MathsSIMD::Vec3_SIMD::GetX(v0_pos.data), MathsSIMD::Vec3_SIMD::GetY(v0_pos.data), MathsSIMD::Vec3_SIMD::GetZ(v0_pos.data), 1.0f
                ));
                MathsSIMD::Vec4_SIMD v1_local(_mm_setr_ps(
                    MathsSIMD::Vec3_SIMD::GetX(v1_pos.data), MathsSIMD::Vec3_SIMD::GetY(v1_pos.data), MathsSIMD::Vec3_SIMD::GetZ(v1_pos.data), 1.0f
                ));
                MathsSIMD::Vec4_SIMD v2_local(_mm_setr_ps(
                    MathsSIMD::Vec3_SIMD::GetX(v2_pos.data), MathsSIMD::Vec3_SIMD::GetY(v2_pos.data), MathsSIMD::Vec3_SIMD::GetZ(v2_pos.data), 1.0f
                ));

                ClipVertex cv0, cv1, cv2;

                cv0.clipPos = mvp * v0_local;
                cv1.clipPos = mvp * v1_local;
                cv2.clipPos = mvp * v2_local;

                cv0.normal = rotModel.TransformDirection(mesh.vertexNormals[face.v0]);
                cv1.normal = rotModel.TransformDirection(mesh.vertexNormals[face.v1]);
                cv2.normal = rotModel.TransformDirection(mesh.vertexNormals[face.v2]);

                cv0.uv = mesh.uvs[face.vt0];
                cv1.uv = mesh.uvs[face.vt1];
                cv2.uv = mesh.uvs[face.vt2];

                ClipTriangle clippedTris[2];
                int triCount = Pipeline::ClipTriangleNear(cv0, cv1, cv2, clippedTris);

                for (int tIdx = 0; tIdx < triCount; ++tIdx) {
                    const auto& tri = clippedTris[tIdx];

                    alignas(16) float raw0[4], raw1[4], raw2[4];
                    _mm_store_ps(raw0, tri.v0.clipPos.data);
                    _mm_store_ps(raw1, tri.v1.clipPos.data);
                    _mm_store_ps(raw2, tri.v2.clipPos.data);

                    float w0 = raw0[3], w1 = raw1[3], w2 = raw2[3];
                    if (w0 <= 0.0001f || w1 <= 0.0001f || w2 <= 0.0001f) continue;

                    float iw0 = 1.0f / w0;
                    float iw1 = 1.0f / w1;
                    float iw2 = 1.0f / w2;

                    float x0 = raw0[0] * iw0, y0 = raw0[1] * iw0, z0 = raw0[2] * iw0;
                    float x1 = raw1[0] * iw1, y1 = raw1[1] * iw1, z1 = raw1[2] * iw1;
                    float x2 = raw2[0] * iw2, y2 = raw2[1] * iw2, z2 = raw2[2] * iw2;

                    Point p0 = { static_cast<int>((x0 + 1.0f) * 0.5f * width), static_cast<int>((1.0f - y0) * 0.5f * height) };
                    Point p1 = { static_cast<int>((x1 + 1.0f) * 0.5f * width), static_cast<int>((1.0f - y1) * 0.5f * height) };
                    Point p2 = { static_cast<int>((x2 + 1.0f) * 0.5f * width), static_cast<int>((1.0f - y2) * 0.5f * height) };

                    if (pipeline.BackCulling(p0, p1, p2)) {
                        continue;
                    }

                    pipeline.SubmitTriangle(
                                    p0, p1, p2,
                                    z0, z1, z2,
                                    iw0, iw1, iw2,
                                    tri.v0.uv, tri.v1.uv, tri.v2.uv,
                                    tri.v0.normal, tri.v1.normal, tri.v2.normal,
                                    texture, sunLight, halfVector
                                );
                }
            }
        }
    }

    void RenderSystem(EntityManager& em, Pipeline& pipeline)
    {
        // 1) La caméra et la lumière : on garde la première trouvée de chaque
        const CameraComponent* camera = nullptr;
        em.ForAllEntities(MakeMask<CameraComponent>(), [&](Entity e)
        {
            if (camera == nullptr)
                camera = &componentManager.getComponent<CameraComponent>(e);
        });

        DirectionalLight sunLight;   // valeurs par défaut s'il n'y a pas de LightComponent
        bool lightFound = false;
        em.ForAllEntities(MakeMask<LightComponent>(), [&](Entity e)
        {
            if (!lightFound)
            {
                sunLight = ToDirectionalLight(componentManager.getComponent<LightComponent>(e));
                lightFound = true;
            }
        });

        // 2) Chaque entité qui a un Transform et un MeshComponent
        if (camera != nullptr)
        {
            em.ForAllEntities(MakeMask<TransformComponent, MeshComponent>(), [&](Entity e)
            {
                TransformComponent& transform = componentManager.getComponent<TransformComponent>(e);
                MeshComponent&      meshComp  = componentManager.getComponent<MeshComponent>(e);
                SubmitMesh(pipeline, meshComp.mesh, &meshComp.texture, transform, *camera, sunLight);
            });
        }

        // 3) Les threads du pipeline rasterisent tous les triangles envoyés
        pipeline.StartFramSync();
    }
}