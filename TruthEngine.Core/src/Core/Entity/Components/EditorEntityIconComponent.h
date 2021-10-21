#pragma once

namespace TruthEngine
{
	class EditorEntityIconComponent
	{
	public:
		EditorEntityIconComponent(uint32_t _TextureMaterialIndex)
			: m_TextureMaterialIndex(_TextureMaterialIndex)
		{}

		uint32_t GetTextureMaterialIndex() { return m_TextureMaterialIndex; }
	private:
		uint32_t m_TextureMaterialIndex;
	};
}