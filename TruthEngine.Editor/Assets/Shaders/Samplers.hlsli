///////////////////////////////////////////////////
//////////////// Samplers
///////////////////////////////////////////////////
sampler sampler_linear : register(s0);
sampler sampler_linear_clamp : register(s1);
sampler sampler_point_borderBlack : register(s2);
sampler sampler_point_borderWhite : register(s3);
SamplerComparisonState samplerComparison_great_point_borderBlack : register(s4);
sampler sampler_point_wrap : register(s5);
SamplerComparisonState samplerComparison_less_point_borderWhite : register(s6);