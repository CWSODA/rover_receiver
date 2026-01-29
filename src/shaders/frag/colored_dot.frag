#version 330 core

uniform vec3 a_color;

out vec4 frag_color;

void main(){
    if (dot(gl_PointCoord-0.5,gl_PointCoord-0.5)>0.25){
        discard;
    }

    frag_color = vec4(a_color, 1.0);
}