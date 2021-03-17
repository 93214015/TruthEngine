#pragma once


enum class TE_IDX_GRESOURCES : IDX
{
	NONE,

	CBuffer_PerFrame,
	CBuffer_PerObject,
	CBuffer_PerMesh,
	CBuffer_LightData,
	CBuffer_Materials,
	CBuffer_UnFrequent,
	CBuffer_HDR_DownScaling,
	CBuffer_HDR_FinalPass,
	CBuffer_GaussianBlur,
	CBuffer_Bones,

	Buffer_HDRAverageLumFirstPass,
	Buffer_HDRAverageLumSecondPass0,
	Buffer_HDRAverageLumSecondPass1,

	Constant_DirectConstants,
	Constant_PerMesh,
	Constant_ShadowMapPerMesh,
	Constant_ShadowMapPerLight,

	Texture_MaterialTextures,
	Texture_CubeMap_Environment,
	Texture_RT_BackBuffer,
	Texture_RT_GBuffer_Color,
	Texture_RT_GBuffer_Normal,
	Texture_RT_GBuffer_Specular,
	Texture_RT_SceneBuffer,
	Texture_RT_SceneBufferHDR,
	Texture_DS_SceneBuffer,
	Texture_DS_ShadowMap,
	Texture_RW_DownScaledHDR,
	Texture_RW_Bloom,
	Texture_RW_BloomBlured,
	Texture_RW_BloomBluredHorz,
};


inline bool IsIDXTextureMaterialTexture(TE_IDX_GRESOURCES _IDX)
{
	return _IDX == TE_IDX_GRESOURCES::Texture_MaterialTextures;
}
inline bool IsIDXTextureCubeMap(TE_IDX_GRESOURCES _IDX)
{
	return _IDX == TE_IDX_GRESOURCES::Texture_CubeMap_Environment;
}
inline bool IsIDXTextureRenderTarget(TE_IDX_GRESOURCES _IDX)
{
	return (static_cast<uint32_t>(_IDX) >= static_cast<uint32_t>(TE_IDX_GRESOURCES::Texture_RT_BackBuffer) && static_cast<uint32_t>(_IDX) <= static_cast<uint32_t>(TE_IDX_GRESOURCES::Texture_RT_SceneBuffer));
}
inline bool IsIDXTextureDepthStencil(TE_IDX_GRESOURCES _IDX)
{
	return (static_cast<uint32_t>(_IDX) >= static_cast<uint32_t>(TE_IDX_GRESOURCES::Texture_DS_SceneBuffer) && static_cast<uint32_t>(_IDX) <= static_cast<uint32_t>(TE_IDX_GRESOURCES::Texture_DS_ShadowMap));
}

enum class TE_IDX_SHADERCLASS : IDX
{
	NONE,
	FORWARDRENDERING,
	GENERATEBASICSHADOWMAP,
	RENDERBOUNDINGBOX,
	RENDERENVIRONMENTMAP,
	POSTPROCESSING_HDR_DOWNSACLING_FIRSTPASS,
	POSTPROCESSING_HDR_DOWNSACLING_SECONDPASS,
	POSTPROCESSING_HDR_BLOOMPASS,
	POSTPROCESSING_HDR_FINALPASS,
	POSTPROCESSING_GAUSSIANBLUR_HORZ,
	POSTPROCESSING_GAUSSIANBLUR_VERT,
	TOTALNUM
};

enum class TE_IDX_RENDERPASS : IDX
{
	NONE,
	FORWARDRENDERING,
	GENERATEBASICSHADOWMAP,
	RENDERBOUNDINGBOX,
	POSTPROCESSING_HDR
};

enum class TE_IDX_MESH_TYPE : IDX
{
	MESH_POINT,
	MESH_SIMPLE,
	MESH_NTT,
	MESH_SKINNED,
	TOTALNUM
};