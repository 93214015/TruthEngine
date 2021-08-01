#pragma once
#include "Core/Entity/Model/MeshHandle.h"

namespace TruthEngine
{
		class Mesh;

		class MeshGenerator
		{
		public:
			MeshGenerator();

			MeshHandle GenerateBox(float size_x, float size_y, float size_z);
			MeshHandle GenerateRoundedBoxMesh(float size_x, float size_y, float size_z);
			MeshHandle GenerateSphere(float size);
			MeshHandle GenerateCylinder(float size);
			MeshHandle GenerateCappedCylinder(float size);
			MeshHandle GeneratePlane(float size_x, float size_z);


			static MeshGenerator* GetInstance()
			{
				static MeshGenerator s_Instance;
				return &s_Instance;
			}
		private:

		private:

		};
}
