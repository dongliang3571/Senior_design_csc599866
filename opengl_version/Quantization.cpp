//
//  Quantization.cpp
//  hw
//
//  Created by dong liang on 8/23/16.
//
//

#include "Quantization.h"

enum {
    SAMPLER,
    MVP,
    ISINPUT,
    ISRGB,
    BIAS,
    ISDITHER,
    SCALE
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
    m_u_isDither = false;
    m_u_scale = 1.0;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Quantization::controlPanel:
//
// Create control panel groupbox.
//
QGroupBox*
Quantization::controlPanel()
{
    m_ctrlGrp = new QGroupBox("Quantization");
    
    QLabel* label_quantize = new QLabel(QString("Quantize"));
    
    m_sliderLevel = new QSlider(Qt::Horizontal, m_ctrlGrp);
    m_sliderLevel->setTickPosition(QSlider::TicksBelow);
    m_sliderLevel->setTickInterval(50);
    m_sliderLevel->setMinimum(1);
    m_sliderLevel->setMaximum(255);
    m_sliderLevel->setValue  (1);
    
    //spinbox for brightness
    m_spinboxLevel = new QSpinBox(m_ctrlGrp);
    m_spinboxLevel->setMinimum   (1);
    m_spinboxLevel->setMaximum   (255);
    m_spinboxLevel->setValue     (1);
    m_spinboxLevel->setSingleStep(1);
    
    m_checkBox_dither = new QCheckBox("Dither");
    m_checkBox_dither->setCheckState(Qt::Unchecked);
    
    connect(m_sliderLevel ,    SIGNAL(valueChanged(int)), this, SLOT(changeLevel(int)));
    connect(m_spinboxLevel,    SIGNAL(valueChanged(int)), this, SLOT(changeLevel(int)));
    connect(m_checkBox_dither, SIGNAL(stateChanged(int)), this, SLOT(changeDither(int)));
    
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(label_quantize,    0, 0);
    layout->addWidget(m_sliderLevel,     0, 1);
    layout->addWidget(m_spinboxLevel,    0, 2);
    layout->addWidget(m_checkBox_dither, 1, 0);
    
    m_ctrlGrp->setLayout(layout);
    
    return m_ctrlGrp;
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



// Quantization::setImage
//
// set image
//
void Quantization::setImage(QImage image)
{
    m_image = QImage(image);
    initTexture();  // init texture
    updateGL();
}



void Quantization::reload()
{
    glUniform1i(m_uniform[ISINPUT], m_isInput); // pass threshold reference to fragment shader
    glUniform1i(m_uniform[ISRGB], m_isRGB); // pass threshold reference to fragment shader
    updateGL();
}



void Quantization::changeLevel(int level) {
    
    m_sliderLevel ->blockSignals(true );
    m_sliderLevel ->setValue    (level);
    m_sliderLevel ->blockSignals(false);
    
    m_spinboxLevel->blockSignals(true );
    m_spinboxLevel->setValue    (level);
    m_spinboxLevel->blockSignals(false);
    
    m_u_scale = MaxGray/level/(GLfloat)MaxGray;
    glUniform1f(m_uniform[SCALE], m_u_scale);
    updateGL();
}



void Quantization::changeDither(int checked) {
    m_checkBox_dither ->blockSignals(true );
    if(checked) {
        m_checkBox_dither ->setCheckState(Qt::Checked);
        m_u_isDither = true;
        qDebug() << m_u_bias;
    } else {
        m_checkBox_dither ->setCheckState(Qt::Unchecked);
        m_u_isDither = false;
    }
    m_checkBox_dither ->blockSignals(false);
    
    //
    glUniform1f(m_uniform[BIAS], m_u_bias);
    glUniform1i(m_uniform[ISDITHER], m_u_isDither);
    updateGL();
}



// Quantization::initializeGL:
//
// Initialization routine before display loop.
// Gets called once before the first time resizeGL() or paintGL() is called.
//
void
Quantization::initializeGL()
{
    initializeGLFunctions();    // initialize GL function resolution for current context
    initTexture();
    initShaders();  // init vertex and fragment shaders
    initVertexBuffer(); // initialize vertex buffer and write positions to vertex shader
    glClearColor(0.0, 0.0, 0.0, 0.0);	// set background color
    glColor3f   (1.0, 1.0, 1.0);		// set foreground color
}





// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Quantization::resizeGL:
//
// Resize event handler.
// The input parameters are the window width (w) and height (h).
//
void Quantization::resizeGL(int w, int h)
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



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Quantization::paintGL:
//
// Update GL scene.
//
void
Quantization::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);   // clear canvas with background values
    if(!m_image.isNull()) glDrawArrays(GL_POLYGON, 0, m_numberVertices);   // draw a rectangle
}



