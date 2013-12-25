
#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;
varying vec3 v_position;

uniform sampler2D u_bg_texture;
uniform vec2 u_bg_size;
uniform vec2 u_bg_CoordOffset;

void main() {

    vec2 texCoord = mod(vec2(v_position.x/u_bg_size.x + u_bg_CoordOffset.x, -v_position.y/u_bg_size.y + u_bg_CoordOffset.y), 1.0);
    vec4 mainColor = texture2D(u_bg_texture, texCoord);
	gl_FragColor = mainColor;
}