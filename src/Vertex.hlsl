cbuffer ConstantBuffer
{
	float4x4 Matrix;
}

struct VERTEX
{
	float4 pos_xyz : POSITION;
	float2 uv : TEXCOORD;
};

struct PIXEL
{
	float4 pos_xy : SV_POSITION;
	float2 uv : TEXCOORD;
};

PIXEL main(VERTEX Input)
{
	PIXEL Output;

	Output.pos_xy = mul(Matrix, Input.pos_xyz);
	Output.uv = Input.uv;

	return Output;
}