/*
 File: Simple.fsh
 Abstract: Fragment shader for each pixel.
 See http://www.raywenderlich.com/3664/opengl-tutorial-for-ios-opengl-es-2-0
 */

varying lowp vec4 DestinationColor; /* lowp: lowest precision for a performance bonus. */

void main(void) {
    gl_FragColor = DestinationColor;
}
