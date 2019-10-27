attribute highp vec4 posAttr;
attribute mediump vec4 texAttr;
varying mediump vec4 texc;
uniform highp mat4 matrix;
uniform mat4 posGlObject;

void main() {
   texc = texAttr;
   gl_Position = posGlObject * matrix * posAttr;
}
