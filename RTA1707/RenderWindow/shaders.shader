cbuffer ModelViewProjectionBuffer : register( b0 )
{
	matrix model;
	matrix view;
	matrix projection;
	matrix joints[ 64 ];
};

struct VertexShaderInput
{
	float4 position : POSITION;
	float4 color : COLOR;
	float4 weights : WEIGHTS;
	int4 indices : INDICES;
};

struct PixelShaderInput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

PixelShaderInput VShader( VertexShaderInput input )
{
	PixelShaderInput output;
	if ( input.indices.x >= 0 )
	{
		output.position = mul( joints[ input.indices.x ], input.position ) * input.weights.x;
		if ( input.indices.y >= 0 )
			output.position += mul( joints[ input.indices.y ], input.position ) * input.weights.y;
		if ( input.indices.z >= 0 )
			output.position += mul( joints[ input.indices.z ], input.position ) * input.weights.z;
		if ( input.indices.w >= 0 )
			output.position += mul( joints[ input.indices.w ], input.position ) * input.weights.w;
	}
	else
		output.position = input.position;

	output.position = mul( mul( mul( output.position, model ), view ), projection );
	output.color = input.color;
	return output;
}

float4 PShader( PixelShaderInput input ) : SV_TARGET
{
	return input.color;
}