///////////////////////////////////////////////////////////////////
//////////////// Data Structs
///////////////////////////////////////////////////////////////////

//struct Material
//{
//    float4 Diffuse;
    
//    float3 FresnelR0;
//    float Shininess;
    
//    float2 UVScale;
//    float2 UVTranslate;
    
//    uint MapIndexDiffuse;
//    uint MapIndexNormal;
//    uint MapIndexDisplacement;
//    uint MapIndexSpecular;
//};

struct Material
{
    float4 Diffuse;
    
    float Roughness;
    float Metallic;
    float AmbientOcclusion;
    float Emission;
    
    float2 UVScale;
    float2 UVTranslate;
    
    uint MapIndexDiffuse;
    uint MapIndexNormal;
    uint MapIndexDisplacement;
    uint MapIndexSpecular;
    
    uint MapIndexRoughness;
    uint MapIndexMetallic;
    uint MapIndexAmbientOcclusion;
    uint EnabledSSR;
};


cbuffer CBMaterials : register(REGISTER_CBMaterials)
{
	Material MaterialArray[200];
}