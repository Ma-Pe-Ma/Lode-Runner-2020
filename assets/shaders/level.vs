layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexturePos;
layout (location = 2) in vec2 gPos;
layout (location = 3) in int tInst;

out vec2 aTexCoord;
flat out int textureID;

vec2 pos;

void main() {
	pos.x = aPos.x + gPos.x / 15.0;
	pos.y = aPos.y + gPos.y / 9.0;
	
	aTexCoord = aTexturePos;
	textureID = tInst;

	gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);
}