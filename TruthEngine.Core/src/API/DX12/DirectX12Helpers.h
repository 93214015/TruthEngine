#pragma once

inline DXGI_FORMAT DX12_GET_FORMAT(const TE_RESOURCE_FORMAT format)
{
	return static_cast<DXGI_FORMAT>(format);
}

inline D3D12_RESOURCE_STATES DX12_GET_STATE(TE_RESOURCE_STATES x)
{
	return static_cast<D3D12_RESOURCE_STATES>(x);
}
