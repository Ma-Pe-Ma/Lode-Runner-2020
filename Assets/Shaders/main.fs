out vec4 FragColor;

in vec2 aTexCoord;
flat in int instanceID;

uniform sampler2D textures[2];

uniform vec2 textureScales[2];
uniform vec2 textureOffsets[2];

void main() {
	vec2 pTexCoord = textureScales[instanceID] * aTexCoord + textureOffsets[instanceID];
	
	//ugly solution but OpenGL ES 3.0 can resolve sampler2D arrays at compile time only
	if (instanceID == 0)
	{
		FragColor = texture(textures[0], pTexCoord);
	}
	else if (instanceID == 1)
	{
		FragColor = texture(textures[1], pTexCoord);
	}
}