varying vec2 v_TexCoord;
uniform sampler2D u_Sampler;
uniform bool u_IsInput;
uniform bool u_IsRGB;
uniform float u_Min_Value;
uniform float u_Max_Value;

float CLIP(float value, float low, float high) {
	if(value <= high || value >= low) {
		return value;
	} else if(value > high) {
		return high;
	} else {
		return low;
	}
}

float histogramStretch(float value) {
	const float MaxGray = 1.0;
	return CLIP(MaxGray * (value - u_Min_Value) / (u_Max_Value - u_Min_Value), 0.0, MaxGray);
}

void main(void) {
	if(u_IsInput) {
		if(u_IsRGB) {
			gl_FragColor = texture2D(u_Sampler,v_TexCoord);
		} else {
			float gray = texture2D(u_Sampler,v_TexCoord).r*0.30 + texture2D(u_Sampler,v_TexCoord).g*0.59 + texture2D(u_Sampler,v_TexCoord).b*0.11;
			gl_FragColor = vec4(gray, gray, gray, 1);
		}
	} else {
		if(u_IsRGB) {
			float x, y, z;
			x = histogramStretch(texture2D(u_Sampler,v_TexCoord).r);
			y = histogramStretch(texture2D(u_Sampler,v_TexCoord).g);
			z = histogramStretch(texture2D(u_Sampler,v_TexCoord).b);
			gl_FragColor = vec4(x, y, z, 1);
		} else {
			float gray = texture2D(u_Sampler,v_TexCoord).r*0.30 + texture2D(u_Sampler,v_TexCoord).g*0.59 + texture2D(u_Sampler,v_TexCoord).b*0.11;
			float x = histogramStretch(gray);
			gl_FragColor = vec4(x, x, x, 1);
		}
	}

}
