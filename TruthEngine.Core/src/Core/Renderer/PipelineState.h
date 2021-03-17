#pragma once

using RendererStateSet = uint32_t;
using RendererShaderIDSet = uint32_t;


#define DX12_CULL_MODE(x) static_cast<D3D12_CULL_MODE>(x)
#define DX12_FILL_MODE(x) static_cast<D3D12_FILL_MODE>(x)
#define DX12_PRIMITIVE_TOPOLOGY(x) static_cast<D3D_PRIMITIVE_TOPOLOGY>(x)


#define GET_RENDERER_STATE(x, y) ((x >> BIT_POS_##y) & ((1 << BIT_NUM_##y) - 1))
#define SET_RENDERER_STATE(states, stateName, stateValue) states = ((states & ~(BIT_MASK_##stateName) ) | (stateValue << BIT_POS_##stateName))


#define BIT_NUM_TE_RENDERER_STATE_ENABLED_SHADER_HS 1
enum TE_RENDERER_STATE_ENABLED_SHADER_HS : RendererStateSet
{
	TE_RENDERER_STATE_ENABLED_SHADER_HS_FALSE = 0,
	TE_RENDERER_STATE_ENABLED_SHADER_HS_TRUE = 1
};


#define BIT_NUM_TE_RENDERER_STATE_ENABLED_SHADER_DS 1
enum TE_RENDERER_STATE_ENABLED_SHADER_DS : RendererStateSet
{
	TE_RENDERER_STATE_ENABLED_SHADER_DS_FALSE = 0,
	TE_RENDERER_STATE_ENABLED_SHADER_DS_TRUE = 1

};


#define BIT_NUM_TE_RENDERER_STATE_ENABLED_SHADER_GS 1
enum TE_RENDERER_STATE_ENABLED_SHADER_GS : RendererStateSet
{
	TE_RENDERER_STATE_ENABLED_SHADER_GS_FALSE = 0,
	TE_RENDERER_STATE_ENABLED_SHADER_GS_TRUE = 1
};


#define BIT_NUM_TE_RENDERER_STATE_FRONTCOUNTERCLOCKWISE 1
enum TE_RENDERER_STATE_FRONTCOUNTERCLOCKWISE : RendererStateSet
{
	TE_RENDERER_STATE_FRONTCOUNTERCLOCKWISE_FALSE = 0,
	TE_RENDERER_STATE_FRONTCOUNTERCLOCKWISE_TRUE = 1
};


#define BIT_NUM_TE_RENDERER_STATE_ENABLED_DEPTH 1
enum TE_RENDERER_STATE_ENABLED_DEPTH : RendererStateSet
{
	TE_RENDERER_STATE_ENABLED_DEPTH_FALSE = 0,
	TE_RENDERER_STATE_ENABLED_DEPTH_TRUE = 1
};


#define BIT_NUM_TE_RENDERER_STATE_ENABLED_STENCIL 1
enum TE_RENDERER_STATE_ENABLED_STENCIL : RendererStateSet
{
	TE_RENDERER_STATE_ENABLED_STENCIL_FALSE = 0,
	TE_RENDERER_STATE_ENABLED_STENCIL_TRUE = 1
};


#define BIT_NUM_TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE 1
enum TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE : RendererStateSet
{
	TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE_FALSE = 0,
	TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE_TRUE = 1
};


#define BIT_NUM_TE_RENDERER_STATE_ENABLED_MAP_NORMAL 1
enum TE_RENDERER_STATE_ENABLED_MAP_NORMAL : RendererStateSet
{
	TE_RENDERER_STATE_ENABLED_MAP_NORMAL_FALSE = 0,
	TE_RENDERER_STATE_ENABLED_MAP_NORMAL_TRUE = 1
};

#define BIT_NUM_TE_RENDERER_STATE_ENABLED_MAP_DISPLACEMENT 1
enum TE_RENDERER_STATE_ENABLED_MAP_DISPLACEMENT : RendererStateSet
{
	TE_RENDERER_STATE_ENABLED_MAP_DISPLACEMENT_FALSE = 0,
	TE_RENDERER_STATE_ENABLED_MAP_DISPLACEMENT_TRUE = 1
};

