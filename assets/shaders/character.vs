layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexturePos;
layout (location = 2) in vec2 gPos;
layout (location = 3) in int tInst;

out vec2 aTexCoord;
flat out int textureID;

const vec2 charSize = vec2(6.0 / 140.0, -1.0 / 14.0);

void main() {
	vec2 pos = aPos + charSize * gPos;
	
	textureID = tInst;

	aTexCoord = aTexturePos;
	gl_Position = vec4(pos, 0.0, 1.0);
}