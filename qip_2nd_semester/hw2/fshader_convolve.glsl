#version 130

in	vec2	  v_TexCoord;	// varying variable for passing texture coordinate from vertex shader

uniform int       u_Size;	// blur width value
uniform float	  u_StepX;
uniform float	  u_StepY;
uniform float     u_Kernel[81];
uniform	sampler2D u_Sampler;	// uniform variable for the texture image


vec3 CLIP(vec3 myInput, float low, float high) {
    vec3 myOutput = vec3(0.0);
    for(int i = 0; i < 3; i++) {
        if(myInput[i] <= high || myInput[i] >= low) {
            myOutput[i] = myInput[i];
        } else if(myOutput[i] > high) {
            myOutput[i] = high;
        } else {
            myOutput[i] = low;
        }
    }
    return myOutput;
}


void main() {

	vec3 avg = vec3(0.0);
	vec2 tc  = v_TexCoord;
    int  size  = u_Size / 2;
    int count = 0;

    for(int i=-size; i<=size; ++i) {
        for(int j=-size; j<=size; ++j) {
            avg += (texture2D(u_Sampler, vec2(tc.x + j*u_StepX, tc.y + i*u_StepY)).rgb * u_Kernel[count]);
            count++;
        }
    }
	gl_FragColor = vec4(CLIP(avg.rgb, 0.0, 1.0), 1.0);
}
