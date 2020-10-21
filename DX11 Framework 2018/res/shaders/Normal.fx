cbuffer ConstantBuffer : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;
    float4 diffuseMaterial;
    float4 diffuseLight;
    float3 lightDirection;
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
    float4 outNrm : NORMAL;
};

VS_OUTPUT VS( VS_INPUT input )
{   
    VS_OUTPUT output;
    output.outPos = mul( float4( input.inPos, 1.0f ), transpose( World ) );
    output.outPos = mul( output.outPos, transpose( View ) );
    output.outPos = mul( output.outPos, transpose( Projection ) );
    
    // convert from local to world space
    float3 normalW = mul( float4( input.inNrm, 0.0f ), World ).xyz;
    normalW = normalize( normalW );
    
    // diffuse lighting
    float diffuseAmount = max( dot( lightDirection, normalW ), 0.0f );
    output.outNrm.rgb = diffuseAmount * ( diffuseMaterial * diffuseLight ).rgb;
    output.outNrm.a = diffuseMaterial.a;
    
    return output;
}

// Pixel Shader
struct PS_INPUT
{
    float4 inPos : SV_POSITION;
    float4 inNrm : NORMAL;
};

float4 PS( PS_INPUT input ) : SV_TARGET
{
    return input.inNrm;
}
