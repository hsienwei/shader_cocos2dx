
#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

uniform sampler2D u_texture;

void main() {
    //from http://www.cnblogs.com/j1223jesus/archive/2012/10/17/2727096.html
    // http://zh.wikipedia.org/wiki/YUV
    // 取YUV的Y(灰階值)
    
    float alpha = texture2D(u_texture, v_texCoord).a; 
    float grey = dot(texture2D(u_texture, v_texCoord).rgb, vec3(0.299, 0.587, 0.114)); 
    gl_FragColor = vec4(grey, grey, grey, alpha); 
}