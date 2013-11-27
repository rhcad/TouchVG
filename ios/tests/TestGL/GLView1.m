// GLView1.m
// Copyright (c) 2013, https://github.com/rhcad/touchvg
// Refrence: http://www.cnblogs.com/andyque/archive/2011/08/08/2131019.html

#import "GLView1.h"
#import "CC3GLMatrix.h"
#import "DetailViewController.h"

typedef struct {
    float position[2];  // XY
    float color[4];     // RGBA
} Vertex;

static const Vertex vertices[] = {
    {{1, -1}, {1, 0, 0, 1}},
    {{1, 1}, {0, 1, 0, 1}},
    {{-1, 1}, {0, 0, 1, 1}},
    {{-1, -1}, {0, 0, 0, 1}}
};

static const GLushort indices[] = {
    0, 1, 2,            // right top corrner triangle
    2, 3, 0             // left bottom corrner triangle
};

@implementation GLView1

+ (Class)layerClass {
    return [CAEAGLLayer class];
}

- (BOOL)setupContext {
    self.contentScaleFactor = [UIScreen mainScreen].scale;  // support for Retina screen
    _context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    if (!_context) {
        NSLog(@"Failed to initialize OpenGLES 2.0 context");
        return NO;
    }
    
    if (![EAGLContext setCurrentContext:_context]) {
        NSLog(@"Failed to set current OpenGL context");
        return NO;
    }
    
    return YES;
}

- (BOOL)setupRenderBuffer {
    glGenRenderbuffers(1, &_renderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, _renderBuffer);
    return [_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer*) self.layer];
}

- (BOOL)setupFrameBuffer {
    glGenFramebuffers(1, &_frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                              GL_RENDERBUFFER, _renderBuffer); // attach renderbuffer
    
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER) ;
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        NSLog(@"failed to make complete framebuffer object %x", status);
        return NO;
    }
    
    return YES;
}

- (GLuint)compileShader:(NSString*)shaderName withType:(GLenum)shaderType {
    NSString* shaderPath = [[NSBundle mainBundle] pathForResource:shaderName ofType:nil];
    NSError* error;
    NSString* shaderString = [NSString stringWithContentsOfFile:shaderPath
                                                       encoding:NSUTF8StringEncoding error:&error];
    if (!shaderString) {
        NSLog(@"Error loading shader (%@): %@", shaderName, error.localizedDescription);
        return 0;
    }
    
    _shader = glCreateShader(shaderType);
    
    const char* shaderStringUTF8 = [shaderString UTF8String];
    GLint shaderStringLength = [shaderString length];
    glShaderSource(_shader, 1, &shaderStringUTF8, &shaderStringLength);
    
    glCompileShader(_shader);
    
    GLint compileSuccess;
    glGetShaderiv(_shader, GL_COMPILE_STATUS, &compileSuccess);
    if (!compileSuccess) {
        GLchar messages[256];
        glGetShaderInfoLog(_shader, sizeof(messages), 0, &messages[0]);
        NSLog(@"%@", [NSString stringWithUTF8String:messages]);
        exit(1);
    }
    
    return _shader;
}

- (BOOL)compileShaders {
    GLuint vertexShader = [self compileShader:@"simple.vsh" withType:GL_VERTEX_SHADER];
    GLuint fragmentShader = [self compileShader:@"simple.fsh" withType:GL_FRAGMENT_SHADER];
    
    if (!vertexShader || !fragmentShader) {
        return NO;
    }
    
    _program = glCreateProgram();
    glAttachShader(_program, vertexShader);
    glAttachShader(_program, fragmentShader);
    glLinkProgram(_program);
    
    GLint linkSuccess;
    glGetProgramiv(_program, GL_LINK_STATUS, &linkSuccess);
    if (!linkSuccess) {
        GLchar messages[256];
        glGetProgramInfoLog(_program, sizeof(messages), 0, &messages[0]);
        NSLog(@"%@", [NSString stringWithUTF8String:messages]);
        return NO;
    }
    
    glUseProgram(_program);
    
    // Enable to store additional values for each point rather than it's coordinates.
    _positionSlot = glGetAttribLocation(_program, "Position");
    _colorSlot = glGetAttribLocation(_program, "SourceColor");
    glEnableVertexAttribArray(_positionSlot);
    glEnableVertexAttribArray(_colorSlot);
    
    _projectionUniform = glGetUniformLocation(_program, "Projection");
    _modelViewUniform = glGetUniformLocation(_program, "Modelview");
    
    return YES;
}

