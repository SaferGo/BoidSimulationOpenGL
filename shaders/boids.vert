#version 330 core
layout (location = 0) in vec2 trianglePos;
layout (location = 1) in vec3 triangleColor;

out vec3 colorSpecie;

void main()
{
   gl_Position = vec4(trianglePos.x, trianglePos.y, 0.0, 1.0);
   colorSpecie = triangleColor;
}
