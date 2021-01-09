#pragma once

namespace TruthEngine
{
	namespace Core
	{

		class TagComponent
		{
		public:
			TagComponent();
			TagComponent(const std::string& tag);

			~TagComponent();

			TagComponent(const TagComponent&);
			TagComponent& operator=(const TagComponent&);

			TagComponent(TagComponent&&)noexcept;
			TagComponent& operator=(TagComponent&&)noexcept;

			inline const std::string& GetTag() const noexcept
			{
				return m_Tag;
			}

		private:

		private:
			std::string m_Tag;
		};
	}
}
