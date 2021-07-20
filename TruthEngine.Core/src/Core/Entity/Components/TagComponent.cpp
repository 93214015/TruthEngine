#include "pch.h"
#include "TagComponent.h"

namespace TruthEngine
{

	TagComponent::TagComponent(const char* tag)
	{
		strcpy_s(m_Tag, strlen(tag), tag);
	}

	TagComponent::TagComponent() = default;
	TagComponent::TagComponent(const TagComponent&) = default;
	TagComponent::TagComponent(TagComponent&&) noexcept = default;
	TagComponent::~TagComponent() = default;
	TagComponent& TagComponent::operator=(const TagComponent&) = default;
	TagComponent& TagComponent::operator=(TagComponent&&) noexcept = default;
}