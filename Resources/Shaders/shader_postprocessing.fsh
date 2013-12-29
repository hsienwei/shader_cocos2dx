
#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

uniform float u_offset;
uniform sampler2D u_texture;

void main() {

    vec2 texcoord = v_texCoord;
    texcoord.x += sin(texcoord.y * 4*2*3.14159 + u_offset) / 100;
    gl_FragColor = texture2D(u_texture, texcoord);

    
    
    //vec4 mainColor = texture2D(u_texture, v_texCoord);
    //vec4 maskColor = texture2D(u_mask, v_texCoord);
    //vec4 srcColor = vec4(mainColor.r * maskColor.a,
    //                     mainColor.g * maskColor.a,
    //                     mainColor.b * maskColor.a,
    //                     mainColor.a * maskColor.a);
    //
    ////dst color = vec4(0.0)
    //
	//gl_FragColor = srcColor;
}