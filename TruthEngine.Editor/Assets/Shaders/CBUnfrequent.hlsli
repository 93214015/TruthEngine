cbuffer CBUnfrequent : register(REGISTER_CBUnfrequent)
{
    uint gDLightCount : packoffset(c0.x);
    uint gSLightCount : packoffset(c0.y);
    bool gEnabledEnvironmentMap : packoffset(c0.z);
    float pad0 : packoffset(c0.w);
	
    float3 gAmbientLightStrength : packoffset(c1.x);
    float pad1 : packoffset(c1.w);
}