#define BIT_NUM_TE_RENDERER_STATE_CULL_MODE 2
enum TE_RENDERER_STATE_CULL_MODE : RendererStateSet
{
	TE_RENDERER_STATE_CULL_MODE_BACK = 0,
	TE_RENDERER_STATE_CULL_MODE_FRONT = 1,
	TE_RENDERER_STATE_CULL_MODE_NONE = 2
};

#define BIT_NUM_TE_RENDERER_STATE_FILL_MODE 1
enum TE_RENDERER_STATE_FILL_MODE : RendererStateSet
{
	TE_RENDERER_STATE_FILL_MODE_SOLID = 0,
	TE_RENDERER_STATE_FILL_MODE_WIREFRAME = 1,
};


#define BIT_NUM_TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY 4
enum TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY : RendererStateSet
{
	TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY_UNDEFINED = 0,
	TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY_POINTLIST = 1,
	TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY_LINELIST = 2,
	TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY_LINESTRIP = 3,
	TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY_TRIANGLELIST = 4,
	TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP = 5,
	TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY_LINELIST_ADJ = 10,
	TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ = 11,
	TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ = 12,
	TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ = 13
};

#define BIT_NUM_TE_RENDERER_STATE_COMPARISSON_FUNC 3
enum TE_RENDERER_STATE_COMPARISSON_FUNC : RendererStateSet
{
	TE_RENDERER_STATE_COMPARISSON_FUNC_NEVER = 1,
	TE_RENDERER_STATE_COMPARISSON_FUNC_LESS = 2,
	TE_RENDERER_STATE_COMPARISSON_FUNC_EQUAL = 3,
	TE_RENDERER_STATE_COMPARISSON_FUNC_LESS_EQUAL = 4,
	TE_RENDERER_STATE_COMPARISSON_FUNC_GREATER = 5,
	TE_RENDERER_STATE_COMPARISSON_FUNC_NOT_EQUAL = 6,
	TE_RENDERER_STATE_COMPARISSON_FUNC_GREATER_EQUAL = 7,
	TE_RENDERER_STATE_COMPARISSON_FUNC_ALWAYS = 8
};

#define BIT_NUM_TE_RENDERER_STATE_DEPTH_WRITE_MASK 1
enum TE_RENDERER_STATE_DEPTH_WRITE_MASK
{
	TE_RENDERER_STATE_DEPTH_WRITE_MASK_ZERO = 0,
	TE_RENDERER_STATE_DEPTH_WRITE_MASK_ALL = 1,
};


//enum class 

#define WRITE_ENUM_BIT_POS(state, stateBefore) BIT_POS_##state = BIT_POS_##stateBefore + BIT_NUM_##stateBefore

enum BIT_POS : RendererStateSet
{
	BIT_POS_TE_RENDERER_STATE_ENABLED_SHADER_HS = 0,

	WRITE_ENUM_BIT_POS(TE_RENDERER_STATE_ENABLED_SHADER_DS, TE_RENDERER_STATE_ENABLED_SHADER_HS),

	WRITE_ENUM_BIT_POS(TE_RENDERER_STATE_ENABLED_SHADER_GS, TE_RENDERER_STATE_ENABLED_SHADER_DS),

	WRITE_ENUM_BIT_POS(TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE, TE_RENDERER_STATE_ENABLED_SHADER_GS),

	WRITE_ENUM_BIT_POS(TE_RENDERER_STATE_ENABLED_MAP_NORMAL, TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE),

	WRITE_ENUM_BIT_POS(TE_RENDERER_STATE_ENABLED_MAP_DISPLACEMENT, TE_RENDERER_STATE_ENABLED_MAP_NORMAL),

	WRITE_ENUM_BIT_POS(TE_RENDERER_STATE_FRONTCOUNTERCLOCKWISE, TE_RENDERER_STATE_ENABLED_MAP_DISPLACEMENT),

	WRITE_ENUM_BIT_POS(TE_RENDERER_STATE_ENABLED_DEPTH, TE_RENDERER_STATE_FRONTCOUNTERCLOCKWISE),

