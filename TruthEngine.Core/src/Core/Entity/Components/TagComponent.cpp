#include "pch.h"
#include "TagComponent.h"

namespace TruthEngine
{

	TagComponent::TagComponent(const char* tag)
	{
		size_t _STRLength = strlen(tag);
		TE_ASSERT_CORE(_STRLength < 16, "The Tag should have 15 character Length at most");
		strcpy_s(m_Tag, _STRLength + 1, tag);
	}

	TagComponent::TagComponent() = default;
	TagComponent::TagComponent(const TagComponent&) = default;
	TagComponent::TagComponent(TagComponent&&) noexcept = default;
	TagComponent::~TagComponent() = default;
	TagComponent& TagComponent::operator=(const TagComponent&) = default;
	TagComponent& TagComponent::operator=(TagComponent&&) noexcept = default;
}