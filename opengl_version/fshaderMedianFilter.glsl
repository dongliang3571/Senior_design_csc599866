varying vec2 v_TexCoord;
uniform sampler2D u_Sampler;
uniform bool u_IsInput;
uniform bool u_IsRGB;
uniform float u_DistanceX;
uniform float u_DistanceY;
uniform float u_NeighborSize;


float CLIP(float value, float low, float high) {
	if(value <= high || value >= low) {
		return value;
	} else if(value > high) {
		return high;
	} else {
		return low;
	}
}

const int size = 9;
float R[size];
float G[size];
float B[size];
float totalR;
float totalG;
float totalB;

void insertion_sortR() {
	int j;
	float temp;
	for (int i = 0; i < size; i++) {
		j = i;
		while (j > 0 && R[j] < R[j-1]) {
			temp = R[j];
			R[j] = R[j-1];
			R[j-1] = temp;
			j--;
		}
	}
}

void insertion_sortG() {
	int j;
	float temp;
	for (int i = 0; i < size; i++) {
		j = i;
		while (j > 0 && G[j] < G[j-1]) {
			temp = G[j];
			G[j] = G[j-1];
			G[j-1] = temp;
			j--;
		}
	}
}

void insertion_sortB() {
	int j;
	float temp;
	for (int i = 0; i < size; i++) {
		j = i;
		while (j > 0 && B[j] < B[j-1]) {
			temp = B[j];
			B[j] = B[j-1];
			B[j-1] = temp;
			j--;
		}
	}
}

