#include "pch.h"
#include "MeshHandle.h"

#include "ModelManager.h"

namespace TruthEngine
{
	Mesh& MeshHandle::GetMesh()
	{
		return TE_INSTANCE_MODELMANAGER->GetMesh(m_MeshIndex);
	}

	const Mesh& MeshHandle::GetMesh() const
	{
		return TE_INSTANCE_MODELMANAGER->GetMesh(m_MeshIndex);
	}
}