Texture2D T2D;
SamplerState ss;

struct PIXEL
{
	float4 pos_xy : SV_POSITION;
	float2 uv : TEXCOORD;
};

float4 main(PIXEL Input) : SV_TARGET
{
	return T2D.Sample(ss, Input.uv);
}