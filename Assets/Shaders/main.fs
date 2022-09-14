
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D textureA;

void main() {	
	vec4 color = texture(textureA, TexCoord); 
	
	if(color.a == 0.0) {
		discard;
	}		
	
	FragColor = color;	
}