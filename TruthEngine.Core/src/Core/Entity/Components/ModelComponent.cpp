#include "pch.h"
#include "ModelComponent.h"

#include "Core/Entity/Components.h"

namespace TruthEngine
{
    ModelComponent::ModelComponent()
    {
        m_EnitiesMesh.reserve(10);
    }

    ModelComponent::ModelComponent(const std::vector<Entity>& _MeshEntities)
        : m_EnitiesMesh(_MeshEntities)
    {}
}