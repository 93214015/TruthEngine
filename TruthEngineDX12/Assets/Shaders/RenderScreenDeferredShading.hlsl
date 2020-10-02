
#include "HelperFunctions.hlsli"
#include "Lights.hlsli"
#include "CommonConstantBuffers.hlsli"
#include "CommonSamplers.hlsli"


#ifdef __INTELLISENSE__

#define DIRLIGHTNUM 0
#define SPOTLIGHTNUM 0

#endif


//
// Structs
//
struct vertexOut
{
    float4 posL : SV_POSITION;
    float2 posCS : TEXCOORD0;
    float2 texCoord : TEXCOORD1;
};




//
// Constants
//

ConstantBuffer<Constants_Unfrequent> CBUnfrequent : register(b0);
ConstantBuffer<Constants_PerFrame> CBPerFrame : register(b1);

ConstantBuffer<DirLight> dirLights[] : register(b2);


//
// Textures
//

Texture2D<float4> texColorBuffer  : register(t0);
Texture2D<float3> texNormalBuffer : register(t1);
Texture2D<float>  texDepthBuffer  : register(t2);

Texture2D<float>  texShadowMaps[] : register(t3);



//
// Vertex Shader
//

vertexOut vs(float3 pos : POSITION, float2 texCoord : TEXCOORD)
{
    vertexOut vout;
    vout.posL = float4(pos, 1.0f);
    vout.posCS = pos.xy;
    vout.texCoord = texCoord;
    return vout;
}




//
// Pixel Shader
//

float4 ps(vertexOut pin) : SV_Target
{
    
    float depthCS = texDepthBuffer.Sample(samplerPoint, pin.texCoord);
    
    if (depthCS > 0.9999)
    {
        discard;
    }
    
    float viewDepth = ConvertToLinearDepth_Perspective(depthCS, CBUnfrequent.perspectiveValues.w, CBUnfrequent.perspectiveValues.z);
    
    float3 posV = CalcViewPos(pin.posCS, viewDepth, CBUnfrequent.perspectiveValues);
    
    float3 posW = CalcWorldPos(posV, CBPerFrame.ViewInv);
    
    float4 color = texColorBuffer.Sample(samplerPoint, pin.texCoord);
    float3 normalW = texNormalBuffer.Sample(samplerPoint, pin.texCoord).xyz;
    
    float3 DiffuseColor = float3(0.0f, 0.0f, 0.0f);
    float3 AmbientColor = float3(0.0f, 0.0f, 0.0f);
    float3 SpecularColor = float3(0.0f, 0.0f, 0.0f);

    
    for (int i = 0; i < DIRLIGHTNUM; i++)
    {
        
        float3 LightVector = dirLights[i].Direction * -1;

        float DiffuseFactor = dot(normalW, LightVector);

        if (DiffuseFactor > 0.0f)
        {
            float shadowFactor = 1.0f;
        
            if (dirLights[i].CastShadow)
            {
                float4 posShadow = mul(float4(posW, 1.0f), dirLights[i].ShadowTransform);
            
                shadowFactor = CalcShadowFactor_BasicShadows(posShadow, texShadowMaps[dirLights[i].ShadowMapIndex]);
            }
            
            DiffuseColor += dirLights[i].Diffuse.xyz * DiffuseFactor * shadowFactor;
            //SpecularColor = IlluminateSpecular(ToEye, LightVector, NormalW, SpecPow);
        }
        

        AmbientColor += dirLights[i].Ambient.xyz;
        
    }
            
    return color * float4((DiffuseColor + AmbientColor), 1.0f);
    
}