- (void)render:(CADisplayLink*)displayLink {
    if (_times++ == 0) {
        glClearColor(0, 104.0/255.0, 55.0/255.0, 1.0);
        
        GLint width = 0, height = 0;
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
        glViewport(0, 0, width, height);
        
        glVertexAttribPointer(_positionSlot, 2, GL_FLOAT, GL_FALSE,
                              sizeof(Vertex), 0);               // two floats (x,y) for each vertex.
        glVertexAttribPointer(_colorSlot, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), // four floats (r,g,b,a)
                              (GLvoid*) (sizeof(float) * 2));   // the offset within the structure
        
        CC3GLMatrix *projection = [CC3GLMatrix matrix];
        float h = 4.0f * self.frame.size.height / self.frame.size.width;
        [projection populateFromFrustumLeft:-2 andRight:2 andBottom:-h/2 andTop:h/2 andNear:4 andFar:10];
        glUniformMatrix4fv(_projectionUniform, 1, 0, projection.glMatrix);
    }
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    for (float i = 10; i >= 0; i -= 1) {
        CC3GLMatrix *modelView = [CC3GLMatrix matrix];
        double a = CACurrentMediaTime() + i / 10;
        [modelView populateFromTranslation:CC3VectorMake(sin(a), cos(a), -7)];
        glUniformMatrix4fv(_modelViewUniform, 1, 0, modelView.glMatrix);
        
        if (i == 0) {
            glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(indices[0]), GL_UNSIGNED_SHORT, 0);
        } else {
            glDrawElements(GL_LINE_LOOP, sizeof(indices)/sizeof(indices[0]), GL_UNSIGNED_SHORT, 0);
        }
    }
    [_context presentRenderbuffer:GL_RENDERBUFFER];     // Display the render buffer to screen
}

- (void)setupDisplayLink {
    _displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(render:)];
    [_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}

// VBO allows data to be stored in server-side memory but still client read/write-able
// Vertex attribute data copied to server memory using glBufferData() or glBufferSubData()
// When glDraw*() is called, data is already resident.
// If there are multiple vertex attributes, we can put them back to back in the buffer object
//   using glBufferSubData.
// To avoid having two copies of data, client can share server memory using glMapBuffer()/glUnmapBuffer()
//   after the call to glBufferData()

- (void)setupVBOs {
    glGenBuffers(1, &_vertexBuffer);                    // generate buffer object descriptor
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);       // set the current buffer object's type
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);  // send data to GL
    
    glGenBuffers(1, &_indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

- (id)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        [self performSelector:@selector(setupGL) withObject:nil afterDelay:0.01];
    }
    return self;
}

- (BOOL)setupGL {
    BOOL ret = ([self setupContext] &&
                [self setupRenderBuffer] &&
                [self setupFrameBuffer] &&
                [self compileShaders]);
    if (ret) {
        [self setupVBOs];
        [self setupDisplayLink];
        [self addButtons];
    }
    
    return ret;
}

