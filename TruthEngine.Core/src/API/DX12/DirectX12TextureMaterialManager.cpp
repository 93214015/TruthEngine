#include "pch.h"
#include "DirectX12TextureMaterialManager.h"

#include <boost/filesystem.hpp>

#include "Core/Renderer/TextureMaterial.h"

#include "API/DX12/DirectX12BufferManager.h"
#include "API/DX12/DirectX12GraphicDevice.h"

#include "DirectXTK12/Inc/ResourceUploadBatch.h"
#include "DirectXTK12/Inc/WICTextureLoader.h"
#include "DirectXTK12/Inc/DDSTextureLoader.h"

using namespace TruthEngine;

namespace TruthEngine::API::DirectX12
{


	TextureMaterial* DirectX12TextureMaterialManager::CreateTexture(const char* name, uint8_t* data, uint32_t width, uint32_t height, uint32_t dataSize, TE_RESOURCE_FORMAT format)
	{
		auto index = static_cast<uint32_t>(m_Textures.size());

		auto tex = m_Textures.emplace_back(std::make_shared<TextureMaterial>(index, name, "", data, width, height, dataSize, format));
		m_Map_Textures[name] = tex.get();

		auto bufferManager = static_cast<DirectX12BufferManager*>(TE_INSTANCE_BUFFERMANAGER);

		auto d3d12device = static_cast<DirectX12GraphicDevice*>(TE_INSTANCE_GRAPHICDEVICE)->GetDevice();
		DirectX::ResourceUploadBatch uploadBatch(d3d12device);

		uploadBatch.Begin(D3D12_COMMAND_LIST_TYPE_COPY);
		tex->m_ResourceIndex = static_cast<uint32_t>(bufferManager->m_Resources.size());
		auto resource = std::addressof(bufferManager->m_Resources.emplace_back());
		DirectX::CreateWICTextureFromMemoryEx(d3d12device, uploadBatch, tex->GetData(), tex->GetDataSize(), 0, D3D12_RESOURCE_FLAG_NONE, DirectX::WIC_LOADER_DEFAULT, resource->ReleaseAndGetAddressOf());
		uploadBatch.End(TE_INSTANCE_API_DX12_COMMANDQUEUECOPY->GetNativeObject());

		CreateTextureView(tex.get());

		return tex.get();
	}


	TextureMaterial* DirectX12TextureMaterialManager::CreateTexture(const char* _texturefilePath, const char* _modelFilePath)
	{

		auto index = static_cast<uint32_t>(m_Textures.size());

		boost::filesystem::path textureFilePath(_texturefilePath);
		boost::filesystem::path modelFilePath(_modelFilePath);
		std::string fileName = textureFilePath.filename().string();

		std::string extension = textureFilePath.extension().string();
		if (extension == ".tga" || extension == ".tga")
		{
			TE_LOG_CORE_ERROR("DX12TextureMaterialManager: The TGA files are not supported at the moment!");
			return nullptr;
		}

		std::string fullPath;

		if (textureFilePath.is_relative())
		{
			if (!modelFilePath.empty())
				fullPath = modelFilePath.make_preferred().parent_path().string() + "\\" + textureFilePath.make_preferred().string();
			else
				fullPath = textureFilePath.make_preferred().string();
		}
		else
		{
			fullPath = textureFilePath.string();
		}

		auto bufferManager = static_cast<DirectX12BufferManager*>(TE_INSTANCE_BUFFERMANAGER);

		auto d3d12device = static_cast<DirectX12GraphicDevice*>(TE_INSTANCE_GRAPHICDEVICE)->GetDevice();
		DirectX::ResourceUploadBatch uploadBatch(d3d12device);

		uploadBatch.Begin(D3D12_COMMAND_LIST_TYPE_COPY);
		auto resourceIndex = static_cast<uint32_t>(bufferManager->m_Resources.size());
		auto resource = std::addressof(bufferManager->m_Resources.emplace_back());
		HRESULT hr = DirectX::CreateWICTextureFromFileEx(d3d12device, uploadBatch, to_wstring(std::string_view(fullPath)).c_str(), 0, D3D12_RESOURCE_FLAG_NONE, DirectX::WIC_LOADER_DEFAULT, resource->ReleaseAndGetAddressOf());

		if (FAILED(hr))
		{
			auto _erCode = GetLastError();
			throw;
		}

		uploadBatch.End(TE_INSTANCE_API_DX12_COMMANDQUEUECOPY->GetNativeObject());

		auto desc = (*resource)->GetDesc();


		auto tex = m_Textures.emplace_back(std::make_shared<TextureMaterial>(index, fileName.c_str(), fullPath.c_str(), nullptr, desc.Width, desc.Height, 0, static_cast<TE_RESOURCE_FORMAT>(desc.Format)));
		tex->m_ResourceIndex = resourceIndex;
		m_Map_Textures[fileName.c_str()] = tex.get();

		CreateTextureView(tex.get());

		return tex.get();
	}

