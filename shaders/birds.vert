#version 330 core
layout (location = 0) in vec2 birdPos;

void main()
{
   gl_Position = vec4(birdPos.x, birdPos.y, 0.0, 1.0);
}
