#ifdef __INTELLISENSE__
#define TEX_TYPE float
#define SIZE 2
#endif


#define ROW_SIZE (SIZE * 2 + 1)
#define KERNEL_SIZE (ROW_SIZE * ROW_SIZE)


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

	TEX_TYPE _Result = 0.0f;

	for (int i = 0; i < ROW_SIZE; ++i)
	{
		for (int j = 0; j < ROW_SIZE; ++j)
		{
			_Result += Colors[i + GTid.y][j + GTid.x];
		}
	}

	_Result /= KERNEL_SIZE;

	gTexOutput[DTid.xy] = _Result;

}