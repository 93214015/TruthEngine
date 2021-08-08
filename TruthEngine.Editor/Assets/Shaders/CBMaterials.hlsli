///////////////////////////////////////////////////////////////////
//////////////// Data Structs
///////////////////////////////////////////////////////////////////
struct Material
{
    float4 Diffuse;
    
    float3 FresnelR0;
    float Shininess;
    
    float2 UVScale;
    float2 UVTranslate;
    
    uint MapIndexDiffuse;
    uint MapIndexNormal;
    uint MapIndexDisplacement;
    uint pad;
};


cbuffer CBMaterials : register(REGISTER_CBMaterials)
{
	Material MaterialArray[200];
}