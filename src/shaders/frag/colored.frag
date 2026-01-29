#version 330 core

uniform vec3 a_color;

out vec4 frag_color;

void main(){
    frag_color = vec4(a_color, 1.0);
}