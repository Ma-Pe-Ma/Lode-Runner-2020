layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexturePos;

out vec2 aTexCoord;

void main() {
	gl_Position = vec4(aPos, 0.0, 1.0);	
	aTexCoord = aTexturePos;	
}