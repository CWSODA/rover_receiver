#version 330 core

uniform vec2 a_pos;
uniform float a_zoom;

out vec2 pos;

void main(){
    gl_Position = vec4(a_pos * (1.0 / a_zoom), 0.0, 1.0);
    pos = a_pos;
}