#include "pch.h"
#include "TagComponent.h"

namespace TruthEngine
{
	TagComponent::TagComponent(const std::string& tag)
		: m_Tag(tag)
	{}

	TagComponent::TagComponent(const char* tag)
		: m_Tag(tag)
	{
	}

	TagComponent::TagComponent() = default;
	TagComponent::TagComponent(const TagComponent&) = default;
	TagComponent::TagComponent(TagComponent&&) noexcept = default;
	TagComponent::~TagComponent() = default;
	TagComponent& TagComponent::operator=(const TagComponent&) = default;
	TagComponent& TagComponent::operator=(TagComponent&&) noexcept = default;
}