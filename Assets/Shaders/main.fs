
out vec4 FragColor;

in vec2 aTexCoord;
flat in int instanceID;

uniform sampler2D textures[2];

uniform vec2 textureScales[2];
uniform vec2 textureOffsets[2];

void main() {	
	vec2 pTexCoord = textureScales[instanceID] * aTexCoord + textureOffsets[instanceID];
	
	FragColor = texture(textures[instanceID], pTexCoord); 
}