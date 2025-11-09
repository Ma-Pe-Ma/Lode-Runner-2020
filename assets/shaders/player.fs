out vec4 FragColor;

in vec2 aTexCoord;
flat in int textureID;
flat in int direction;
flat in int carryGold;

uniform sampler2D textureA;

const mediump vec4 clothColor = vec4(112.0 / 255.0, 224.0 / 255.0, 144.0 / 255.0, 1.0); 

void main() {
	vec2 pTexCoord = aTexCoord + vec2(float(textureID % 12) / 12.0, float(textureID / 12) / 6.0);

	mediump vec4 texColor = direction > 0 ? texture(textureA, pTexCoord) : texture(textureA, vec2(pTexCoord.x - 2.0 * aTexCoord.x + 1.0 / 12.0, pTexCoord.y));

	if(carryGold > 0 && texColor == clothColor)	{
		texColor.b = 1.0;
	}
	
	FragColor = texColor;
}