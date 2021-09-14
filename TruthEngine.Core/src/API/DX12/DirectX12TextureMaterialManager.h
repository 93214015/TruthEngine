#pragma once
#include "Core/Renderer/TextureMaterialManager.h"

namespace TruthEngine
{

	namespace API::DirectX12
	{
		class DirectX12BufferManager;

		class DirectX12TextureMaterialManager : public TextureMaterialManager
		{
		public:
			TextureMaterial* CreateTexture(const char* name, uint8_t* data, uint32_t width, uint32_t height, uint32_t dataSize, TE_RESOURCE_FORMAT format) override;
			TextureMaterial* CreateTexture(const char* _texturefilePath, const char* _modelFilePath) override;

			CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandle()const;

			inline uint32_t GetIndexOffset()const noexcept
			{
				return m_DefaultOffset;
			}

			static DirectX12TextureMaterialManager* GetInstance()
			{
				static DirectX12TextureMaterialManager s_Instance;
				return &s_Instance;
			}

		protected:
			void CreateTextureView(TextureMaterial* tex);


		protected:
			const uint32_t m_DefaultOffset = 500;

			uint32_t m_CurrentIndex = 0;
		};
	}
}
