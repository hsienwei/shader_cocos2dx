
#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

uniform sampler2D u_texture;
uniform sampler2D u_mask;

void main() {
    
    //implement OpenGl Blend
    //src: GL_DST_ALPHA
    //dst: GL_ZERO
    //func: GL_FUNC_ADD
    
    vec4 mainColor = texture2D(u_texture, v_texCoord);
    vec4 maskColor = texture2D(u_mask, v_texCoord);
    vec4 srcColor = vec4(mainColor.r * maskColor.a,
                         mainColor.g * maskColor.a,
                         mainColor.b * maskColor.a,
                         mainColor.a * maskColor.a);
    
    //dst color = vec4(0.0)
    
	gl_FragColor = srcColor;
}