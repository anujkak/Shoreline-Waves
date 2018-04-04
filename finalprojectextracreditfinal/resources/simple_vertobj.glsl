#version  330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;
uniform mat4 P;
uniform mat4 M;
uniform mat4 V;
uniform vec3 camPos;

out vec3 fragNor;
out vec2 fragTex;
out vec3 pos;

void main()
{
pos=vertPos.xyz;
gl_Position = P * M*V * vertPos;
fragNor = (M*V * vec4(vertNor, 0.0)).xyz;
fragNor = vertNor;
fragTex = vertTex;
fragTex.y*=-1;
}