	WRITE_ENUM_BIT_POS(TE_RENDERER_STATE_ENABLED_STENCIL, TE_RENDERER_STATE_ENABLED_DEPTH),

	WRITE_ENUM_BIT_POS(TE_RENDERER_STATE_FILL_MODE, TE_RENDERER_STATE_ENABLED_STENCIL),

	WRITE_ENUM_BIT_POS(TE_RENDERER_STATE_CULL_MODE, TE_RENDERER_STATE_FILL_MODE),

	WRITE_ENUM_BIT_POS(TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY, TE_RENDERER_STATE_CULL_MODE),

	WRITE_ENUM_BIT_POS(TE_RENDERER_STATE_COMPARISSON_FUNC, TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY),

	WRITE_ENUM_BIT_POS(TE_RENDERER_STATE_DEPTH_WRITE_MASK, TE_RENDERER_STATE_COMPARISSON_FUNC)

};



#define WRITE_ENUM_MASK(state) BIT_MASK_##state = ((1 << BIT_NUM_##state) - 1) << BIT_POS_##state

enum BIT_MASK : RendererStateSet
{
	WRITE_ENUM_MASK(TE_RENDERER_STATE_ENABLED_SHADER_HS),

	WRITE_ENUM_MASK(TE_RENDERER_STATE_ENABLED_SHADER_DS),

	WRITE_ENUM_MASK(TE_RENDERER_STATE_ENABLED_SHADER_GS),

	WRITE_ENUM_MASK(TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE),

	WRITE_ENUM_MASK(TE_RENDERER_STATE_ENABLED_MAP_NORMAL),

	WRITE_ENUM_MASK(TE_RENDERER_STATE_ENABLED_MAP_DISPLACEMENT),

	WRITE_ENUM_MASK(TE_RENDERER_STATE_FRONTCOUNTERCLOCKWISE),

	WRITE_ENUM_MASK(TE_RENDERER_STATE_ENABLED_DEPTH),

	WRITE_ENUM_MASK(TE_RENDERER_STATE_ENABLED_STENCIL),

	WRITE_ENUM_MASK(TE_RENDERER_STATE_FILL_MODE),

	WRITE_ENUM_MASK(TE_RENDERER_STATE_CULL_MODE),

	WRITE_ENUM_MASK(TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY),

	WRITE_ENUM_MASK(TE_RENDERER_STATE_COMPARISSON_FUNC),

	WRITE_ENUM_MASK(TE_RENDERER_STATE_DEPTH_WRITE_MASK),

};

