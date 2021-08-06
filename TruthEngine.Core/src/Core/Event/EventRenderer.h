#pragma once
#include "Event.h"

namespace TruthEngine
{

	class PipelineGraphics;
	class PipelineCompute;

	class EventRendererTextureResize : public Event
	{
	public:
		EventRendererTextureResize(const uint32_t width, const uint32_t height, TE_IDX_GRESOURCES texturedIDX)
			: m_Width(width), m_Height(height), m_TextureIDX(texturedIDX)
		{}

		uint32_t GetWidth() const noexcept { return m_Width; }
		uint32_t GetHeight() const noexcept { return m_Height; }
		TE_IDX_GRESOURCES GetIDX()const noexcept { return m_TextureIDX; }


		EVENT_CLASS_TYPE(RendererTextureResize)
		EVENT_CLASS_CATEGORY(EventCategoryRenderer)

	protected:
		uint32_t m_Width, m_Height;
		TE_IDX_GRESOURCES m_TextureIDX;
	};

	class EventRendererViewportResize : public Event
	{
	public:
		EventRendererViewportResize(const uint32_t _Width, const uint32_t _Height)
			: m_Width(_Width), m_Height(_Height)
		{}

		uint32_t GetWidth() const noexcept { return m_Width; }
		uint32_t GetHeight() const noexcept { return m_Height; }

		EVENT_CLASS_TYPE(RendererViewportResize)
		EVENT_CLASS_CATEGORY(EventCategoryRenderer)

	private:
		uint32_t m_Width, m_Height;
	};

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
