#version 330 core 
in vec3 fragNor;
out vec4 color;
in vec3 pos;
in vec2 fragTex;
uniform sampler2D tex;
uniform sampler2D tex2;
uniform sampler2D tex3;
uniform vec3 camPos;
void main()
{
	vec3 normal = normalize(fragNor);
	// Map normal in the range [-1, 1] to color in range [0, 1];
	vec3 Ncolor = 0.5*normal + 0.5;
	color = vec4(Ncolor, 1.0);

	vec3 lp = vec3(100,100,100);
	vec3 ld = normalize(lp - pos);
	//float light = clamp(dot(ld,normal), 0, 1);
	float light = dot(ld,normal);
	vec3 camPosCopy = camPos;
	camPosCopy = camPosCopy - pos;
	camPosCopy = normalize(camPosCopy);
	vec3 lightV = light*vec3(1,1,1);
	lightV = normalize(lightV);
	vec3 h =  camPosCopy + light*vec3(1,1,1);
	h = normalize(h);
	float specular;
	specular = dot(h, normal);
	specular = pow(specular, 15.0);
	specular = clamp(specular, 0.0, 1.0);
	color = texture(tex, fragTex)*light;
	//color.rgb = fragNor/2.+vec3(.5,.5,.5);
	color+= texture(tex, fragTex) *specular;
	color.a=1;
	color = vec4(1,1,1,1);
	 
}
