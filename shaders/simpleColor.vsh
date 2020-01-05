attribute vec4 in_position;
attribute vec3 colAttr;
varying vec4 uv;
varying vec3 col;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform float size;

void main() {
    col=colAttr;
    vec4 pos = vec4(in_position.x * size,in_position.y,in_position.z * size, 1.0);
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * pos;
}
