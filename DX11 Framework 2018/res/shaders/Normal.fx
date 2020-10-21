cbuffer ConstantBufferVertex : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;
}

// Vertex Shader
struct VS_INPUT
{
    float3 inPos : POSITION;
    float3 inNrm : NORMAL;
};

struct VS_OUTPUT
{
    float4 outPos : SV_POSITION;
    float3 outNrm : NORMAL;
};

VS_OUTPUT VS( VS_INPUT input )
{   
    VS_OUTPUT output;
    output.outPos = mul( float4( input.inPos, 1.0f ), transpose( World ) );
    output.outPos = mul( output.outPos, transpose( View ) );
    output.outPos = mul( output.outPos, transpose( Projection ) );
    // convert normals from local to world space
    output.outNrm = mul( float4( input.inNrm, 0.0f ), transpose( World ) ).xyz;
    output.outNrm = output.outNrm;
    return output;
}

// Pixel Shader
cbuffer ConstantBufferPixel : register( b1 )
{
    float4 ambientLight;
    float4 diffuseMaterial;
    float4 diffuseLight;
    float3 lightDirection;
}

struct PS_INPUT
{
    float4 inPos : SV_POSITION;
    float3 inNrm : NORMAL;
};

float4 PS( PS_INPUT input ) : SV_TARGET
{   
    // ambient lighting
    float3 ambient = diffuseMaterial * ambientLight;
    
    // diffuse lighting
    float3 diffuseAmount = max( dot( lightDirection, input.inNrm ), 0.0f );
    float3 diffuse = diffuseAmount * ( diffuseMaterial * diffuseLight ).rgb;
    
    float4 result;
    result.rgb = ambient + diffuse;
    result.a = diffuseMaterial.a;
    
    return result;
}
