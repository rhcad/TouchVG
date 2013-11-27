/*
 File: stroke1.fsh
 Abstract: A fragment shader that draws points with assigned color and texture.
 See Apple sample code GLPaint.
 */

uniform sampler2D texture;
varying lowp vec4 color;

void main()
{
	gl_FragColor = color * texture2D(texture, gl_PointCoord);
}
