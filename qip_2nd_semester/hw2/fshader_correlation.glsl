#version 120

varying vec2      v_TexCoord;   // varying variable for passing texture coordinate from vertex shader

uniform int       u_SizeW;  // blur width value
uniform int       u_SizeH;   // blur height value
uniform float     u_StepX;
uniform float     u_StepY;
uniform sampler2D u_Sampler;    // uniform variable for the texture image

uniform float     u_StepX_T;
uniform float     u_StepY_T;
uniform sampler2D u_Sampler_T;
uniform float     u_Sqrt_Sum_T;


void main() {

    vec4 convolve = vec4(0.0);
    vec4 sum = vec4(0.0);
    vec2 tc  = v_TexCoord;
    int  sizeW  = u_SizeW / 2;
    int  sizeH  = u_SizeH / 2;
    int count = 0;

    for(int i=-sizeH; i<=sizeH; ++i) {
        for(int j=-sizeW; j<=sizeW; ++j) {
            // Compute convolution, note template image always has offset in center
            convolve += (texture2D(u_Sampler, vec2(tc.x + j*u_StepX, tc.y + i*u_StepY)) *
                    texture2D(u_Sampler_T, vec2(0.5 + j*u_StepX_T, 0.5 + i*u_StepY_T)));

            // Computer sum of input square
            sum += (texture2D(u_Sampler, vec2(tc.x + j*u_StepX, tc.y + i*u_StepY)) *
                    texture2D(u_Sampler, vec2(tc.x + j*u_StepX, tc.y + i*u_StepY)));
        }
    }

    // first division is to get correlation
    // second division is to normalize correlation, that is range 0 - 1
    // formula:
    //             sum of {T(x,y) * I(x-u,y-v)}
    //   C(u,v) = --------------------------------
    //             sqrt{ sum of I(x-u,y-v)^2 }
    //
    //                               C(u,v)
    //  Cnormalized(u,v) = ------------------------
    //                      sqrt{ sum of T(X,Y)^2 }
    //
    gl_FragColor = vec4(convolve.rgb/sqrt(sum.rgb)/u_Sqrt_Sum_T, 1.0);
}
