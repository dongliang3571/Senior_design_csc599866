varying vec2 v_TexCoord;
uniform sampler2D u_Sampler;

void main(void){

	float gray = texture2D(u_Sampler,v_TexCoord).r*0.30 + texture2D(u_Sampler,v_TexCoord).g*0.59 + texture2D(u_Sampler,v_TexCoord).b*0.11;
	gl_FragColor = vec4(gray, gray, gray, 1);
}
