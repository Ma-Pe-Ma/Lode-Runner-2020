out vec4 FragColor;

in vec2 aTexCoord;
flat in int instanceID;

uniform int textureID[540];
uniform sampler2D textureA;

void main() {
	vec2 pTexCoord = aTexCoord + vec2(float(textureID[instanceID] / 6) / 11.0, -float(textureID[instanceID] % 6) / 6.0);

	FragColor = texture(textureA, pTexCoord);
} 