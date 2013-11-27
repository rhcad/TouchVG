/*
 File: stroke1.vsh
 Abstract: A vertex shader that draws points with assigned color.
 See Apple sample code GLPaint.
 */

attribute vec4 inVertex;

uniform mat4 MVP;
uniform float pointSize;
uniform lowp vec4 vertexColor;

varying lowp vec4 color;

void main()
{
	gl_Position = MVP * inVertex;
    gl_PointSize = pointSize;
    color = vertexColor;
}
