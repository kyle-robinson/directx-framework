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
    float3 directionalLightColor;
    float3 directionalLightPosition;
    
    float ambientLightStrength;
    float dynamicLightStrength;
    float specularLightIntensity;
    float specularLightPower;
    float lightConstant;
    float lightLinear;
    float lightQuadratic;
    
    float directionalLightIntensity;
    bool usePointLight;
    float quadIntensity;
    bool useQuad;
    float lightTimer;
    bool lightFlicker;
    float randLightAmount;
    bool padding;
    
    float flickerAmount;
};

cbuffer SceneBuffer : register( b3 )
{
    float alphaFactor;
    bool useTexture;
}

struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
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
    const float diffuseAmount = attenuation * max( 0.0f, dot( normalize( dirToL ), input.inNormal ) );
    
    // specular lighting
    const float3 incidence = input.inNormal * dot( distToL, input.inNormal );
    const float3 reflection = incidence * 2.0f - distToL;
    float specularAmount = pow( max( 0.0f, dot( normalize( -reflection ), normalize( input.inViewPos ) ) ), specularLightPower );
    
    if ( diffuseAmount <= 0.0f )
        specularAmount = 0.0f;
    
    // directional lighting
    const float3 toLight = directionalLightPosition - normalize( input.inViewPos );
    const float distanceToLight = length( toLight );
    const float3 directionToLight = toLight / distanceToLight;
    float NDotL = dot( directionToLight, input.inNormal );
    float3 directionalLight = directionalLightColor * saturate( NDotL );
    
    const float3 directionalIncidence = input.inNormal * dot( distanceToLight, input.inNormal );
    const float3 directionalReflection = directionalIncidence * 2.0f - distanceToLight;
    float directionalSpecularAmount = pow( max( 0.0f, dot( normalize( -directionalReflection ),
        normalize( input.inViewPos ) ) ), specularLightPower );
    
    // calculate lighting
    float3 diffuse = dynamicLightColor * dynamicLightStrength * diffuseAmount;
    const float3 specular = attenuation * ( specularLightColor * specularLightIntensity ) * specularAmount;
    
    // final color
    float3 combinedColor = { 0.0f, 0.0f, 0.0f };
    if ( usePointLight )
    {
        if ( lightFlicker )
        {
            if ( lightTimer % randLightAmount <= 10.0f )
                diffuse.rgb *= flickerAmount;
        }
        combinedColor = ( ambient + diffuse + specular );
        if ( useQuad )
            combinedColor = dynamicLightStrength / 2.0f;
    }
    if ( !usePointLight )
    {
        combinedColor = ( ambient + directionalLight + directionalSpecularAmount ) * directionalLightIntensity;
        if ( useQuad )
            combinedColor = directionalLightIntensity / 2.0f;
    }
    float3 finalColor = combinedColor * ( albedoSample = useTexture ? albedoSample : 1 );
    
    // fog factor
    float fogValue = input.inFog * finalColor + ( 1.0 - input.inFog );
    finalColor += fogEnable ? fogValue * fogColor : 0;
    
    // output colour
    return float4( saturate( finalColor ), alphaFactor );
}