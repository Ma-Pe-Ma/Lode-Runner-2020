out vec4 FragColor;

uniform sampler2D touchTexture;

in vec2 aTexCoord;

void main() {
	FragColor = texture(touchTexture, aTexCoord);
}