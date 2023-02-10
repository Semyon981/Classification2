#version 330
out vec4 fragColor;
uniform sampler2D texture;
void main()
{		
	
	vec3 col = vec3(0,1,0);
	
	fragColor = texture2D(texture, vec2(gl_FragCoord.x/800,gl_FragCoord.y/600));	
	
}