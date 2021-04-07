#pragma once

namespace TruthEngine
{
		class Mesh;

		class MeshGenerator
		{
		public:
			MeshGenerator();

			Mesh* GenerateBox(float size_x, float size_y, float size_z);
			Mesh* GenerateRoundedBoxMesh(float size_x, float size_y, float size_z);
			Mesh* GenerateSphere(float size);
			Mesh* GenerateCylinder(float size);
			Mesh* GenerateCappedCylinder(float size);
			Mesh* GeneratePlane(float size_x, float size_z);


			static MeshGenerator* GetInstance()
			{
				static MeshGenerator s_Instance;
				return &s_Instance;
			}
		private:

		private:

		};
}
