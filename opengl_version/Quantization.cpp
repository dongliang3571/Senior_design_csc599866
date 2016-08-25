//
//  Quantization.cpp
//  hw
//
//  Created by dong liang on 8/23/16.
//
//

#include "Quantization.h"

enum {
    MV,
    THETA,
    SUBDIV,
    TWIST,
    SAMPLER
};


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Quantization::Quantization:
//
// Quantization constructor.
//
Quantization::Quantization(QWidget *parent)
: GLWidget(parent)
{
    m_numberVertices = 4;
    m_isInitialized = false;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Quantization::controlPanel:
//
// Create control panel groupbox.
//
QGroupBox*
Quantization::controlPanel()
{
    // init group box
    QGroupBox *groupBox = new QGroupBox("Homework 3a");
    
    
    return(groupBox);
}



// Quantization::setImage
//
// set image
//
void Quantization::setImage(QImage image)
{
    m_image = QImage(image);
    if(!m_isInitialized) {
        initializeGL();
        resizeGL(m_winW, m_winH);
        updateGL();
    }
    updateGL();
}



void Quantization::reload()
{
//    glUniform1i(m_uniform[ISINPUT], m_isInput); // pass threshold reference to fragment shader
//    glUniform1i(m_uniform[ISRGB], m_isRGB); // pass threshold reference to fragment shader
    initTexture();
    updateGL();
}
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Quantization::reset:
//
// Reset parameters.
//
void
Quantization::reset()
{
    
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Quantization::initializeGL:
//
// Initialization routine before display loop.
// Gets called once before the first time resizeGL() or paintGL() is called.
//
void
Quantization::initializeGL()
{
    initializeGLFunctions();    // initialize GL function resolution for current context
    
    if(!m_image.isNull()) {
        initTexture();  // init texture
        initShaders();  // init vertex and fragment shaders
        initVertexBuffer(); // initialize vertex buffer and write positions to vertex shader
        m_isInitialized = true;
    }
    
    glClearColor(0.0, 0.0, 0.0, 0.0);	// set background color
    glColor3f   (1.0, 1.0, 1.0);		// set foreground color
}




// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Quantization::resizeGL:
//
// Resize event handler.
// The input parameters are the window width (w) and height (h).
//
void
Quantization::resizeGL(int w, int h)
{
    // save window dimensions
    m_winW = w;
    m_winH = h;
    
    // compute aspect ratio
    float ar = (float) w / h;
    float xmax, ymax;
    if(ar > 1.0) {		// wide screen
        xmax = ar;
        ymax = 1.;
    }
    else {			// tall screen
        xmax = 1.;
        ymax = 1 / ar;
    }
    
    // set viewport to occupy full canvas
    glViewport(0, 0, w, h);
    
    // init viewing coordinates for orthographic projection
    glLoadIdentity();
    glOrtho(-xmax, xmax, -ymax, ymax, -1.0, 1.0);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Quantization::paintGL:
//
// Update GL scene.
//
void
Quantization::paintGL()
{
    // clear canvas with background color
    glClear(GL_COLOR_BUFFER_BIT);
    
    // draw texture mapped triangles
    glUseProgram(m_program[0].programId());		// bind the glsl progam
    glDrawArrays(GL_TRIANGLES, 0, 3);
    
    
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Quantization::initTexture:
//
// Initialize texture.
//
void
Quantization::initTexture()
{
    // read image into buffer
    if(!m_image.isNull()) {
        // convert jpg to GL formatted image
        QImage qImage = QGLWidget::convertToGLFormat(m_image);
        
        // init vars
        int w = qImage.width ();
        int h = qImage.height();
        
        // generate texture object and bind it
        // GL_TEXTURE0 is texture unit, which is for managing a texture image, each has an associated GL_TEXTURE_2D,
        // which is the textuture target for specifying the type of texture
        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &m_texture);
        glBindTexture(GL_TEXTURE_2D, m_texture);
        
        // set the texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, qImage.bits());
    } else {
        QMessageBox::critical(0, "Error", "Image loading error",QMessageBox::Ok);
        QApplication::quit();
    }
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Quantization::initShader1:
//
// Initialize vertex and fragment shaders for texture mapping.
//
void
Quantization::initShaders()
{
    // compile vertex shader
    if(!m_program[0].addShaderFromSourceFile(QGLShader::Vertex, ":/vshaderQuantization.glsl")) {
        QMessageBox::critical(0, "Error", "Vertex shader error", QMessageBox::Ok);
        QApplication::quit();
    }
    
    // compile fragment shader
    if(!m_program[0].addShaderFromSourceFile(QGLShader::Fragment, ":/fshaderQuantization.glsl")) {
        QMessageBox::critical(0, "Error", "Fragment shader error",QMessageBox::Ok);
        QApplication::quit();
    }
    
    // bind the attribute variable in the glsl program with a generic vertex attribute index;
    // values provided via ATTRIB_VERTEX will modify the value of "a_position")
    glBindAttribLocation(m_program[0].programId(), ATTRIB_VERTEX, "a_Position");
    
    // bind the attribute variable in the glsl program with a generic vertex attribute index;
    // values provided via ATTRIB_TEXTURE_POSITION will modify the value of "a_TexCoord")
    glBindAttribLocation(m_program[0].programId(), ATTRIB_TEXTURE_POSITION, "a_TexCoord");
    
    // link shader pipeline; attribute bindings go into effect at this point
    if(!m_program[0].link()) {
        QMessageBox::critical(0, "Error", "Could not link shader", QMessageBox::Ok);
        QApplication::quit();
    }
    
    
    
    
    // get storage location of u_Sampler in fragment shader
    m_uniform[0][SAMPLER] = glGetUniformLocation(m_program[0].programId(), "u_Sampler");
    if((int) m_uniform[0][SAMPLER] < 0) {
        qDebug() << "Failed to get the storage location of u_Sampler";
        exit(-1);
    }
    
    // bind the glsl program
    glUseProgram(m_program[0].programId());
    
    
    glUniform1i(m_uniform[0][SAMPLER], 0);
}





// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Quantization::initVertexBuffer:
//
// Initialize vertex buffer.
//
void
Quantization::initVertexBuffer()
{
    // init geometry data
    const vec2 vertices[] = {
        vec2( 0.0f,   0.75f ),
        vec2( 0.65f, -0.375f),
        vec2(-0.65f, -0.375f)
    };
    
    
    // init texture coordinates
    const vec2 textureCoords[] = {
        vec2((vertices[0][0]+.65)/1.3, (vertices[0][1]+.375)/1.125),
        vec2((vertices[1][0]+.65)/1.3, (vertices[1][1]+.375)/1.125),
        vec2((vertices[2][0]+.65)/1.3, (vertices[2][1]+.375)/1.125)
    };
    
    
    
    static GLuint vertexBuffer = -1;
    glGenBuffers(1, &vertexBuffer);
    // bind the buffer to the GPU; all future drawing calls gets data from this buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    // copy the vertices from CPU to GPU
    glBufferData(GL_ARRAY_BUFFER, 3*sizeof(vec2), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, false, 0, NULL);
    glEnableVertexAttribArray(ATTRIB_VERTEX);
    
    
    //Create texture coordinates buffer
    static GLuint texCoordBuffer = -1;
    glGenBuffers(1, &texCoordBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, 3*sizeof(vec2), textureCoords, GL_STATIC_DRAW);
    glVertexAttribPointer(ATTRIB_TEXTURE_POSITION, 2, GL_FLOAT, false, 0, NULL);
    glEnableVertexAttribArray(ATTRIB_TEXTURE_POSITION);
    
    
    
}
