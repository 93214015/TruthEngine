#pragma once

namespace TruthEngine
{
	namespace Core
	{
		class Mesh;

		class MeshGenerator
		{
		public:
			MeshGenerator();

			Mesh* GenerateBox(float size);
			Mesh* GenerateRoundedBoxMesh(float size);
			Mesh* GenerateSphere(float size);
			Mesh* GenerateCylinder(float size);
			Mesh* GenerateCappedCylinder(float size);
			Mesh* GeneratePlane(float size);


			static MeshGenerator* GetInstance()
			{
				static MeshGenerator s_Instance;
				return &s_Instance;
			}
		private:

		private:

		};
	}
}
