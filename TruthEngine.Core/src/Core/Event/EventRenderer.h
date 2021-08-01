#pragma once
#include "Event.h"

namespace TruthEngine
{

	class PipelineGraphics;
	class PipelineCompute;

	class EventRendererNewGraphicsPipeline : public Event
	{
	public:
		EventRendererNewGraphicsPipeline(PipelineGraphics* _Pipeline)
			: m_PipelineGraphics(_Pipeline)
		{}

		EVENT_CLASS_CATEGORY(EventCategoryRenderer);
		EVENT_CLASS_TYPE(RendererNewGraphicsPipeline);

		inline const PipelineGraphics* GetPipeline() const
		{
			return m_PipelineGraphics;
		}

	private:
		const PipelineGraphics* m_PipelineGraphics;
	};

	class EventRendererNewComputePipeline : public Event
	{
	public:
		EventRendererNewComputePipeline(PipelineCompute* _Pipeline)
			: m_PipelineGraphics(_Pipeline)
		{}

		EVENT_CLASS_CATEGORY(EventCategoryRenderer);
		EVENT_CLASS_TYPE(RendererNewComputePipeline);

		inline const PipelineCompute* GetPipeline() const
		{
			return m_PipelineGraphics;
		}

	private:
		const PipelineCompute* m_PipelineGraphics;
	};


}
