uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;

layout (std140) uniform InstanceBlock
{
	mat4 modelMatrix;
	vec4 color;
	vec2 spriteSize;
};

out vec4 vColor;

void main()
{
	vec2 aPosition = vec2(0.5 - float(gl_VertexID >> 1), 0.5 - float(gl_VertexID % 2));
	vec4 position = vec4(aPosition.x * spriteSize.x, aPosition.y * spriteSize.y, 0.0, 1.0);

	gl_Position = uProjectionMatrix * uViewMatrix * modelMatrix * position;
	vColor = color;
}
