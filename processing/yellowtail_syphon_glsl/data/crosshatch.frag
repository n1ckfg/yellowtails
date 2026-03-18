varying vec4 vertColor;
varying vec4 vertTexCoord;

uniform sampler2D texture;

uniform float hatch_y_offset;
uniform float lum_threshold_1;
uniform float lum_threshold_2;
uniform float lum_threshold_3;
uniform float lum_threshold_4;

void main()
{
  vec2 uv = vertTexCoord.st;

  vec3 tc = vec3(1.0, 0.0, 0.0);
    float lum = length(texture2D(texture, uv).rgb);
    tc = vec3(1.0, 1.0, 1.0);

    if (lum < lum_threshold_1)
    {
      if (mod(gl_FragCoord.x + gl_FragCoord.y, 10.0) == 0.0)
        tc = vec3(0.0, 0.0, 0.0);
    }  

    if (lum < lum_threshold_2)
    {
      if (mod(gl_FragCoord.x - gl_FragCoord.y, 10.0) == 0.0)
        tc = vec3(0.0, 0.0, 0.0);
    }  

    if (lum < lum_threshold_3)
    {
      if (mod(gl_FragCoord.x + gl_FragCoord.y - hatch_y_offset, 10.0) == 0.0)
        tc = vec3(0.0, 0.0, 0.0);
    }  

    if (lum < lum_threshold_4)
    {
      if (mod(gl_FragCoord.x - gl_FragCoord.y - hatch_y_offset, 10.0) == 0.0)
        tc = vec3(0.0, 0.0, 0.0);
    }



  gl_FragColor = vec4(tc, 1.0);
}