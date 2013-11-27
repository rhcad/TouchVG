/*
 File: Simple.vsh
 Abstract: Vertex shader that passes attributes through to fragment shader.
 See http://www.raywenderlich.com/3664/opengl-tutorial-for-ios-opengl-es-2-0
 uniform, attribute, varying: http://blog.csdn.net/jackers679/article/details/6848085
 */

attribute vec4 Position;        /* input variable for the position of the vertex. */
attribute vec4 SourceColor;     /* input variable for the color of the vertex with 4 components. */

varying vec4 DestinationColor;  /* output variable that will be passed to the fragment shader. */
uniform mat4 Projection;
uniform mat4 Modelview;

void main(void) {
    DestinationColor = SourceColor;
    gl_Position = Projection * Modelview * Position;
}
