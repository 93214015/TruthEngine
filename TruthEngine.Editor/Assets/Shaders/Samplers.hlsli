///////////////////////////////////////////////////
//////////////// Samplers
///////////////////////////////////////////////////
sampler sampler_linear : register(s0);
sampler sampler_point_borderBlack : register(s1);
sampler sampler_point_borderWhite : register(s2);
SamplerComparisonState samplerComparison_great_point_borderBlack : register(s3);
sampler sampler_point_wrap : register(s4);
SamplerComparisonState samplerComparison_less_point_borderWhite : register(s5);