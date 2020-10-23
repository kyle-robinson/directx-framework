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
    return output;
}

// Pixel Shader
cbuffer ConstantBufferPixel : register( b1 )
{
    float4 ambientLight;
    float4 diffuseMaterial;
    float4 diffuseLight;
    float4 specularLight;
    float3 lightDirection;
    float specularPower;
    float3 eyePos;
}

struct PS_INPUT
{
    float4 inPos : SV_POSITION;
    float3 inNrm : NORMAL;
};

float4 PS( PS_INPUT input ) : SV_TARGET
{   
    input.inNrm = normalize( input.inNrm );
    
    // ambient lighting
    float3 ambient = diffuseMaterial * ambientLight;
    
    // diffuse lighting
    float3 diffuseAmount = max( dot( lightDirection, input.inNrm ), 0.0f );
    float3 diffuse = diffuseAmount * ( diffuseMaterial * diffuseLight ).rgb;
    
    // specular lighting
    float3 toEye = normalize( eyePos - input.inPos.xyz );
    float3 reflection = reflect( -lightDirection, input.inNrm );
    float specularAmount = pow( max( dot( reflection, toEye ), 0.0f ), specularPower );
    float3 specular = specularAmount * ( diffuseMaterial * specularLight ).rgb;
    
    float4 result;
    result.rgb = ambient + diffuse + specular;
    result.a = diffuseMaterial.a;
    
    return result;
}
