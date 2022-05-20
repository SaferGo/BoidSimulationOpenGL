#version 330 core

in vec3 colorSpecie;
out vec4 FragColor;

void main()
{
   FragColor = vec4(colorSpecie, 1.0);
}
