out vec4 FragColor;

in vec2 aTexCoord;
flat in int textureID;

uniform sampler2D textureA;

void main() {

	vec2 pTexCoord = vec2(aTexCoord.x + float(textureID % 16) / 16.0, aTexCoord.y - float(textureID / 16) / 4.0);

	FragColor = texture(textureA, pTexCoord);
} 