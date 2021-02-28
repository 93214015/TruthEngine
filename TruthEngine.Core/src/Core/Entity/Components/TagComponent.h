#pragma once

namespace TruthEngine
{

		class TagComponent
		{
		public:
			TagComponent();
			TagComponent(const std::string& tag);
			TagComponent(const char* tag);

			~TagComponent();

			TagComponent(const TagComponent&);
			TagComponent& operator=(const TagComponent&);

			TagComponent(TagComponent&&)noexcept;
			TagComponent& operator=(TagComponent&&)noexcept;

			inline const std::string& GetTag() const noexcept
			{
				return m_Tag;
			}

			inline void SetTag(const char* buffer)
			{
				m_Tag = std::string(buffer);
			}

		private:

		private:
			std::string m_Tag;
		};
}