	/*TextureCube* DirectX12TextureMaterialManager::CreateTextureCube(const char* _texturefilePath)
	{
		auto index = static_cast<uint32_t>(m_Textures.size());

		boost::filesystem::path textureFilePath(_texturefilePath);
		std::string fileName = textureFilePath.filename().string();

		auto bufferManager = static_cast<DirectX12BufferManager*>(TE_INSTANCE_BUFFERMANAGER);

		auto d3d12device = static_cast<DirectX12GraphicDevice*>(TE_INSTANCE_GRAPHICDEVICE)->GetDevice();
		DirectX::ResourceUploadBatch uploadBatch(d3d12device);

		uploadBatch.Begin(D3D12_COMMAND_LIST_TYPE_COPY);
		auto resourceIndex = static_cast<uint32_t>(bufferManager->m_Resources.size());
		auto resource = std::addressof(bufferManager->m_Resources.emplace_back());
		DirectX::CreateDDSTextureFromFileEx(d3d12device, uploadBatch, to_wstring(std::string_view(_texturefilePath)).c_str(), 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_DEFAULT, resource->ReleaseAndGetAddressOf());
		uploadBatch.End(TE_INSTANCE_API_DX12_COMMANDQUEUECOPY->GetNativeObject());

		auto desc = (*resource)->GetDesc();


		auto tex = m_TexturesCube.emplace_back(std::make_shared<TextureCube>(index, fileName.c_str(), _texturefilePath, nullptr, desc.Width, desc.Height, 0, static_cast<TE_RESOURCE_FORMAT>(desc.Format)));
		tex->m_ResourceIndex = resourceIndex;
		m_Map_TexturesCube[fileName.c_str()] = tex.get();

		CreateTextureView(tex.get());

		return tex.get();
	}*/


	CD3DX12_GPU_DESCRIPTOR_HANDLE DirectX12TextureMaterialManager::GetGPUHandle() const
	{
		auto bufferManager = static_cast<DirectX12BufferManager*>(TE_INSTANCE_BUFFERMANAGER);
		return bufferManager->m_DescHeapSRV.GetGPUHandle(m_DefaultOffset);
	}


	void DirectX12TextureMaterialManager::CreateTextureView(TextureMaterial* tex)
	{
		auto bufferManager = static_cast<DirectX12BufferManager*>(TE_INSTANCE_BUFFERMANAGER);

		auto& descHeap = bufferManager->m_DescHeapSRV;

		auto graphicDevice = static_cast<DirectX12GraphicDevice*>(GraphicDevice::GetPrimaryDevice());


		D3D12_SHADER_RESOURCE_VIEW_DESC desc{};
		desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.Format = static_cast<DXGI_FORMAT>(tex->m_Format);
		desc.Texture2D.MipLevels = tex->m_MipLevels;
		desc.Texture2D.MostDetailedMip = 0;

		auto d3d12Resource = bufferManager->GetResource(tex);

		tex->m_ViewIndex = m_CurrentIndex;

		auto d3d12device = graphicDevice->GetDevice();
		d3d12device->CreateShaderResourceView(d3d12Resource, &desc, descHeap.GetCPUHandle(m_CurrentIndex + m_DefaultOffset));

		m_CurrentIndex++;
	}

	/*void DirectX12TextureMaterialManager::CreateTextureView(TextureCube* tex)
	{
		auto bufferManager = static_cast<DirectX12BufferManager*>(TE_INSTANCE_BUFFERMANAGER);

		auto& descHeap = bufferManager->m_DescHeapSRV;

		auto graphicDevice = static_cast<DirectX12GraphicDevice*>(GraphicDevice::GetPrimaryDevice());

		D3D12_SHADER_RESOURCE_VIEW_DESC desc{};
		desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.Format = static_cast<DXGI_FORMAT>(tex->m_Format);
		desc.TextureCube.MipLevels = tex->m_MipLevels;
		desc.TextureCube.MostDetailedMip = 0;
		desc.TextureCube.ResourceMinLODClamp = 0.0f;

		auto d3d12Resource = bufferManager->GetResource(tex);

		tex->m_ViewIndex = descHeap.AddDescriptorSRV(d3d12Resource, &desc);

	}*/

}