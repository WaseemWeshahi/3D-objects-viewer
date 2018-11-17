#version 150

in  vec3 vPosition;
in vec3 vcolor;
in vec2 vnormal;
in vec2 vTexCoord;

out vec3 vColor;
out vec2 normal;
out vec4 vpos;
out vec2 texCoord;

void main()
{
    gl_Position.xyz = vPosition;
	gl_Position.w=1;

	vec4 vpos = gl_Position;
	texCoord = vTexCoord;

	vColor = vcolor;
	normal = vnormal;
}
