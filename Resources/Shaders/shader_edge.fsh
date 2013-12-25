#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

uniform sampler2D u_texture;

//void main() {
    
   ////implement OpenGl Blend
   ////src: GL_DST_ALPHA
   ////dst: GL_ZERO
   ////func: GL_FUNC_ADD
   //
   //vec4 mainColor = texture2D(u_texture, v_texCoord);
   ////vec4 maskColor = texture2D(u_mask, v_texCoord);
   ////vec4 srcColor = vec4(mainColor.r * maskColor.a,
   ////                     mainColor.g * maskColor.a,
   ////                     mainColor.b * maskColor.a,
   ////                     mainColor.a * maskColor.a);
   //
   ////dst color = vec4(0.0)
   //
	//gl_FragColor = mainColor;
    //---------------------
    //vec4 total = vec4(0.0);
    //vec4 u_borderColor = vec4(1, 0, 0, 1);
    //float alpha = texture2D(u_texture, v_texCoord).a; 
    ////If somewhere between complete transparent and completely opaque
    //if (alpha > 0.0 && alpha < 1.0)
    //{
    //    total.rgb = u_borderColor.rgb;
    //    total.a = 1 - alpha;
    //    gl_FragColor = u_borderColor;
    //}
    //else
    //{
    //    gl_FragColor = texture2D(u_texture, v_texCoord);
    //}
    //------------------------
//}
    
float threshold(in float thr1, in float thr2 , in float val) {
 if (val < thr1) {return 0.0;}
 if (val > thr2) {return 1.0;}
 return val;
}

// averaged pixel intensity from 3 color channels
float avg_intensity(in vec4 pix) {
 return (pix.r + pix.g + pix.b)/3.;
}

vec4 get_pixel(in vec2 coords, in float dx, in float dy) {
 return texture2D(u_texture,coords + vec2(dx, dy));
}

// returns pixel color
float IsEdge(in vec2 coords){
  float dxtex = 1.0 / 74.0 /*image width*/;
  float dytex = 1.0 / 240.0 /*image height*/;
  float pix[9];
  int k = -1;
  float delta;

  // read neighboring pixel intensities
  for (int i=-1; i<2; i++) {
   for(int j=-1; j<2; j++) {
    k++;
    pix[k] = get_pixel(coords,float(i)*dxtex,float(j)*dytex).a;//avg_intensity(get_pixel(coords,float(i)*dxtex,
             //                             float(j)*dytex));
   }
  }

  // average color differences around neighboring pixels
  delta = (abs(pix[1]-pix[7])+
          abs(pix[5]-pix[3]) +
          abs(pix[0]-pix[8])+
          abs(pix[2]-pix[6])
           )/4.;

  return threshold(0.5,0.9,clamp(1.8*delta,0.0,1.0));
}

void main()
{
    vec4 color = vec4(0.0,0.0,1.0,1.0);
    if(IsEdge(v_texCoord))
        gl_FragColor = color;
    else
        gl_FragColor=  texture2D(u_texture, v_texCoord);

}