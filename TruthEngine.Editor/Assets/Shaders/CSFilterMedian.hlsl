
#ifdef __INTELLISENSE__
#define TEX_TYPE float
#define SIZE 5
#define MAX_BIN_SIZE 100
#endif

#define ROW_SIZE (SIZE * 2 + 1)
#define KERNEL_SIZE (ROW_SIZE * ROW_SIZE)
#define MEDIAN_LIMIT ((KERNEL_SIZE / 2) + 1)

cbuffer CB_BlurMedian : register(b0)
{
	uint2 gTextureSize : packoffset(c0.x);
	uint2 _pad0 : packoffset(c0.z);
};

Texture2D<TEX_TYPE> gTexInput : register(t0);

RWTexture2D<TEX_TYPE> gTexOutput : register(u0);

groupshared TEX_TYPE Colors[ROW_SIZE + (2 * SIZE)][ROW_SIZE + (2 * SIZE)];

[numthreads(ROW_SIZE, ROW_SIZE, 1)]
void cs(uint3 GTid : SV_GroupThreadID, uint3 DTid : SV_DispatchThreadID)
{

	//Populating shared memory
	{
		Colors[GTid.y + SIZE][GTid.x + SIZE] = gTexInput.Load(int3(DTid.x, DTid.y, 0));

		bool _LeftEdge = false;
		if (GTid.x < SIZE)
		{
			_LeftEdge = true;
			Colors[GTid.y + SIZE][GTid.x] = gTexInput.Load(int3(DTid.x - SIZE, DTid.y, 0));
		}

		bool _TopEdge = false;
		if (GTid.y < SIZE)
		{
			_TopEdge = true;
			Colors[GTid.y][GTid.x + SIZE] = gTexInput.Load(int3(DTid.x, DTid.y - SIZE, 0));
		}


		bool _RightEdge = false;
		if (GTid.x > SIZE)
		{
			_RightEdge = true;
			Colors[GTid.y + SIZE][GTid.x + (2 * SIZE)] = gTexInput.Load(int3(DTid.x + SIZE, DTid.y, 0));
		}

		bool _BottomEdge = false;
		if (GTid.y > SIZE)
		{
			_BottomEdge = true;
			Colors[GTid.y + (2 * SIZE)][GTid.x + SIZE] = gTexInput.Load(int3(DTid.x, DTid.y + SIZE, 0));
		}


		if (_LeftEdge && _TopEdge)
		{
			Colors[GTid.y][GTid.x] = gTexInput.Load(int3(DTid.x - SIZE, DTid.y - SIZE, 0));
		}

		if (_LeftEdge && _BottomEdge)
		{
			Colors[GTid.y + (2 * SIZE)][GTid.x] = gTexInput.Load(int3(DTid.x - SIZE, DTid.y + SIZE, 0));
		}

		if (_RightEdge && _TopEdge)
		{
			Colors[GTid.y][GTid.x + (2 * SIZE)] = gTexInput.Load(int3(DTid.x + SIZE, DTid.y - SIZE, 0));
		}

		if (_RightEdge && _BottomEdge)
		{
			Colors[GTid.y + (2 * SIZE)][GTid.x + (2 * SIZE)] = gTexInput.Load(int3(DTid.x + SIZE, DTid.y + SIZE, 0));
		}
	}

	GroupMemoryBarrierWithGroupSync();

	if (DTid.x > gTextureSize.x || DTid.y > gTextureSize.y)
		return;

	uint _bins[MAX_BIN_SIZE];
	uint _binsIndex[ROW_SIZE][ROW_SIZE];

	for (uint i = 0; i < MAX_BIN_SIZE; i++)
	{
		_bins[i] = 0;
	}

#if (TEX_TYPE != float)
	float3 _GreyScaleRatio = float3(0.3, 0.59, 0.11);
#endif

	for (uint i = 0; i < ROW_SIZE; ++i)
	{
		for (uint j = 0; j < ROW_SIZE; ++j)
		{

			#if (TEX_TYPE == float)
						float _value = Colors[i + GTid.y][j + GTid.x];
			#else
						float _value = dot([i + GTid.y][j + GTid.x].xyz, _GreyScaleRatio);
			#endif

			uint _Index = uint(floor(_value * MAX_BIN_SIZE));
			_Index = clamp(_Index, 0, MAX_BIN_SIZE - 1);
			_bins[_Index] += 1;
			_binsIndex[i][j] = _Index;
		}
	}

	uint _Total = 0;
	uint _bIndex = 0;

	for (uint i = 0; i < MAX_BIN_SIZE; ++i)
	{
		_Total += _bins[i];

		if (_Total >= MEDIAN_LIMIT)
		{
			_bIndex = i;
			break;
		}
	}

	TEX_TYPE _ResultColor;

	for (uint i = 0; i < ROW_SIZE; ++i)
	{
		for (uint j = 0; j < ROW_SIZE; ++j)
		{
			if (_binsIndex[i][j] == _bIndex)
			{
				_ResultColor = Colors[i + GTid.y][j + GTid.x];
				break;
			}
		}
	}

	gTexOutput[DTid.xy] = _ResultColor;

}