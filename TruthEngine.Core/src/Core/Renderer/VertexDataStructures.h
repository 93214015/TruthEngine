#pragma once

namespace TruthEngine
{
	namespace VertexData
	{
		struct Pos 
		{
			Pos()
				: Position{ 0.0f, 0.0f, 0.0f }
			{}
			Pos(float3 position)
				: Position(position)
			{}

			float3 Position;
		};

		struct Tex 
		{
			Tex()
				: TexCoord{0.0f, 0.0f}
			{}
			Tex(float2 texCoord)
				: TexCoord(texCoord)
			{}

			float2 TexCoord;
		};

		struct PosTex 
		{
			PosTex()
				: Position{.0f, .0f, .0f}, TexCoord{.0f, .0f}
			{}
			PosTex(float3 position, float2 texCoord)
				: Position(position), TexCoord(texCoord)
			{}

			float3 Position;
			float2 TexCoord;
		};

		struct NormTanTex 
		{
			NormTanTex()
				: Normal{.0f, .0f, .0f}, Tangent{.0f, .0f, .0f}, TexCoord{.0f, .0f}
			{}
			NormTanTex(float3 normal, float3 tangent, float2 texCoord)
				: Normal(normal), Tangent(tangent), TexCoord(texCoord)
			{}

			float3 Normal;
			float3 Tangent;
			float2 TexCoord;
		};

		struct Bone
		{
			float3 BoneWeights = float3{.0f, .0f, .0f};
			byte     BoneIndex[4] = {0,0,0,0};
		};

		struct PosNormTex 
		{
			PosNormTex()
				: Position{.0f, .0f, .0f}, Normal{.0f, .0f, .0f}, TexCoord{.0f, .0f}
			{}
			PosNormTex(float3 position, float3 normal, float2 texCoord)
				: Position(position), Normal(normal), TexCoord(texCoord)
			{}

			DirectX::XMFLOAT3 Position;
			DirectX::XMFLOAT3 Normal;
			DirectX::XMFLOAT2 TexCoord;
		};
	}
}