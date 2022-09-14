
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexturePos;

out vec2 TexCoord;
uniform vec2 gPos;

vec2 abs;
vec2 pos;

void main() {		
	pos.x = (6.0 * (aPos.x + gPos.x) - 93.0) / 140.0;
	pos.y = (2.0 * (aPos.y - gPos.y) + 27.0) / 28.0;
	
	TexCoord = aTexturePos;
	gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);
}