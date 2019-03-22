attribute vec4 vertex;
attribute vec2 a_texCoord;
varying vec2 tcoord;
void main(void)
{
        vec4 pos = vertex;
        tcoord = a_texCoord;
        gl_Position = pos;
}

