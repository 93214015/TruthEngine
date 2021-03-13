#pragma once

namespace TruthEngine
{

	
		class Shader;

	namespace API::DirectX12
	{


		enum class RootParameterDescriptorType
		{
			SRV = 0,
			UAV = (SRV + 1),
			CBV = (UAV + 1),
			SAMPLER = (CBV + 1)
		};

		enum class RootParameterType
		{
			Table,
			DirectConstant,
			Descriptor
		};

		struct DirectX12RootParameter
		{
			struct ParamTable
			{
				ParamTable() = default;
				ParamTable(uint32_t _register, uint32_t _registerSpace, uint32_t _numDescriptor, RootParameterDescriptorType _type)
					: Register(_register), RegisterSpace(_registerSpace), NumDescriptor(_numDescriptor), Type(_type)
				{}


				uint32_t Register;
				uint32_t RegisterSpace;
				uint32_t NumDescriptor;
				RootParameterDescriptorType Type;
			};
			struct ParamDirectConstant
			{
				ParamDirectConstant() = default;
				ParamDirectConstant(uint32_t _register, uint32_t _registerSpace, uint32_t _num32Bits)
					: Register(_register), RegisterSpace(_registerSpace), Num32Bits(_num32Bits)
				{}

				uint32_t Register;
				uint32_t RegisterSpace;
				uint32_t Num32Bits;
			};
			struct ParamDescriptor
			{
				ParamDescriptor() = default;
				ParamDescriptor(uint32_t _register, uint32_t _registerSpace, RootParameterDescriptorType _type)
					: Register(_register), RegisterSpace(_registerSpace), Type(_type)
				{}

				uint32_t Register;
				uint32_t RegisterSpace;
				RootParameterDescriptorType Type;
			};

			RootParameterType Type;
			union
			{
				ParamTable Table;
				ParamDirectConstant DirectConstant;
				ParamDescriptor Descriptor;
			};

		};

		struct DirectX12RootSignature
		{
			std::vector<DirectX12RootParameter> Parameters;
		};




		struct DirectX12RootArgumentTable
		{
			DirectX12RootArgumentTable() = default;
			DirectX12RootArgumentTable(uint32_t parameterIndex, uint32_t heapStartOffset, D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle)
				: ParameterIndex(parameterIndex), HeapStartOffset(heapStartOffset), GPUDescriptorHandle(gpuDescriptorHandle)
			{}


			uint32_t ParameterIndex = -1;
			uint32_t HeapStartOffset = -1;
			D3D12_GPU_DESCRIPTOR_HANDLE GPUDescriptorHandle;

		};

		struct DirectX12RootArgumentDirectConstant
		{
			DirectX12RootArgumentDirectConstant() = default;
			DirectX12RootArgumentDirectConstant(uint32_t parameterIndex)
				: ParameterIndex(parameterIndex)
			{}

			uint32_t ParameterIndex;
		};

		struct DirectX12RootArgumentDescriptor
		{
			DirectX12RootArgumentDescriptor() = default;
			DirectX12RootArgumentDescriptor(uint32_t _ParameterIndex)
				: mParameterIndex(_ParameterIndex)
			{}

			uint32_t mParameterIndex;
		};

		struct DirectX12RootArguments
		{
			std::vector<DirectX12RootArgumentTable> Tables;
			std::unordered_map<TE_IDX_GRESOURCES, DirectX12RootArgumentDirectConstant> DircectConstants;
			std::unordered_map<uint32_t, DirectX12RootArgumentDescriptor> DescriptorCBV;
			std::unordered_map<uint32_t, DirectX12RootArgumentDescriptor> DescriptorSRV;
			std::unordered_map<uint32_t, DirectX12RootArgumentDescriptor> DescriptorUAV;
		};




		class DirectX12Manager
		{
		public:
			TE_RESULT AddRootSignature(TE_IDX_SHADERCLASS shaderClassIDX);

			ID3D12RootSignature* GetD3D12RootSignature(TE_IDX_SHADERCLASS shaderClassIDX)
			{
				auto itr = m_ID3D12RootSignatures.find(shaderClassIDX);
				if (itr != m_ID3D12RootSignatures.end())
				{
					return itr->second.Get();
				}
				else
				{
					AddRootSignature(shaderClassIDX);
					itr = m_ID3D12RootSignatures.find(shaderClassIDX);
					if (itr == m_ID3D12RootSignatures.end())
					{
						throw;
					}
					return itr->second.Get();
				}
			}

			const DirectX12RootArguments* GetRootArguments(TE_IDX_SHADERCLASS shaderClassIDX, uint8_t frameIndex)
			{
				auto itr = m_DirectX12RootArguments.find(shaderClassIDX);
				if (itr != m_DirectX12RootArguments.end())
				{
					return &itr->second[frameIndex];
				}
				else
				{
					AddRootSignature(shaderClassIDX);
					itr = m_DirectX12RootArguments.find(shaderClassIDX);
					if (itr == m_DirectX12RootArguments.end())
					{
						throw;
					}
					return &itr->second[frameIndex];
				}
			}

			static std::shared_ptr<DirectX12Manager> GetInstance()
			{
				static auto s_Instance = std::make_shared<DirectX12Manager>();
				return s_Instance;
			}
		private:

		private:

			std::unordered_map<TE_IDX_SHADERCLASS, DirectX12RootSignature> m_DirectX12RootSignatures;
			std::unordered_map<TE_IDX_SHADERCLASS, std::vector<DirectX12RootArguments>> m_DirectX12RootArguments;
			std::unordered_map<TE_IDX_SHADERCLASS, COMPTR<ID3D12RootSignature>> m_ID3D12RootSignatures;

		};
	}
}