#version 330 core
layout (location = 0) in vec3 birdPos;

void main()
{
   gl_Position = vec4(birdPos.x, birdPos.y, birdPos.z, 1.0);
}
