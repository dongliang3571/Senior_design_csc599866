varying vec2 v_TexCoord;
uniform sampler2D u_Sampler;
uniform bool u_IsInput;
uniform bool u_IsRGB;
uniform float u_Contrast;
uniform float u_Brightness;

float CLIP(float value, float low, float high) {
	if(value <= high || value >= low) {
		return value;
	} else if(value > high) {
		return high;
	} else {
		return low;
	}
}

float contrast(float value) {
	float reference = 0.5;
	float temp = value;
	return CLIP((temp - reference) * u_Contrast + reference + u_Brightness,
				0.0,
				1.0);
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
			x = contrast(texture2D(u_Sampler,v_TexCoord).r);
			y = contrast(texture2D(u_Sampler,v_TexCoord).g);
			z = contrast(texture2D(u_Sampler,v_TexCoord).b);
			gl_FragColor = vec4(x, y, z, 1);
		} else {
			float gray = texture2D(u_Sampler,v_TexCoord).r*0.30 +
			             texture2D(u_Sampler,v_TexCoord).g*0.59 +
						 texture2D(u_Sampler,v_TexCoord).b*0.11;
			float x = contrast(gray);
			gl_FragColor = vec4(x, x, x, 1);
		}
	}

}