constexpr RendererStateSet InitRenderStates(TE_RENDERER_STATE_ENABLED_SHADER_HS enabledHS = TE_RENDERER_STATE_ENABLED_SHADER_HS_FALSE
	, TE_RENDERER_STATE_ENABLED_SHADER_DS enabledDS = TE_RENDERER_STATE_ENABLED_SHADER_DS_FALSE
	, TE_RENDERER_STATE_ENABLED_SHADER_GS enabledGS = TE_RENDERER_STATE_ENABLED_SHADER_GS_FALSE
	, TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE enabledDiffuseMap = TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE_FALSE
	, TE_RENDERER_STATE_ENABLED_MAP_NORMAL enabledMapNormal = TE_RENDERER_STATE_ENABLED_MAP_NORMAL_FALSE
	, TE_RENDERER_STATE_ENABLED_MAP_DISPLACEMENT enabledDisplacementMap = TE_RENDERER_STATE_ENABLED_MAP_DISPLACEMENT_FALSE
	, TE_RENDERER_STATE_FRONTCOUNTERCLOCKWISE frontClockWise = TE_RENDERER_STATE_FRONTCOUNTERCLOCKWISE_FALSE
	, TE_RENDERER_STATE_ENABLED_DEPTH enabledDepth = TE_RENDERER_STATE_ENABLED_DEPTH_TRUE
	, TE_RENDERER_STATE_ENABLED_STENCIL enabledStencil = TE_RENDERER_STATE_ENABLED_STENCIL_FALSE
	, TE_RENDERER_STATE_FILL_MODE fillMode = TE_RENDERER_STATE_FILL_MODE_SOLID
	, TE_RENDERER_STATE_CULL_MODE cullMode = TE_RENDERER_STATE_CULL_MODE_BACK
	, TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY primitiveTopology = TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	, TE_RENDERER_STATE_COMPARISSON_FUNC comparisonFunc = TE_RENDERER_STATE_COMPARISSON_FUNC_LESS_EQUAL
	, TE_RENDERER_STATE_DEPTH_WRITE_MASK depthWriteMask = TE_RENDERER_STATE_DEPTH_WRITE_MASK_ALL)
{
	RendererStateSet states = 0;

	SET_RENDERER_STATE(states, TE_RENDERER_STATE_ENABLED_SHADER_HS, enabledHS);
	SET_RENDERER_STATE(states, TE_RENDERER_STATE_ENABLED_SHADER_DS, enabledDS);
	SET_RENDERER_STATE(states, TE_RENDERER_STATE_ENABLED_SHADER_GS, enabledGS);
	SET_RENDERER_STATE(states, TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE, enabledDiffuseMap);
	SET_RENDERER_STATE(states, TE_RENDERER_STATE_ENABLED_MAP_NORMAL, enabledMapNormal);
	SET_RENDERER_STATE(states, TE_RENDERER_STATE_ENABLED_MAP_DISPLACEMENT, enabledDisplacementMap);
	SET_RENDERER_STATE(states, TE_RENDERER_STATE_FRONTCOUNTERCLOCKWISE, frontClockWise);
	SET_RENDERER_STATE(states, TE_RENDERER_STATE_ENABLED_DEPTH, enabledDepth);
	SET_RENDERER_STATE(states, TE_RENDERER_STATE_ENABLED_STENCIL, enabledStencil);
	SET_RENDERER_STATE(states, TE_RENDERER_STATE_FILL_MODE, fillMode);
	SET_RENDERER_STATE(states, TE_RENDERER_STATE_CULL_MODE, cullMode);
	SET_RENDERER_STATE(states, TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY, primitiveTopology);
	SET_RENDERER_STATE(states, TE_RENDERER_STATE_COMPARISSON_FUNC, comparisonFunc);
	SET_RENDERER_STATE(states, TE_RENDERER_STATE_DEPTH_WRITE_MASK, depthWriteMask);


	return states;
}

