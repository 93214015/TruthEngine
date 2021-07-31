#include "pch.h"
#include "MeshHandle.h"

#include "ModelManager.h"

namespace TruthEngine
{
	Mesh& MeshHandle::GetMesh()
	{
		TE_INSTANCE_MODELMANAGER->GetMesh(m_MeshIndex);
	}
}