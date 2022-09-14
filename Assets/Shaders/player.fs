
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D textureA;
uniform bool direction;
uniform bool carryGold;
uniform float ref;

void main() {
	vec4 texColor;

	if(direction) {
		texColor = texture(textureA, TexCoord);
	}		
	else {
		texColor = texture(textureA, vec2(-TexCoord.x + 2.0 * ref + 1.0 / 12.0, TexCoord.y));
	}		
	
	if(texColor.r < 0.01 && texColor.g < 0.01 && texColor.b < 0.01) {
		discard;
	}
	
	if(carryGold) {
		if(texColor.r == 112.0/255.0 && texColor.g == 224.0/255.0 && texColor.b == 144.0/255.0) {
			texColor.b = 1.0;
		}
	}		
	
	FragColor = texColor;
} 