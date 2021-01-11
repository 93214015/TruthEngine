#pragma once


enum class TE_IDX_CONSTANTBUFFER : IDX
{
	NONE,

	ARRAYS_PER_MESH,
	ARRAYS_MATERIALS,
	ARRAYS,

	PER_FRAME,
	PER_OBJECT,
	PER_MESH,
	PER_DLIGHT,
	MATERIALS,

	DIRECT_CONSTANTS,
	DIRECT_PER_MESH
};


enum class TE_IDX_TEXTURE : IDX
{
	MATERIALTEXTURE_DIFFUSE,
	MATERIALTEXTURE_NORMAL,
	MATERIALTEXTURE_DISPLACEMENT,
	DS_SHADOWMAP,
	RT_BACKBUFFER,
	RT_GBUFFER_COLOR,
	RT_GBUFFER_NORMAL,
	RT_GBUFFER_SPECULAR,
	RT_SCENEBUFFER,
	DS_SCENEBUFFER,
};


enum class TE_IDX_SHADERCLASS : IDX
{
	NONE,
	FORWARDRENDERING,

	TOTALNUM
};

enum class TE_IDX_RENDERPASS : IDX
{
	NONE,
	FORWARDRENDERING
};

enum class TE_IDX_MESH_TYPE : IDX
{
	MESH_POINT,
	MESH_SIMPLE,
	MESH_NTT,
	TOTALNUM
};