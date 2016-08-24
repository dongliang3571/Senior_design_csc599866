varying vec2      v_TexCoord;
uniform sampler2D u_Sampler;
uniform float u_reference;

float threshold(float value) {
	if(value > u_reference) {
		return 1.0;
	} else {
		return 0.0;
	}
}

void main(void) {
	float x, y, z;
	x = threshold(texture2D(u_Sampler,v_TexCoord).r);
	y = threshold(texture2D(u_Sampler,v_TexCoord).g);
	z = threshold(texture2D(u_Sampler,v_TexCoord).b);

	gl_FragColor = vec4(x, y, z, 1);
}
