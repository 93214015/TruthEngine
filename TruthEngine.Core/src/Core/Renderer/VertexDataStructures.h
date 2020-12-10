#pragma once

namespace TruthEngine::Core
{
	namespace VertexData
	{
		struct Pos 
		{
			DirectX::XMFLOAT3 Position;
		};

		struct Tex 
		{
			DirectX::XMFLOAT2 TexCoord;
		};

		struct PosTex 
		{
			DirectX::XMFLOAT3 Position;
			DirectX::XMFLOAT2 TexCoord;
		};

		struct NormTex 
		{
			DirectX::XMFLOAT3 Normal;
			DirectX::XMFLOAT2 TexCoord;
		};

		struct PosNormTex 
		{
			DirectX::XMFLOAT3 Position;
			DirectX::XMFLOAT3 Normal;
			DirectX::XMFLOAT2 TexCoord;
		};
	}
}