// get all pixels inside a kernel
vec3 pixelsInNeighborhood() {

	int count = 0;
	// Add up all pixels vertically aligned to the middle pixel
	for(float k = 1.0; k <= u_NeighborSize; k += 1.0) {
		totalR  += texture2D(u_Sampler,vec2(v_TexCoord.x, v_TexCoord.y-k*u_DistanceY)).r;
		R[count] = texture2D(u_Sampler,vec2(v_TexCoord.x, v_TexCoord.y-k*u_DistanceY)).r;
		totalG  += texture2D(u_Sampler,vec2(v_TexCoord.x, v_TexCoord.y-k*u_DistanceY)).g;
		G[count] = texture2D(u_Sampler,vec2(v_TexCoord.x, v_TexCoord.y-k*u_DistanceY)).g;
		totalB  += texture2D(u_Sampler,vec2(v_TexCoord.x, v_TexCoord.y-k*u_DistanceY)).b;
		B[count] = texture2D(u_Sampler,vec2(v_TexCoord.x, v_TexCoord.y-k*u_DistanceY)).b;
		count++;

		totalR  += texture2D(u_Sampler,vec2(v_TexCoord.x, v_TexCoord.y+k*u_DistanceY)).r;
		R[count] = texture2D(u_Sampler,vec2(v_TexCoord.x, v_TexCoord.y+k*u_DistanceY)).r;
		totalG  += texture2D(u_Sampler,vec2(v_TexCoord.x, v_TexCoord.y+k*u_DistanceY)).g;
		G[count] = texture2D(u_Sampler,vec2(v_TexCoord.x, v_TexCoord.y+k*u_DistanceY)).g;
		totalB  += texture2D(u_Sampler,vec2(v_TexCoord.x, v_TexCoord.y+k*u_DistanceY)).b;
		B[count] = texture2D(u_Sampler,vec2(v_TexCoord.x, v_TexCoord.y+k*u_DistanceY)).b;
		count++;
	}

	// Add up all pixels horizontally aligned to the middle pixel
	for(float k = 1.0; k <= u_NeighborSize; k += 1.0) {
		totalR  += texture2D(u_Sampler,vec2(v_TexCoord.x-k*u_DistanceX, v_TexCoord.y)).r;
		R[count] = texture2D(u_Sampler,vec2(v_TexCoord.x-k*u_DistanceX, v_TexCoord.y)).r;
		totalG  += texture2D(u_Sampler,vec2(v_TexCoord.x-k*u_DistanceX, v_TexCoord.y)).g;
		G[count] = texture2D(u_Sampler,vec2(v_TexCoord.x-k*u_DistanceX, v_TexCoord.y)).g;
		totalB  += texture2D(u_Sampler,vec2(v_TexCoord.x-k*u_DistanceX, v_TexCoord.y)).b;
		B[count] = texture2D(u_Sampler,vec2(v_TexCoord.x-k*u_DistanceX, v_TexCoord.y)).b;
		count++;

		totalR  += texture2D(u_Sampler,vec2(v_TexCoord.x+k*u_DistanceX, v_TexCoord.y)).r;
		R[count] = texture2D(u_Sampler,vec2(v_TexCoord.x+k*u_DistanceX, v_TexCoord.y)).r;
		totalG  += texture2D(u_Sampler,vec2(v_TexCoord.x+k*u_DistanceX, v_TexCoord.y)).g;
		G[count] = texture2D(u_Sampler,vec2(v_TexCoord.x+k*u_DistanceX, v_TexCoord.y)).g;
		totalB  += texture2D(u_Sampler,vec2(v_TexCoord.x+k*u_DistanceX, v_TexCoord.y)).b;
		B[count] = texture2D(u_Sampler,vec2(v_TexCoord.x+k*u_DistanceX, v_TexCoord.y)).b;
		count++;
	}

	// add up all pixels within neighborhood
	for(float i = 1.0; i <= u_NeighborSize; i += 1.0) {
		for(float j = 1.0; j <= u_NeighborSize; j +=1.0) {
			totalR   += texture2D(u_Sampler,vec2(v_TexCoord.x-j*u_DistanceX, v_TexCoord.y-i*u_DistanceY)).r;
			R[count]  = texture2D(u_Sampler,vec2(v_TexCoord.x-j*u_DistanceX, v_TexCoord.y-i*u_DistanceY)).r;
			totalG   += texture2D(u_Sampler,vec2(v_TexCoord.x-j*u_DistanceX, v_TexCoord.y-i*u_DistanceY)).g;
			G[count]  = texture2D(u_Sampler,vec2(v_TexCoord.x-j*u_DistanceX, v_TexCoord.y-i*u_DistanceY)).g;
			totalB   += texture2D(u_Sampler,vec2(v_TexCoord.x-j*u_DistanceX, v_TexCoord.y-i*u_DistanceY)).b;
			B[count]  = texture2D(u_Sampler,vec2(v_TexCoord.x-j*u_DistanceX, v_TexCoord.y-i*u_DistanceY)).b;
			count++;

			totalR   += texture2D(u_Sampler,vec2(v_TexCoord.x+j*u_DistanceX, v_TexCoord.y+i*u_DistanceY)).r;
			R[count]  = texture2D(u_Sampler,vec2(v_TexCoord.x+j*u_DistanceX, v_TexCoord.y+i*u_DistanceY)).r;
			totalG   += texture2D(u_Sampler,vec2(v_TexCoord.x+j*u_DistanceX, v_TexCoord.y+i*u_DistanceY)).g;
			G[count]  = texture2D(u_Sampler,vec2(v_TexCoord.x+j*u_DistanceX, v_TexCoord.y+i*u_DistanceY)).g;
			totalB   += texture2D(u_Sampler,vec2(v_TexCoord.x+j*u_DistanceX, v_TexCoord.y+i*u_DistanceY)).b;
			B[count]  = texture2D(u_Sampler,vec2(v_TexCoord.x+j*u_DistanceX, v_TexCoord.y+i*u_DistanceY)).b;
			count++;

			totalR   += texture2D(u_Sampler,vec2(v_TexCoord.x-j*u_DistanceX, v_TexCoord.y+i*u_DistanceY)).r;
			R[count]  = texture2D(u_Sampler,vec2(v_TexCoord.x-j*u_DistanceX, v_TexCoord.y+i*u_DistanceY)).r;
			totalG   += texture2D(u_Sampler,vec2(v_TexCoord.x-j*u_DistanceX, v_TexCoord.y+i*u_DistanceY)).g;
			G[count]  = texture2D(u_Sampler,vec2(v_TexCoord.x-j*u_DistanceX, v_TexCoord.y+i*u_DistanceY)).g;
			totalB   += texture2D(u_Sampler,vec2(v_TexCoord.x-j*u_DistanceX, v_TexCoord.y+i*u_DistanceY)).b;
			B[count]  = texture2D(u_Sampler,vec2(v_TexCoord.x-j*u_DistanceX, v_TexCoord.y+i*u_DistanceY)).b;
			count++;

			totalR   += texture2D(u_Sampler,vec2(v_TexCoord.x+j*u_DistanceX, v_TexCoord.y-i*u_DistanceY)).r;
			R[count]  = texture2D(u_Sampler,vec2(v_TexCoord.x+j*u_DistanceX, v_TexCoord.y-i*u_DistanceY)).r;
			totalG   += texture2D(u_Sampler,vec2(v_TexCoord.x+j*u_DistanceX, v_TexCoord.y-i*u_DistanceY)).g;
			G[count]  = texture2D(u_Sampler,vec2(v_TexCoord.x+j*u_DistanceX, v_TexCoord.y-i*u_DistanceY)).g;
			totalB   += texture2D(u_Sampler,vec2(v_TexCoord.x+j*u_DistanceX, v_TexCoord.y-i*u_DistanceY)).b;
			B[count]  = texture2D(u_Sampler,vec2(v_TexCoord.x+j*u_DistanceX, v_TexCoord.y-i*u_DistanceY)).b;
			count++;
		}
	}

	// Add middle pixel itself
	totalR  += texture2D(u_Sampler,vec2(v_TexCoord.x, v_TexCoord.y)).r;
	R[count] = texture2D(u_Sampler,vec2(v_TexCoord.x, v_TexCoord.y)).r;
	totalG  += texture2D(u_Sampler,vec2(v_TexCoord.x, v_TexCoord.y)).g;
	G[count] = texture2D(u_Sampler,vec2(v_TexCoord.x, v_TexCoord.y)).g;
	totalB  += texture2D(u_Sampler,vec2(v_TexCoord.x, v_TexCoord.y)).b;
	B[count] = texture2D(u_Sampler,vec2(v_TexCoord.x, v_TexCoord.y)).b;
	count++;


	int j=0;
	float temp=0.0;
	for (int i = 0; i < size; i++) {
		j = i;
		while (j > 0 && R[j] < R[j-1]) {
			temp = R[j];
			R[j] = R[j-1];
			R[j-1] = temp;
			j--;
		}
	}

	j=0;
	temp=0.0;
	for (int i = 0; i < size; i++) {
		j = i;
		while (j > 0 && G[j] < G[j-1]) {
			temp = G[j];
			G[j] = G[j-1];
			G[j-1] = temp;
			j--;
		}
	}

	j=0;
	temp=0.0;
	for (int i = 0; i < size; i++) {
		j = i;
		while (j > 0 && B[j] < B[j-1]) {
			temp = B[j];
			B[j] = B[j-1];
			B[j-1] = temp;
			j--;
		}
	}

	int newSizesize = 4;
	vec3 median = vec3(R[newSizesize], G[newSizesize], B[newSizesize]);

	return median;
}


// main function
void main(void) {
	if(u_IsInput || u_NeighborSize == 0.0) {
		if(u_IsRGB) {
			gl_FragColor = texture2D(u_Sampler,v_TexCoord);
		} else {
			float gray = texture2D(u_Sampler,v_TexCoord).r*0.30 +
						 texture2D(u_Sampler,v_TexCoord).g*0.59 +
						 texture2D(u_Sampler,v_TexCoord).b*0.11;
			gl_FragColor = vec4(gray, gray, gray, 1);
		}
	} else {
		vec3 median = pixelsInNeighborhood();
		if(u_IsRGB) {
			gl_FragColor = vec4(median.xyz, 1);
		} else {
			float grey = median.r*0.30 +
						 median.g*0.59 +
						 median.b*0.11;

			gl_FragColor = vec4(grey, grey, grey, 1);
		}
	}
}
