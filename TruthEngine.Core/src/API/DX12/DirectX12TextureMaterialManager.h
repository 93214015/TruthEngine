#pragma once
#include "Core/Renderer/TextureMaterialManager.h"

namespace TruthEngine
{

	namespace API::DirectX12
	{
		class DirectX12BufferManager;

		class DirectX12TextureMaterialManager : public Core::TextureMaterialManager
		{
		public:
			uint32_t CreateTexture(const char* name, uint8_t* data, uint32_t width, uint32_t height, uint32_t dataSize, TE_RESOURCE_FORMAT format) override;
			uint32_t CreateTexture(const char* _texturefilePath, const char* _modelFilePath) override;


			uint32_t CreateTextureMaterialDiffuse(uint32_t texIndex) override;
			uint32_t CreateTextureMaterialNormal(uint32_t texIndex) override;
			uint32_t CreateTextureMaterialDisplacement(uint32_t texIndex) override;

			D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle_Diffuse()const;
			D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle_Normal()const;
			D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle_Displacement()const;
		protected:

		protected:
			const uint32_t m_DefaultOffset_Diffuse = 50;
			const uint32_t m_DefaultOffset_Normal = 400;
			const uint32_t m_DefaultOffset_Displacement = 700;

			uint32_t m_Index_Diffuse = 0;
			uint32_t m_Index_Normal = 0;
			uint32_t m_Index_Displacement = 0;
		};
	}
}
