#pragma once


namespace TruthEngine
{
	struct UpdatedComponent
	{
		UpdatedComponent(bool _Updated)
			: m_Updated(_Updated)
		{}

		bool m_Updated;
	};
}