- (void)tearDown {
    if (_displayLink) {
        [_displayLink removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        _displayLink = nil;
        glDeleteBuffers(1, &_renderBuffer);
        glDeleteBuffers(1, &_frameBuffer);
        glDeleteBuffers(1, &_vertexBuffer);
        glDeleteBuffers(1, &_indexBuffer);
        glDeleteShader(_shader);
        glDeleteProgram(_program);
        [EAGLContext setCurrentContext:nil];
    }
}

- (UINavigationItem *) navigationItem {
    UIViewController *detailc = (UIViewController *)self.superview.nextResponder;
    if (![detailc respondsToSelector:@selector(saveDetailPage:)]) {
        detailc = (UIViewController *)self.superview.superview.nextResponder;
    }
    return detailc.navigationItem;
}

- (void)addButtons {
    UINavigationItem *navigationItem = [self navigationItem];
    UIBarButtonItem *pauseButton = [[UIBarButtonItem alloc]
                                    initWithBarButtonSystemItem:UIBarButtonSystemItemPause
                                    target:self action:@selector(clickPause:)];
    
    navigationItem.rightBarButtonItems = @[navigationItem.rightBarButtonItem, pauseButton];
}

- (void)clickPause:(id)sender {
    _displayLink.paused = !_displayLink.paused;
}

// See https://developer.apple.com/library/ios/qa/qa1704/_index.html
- (UIImage *)snapshot {
    GLint width = 0, height = 0;
    
    // Get the size of the backing CAEAGLLayer
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
    
    NSInteger dataLength = width * height * 4;
    GLubyte *data = dataLength ? (GLubyte*)malloc(dataLength * sizeof(GLubyte)) : NULL;
    
    if (!data) {
        NSLog(dataLength ? @"Out of memory to snapshot" : @"No backing layer to snapshot");
        return nil;
    }
    
    // Read pixel data from the framebuffer
    glPixelStorei(GL_PACK_ALIGNMENT, 4);
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
    
    // Create a CGImage with the pixel data
    // If your OpenGL ES content is opaque, use kCGImageAlphaNoneSkipLast to ignore the alpha channel
    // otherwise, use kCGImageAlphaPremultipliedLast
    
    CGDataProviderRef ref = CGDataProviderCreateWithData(NULL, data, dataLength, NULL);
    CGColorSpaceRef colorspace = CGColorSpaceCreateDeviceRGB();
    CGImageRef iref = CGImageCreate(width, height, 8, 32, width * 4, colorspace,
                                    kCGBitmapByteOrder32Big | kCGImageAlphaNoneSkipLast,
                                    ref, NULL, true, kCGRenderingIntentDefault);
    
    // OpenGL ES measures data in PIXELS
    // Create a graphics context with the target size measured in POINTS
    
    CGFloat scale = self.contentScaleFactor;
    CGSize sizeInPt = CGSizeMake(width / scale, height / scale);
    
    UIGraphicsBeginImageContextWithOptions(sizeInPt, NO, scale);
    
    CGContextRef cgcontext = UIGraphicsGetCurrentContext();
    
    // UIKit coordinate system is upside down to GL/Quartz coordinate system
    // Flip the CGImage by rendering it to the flipped bitmap context
    // The size of the destination area is measured in POINTS
    
    CGContextSetBlendMode(cgcontext, kCGBlendModeCopy);
    CGContextDrawImage(cgcontext, CGRectMake(0.f, 0.f, sizeInPt.width, sizeInPt.height), iref);
    
    // Retrieve the UIImage from the current context
    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
    
    UIGraphicsEndImageContext();
    
    // Clean up
    free(data);
    CFRelease(ref);
    CFRelease(colorspace);
    CGImageRelease(iref);
    
    return image;
}

- (void)save {
    UIImage *image = [self snapshot];
    if (!image) {
        return;
    }
    
    NSString *path = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,
                                                          NSUserDomainMask, YES) objectAtIndex:0];
    static int order = 0;
    NSString *fmt = image.scale > 1.5f ? @"%@/page%d@2x.png" : @"%@/page%d.png";
    NSString *filename = [NSString stringWithFormat:fmt, path, order++ % 10];
    
    NSData* imageData = UIImagePNGRepresentation(image);
    BOOL ret = imageData && [imageData writeToFile:filename atomically:NO];
    
    if (ret) {
        NSString *msg = [NSString stringWithFormat:@"%.0f x %.0f\n%@",
                         image.size.width, image.size.height,
                         [filename substringFromIndex:[filename rangeOfString:@"Doc"].location]];
        [[[UIAlertView alloc] initWithTitle:@"Save" message:msg
                                   delegate:nil cancelButtonTitle:@"OK"
                          otherButtonTitles:nil] show];
    }
}

@end
