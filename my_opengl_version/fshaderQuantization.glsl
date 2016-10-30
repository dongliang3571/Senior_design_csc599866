varying vec2 v_TexCoord;
uniform sampler2D u_Sampler;
uniform bool u_IsInput;
uniform bool u_IsRGB;
uniform bool u_IsDither;
uniform float u_Scale;
uniform float u_DistanceX;
// uniform float u_DistanceY;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float CLIP(float value, float low, float high) {
	if(value <= high || value >= low) {
		return value;
	} else if(value > high) {
		return high;
	} else {
		return low;
	}
}

float quantize(float value, float bias) {
	if(u_IsDither) {
		float temp = value + bias;
		return CLIP(u_Scale * floor(temp/u_Scale) + u_Scale/2.0, 0.0, 1.0);
	} else {
		return CLIP(u_Scale * floor(value/u_Scale) + u_Scale/2.0, 0.0, 1.0);
	}
}


void main(void) {

    float newBias = 0.0;
	// Check if display input or output image
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
		if(u_IsDither) {
			if(mod(v_TexCoord.x, u_DistanceX*2.0) == 0.0) {
				newBias = mod(rand(vec2(gl_FragCoord.x/1.0, gl_FragCoord.y/1.0)), u_Scale/2.0);
			} else {
				newBias = - mod(rand(vec2(gl_FragCoord.x/1.0, gl_FragCoord.y/1.0)), u_Scale/2.0);
			}
		}
		if(u_IsRGB) {
			float x, y, z;
			x = quantize(texture2D(u_Sampler,v_TexCoord).r, newBias);
			y = quantize(texture2D(u_Sampler,v_TexCoord).g, newBias);
			z = quantize(texture2D(u_Sampler,v_TexCoord).b, newBias);
			gl_FragColor = vec4(x, y, z, 1);
		} else {
			float gray = texture2D(u_Sampler,v_TexCoord).r*0.30 +
                         texture2D(u_Sampler,v_TexCoord).g*0.59 +
                         texture2D(u_Sampler,v_TexCoord).b*0.11;
			float x = quantize(gray, newBias);
			gl_FragColor = vec4(x, x, x, 1);
		}
	}

}
