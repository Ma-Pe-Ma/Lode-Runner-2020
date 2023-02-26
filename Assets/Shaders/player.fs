out vec4 FragColor;

in vec2 aTexCoord;
flat in int instanceID;

uniform sampler2D textureA;

uniform int textureID[340];
uniform bool direction[340];
uniform bool carryGold[340];

const vec4 clothColor = vec4(112.0 / 255.0, 224.0 / 255.0, 144.0 / 255.0, 1.0); 

void main() {

	vec2 pTexCoord = aTexCoord + vec2(float(textureID[instanceID] % 12) / 12.0, float(textureID[instanceID] / 12) / 5.0);

	//vec4 texColor = direction[instanceID] ? texture(textureA, pTexCoord) : texture(textureA, vec2(-aTexCoord.x + 1.0 / 12 + (textureID[instanceID] % 12) / 12.0, pTexCoord.y));
	vec4 texColor = direction[instanceID] ? texture(textureA, pTexCoord) : texture(textureA, vec2(pTexCoord.x - 2.0 * aTexCoord.x + 1.0 / 12.0, pTexCoord.y));
	
	if(carryGold[instanceID] && texColor == clothColor)
	{
		texColor.b = 1.0;
	}
	
	FragColor = texColor;
} 