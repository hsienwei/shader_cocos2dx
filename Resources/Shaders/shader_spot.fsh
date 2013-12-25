
#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;
varying vec3 v_position;
//varying vec2 v_texCoord;

uniform vec3 v_spot_pos;
uniform vec4 v_spot_innerColor;
uniform float v_spot_innerRadius;
uniform float v_spot_outerRadius;

void main() {
   
    //float alpha = texture2D(u_texture, v_texCoord).a; 
    //float grey = dot(texture2D(u_texture, v_texCoord).rgb, vec3(0.299, 0.587, 0.114)); 
    //gl_FragColor = vec4(grey, grey, grey, alpha); 
    float spotDistance = distance(v_position, v_spot_pos);
    if( spotDistance > v_spot_outerRadius)
        gl_FragColor = v_fragmentColor;//vec4(1, 0, 0, 1); //
    else if(spotDistance < v_spot_innerRadius)
        gl_FragColor = v_spot_innerColor; //    
    else
    {
        float step = smoothstep(v_spot_outerRadius, v_spot_innerRadius, spotDistance);
        gl_FragColor = v_spot_innerColor * step + v_fragmentColor * (1- step);
    }
}