
#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

uniform float u_offset;
uniform sampler2D u_texture;

void main() {
    float pi = 3.14159;
    vec2 texcoord = v_texCoord;
    
    texcoord.x += sin(texcoord.y * 2.0 * 4.0 * pi + u_offset) / 100.0;
    gl_FragColor = texture2D(u_texture, texcoord);
}