#pragma once

namespace TruthEngine
{

		class TagComponent
		{
		public:
			TagComponent();
			TagComponent(const char* tag);

			~TagComponent();

			TagComponent(const TagComponent&);
			TagComponent& operator=(const TagComponent&);

			TagComponent(TagComponent&&)noexcept;
			TagComponent& operator=(TagComponent&&)noexcept;

			inline char* GetTag() noexcept
			{
				return m_Tag;
			}

			inline const char* GetTag() const noexcept
			{
				return m_Tag;
			}

			inline void SetTag(const char* buffer)
			{
				strcpy_s(m_Tag, strnlen(buffer, MAX_LENGTH), buffer);
			}

			inline static constexpr size_t GetTagMaxLength()
			{
				return MAX_LENGTH;
			}

		private:

			static constexpr size_t MAX_LENGTH = 32;

		private:
			char m_Tag[MAX_LENGTH];
		};
}
