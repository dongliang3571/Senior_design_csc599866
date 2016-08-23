//
//  Threshold.cpp
//  hw
//
//  Created by dong liang on 8/16/16.
//
//

#include "Threshold.h"


enum {
    SAMPLER,
    MVP
};



// Threshold::Threshold
//
// Constructor for class Threshold
//
Threshold::Threshold(QWidget *parent)
: HW (parent)
{}



// Threshold::controlPanel
//
// Create control panel groupbox.
//
QGroupBox* Threshold::controlPanel()
{
    QGroupBox *groupBox = new QGroupBox("Threshold"); // init group box
    return(groupBox);
}



// Threshold::reset
//
// Reset all parameters
//
void Threshold::reset()
{
    
}



// Threshold::initializeGL
//
// Initialization routine before display loop.
// Gets called once before the first time resizeGL() or paintGL() is called.
//
void Threshold::initializeGL()
{
    initializeGLFunctions();    // initialize GL function resolution for current context
    initTexture();  // init texture
    initShaders();  // init vertex and fragment shaders
    initVertexBuffer(); // initialize vertex buffer and write positions to vertex shader

    glClearColor(0.0, 0.0, 0.0, 0.0);	// set background color
    glColor3f   (1.0, 1.0, 1.0);		// set foreground color
}



// Threshold::resizeGL
//
// Resize event handler.
// The input parameters are the window width (w) and height (h).
//
void Threshold::resizeGL(int w, int h)
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
    m_u_mvpMatrix.setToIdentity();
    m_u_mvpMatrix.ortho(-xmax, xmax, -ymax, ymax, -1.0, 1.0);
    glUniformMatrix4fv(m_uniform[MVP], 1, GL_FALSE, m_u_mvpMatrix.constData());
    
}



// Threshold::paintGL
//
// Update GL scene.
//
void Threshold::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);   // clear canvas with background values
    glDrawArrays(GL_TRIANGLES, 0, 3);   // draw a rectangle
}



// Thresold::initTexture
//
// Initialize setup for texture
//
void
Threshold::initTexture()
{
    // read image from file
    if(m_image.load(QString(":/mandrill.jpg"))) {
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



// Threshold::initShaders
//
// Initialize vertex and fragment shaders.
//
void Threshold::initShaders()
{
    // compile vertex shader
    if(!m_program.addShaderFromSourceFile(QGLShader::Vertex, ":/vshaderThreshold.glsl")) {
        QMessageBox::critical(0, "Error", "Vertex shader error", QMessageBox::Ok);
        QApplication::quit();
    }
    
    // compile fragment shader
    if(!m_program.addShaderFromSourceFile(QGLShader::Fragment, ":/fshaderThreshold.glsl")) {
        QMessageBox::critical(0, "Error", "Fragment shader error",QMessageBox::Ok);
        QApplication::quit();
    }
    
    
    
    // bind the attribute variable in the glsl program with a generic vertex attribute index;
    // values provided via ATTRIB_VERTEX will modify the value of "a_Position")
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
    
    // get storage location of u_MvpMatrix in fragment shader
    m_uniform[MVP] = glGetUniformLocation(m_program.programId(), "u_MvpMatrix");
    if((int) m_uniform[SAMPLER] < 0) {
        qDebug() << "Failed to get the storage location of u_Sampler";
        exit(-1);
    }
    
    // bind the glsl progam
    glUseProgram(m_program.programId());
    
    glUniform1i(m_uniform[SAMPLER], 0);
    
    m_u_mvpMatrix.setToIdentity();
    glUniformMatrix4fv(m_uniform[MVP], 1, GL_FALSE, m_u_mvpMatrix.constData());
    glUniform1i(m_uniform[MVP], 0);

}



// Threshold::initVertexBuffer:
//
// Initialize vertex buffer.
//
void Threshold::initVertexBuffer()
{
    // set flag for creating buffers (1st time only)
    static bool flag = 1;
    
    // verify that we have valid vertex and texture buffers
    static GLuint vertexBuffer = -1;
    static GLuint texCoordBuffer = -1;
    
    if(flag) {	// create vertex and color buffers
        glGenBuffers(1, &vertexBuffer);
        glGenBuffers(1, &texCoordBuffer);
        flag = 0;	// reset flag
    }
    
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
    
    // Bind the vertice buffer and assign data to attribute variables
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer); // bind the buffer to the GPU; all future drawing calls gets data from this buffer
    glBufferData(GL_ARRAY_BUFFER, 3*sizeof(vec2), vertices, GL_STATIC_DRAW); // copy the vertices from CPU to GPU
    glEnableVertexAttribArray(ATTRIB_VERTEX); // enable the assignment of attribute vertex variable
    glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, false, 0, NULL); // assign the buffer object to the attribute vertex variable
    
    
    //Create texture coordinates buffer
    glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer); // Bind the buffer object(textCoordBuffer) to a target(GL_ARRAY_BUFFER)
    glBufferData(GL_ARRAY_BUFFER, 3*sizeof(vec2), textureCoords, GL_STATIC_DRAW); // Wrtie data into the buffer object
    glEnableVertexAttribArray(ATTRIB_TEXTURE_POSITION); // Enable assignment
    glVertexAttribPointer(ATTRIB_TEXTURE_POSITION, 2, GL_FLOAT, false, 0, NULL); // Assign the buffer object to an attribute variable
}

