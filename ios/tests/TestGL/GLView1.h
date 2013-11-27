// GLView1.h
// Copyright (c) 2013, https://github.com/rhcad/touchvg

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#include <OpenGLES/ES2/gl.h>

@interface GLView1 : UIView {
  @protected
    int    _times;
    CADisplayLink* _displayLink;
    EAGLContext* _context;
    GLuint _renderBuffer;
    GLuint _frameBuffer;
    GLuint _positionSlot;
    GLuint _colorSlot;
    GLuint _projectionUniform;
    GLuint _modelViewUniform;
    GLuint _vertexBuffer;
    GLuint _indexBuffer;
    GLuint _shader;
    GLuint _program;
}

- (void)tearDown;
- (BOOL)setupGL;

- (BOOL)setupContext;
- (BOOL)setupRenderBuffer;
- (BOOL)setupFrameBuffer;
- (GLuint)compileShader:(NSString*)shaderName withType:(GLenum)shaderType;
- (BOOL)compileShaders;
- (void)render:(CADisplayLink*)displayLink;
- (void)setupDisplayLink;
- (void)setupVBOs;

- (UIImage *)snapshot;
- (void)save;
- (void)addButtons;

@end
