varying vec2 tcoord;
uniform sampler2D tex;
uniform sampler2D bright_phi;
uniform sampler2D dark_phi;
uniform float alpha;
void main(void)
{
    float r,b,g,k;
    b = texture2D(tex,tcoord).r;
    g = texture2D(tex,tcoord).g;
    r = texture2D(tex,tcoord).b;
    
    r = texture2D(dark_phi,vec2(r, 0.0)).a *alpha + texture2D(bright_phi,vec2(r, 0.0)).a * (1.0-alpha);
    g = texture2D(dark_phi,vec2(g, 0.0)).a *alpha + texture2D(bright_phi,vec2(g, 0.0)).a * (1.0-alpha);
    b = texture2D(dark_phi,vec2(b, 0.0)).a *alpha + texture2D(bright_phi,vec2(b, 0.0)).a * (1.0-alpha);

    gl_FragColor = vec4(r,g,b,1.0);

}

