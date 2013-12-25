attribute vec4 a_position;
attribute vec4 a_color;
#ifdef GL_ES
varying lowp vec4 v_fragmentColor;
#else
varying vec4 v_fragmentColor;
varying vec3 v_position;
#endif

void main()
{
    gl_Position = CC_MVPMatrix * a_position;
    v_position = a_position;
    v_fragmentColor = a_color;
}

