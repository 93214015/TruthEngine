#include "pch.h"
#include "MaterialManager.h"

#include "Core/Application.h"
#include "Core/Event/EventEntity.h"

namespace TruthEngine::Core
{

	void MaterialManager::Init(BufferManager* bufferManager)
	{
		m_BufferManager = bufferManager;
	}

	Material* MaterialManager::AddMaterial(
		 RendererStateSet states
		, uint8_t shaderProperties
		, float4 colorDiffuse
		, float4 colorAmbient
		, float4 colorSpecular
		, MaterialTexture* diffuseMap
		, MaterialTexture* normalMap
		, MaterialTexture* displacementMap
		, int32_t extraDepthBias
		, float extraSlopeScaledDepthBias
		, float extraDepthBiasClamp)
	{
		auto ID = static_cast<uint32_t>(m_Map_Materials.size());

		auto material = std::make_shared<Material>(ID, states, shaderProperties, colorDiffuse, colorAmbient, colorSpecular, diffuseMap, normalMap, displacementMap, extraDepthBias, extraSlopeScaledDepthBias, extraDepthBiasClamp);

		m_Map_Materials[ID] = material;
		m_Materials.push_back(material.get());

		EventEntityAddMaterial event(material.get());

		TE_INSTANCE_APPLICATION->OnEvent(event);		

		return material.get();
	}

	void MaterialManager::AddSampleMaterial()
	{
		AddMaterial(InitRenderStates(), 0, float4{}, float4{}, float4{}, nullptr, nullptr, nullptr, 0, 0.0f, 0.0f);
	}

}