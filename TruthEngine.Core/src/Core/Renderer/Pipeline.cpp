#include "pch.h"
#include "Pipeline.h"

TruthEngine::Core::Pipeline::Pipeline(RendererStateSet states, Shader* shader)
	: m_States(states), m_Shader(shader)
{
}
