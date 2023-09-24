layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexturePos;

out vec2 aTexCoord;
flat out int instanceID;

uniform vec2 vertexScales[2];
uniform vec2 vertexOffsets[2];

uniform vec2 cursorOffsets[2];
uniform int cursor[2];

void main() {
	vec2 position = vertexScales[gl_InstanceID] * aPos + vertexOffsets[gl_InstanceID] + float(cursor[gl_InstanceID]) * cursorOffsets[gl_InstanceID];
	instanceID = gl_InstanceID;

	gl_Position = vec4(position, 0.0, 1.0);
	
	aTexCoord = aTexturePos;	
}