// Quantization::initTexture:
//
// Initialize texture.
//
void
Quantization::initTexture()
{
    qDebug() <<"init texture";
    // read image from file
    if(m_image.isNull()) {
        m_image.load(QString(":/mandrill.jpg"));
        
    }
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, qImage.bits());
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Quantization::initShader:
//
// Initialize vertex and fragment shaders for texture mapping.
//
void Quantization::initShaders()
{
    // compile vertex shader
    if(!m_program.addShaderFromSourceFile(QGLShader::Vertex, ":/vshaderQuantization.glsl")) {
        QMessageBox::critical(0, "Error", "Vertex shader error", QMessageBox::Ok);
        QApplication::quit();
    }
    
    // compile fragment shader
    if(!m_program.addShaderFromSourceFile(QGLShader::Fragment, ":/fshaderQuantization.glsl")) {
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
    if((int) m_uniform[MVP] < 0) {
        qDebug() << "Failed to get the storage location of u_MvpMatrix";
        exit(-1);
    }
    
    // get location of u_reference in fragment shader
    m_uniform[ISINPUT] = glGetUniformLocation(m_program.programId(), "u_IsInput");
    if((int) m_uniform[ISINPUT] < 0) {
        qDebug() << "Failed to get the storage location of u_IsInput";
        exit(-1);
    }
    
    // get location of u_reference in fragment shader
    m_uniform[ISRGB] = glGetUniformLocation(m_program.programId(), "u_IsRGB");
    if((int) m_uniform[ISRGB] < 0) {
        qDebug() << "Failed to get the storage location of u_IsRGB";
        exit(-1);
    }
    
    // get location of u_Quantization in fragment shader
    m_uniform[ISDITHER] = glGetUniformLocation(m_program.programId(), "u_IsDither");
    if((int) m_uniform[ISDITHER] < 0) {
        qDebug() << "Failed to get the storage location of u_IsDither";
        exit(-1);
    }
    
    // get location of u_Quantization in fragment shader
    m_uniform[SCALE] = glGetUniformLocation(m_program.programId(), "u_Scale");
    if((int) m_uniform[SCALE] < 0) {
        qDebug() << "Failed to get the storage location of u_Scale";
        exit(-1);
    }
    
    // bind the glsl progam
    glUseProgram(m_program.programId());
    
    glUniform1i(m_uniform[SAMPLER], 0); // pass sampler aka. texture unit number(GL_TEXTURE0) to fragment shader
    
    m_u_mvpMatrix.setToIdentity();
    glUniformMatrix4fv(m_uniform[MVP], 1, GL_FALSE, m_u_mvpMatrix.constData()); // Pass MVP matrix to vertex shader
    
    glUniform1i(m_uniform[ISINPUT], m_isInput);
    glUniform1i(m_uniform[ISRGB], m_isRGB);
    glUniform1i(m_uniform[ISDITHER], m_u_isDither);
    glUniform1f(m_uniform[SCALE], m_u_scale);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Quantization::initVertexBuffer:
//
// Initialize vertex buffer.
//
void
Quantization::initVertexBuffer()
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
        vec2(-0.75f,  0.75f),
        vec2( 0.75f,  0.75f),
        vec2( 0.75f, -0.75f),
        vec2(-0.75f, -0.75f)
    };
    
    // init texture coordinates
    const vec2 textureCoords[] = {
        vec2(0, 1),
        vec2(1, 1),
        vec2(1, 0),
        vec2(0, 0)
    };
    
    // Bind the vertice buffer and assign data to attribute variables
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer); // bind the buffer to the GPU; all future drawing calls gets data from this buffer
    glBufferData(GL_ARRAY_BUFFER, m_numberVertices*sizeof(vec2), vertices, GL_STATIC_DRAW); // copy the vertices from CPU to GPU
    glEnableVertexAttribArray(ATTRIB_VERTEX); // enable the assignment of attribute vertex variable
    glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, false, 0, NULL); // assign the buffer object to the attribute vertex variable
    
    //Create texture coordinates buffer
    glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer); // Bind the buffer object(textCoordBuffer) to a target(GL_ARRAY_BUFFER)
    glBufferData(GL_ARRAY_BUFFER, m_numberVertices*sizeof(vec2), textureCoords, GL_STATIC_DRAW); // Wrtie data into the buffer object
    glEnableVertexAttribArray(ATTRIB_TEXTURE_POSITION); // Enable assignment
    glVertexAttribPointer(ATTRIB_TEXTURE_POSITION, 2, GL_FLOAT, false, 0, NULL); // Assign the buffer object to an attribute variable
}
