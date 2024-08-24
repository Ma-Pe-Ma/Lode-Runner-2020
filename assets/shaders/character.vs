layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexturePos;

out vec2 aTexCoord;
flat out int instanceID;

uniform vec2 gPos[{0}];

const vec2 charSize = vec2(6.0 / 140.0, -1.0 / 14.0);

void main() {
	vec2 pos = aPos + charSize * gPos[gl_InstanceID];
	
	instanceID = gl_InstanceID;

	aTexCoord = aTexturePos;
	gl_Position = vec4(pos, 0.0, 1.0);
}