constexpr RendererStateSet InitRenderStates_CCW(TE_RENDERER_STATE_ENABLED_SHADER_HS enabledHS = TE_RENDERER_STATE_ENABLED_SHADER_HS_FALSE
	, TE_RENDERER_STATE_ENABLED_SHADER_DS enabledDS = TE_RENDERER_STATE_ENABLED_SHADER_DS_FALSE
	, TE_RENDERER_STATE_ENABLED_SHADER_GS enabledGS = TE_RENDERER_STATE_ENABLED_SHADER_GS_FALSE
	, TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE enabledDiffuseMap = TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE_FALSE
	, TE_RENDERER_STATE_ENABLED_MAP_NORMAL enabledMapNormal = TE_RENDERER_STATE_ENABLED_MAP_NORMAL_FALSE
	, TE_RENDERER_STATE_ENABLED_MAP_DISPLACEMENT enabledDisplacementMap = TE_RENDERER_STATE_ENABLED_MAP_DISPLACEMENT_FALSE
	, TE_RENDERER_STATE_FRONTCOUNTERCLOCKWISE frontClockWise = TE_RENDERER_STATE_FRONTCOUNTERCLOCKWISE_TRUE
	, TE_RENDERER_STATE_ENABLED_DEPTH enabledDepth = TE_RENDERER_STATE_ENABLED_DEPTH_TRUE
	, TE_RENDERER_STATE_ENABLED_STENCIL enabledStencil = TE_RENDERER_STATE_ENABLED_STENCIL_FALSE
	, TE_RENDERER_STATE_FILL_MODE fillMode = TE_RENDERER_STATE_FILL_MODE_SOLID
	, TE_RENDERER_STATE_CULL_MODE cullMode = TE_RENDERER_STATE_CULL_MODE_BACK
	, TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY primitiveTopology = TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	, TE_RENDERER_STATE_COMPARISSON_FUNC comparisonFunc = TE_RENDERER_STATE_COMPARISSON_FUNC_LESS_EQUAL
	, TE_RENDERER_STATE_DEPTH_WRITE_MASK depthWriteMask = TE_RENDERER_STATE_DEPTH_WRITE_MASK_ALL)
{
	RendererStateSet states = 0;

	SET_RENDERER_STATE(states, TE_RENDERER_STATE_ENABLED_SHADER_HS, enabledHS);
	SET_RENDERER_STATE(states, TE_RENDERER_STATE_ENABLED_SHADER_DS, enabledDS);
	SET_RENDERER_STATE(states, TE_RENDERER_STATE_ENABLED_SHADER_GS, enabledGS);
	SET_RENDERER_STATE(states, TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE, enabledDiffuseMap);
	SET_RENDERER_STATE(states, TE_RENDERER_STATE_ENABLED_MAP_NORMAL, enabledMapNormal);
	SET_RENDERER_STATE(states, TE_RENDERER_STATE_ENABLED_MAP_DISPLACEMENT, enabledDisplacementMap);
	SET_RENDERER_STATE(states, TE_RENDERER_STATE_FRONTCOUNTERCLOCKWISE, frontClockWise);
	SET_RENDERER_STATE(states, TE_RENDERER_STATE_ENABLED_DEPTH, enabledDepth);
	SET_RENDERER_STATE(states, TE_RENDERER_STATE_ENABLED_STENCIL, enabledStencil);
	SET_RENDERER_STATE(states, TE_RENDERER_STATE_FILL_MODE, fillMode);
	SET_RENDERER_STATE(states, TE_RENDERER_STATE_CULL_MODE, cullMode);
	SET_RENDERER_STATE(states, TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY, primitiveTopology);
	SET_RENDERER_STATE(states, TE_RENDERER_STATE_COMPARISSON_FUNC, comparisonFunc);
	SET_RENDERER_STATE(states, TE_RENDERER_STATE_DEPTH_WRITE_MASK, depthWriteMask);


	return states;
}

enum TE_RENDERER_SHADER_STAGES : uint8_t
{
	TE_RENDERER_SHADER_STAGE_VS = BIT(0),
	TE_RENDERER_SHADER_STAGE_DS = BIT(1),
	TE_RENDERER_SHADER_STAGE_HS = BIT(2),
	TE_RENDERER_SHADER_STAGE_GS = BIT(3),
	TE_RENDERER_SHADER_STAGE_PS = BIT(4),
	TE_RENDERER_SHADER_STAGE_CS = BIT(5)
};

//#define TE_RENDERER_SHADERID_VS_BIT_NUM 5
//#define TE_RENDERER_SHADERID_HS_BIT_NUM 5
//#define TE_RENDERER_SHADERID_DS_BIT_NUM 5
//#define TE_RENDERER_SHADERID_GS_BIT_NUM 5
//#define TE_RENDERER_SHADERID_PS_BIT_NUM 5
//#define TE_RENDERER_SHADERID_CS_BIT_NUM 5
//
//
//
//enum TE_RENDERER_SHADERID_BIT_POS
//{
//	TE_RENDERER_SHADERID_VS_BIT_POS = 0,
//	TE_RENDERER_SHADERID_HS_BIT_POS = TE_RENDERER_SHADERID_VS_BIT_POS + TE_RENDERER_SHADERID_VS_BIT_NUM,
//	TE_RENDERER_SHADERID_DS_BIT_POS = TE_RENDERER_SHADERID_HS_BIT_POS + TE_RENDERER_SHADERID_HS_BIT_NUM,
//	TE_RENDERER_SHADERID_GS_BIT_POS = TE_RENDERER_SHADERID_DS_BIT_POS + TE_RENDERER_SHADERID_DS_BIT_NUM,
//	TE_RENDERER_SHADERID_PS_BIT_POS = TE_RENDERER_SHADERID_GS_BIT_POS + TE_RENDERER_SHADERID_GS_BIT_NUM,
//	TE_RENDERER_SHADERID_CS_BIT_POS = TE_RENDERER_SHADERID_PS_BIT_POS + TE_RENDERER_SHADERID_PS_BIT_NUM
//};