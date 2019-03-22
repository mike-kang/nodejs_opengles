varying vec2 tcoord;
uniform sampler2D tex_y;
uniform sampler2D tex_uv;
uniform sampler2D bright_phi;
uniform sampler2D dark_phi;
uniform float alpha;
void main(void)
{
    float r,g,b,y,u,v;
    y = texture2D(tex_y,tcoord).a;
    u = texture2D(tex_uv,tcoord).r;
    v = texture2D(tex_uv,tcoord).a;
    if(alpha < 2.0){
        y = texture2D(dark_phi,vec2(y, 0.0)).a *alpha + texture2D(bright_phi,vec2(y, 0.0)).a * (1.0-alpha);
    }
    y=1.1643*(y-0.0625);
    u=u-0.5;
    v=v-0.5;
    r=y+1.5958*v;
    g=y-0.39173*u-0.81290*v;
    b=y+2.017*u;
    gl_FragColor = vec4(r,g,b,1.0);
}

