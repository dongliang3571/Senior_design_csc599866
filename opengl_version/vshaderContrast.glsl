attribute vec2  a_Position;	// attribute variable: position vector
attribute vec2  a_TexCoord;
varying   vec2  v_TexCoord;

void main() {
 	gl_Position = vec4(a_Position, 0, 1);
	v_TexCoord  = a_TexCoord;
}
