#version 300 es

layout(location = 0) in vec3 inPosition;

in float field;

uniform vec4 color;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out vec4 fragColor;
out float fieldDoppler;

void main() {
  vec4 posEyeSpace = viewMatrix * modelMatrix * vec4(inPosition, 1);

  float i = 1.0 - (-posEyeSpace.z / 100.0);
  fragColor = vec4(i, i, i, 1) * color;
  fieldDoppler = field;

  gl_Position = projMatrix * posEyeSpace;
}