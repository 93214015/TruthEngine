#include "pch.h"
#include "RenderPassStack.h"

#include "RenderPass.h";

namespace TruthEngine 
{



    RenderPassStack::RenderPassStack()
    {
        m_Vector.reserve(10);
    }

    RenderPassStack::~RenderPassStack() = default;

    RenderPassStack::RenderPassStack(const RenderPassStack&) = default;

    RenderPassStack& RenderPassStack::operator=(const RenderPassStack&) = default;

    RenderPassStack::RenderPassStack(RenderPassStack&&) noexcept = default;

    RenderPassStack& RenderPassStack::operator=(RenderPassStack&&) noexcept = default;

    void RenderPassStack::PushRenderPass(RenderPass* renderPass, uint32_t placeIndex)
    {
        auto index = placeIndex == -1 ? m_Vector.size() : placeIndex;

        m_Vector.emplace(m_Vector.cbegin() + index, renderPass);

        renderPass->OnAttach();
    }

    void RenderPassStack::PopRenderPass(RenderPass* renderPass)
    {
        auto itr = std::find(m_Vector.cbegin(), m_Vector.cend(), renderPass);
        if (itr != m_Vector.cend())
        {
            renderPass->OnDetach();
            m_Vector.erase(itr);
        }
    }

    void RenderPassStack::PopAll()
    {
        for (auto rp : m_Vector)
        {
            rp->OnDetach();
        }

        m_Vector.clear();
    }

    std::vector<RenderPass*>::iterator RenderPassStack::begin()
    {
        return m_Vector.begin();
    }

    std::vector<RenderPass*>::iterator RenderPassStack::end()
    {
        return m_Vector.end();
    }

    std::vector<RenderPass*>::const_iterator RenderPassStack::begin() const
    {
        return m_Vector.begin();
    }

    std::vector<RenderPass*>::const_iterator RenderPassStack::end() const
    {
        return m_Vector.end();
    }

    std::vector<RenderPass*>::reverse_iterator RenderPassStack::rbegin()
    {
        return m_Vector.rbegin();
    }

    std::vector<RenderPass*>::reverse_iterator RenderPassStack::rend()
    {
        return m_Vector.rend();
    }

    std::vector<RenderPass*>::const_reverse_iterator RenderPassStack::rbegin() const
    {
        return m_Vector.rbegin();
    }

    std::vector<RenderPass*>::const_reverse_iterator RenderPassStack::rend() const
    {
        return m_Vector.rend();
    }

}