#pragma once
#include "MathLib/MathLib_SIMD/include/Matrix4x4_SIMD.h"
#include "MathLib/MathLib_SIMD/include/Vec3_SIMD.h"


namespace ECS
{
	struct CameraComponent
	{
		MathsSIMD::Vec3_SIMD eye;
		MathsSIMD::Vec3_SIMD target;
		MathsSIMD::Vec3_SIMD up;
		int fov = 90;
	};
}