#pragma pack_matrix( row_major )

// vertex shader
cbuffer ObjectBuffer : register( b0 )
{
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
};

cbuffer FogBuffer : register( b1 )
{
    float3 fogColor;
    float fogStart;
    float fogEnd;
    bool fogEnable;
}

struct VS_INPUT
{
    float3 inPosition : POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
};

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float2 outTexCoord : TEXCOORD;
    float3 outNormal : NORMAL;
    float3 outNormalView : NORMAL_VIEW;
    float3 outWorldPos : WORLD_POSITION;
    float3 outViewPos : VIEW_POSITION;
    float  outFog : FOG;
};

VS_OUTPUT VS( VS_INPUT input )
{
    VS_OUTPUT output;
    
    float4x4 worldView = mul( worldMatrix, viewMatrix );
    float4x4 worldViewProj = mul( worldView, projectionMatrix );
    
    output.outPosition = mul( float4( input.inPosition, 1.0f ), worldViewProj );
    output.outWorldPos = (float3)mul( float4( input.inPosition, 1.0f ), worldMatrix );
    output.outViewPos = (float3)mul( float4( input.inPosition, 1.0f ), worldView );
    
    output.outTexCoord = input.inTexCoord;
    output.outNormal = mul( input.inNormal, (float3x3)worldMatrix );
    output.outNormalView = mul( input.inNormal, (float3x3)worldView );
    
    output.outFog = saturate( ( output.outViewPos.z - fogEnd ) / ( fogEnd - fogStart ) ); // linear fog
    
    return output;
}

// pixel shader
cbuffer LightBuffer : register( b2 )
{
    float3 ambientLightColor;
    float3 dynamicLightColor;
    float3 specularLightColor;
    float3 dynamicLightPosition;
    float ambientLightStrength;
    float dynamicLightStrength;
    float specularLightIntensity;
    float specularLightPower;
    float lightConstant;
    float lightLinear;
    float lightQuadratic;
    bool useTexture;
    float alphaFactor;
};

struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inNormalView : NORMAL_VIEW;
    float3 inWorldPos : WORLD_POSITION;
    float3 inViewPos : VIEW_POSITION;
    float  inFog : FOG;
};

Texture2D albedoTexture : DIFFUSE_TEXTURE : register( t0 );
SamplerState samplerState : SAMPLER : register( s0 );

float4 PS( PS_INPUT input ) : SV_TARGET
{   
    // light vector data
    const float3 vToL = dynamicLightPosition - input.inWorldPos;
    const float distToL = length( vToL );
    const float3 dirToL = vToL / distToL;
    
    // sample from texture
    float3 albedoSample = albedoTexture.Sample( samplerState, input.inTexCoord );
    
    // attenuation
    const float attenuation = 1.0f / ( lightConstant + lightLinear * distToL + lightQuadratic * ( distToL * distToL ) );
    
    // ambient lighting
    const float3 ambient = ambientLightColor * ambientLightStrength;
    
    // diffuse lighting
    const float diffuseAmount = attenuation * max( 0.0f, dot( dirToL, input.inNormal ) );
    
    // specular lighting
    const float3 incidence = input.inNormalView * dot( vToL, input.inNormalView );
    const float3 reflection = incidence * 2.0f - vToL;
    float specularAmount = pow( max( 0.0f, dot( normalize( -reflection ), normalize( input.inViewPos ) ) ), specularLightPower );
    
    if ( diffuseAmount <= 0.0f )
        specularAmount = 0.0f;
    
    // directional lighting
    const float3 toLight = float3( 50.0f, 50.0f, -50.0f ) - input.inWorldPos;
    const float distanceToLight = length( toLight );
    const float3 directionToLight = toLight / distanceToLight;
    float NDotL = dot( directionToLight, input.inNormal );
    float3 directionalLight = dynamicLightColor * saturate( NDotL );
    
    // calculate lighting
    const float3 diffuse = dynamicLightColor * dynamicLightStrength * diffuseAmount;
    const float3 specular = attenuation * ( specularLightColor * specularLightIntensity ) * specularAmount;
    
    // final colour
    float3 finalColor = ( directionalLight ) * ( albedoSample = ( useTexture == true ) ? albedoSample : 1 );
    
    // fog factor
    const float fogValue = input.inFog * finalColor + ( 1.0 - input.inFog ) * fogColor;
    finalColor += ( fogEnable == true ) ? fogValue : 0;
    
    // output colour
    return float4( saturate( finalColor ), alphaFactor );
}