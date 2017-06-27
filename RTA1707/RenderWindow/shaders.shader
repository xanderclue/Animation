struct VertexShaderInput
{
	float4 position : POSITION;
	float4 color : COLOR;
};

struct PixelShaderInput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

PixelShaderInput VShader( VertexShaderInput input )
{
	PixelShaderInput output;
	output.position = input.position;
	output.color = input.color;
	return output;
}

float4 PShader( PixelShaderInput input ) : SV_TARGET
{
	return input.color;
}