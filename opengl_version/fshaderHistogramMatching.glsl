varying vec2 v_TexCoord;
uniform sampler2D u_Sampler;
uniform float u_reference;
uniform bool u_IsInput;
uniform bool u_IsRGB;


float threshold(float value) {
	if(value > u_reference) {
		return 1.0;
	} else {
		return 0.0;
	}
}

void main(void) {
	if(u_IsInput) {
		if(u_IsRGB) {
			gl_FragColor = texture2D(u_Sampler,v_TexCoord);
		} else {
			float gray = texture2D(u_Sampler,v_TexCoord).r*0.30 +
						 texture2D(u_Sampler,v_TexCoord).g*0.59 +
						 texture2D(u_Sampler,v_TexCoord).b*0.11;
			gl_FragColor = vec4(gray, gray, gray, 1);
		}
	} else {
		if(u_IsRGB) {
			float x, y, z;
			x = threshold(texture2D(u_Sampler,v_TexCoord).r);
			y = threshold(texture2D(u_Sampler,v_TexCoord).g);
			z = threshold(texture2D(u_Sampler,v_TexCoord).b);
			gl_FragColor = vec4(x, y, z, 1);
		} else {
			float gray = texture2D(u_Sampler,v_TexCoord).r*0.30 +
						 texture2D(u_Sampler,v_TexCoord).g*0.59 +
						 texture2D(u_Sampler,v_TexCoord).b*0.11;
			float x = threshold(gray);
			gl_FragColor = vec4(x, x, x, 1);
		}
	}

}
