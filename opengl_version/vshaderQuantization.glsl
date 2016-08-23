attribute vec2  a_Position;	// attribute variable: position vector
varying   vec2  v_TexCoord;
attribute vec2  a_TexCoord;


void main() {

            gl_Position = gl_ModelViewProjectionMatrix * vec4(a_Position,0,1);
            v_TexCoord  = a_TexCoord;

}
