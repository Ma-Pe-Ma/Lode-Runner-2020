
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexturePos;

out vec2 TexCoord;

uniform vec2 gPos;

uniform bool front;
vec2 abs;
vec2 pos;

void main() {	
	abs.x = 1.0 / 15.0 * gPos.x - 29.0 / 30.0;
	abs.y = 1.0 / 9.0 * gPos.y - 17.0 / 18.0;
	
	pos.x = aPos.x / 15.0 + abs.x;
	pos.y = aPos.y / 9.0 + abs.y;
	
	TexCoord = aTexturePos;
	
	if (front != true) {
		gl_Position = vec4(pos.x, pos.y, -0.998, 1.0);
	}		
	else {
		gl_Position = vec4(pos.x, pos.y, -0.999, 1.0);
	}		
}