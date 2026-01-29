#version 330 core

layout (location = 0) in vec2 a_pos;

uniform float a_radius;
uniform float a_zoom;

void main(){
    gl_Position = vec4(a_pos * a_radius * (1.0 / a_zoom), 0.0, 1.0);
}