#include "pch.h"
#include "TransformComponent.h"

namespace TruthEngine::Core
{
	TransformComponent::TransformComponent() = default;

	TransformComponent::TransformComponent(const float4x4 & transformMatrix)
		: m_Transform(transformMatrix)
	{}

	TransformComponent& TransformComponent::operator=(TransformComponent&&) noexcept = default;

	TransformComponent::TransformComponent(TransformComponent&&) noexcept = default;

	TransformComponent::TransformComponent(const TransformComponent&) = default;

	TransformComponent& TransformComponent::operator=(const TransformComponent&) = default;

	TransformComponent::~TransformComponent() = default;

}