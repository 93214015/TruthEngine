#include "pch.h"
#include "EventEntity.h"

namespace TruthEngine
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

	EventEntityTransform::EventEntityTransform(Entity _entity, ETransformType _type)
		: mEntity(_entity), mTransformType(_type)
	{}

}