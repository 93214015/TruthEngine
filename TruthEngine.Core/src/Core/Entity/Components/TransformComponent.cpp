#include "pch.h"
#include "TransformComponent.h"

namespace TruthEngine
{
	TransformComponent::TransformComponent() = default;

	TransformComponent::TransformComponent(const float4x4 & transformMatrix, const float3& _WorldCenterOffset)
		: m_Transform(transformMatrix), m_WorldCenterOffset(_WorldCenterOffset)
	{}

	TransformComponent& TransformComponent::operator=(TransformComponent&&) noexcept = default;

	TransformComponent::TransformComponent(TransformComponent&&) noexcept = default;

	TransformComponent::TransformComponent(const TransformComponent&) = default;

	TransformComponent& TransformComponent::operator=(const TransformComponent&) = default;

	TransformComponent::~TransformComponent() = default;

}