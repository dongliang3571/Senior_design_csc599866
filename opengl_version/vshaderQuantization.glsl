attribute vec2  a_Position;	// attribute variable: position vector
attribute vec2  a_TexCoord;
varying   vec2  v_TexCoord;
uniform   mat4  u_MvpMatrix;	// uniform variable for passing rotation matrix

void main() {
	gl_Position = u_MvpMatrix * vec4(a_Position, 0, 1);
	v_TexCoord  = a_TexCoord;
}
