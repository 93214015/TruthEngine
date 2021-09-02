#pragma once
#include "Core/Entity/Model/MeshHandle.h"

namespace TruthEngine
{
		class Mesh;

		class MeshGenerator
		{
		public:
			MeshGenerator();

			MeshHandle GenerateBox(const float3& size, const int3& segments);
			MeshHandle GenerateRoundedBoxMesh(float radius, const float3& size, int slices, const int3& segments);
			MeshHandle GenerateSphere(float radius, int slices, int segments);
			MeshHandle GenerateCylinder(float radius, float size, int slices, int segments);
			MeshHandle GenerateCappedCylinder(float radius, float size, int slices, int segments, int rings);
			MeshHandle GeneratePlane(const float2& size, const int2& segments);


			static MeshGenerator* GetInstance()
			{
				static MeshGenerator s_Instance;
				return &s_Instance;
			}
		private:

		private:

		};
}
