varying vec2 v_TexCoord;
uniform sampler2D u_Sampler;
uniform float u_reference;
uniform bool u_IsInput;
uniform bool u_IsRGB;
uniform bool u_IsEdge;
uniform float u_DistanceX;
uniform float u_DistanceY;
uniform float u_FilterWidth;
uniform float u_FilterHeight;
uniform float u_Factor;

float CLIP(float value, float low, float high) {
	if(value <= high || value >= low) {
		return value;
	} else if(value > high) {
		return high;
	} else {
		return low;
	}
}


// blurring operation function
vec3 blur() {
	vec3 avg = vec3(0.0, 0.0, 0.0); // init an avg variable

	// Add up all pixels vertically aligned to the middle pixel
	for(float k = 1.0; k <=u_FilterHeight; k += 1.0) {
		avg += texture2D(u_Sampler,vec2(v_TexCoord.x, v_TexCoord.y-k*u_DistanceY)).rgb;
		avg += texture2D(u_Sampler,vec2(v_TexCoord.x, v_TexCoord.y+k*u_DistanceY)).rgb;
	}

	// Add up all pixels horizontally aligned to the middle pixel
	for(float k = 1.0; k <=u_FilterWidth; k += 1.0) {
		avg += texture2D(u_Sampler,vec2(v_TexCoord.x-k*u_DistanceX, v_TexCoord.y)).rgb;
		avg += texture2D(u_Sampler,vec2(v_TexCoord.x+k*u_DistanceX, v_TexCoord.y)).rgb;
	}

	// add up all pixels within neighborhood
	for(float i = 1.0; i <= u_FilterHeight; i += 1.0) {
		for(float j = 1.0; j <= u_FilterWidth; j +=1.0) {
			avg += texture2D(u_Sampler,vec2(v_TexCoord.x-j*u_DistanceX,
				             v_TexCoord.y-i*u_DistanceY)).rgb;
			avg += texture2D(u_Sampler,vec2(v_TexCoord.x+j*u_DistanceX,
							 v_TexCoord.y+i*u_DistanceY)).rgb;
			avg += texture2D(u_Sampler,vec2(v_TexCoord.x-j*u_DistanceX,
				             v_TexCoord.y+i*u_DistanceY)).rgb;
			avg += texture2D(u_Sampler,vec2(v_TexCoord.x+j*u_DistanceX,
				             v_TexCoord.y-i*u_DistanceY)).rgb;
		}
	}

	// Add middle pixel itself
	avg += texture2D(u_Sampler,vec2(v_TexCoord.x, v_TexCoord.y)).rgb;
	return avg;
}


// main function
void main(void) {
	if(u_IsInput || (u_FilterWidth == 0.0 && u_FilterHeight == 0.0)) {
		if(u_IsRGB) {
			gl_FragColor = texture2D(u_Sampler,v_TexCoord);
		} else {
			float gray = texture2D(u_Sampler,v_TexCoord).r*0.30 +
						 texture2D(u_Sampler,v_TexCoord).g*0.59 +
						 texture2D(u_Sampler,v_TexCoord).b*0.11;
			gl_FragColor = vec4(gray, gray, gray, 1);
		}
	} else {
		float neighborSize = (u_FilterWidth*2.0+1.0) * (u_FilterHeight*2.0+1.0);
		vec3 tempAvg = blur();
		if(u_IsRGB) {
			if(u_IsEdge) {
				gl_FragColor = vec4(CLIP(texture2D(u_Sampler, v_TexCoord).r - tempAvg.r/neighborSize, 0.0, 1.0),
									CLIP(texture2D(u_Sampler, v_TexCoord).g - tempAvg.g/neighborSize, 0.0, 1.0),
									CLIP(texture2D(u_Sampler, v_TexCoord).b - tempAvg.b/neighborSize, 0.0, 1.0),
									1);
			} else {
				gl_FragColor = vec4(CLIP(texture2D(u_Sampler, v_TexCoord).r + u_Factor*(texture2D(u_Sampler, v_TexCoord).r - tempAvg.r/neighborSize), 0.0, 1.0),
									CLIP(texture2D(u_Sampler, v_TexCoord).g + u_Factor*(texture2D(u_Sampler, v_TexCoord).g - tempAvg.g/neighborSize), 0.0, 1.0),
									CLIP(texture2D(u_Sampler, v_TexCoord).b + u_Factor*(texture2D(u_Sampler, v_TexCoord).b - tempAvg.b/neighborSize), 0.0, 1.0),
									1);
			}
		} else {
			float original = texture2D(u_Sampler,v_TexCoord).r*0.30 +
						 	 texture2D(u_Sampler,v_TexCoord).g*0.59 +
						 	 texture2D(u_Sampler,v_TexCoord).b*0.11;

			float blurredGray = tempAvg.r/neighborSize*0.30 +
						    tempAvg.g/neighborSize*0.59 +
						    tempAvg.b/neighborSize*0.11;
			if(u_IsEdge) {
				gl_FragColor = vec4(CLIP(original-blurredGray, 0.0, 1.0),
									CLIP(original-blurredGray, 0.0, 1.0),
									CLIP(original-blurredGray, 0.0, 1.0),
									1);
			} else {
				gl_FragColor = vec4(CLIP(original + u_Factor*(original-blurredGray), 0.0, 1.0),
									CLIP(original + u_Factor*(original-blurredGray), 0.0, 1.0),
									CLIP(original + u_Factor*(original-blurredGray), 0.0, 1.0),
									1);
			}
		}
	}
}
