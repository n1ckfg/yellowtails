varying vec4 vertColor;
varying vec4 vertTexCoord;

uniform sampler2D texture;

void main ()
{
  vec2 uv = vertTexCoord.st;
  vec4 c = texture2D(texture, uv);

  c += texture2D(texture, uv+0.001);
  c += texture2D(texture, uv+0.003);
  c += texture2D(texture, uv+0.005);
  c += texture2D(texture, uv+0.007);
  c += texture2D(texture, uv+0.009);
  c += texture2D(texture, uv+0.011);

  c += texture2D(texture, uv-0.001);
  c += texture2D(texture, uv-0.003);
  c += texture2D(texture, uv-0.005);
  c += texture2D(texture, uv-0.007);
  c += texture2D(texture, uv-0.009);
  c += texture2D(texture, uv-0.011);

  c.rgb = vec3((c.r+c.g+c.b)/3.0);
  c = c / 9.5;
  gl_FragColor = c;
}