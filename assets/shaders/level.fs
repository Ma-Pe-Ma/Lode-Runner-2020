out vec4 FragColor;

in vec2 aTexCoord;
flat in int textureID;

uniform sampler2D textureA;

void main() {
	vec2 pTexCoord = aTexCoord + vec2(float(textureID / 6) / 11.0, -float(textureID % 6) / 6.0);

	FragColor = texture(textureA, pTexCoord);
} 