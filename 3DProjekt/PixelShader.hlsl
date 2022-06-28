Texture2D ambient : register(t0);
Texture2D diffuse : register(t1);
Texture2D specular : register(t2);

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

cbuffer cam : register(b2)
{
	float3 cameraPosition; // Not Working!
	float padding;
};

struct PixelShaderInput
{
	float4 position : SV_POSITION;
	float4 newPos : NEWPOSITION;
	float3 normal : NORMAL;
	float2 uv : UV;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	float3 ambientColour = ambient.Sample(sampl, input.uv).xyz;
	float3 diffuseColour = diffuse.Sample(sampl, input.uv).xyz;
	float3 specularColour = specular.Sample(sampl, input.uv).xyz;

	input.normal = normalize(input.normal);
	float3 vectorToLight = normalize(lightPos - input.newPos.xyz);
	float3 vectorToCam = normalize(cameraPosition - input.newPos.xyz);
	float3 reflection = reflect(-vectorToLight, input.normal);
	reflection = normalize(reflection);

	float3 ambientClr = ambientMatCoefficient * ambientColour;
	float3 diffuseClr = diffuseMatCoefficient * diffuseColour * max(dot(input.normal, vectorToLight),0.0f);
	float3 specularClr = (specularMatCoefficient * specularColour * pow(max(dot(reflection, vectorToCam), 0.0f), shininess));

	
	float3 finalColour = (ambientClr + diffuseClr);
	//finalColour += specularClr;

	float3 number = (input.normal + float3(1.0f, 1.0f, 1.0f)) / 2;
	if (padd1 == 0.0f) return float4(finalColour,1.0f);
	return float4(number, 1.0f);
}