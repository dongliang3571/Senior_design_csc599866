//
//  Contrast.cpp
//  hw
//
//  Created by dong liang on 8/16/16.
//
//

#include "Contrast.h"

enum {
    SAMPLER
};


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contrast::Contrast:
//
// Contrast constructor.
//
Contrast::Contrast(QWidget *parent)
: HW(parent)
{
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contrast::controlPanel:
//
// Create control panel groupbox.
//
QGroupBox*
Contrast::controlPanel()
{
    // init group box
    QGroupBox *groupBox = new QGroupBox("Homework 3a");
    
    return(groupBox);
}





// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contrast::reset:
//
// Reset parameters.
//
void
Contrast::reset()
{
    
}




// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contrast::initializeGL:
//
// Initialization routine before display loop.
// Gets called once before the first time resizeGL() or paintGL() is called.
//
void
Contrast::initializeGL()
{
    // initialize GL function resolution for current context
    initializeGLFunctions();
    
    // init texture
    initTexture();
    
    // init vertex and fragment shaders
    initShaders();
    
    // initialize vertex buffer and write positions to vertex shader
    initVertexBuffer();
    
    // init state variables
    glClearColor(0.0, 0.0, 0.0, 0.0);	// set background color
    glColor3f   (1.0, 1.0, 0.0);		// set foreground color
    glRotated(70, 0, 0, 0);
}





// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contrast::resizeGL:
//
// Resize event handler.
// The input parameters are the window width (w) and height (h).
//
void Contrast::resizeGL(int w, int h)
{
    // save window dimensions
    m_winW = w;
    m_winH = h;
    
    // compute aspect ratio
    float ar = (float) w / h;
    
    // set xmax, ymax;
    float xmax, ymax;
    if(ar > 1.0) {		// wide screen
        xmax = ar;
        ymax = 1.;
    } else {		// tall screen
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
// Contrast::paintGL:
//
// Update GL scene.
//
void
Contrast::paintGL()
{
    // clear canvas with background color
    glClear(GL_COLOR_BUFFER_BIT);
    
    // draw texture mapped triangles
    glDrawArrays(GL_TRIANGLES, 0, 3);
    
    
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contrast::initTexture:
//
// Initialize texture.
//
void
Contrast::initTexture()
{
    // read image from file
    m_image.load(QString(":/mandrill.jpg"));
    
    // convert jpg to GL formatted image
    QImage qImage = QGLWidget::convertToGLFormat(m_image);
    
    // init vars
    int w = qImage.width ();
    int h = qImage.height();
    
    // generate texture name and bind it
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    
    // set the texture parameters
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, qImage.bits());
}







// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contrast::initShader:
//
// Initialize vertex and fragment shaders for texture mapping.
//
void Contrast::initShaders()
{
    // compile vertex shader
    if(!m_program.addShaderFromSourceFile(QGLShader::Vertex, ":/vshaderContrast.glsl")) {
        QMessageBox::critical(0, "Error", "Vertex shader error", QMessageBox::Ok);
        QApplication::quit();
    }
    
    // compile fragment shader
    if(!m_program.addShaderFromSourceFile(QGLShader::Fragment, ":/fshaderContrast.glsl")) {
        QMessageBox::critical(0, "Error", "Fragment shader error",QMessageBox::Ok);
        QApplication::quit();
    }
    
    // bind the attribute variable in the glsl program with a generic vertex attribute index;
    // values provided via ATTRIB_VERTEX will modify the value of "a_position")
    glBindAttribLocation(m_program.programId(), ATTRIB_VERTEX, "a_Position");
    
    // bind the attribute variable in the glsl program with a generic vertex attribute index;
    // values provided via ATTRIB_TEXTURE_POSITION will modify the value of "a_TexCoord")
    glBindAttribLocation(m_program.programId(), ATTRIB_TEXTURE_POSITION, "a_TexCoord");
    
    // link shader pipeline; attribute bindings go into effect at this point
    if(!m_program.link()) {
        QMessageBox::critical(0, "Error", "Could not link shader", QMessageBox::Ok);
        QApplication::quit();
    }
    
    // get storage location of u_Sampler in fragment shader
    m_uniform[SAMPLER] = glGetUniformLocation(m_program.programId(), "u_Sampler");
    if((int) m_uniform[SAMPLER] < 0) {
        qDebug() << "Failed to get the storage location of u_Sampler";
        exit(-1);
    }
    
    // bind the glsl program
    glUseProgram(m_program.programId());
   
    glUniform1i(m_uniform[SAMPLER], 0);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contrast::initVertexBuffer:
//
// Initialize vertex buffer.
//
void
Contrast::initVertexBuffer()
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
    
    static GLuint texCoordBuffer = -1;
    glGenBuffers(1, &texCoordBuffer);
    
    // bind the buffer to the GPU; all future drawing calls gets data from this buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 3*sizeof(vec2), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, false, 0, NULL);
    glEnableVertexAttribArray(ATTRIB_VERTEX);
    
    //Create texture coordinates buffer
    glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, 3*sizeof(vec2), textureCoords, GL_STATIC_DRAW);
    glVertexAttribPointer(ATTRIB_TEXTURE_POSITION, 2, GL_FLOAT, false, 0, NULL);
    glEnableVertexAttribArray(ATTRIB_TEXTURE_POSITION);
    
}
