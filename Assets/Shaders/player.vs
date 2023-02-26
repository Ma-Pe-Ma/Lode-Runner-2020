layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexturePos;

out vec2 aTexCoord;
flat out int instanceID;

uniform vec2 gPos[540];
vec2 pos;

void main() {
	pos.x = aPos.x + gPos[gl_InstanceID].x / 15.0;
	pos.y = aPos.y + gPos[gl_InstanceID].y / 9.0;
	
	aTexCoord = aTexturePos;
	instanceID = gl_InstanceID;

	gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);
}