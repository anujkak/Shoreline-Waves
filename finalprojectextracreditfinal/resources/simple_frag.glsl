#version 330 core 
in vec3 fragNor;
out vec4 color;
in vec3 pos;
in vec2 fragTex;


uniform sampler2D tex;
uniform sampler2D tex2;
uniform sampler2D tex3;
uniform float w;
uniform float x;
uniform vec2 offset;
uniform vec3 camPos;

void main(){

	vec3 normal = vec3(0,1,0);
	vec3 tangent = vec3(1,0,0);
	vec3 bitangent = vec3(0,0,1);
	if(pos.x > 90){
		vec3 normal = normalize(vec3(0,1,pos.z));
		vec3 tangent = vec3(1,0,0);
		vec3 bitangent = vec3(0,0,1);
	}
	



	vec2 texcoord1 = fragTex + w/75;
	vec2 texcoord2 = fragTex + x/50;
	vec3 binormal = cross(normal, tangent);
	//vec3 normal = normalize(fragNor);
	vec3 texturenormal1 = texture(tex2, texcoord1).rgb;
	vec3 texturenormal2 = texture(tex3, texcoord2).rgb;
	texturenormal1 = (texturenormal1 - vec3(0.5, 0.5, 0.5))*2.0;
	texturenormal2 = (texturenormal2 - vec3(0.5,0.5,0.5))*2.0;
	vec3 textureNormal = vec3(texturenormal1.x + texturenormal2.x,texturenormal1.y + texturenormal2.y,texturenormal1.z + texturenormal2.z);
	vec3 bumpNormal = (textureNormal.x * tangent) + (textureNormal.y * normal) + (textureNormal.z * binormal);
	bumpNormal = normalize(bumpNormal);

	vec3 lp = vec3(100,100,100);
	vec3 ld = normalize(lp - pos);
	float light = dot(ld,bumpNormal);
	light = clamp(light,0,1);
	//vec2 texcoord = fragTex;// + offset/100.;
	vec4 baseColor = vec4(.282,.239,.545,0);
	vec3 camPosCopy = camPos;
	camPosCopy = camPosCopy-pos;
	camPosCopy = normalize(camPosCopy);
	vec3 lightV = light*vec3(1,1,1);
	lightV = normalize(lightV);
	vec3 h =  camPosCopy + light*vec3(1,1,1);
	h = normalize(camPosCopy + ld);
	float specular;
	specular = dot(h, bumpNormal);
	specular = pow(specular,0.5);
	specular = clamp(specular, 0.0, 1.0);
	color = baseColor;// + vec4(1,1,1,1) * specular;// *texture(tex2, texcoord);// *light; //[TWOTEXTURES]
	color += vec4(1,1,1,0) * specular;

	color.a=1;
	
}
