out vec4 FragColor;

in vec2 aTexCoord;
flat in int instanceID;

uniform sampler2D textureA;

uniform int textureID[864];

void main() {

	vec2 pTexCoord = vec2(aTexCoord.x + float(textureID[instanceID] % 16) / 16.0, aTexCoord.y - float(textureID[instanceID] / 16) / 4.0);

	FragColor = texture(textureA, pTexCoord);
} 