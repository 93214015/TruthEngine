cbuffer CBBoneTransforms : register(REGISTER_CBBoneTransforms)
{
    row_major matrix gBoneTransformations[256];
};