#include "pch.h"
#include "EventEntity.h"

namespace TruthEngine::Core
{
	EventEntityAddMesh::EventEntityAddMesh(Mesh* mesh)
		: m_Mesh(mesh)
	{}

	EventEntityAddMaterial::EventEntityAddMaterial(Material* material)
		: m_Material(material)
	{}

	EventEntityAddMaterial::~EventEntityAddMaterial() = default;


	EventEntityUpdateMaterial::EventEntityUpdateMaterial(Material* material)
		: m_Material(material)
	{}

	EventEntityUpdateMaterial::~EventEntityUpdateMaterial() = default;
}