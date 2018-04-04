#version  430 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform float w;
uniform vec2 offset;
out vec3 fragNor;
out vec2 fragTex;
out vec3 pos;
uniform sampler2D tex;

void main()
{

vec4 tpos = vertPos;


tpos.x += (sin(w*0.3+vertNor.y*1.4)*vertNor.x*20)/1.4;
tpos.z = ((sin(w*0.3+3)+1)/2)*vertNor.z;
pos = tpos.xyz;
gl_Position = P * V * M * tpos;
fragNor = (M * vec4(vertNor, 0.0)).xyz;
fragTex = vertTex;

}
