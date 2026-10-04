#version 330 core
layout(location=0)in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 pos;
out vec3 fragPosition;

void main()
{
    gl_Position=projection*view*model*vec4(aPos,1.f);
    fragPosition=vec3(model*vec4(aPos,1.f));
    pos=aPos;
}