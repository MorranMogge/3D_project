Texture2D textur : register(t0);
SamplerState sampl;

cbuffer lightBuffer : register (b0)
{
	//Light properties
	float3 ambientLight;
	float padd1;
	float3 diffuseLight;
	float padd2;
	float3 specularLight;
	float padd3;
	float3 lightPos;
	float padd4;
	float3 cameraPos;
	float padd5;
}

cbuffer materialInfo : register (b1)
{
	//Material properties
	float ambientMatCoefficient;
	float diffuseMatCoefficient;
	float specularMatCoefficient;
	float shininess;
}

struct PixelShaderInput
{
	float4 position : SV_POSITION;
	float4 newPos : NEWPOSITION;
	float3 normal : NORMAL;
	float2 uv : UV;
};

float4 main(PixelShaderInput input) : SV_TARGET
{

	input.normal = normalize(input.normal);
	float3 vectorToLight = normalize(lightPos - input.newPos.xyz);
	float3 vectorToCam = normalize(cameraPos - input.newPos.xyz);
	float3 reflection = reflect(-vectorToLight, input.normal);
	reflection = normalize(reflection);

	float3 ambient = ambientMatCoefficient * ambientLight;
	float3 diffuse = diffuseMatCoefficient * diffuseLight * max(dot(input.normal, vectorToLight),0.0f);
	float3 specular = (specularMatCoefficient * specularLight * pow(max(dot(reflection, vectorToCam), 0.0f), shininess));

	float3 colour = textur.Sample(sampl, input.uv).xyz;
	colour *= (ambient + diffuse);
	//colour += specular;

	//return float4(1.0f, 0.0f, 0.0f, 1.0f);
	return float4(colour,1.0f);
	//return (normalize(input.position));
	//return float4(input.normal, 1.0f);
	//return float4(float3(1.f,1.f,1.f), 1.0f);
}