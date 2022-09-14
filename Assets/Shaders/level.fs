
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D textureA;

void main() {	
	FragColor = texture(textureA, TexCoord);
} 