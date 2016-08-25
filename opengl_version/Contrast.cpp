//
//  Contrast.cpp
//  hw
//
//  Created by dong liang on 8/16/16.
//
//

#include "Contrast.h"


enum {
    SAMPLER,
    MVP,
    ISINPUT,
    ISRGB,
    CONTRAST,
    BRIGHTNESS
};


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contrast::Contrast:
//
// Contrast constructor.
//
Contrast::Contrast(QWidget *parent)
: GLWidget(parent)
{
    m_numberVertices = 4;
    m_isInitialized = false;
    m_u_contrast = 1.0;
    m_u_brightness = 0.0;
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
    QGroupBox *groupBox = new QGroupBox("Contrast");
    
//    QLabel *label_brightness = new QLabel;
//    label_brightness->setText(QString("Brightness"));
//    
//    //slider for brightness
//    m_sliderB = new QSlider(Qt::Horizontal, m_ctrlGrp);
//    m_sliderB->setTickPosition(QSlider::TicksBelow);
//    m_sliderB->setTickInterval(25);
//    m_sliderB->setMinimum(-256);
//    m_sliderB->setMaximum(256);
//    m_sliderB->setValue  (0);
//    
//    //spinbox for brightness
//    m_spinBoxB = new QSpinBox(m_ctrlGrp);
//    m_spinBoxB->setMinimum   (-256);
//    m_spinBoxB->setMaximum   (256);
//    m_spinBoxB->setValue     (0);
//    m_spinBoxB->setSingleStep(10);
//    
//    QLabel *label_contrast = new QLabel;
//    label_contrast->setText(QString("Contrast"));
//    
//    //slider for contrast
//    m_sliderC = new QSlider(Qt::Horizontal, m_ctrlGrp);
//    m_sliderC->setTickPosition(QSlider::TicksBelow);
//    m_sliderC->setTickInterval(25);
//    m_sliderC->setRange(-100, 100);
//    m_sliderC->setValue  (0);
//    
//    
//    // spinbox for contrast
//    m_spinBoxC = new QDoubleSpinBox(m_ctrlGrp);
//    m_spinBoxC->setRange     (0.25, 5.0);
//    m_spinBoxC->setValue     (0.0);
//    m_spinBoxC->setDecimals  (1);
//    m_spinBoxC->setSingleStep(0.5);
//    
//    
//    // init signal/slot connections for Threshold
//    connect(m_sliderB , SIGNAL(valueChanged(int)),    this, SLOT(changeBrightness (int)));
//    connect(m_spinBoxB, SIGNAL(valueChanged(int)),    this, SLOT(changeBrightness (int)));
//    connect(m_sliderC,  SIGNAL(valueChanged(int)),    this, SLOT(changeContr_slide(int)));
//    connect(m_spinBoxC, SIGNAL(valueChanged(double)), this, SLOT(changeContr_spinB(double)));
//    
//    // INSERT YOUR CODE HERE
//    
//    QGridLayout *layout = new QGridLayout;
//    layout->addWidget(label_brightness, 0, 0);
//    layout->addWidget(m_sliderB,        0, 1);
//    layout->addWidget(m_spinBoxB,       0, 2);
//    
//    layout->addWidget(label_contrast,   1, 0);
//    layout->addWidget(m_sliderC,        1, 1);
//    layout->addWidget(m_spinBoxC,       1, 2);
//    
//    m_ctrlGrp->setLayout(layout);

    
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



// Contrast::setImage
//
// set image
//
void Contrast::setImage(QImage image)
{
    m_image = QImage(image);
    initTexture();  // init texture
//    if(!m_isInitialized) {
//        initializeGL();
//        resizeGL(m_winW, m_winH);
//        paintGL();
//        updateGL();
//    }
    updateGL();
}



void Contrast::reload()
{
    glUniform1i(m_uniform[ISINPUT], m_isInput); // pass threshold reference to fragment shader
    glUniform1i(m_uniform[ISRGB], m_isRGB); // pass threshold reference to fragment shader
    updateGL();
}



// Contrast::initializeGL:
//
// Initialization routine before display loop.
// Gets called once before the first time resizeGL() or paintGL() is called.
//
void
Contrast::initializeGL()
{
    initializeGLFunctions();    // initialize GL function resolution for current context
    
//    if(!m_image.isNull()) {
//        m_isInitialized = true;
//    }
    initTexture();
    initShaders();  // init vertex and fragment shaders
    initVertexBuffer(); // initialize vertex buffer and write positions to vertex shader
    glClearColor(0.0, 0.0, 0.0, 0.0);	// set background color
    glColor3f   (1.0, 1.0, 1.0);		// set foreground color
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
    m_u_mvpMatrix.setToIdentity();
    m_u_mvpMatrix.ortho(-xmax, xmax, -ymax, ymax, -1.0, 1.0);
    glUniformMatrix4fv(m_uniform[MVP], 1, GL_FALSE, m_u_mvpMatrix.constData());
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contrast::paintGL:
//
// Update GL scene.
//
void
Contrast::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);   // clear canvas with background values
    if(!m_image.isNull()) glDrawArrays(GL_POLYGON, 0, m_numberVertices);   // draw a rectangle
}



// Contrast::initTexture:
//
// Initialize texture.
//
void
Contrast::initTexture()
{
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
    
    // get location of u_Contrast in fragment shader
    m_uniform[CONTRAST] = glGetUniformLocation(m_program.programId(), "u_Contrast");
    if((int) m_uniform[CONTRAST] < 0) {
        qDebug() << "Failed to get the storage location of u_Contrast";
        exit(-1);
    }
    
    // get location of u_Brightness in fragment shader
    m_uniform[BRIGHTNESS] = glGetUniformLocation(m_program.programId(), "u_Brightness");
    if((int) m_uniform[BRIGHTNESS] < 0) {
        qDebug() << "Failed to get the storage location of u_Brightness";
        exit(-1);
    }
    
    // bind the glsl progam
    glUseProgram(m_program.programId());
    
    glUniform1i(m_uniform[SAMPLER], 0); // pass sampler aka. texture unit number(GL_TEXTURE0) to fragment shader
    
    m_u_mvpMatrix.setToIdentity();
    glUniformMatrix4fv(m_uniform[MVP], 1, GL_FALSE, m_u_mvpMatrix.constData()); // Pass MVP matrix to vertex shader
    
    glUniform1i(m_uniform[ISINPUT], m_isInput);
    glUniform1i(m_uniform[ISRGB], m_isRGB);
    glUniform1f(m_uniform[CONTRAST], m_u_contrast); // contrast value
    glUniform1i(m_uniform[BRIGHTNESS], m_u_brightness); // brightness
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contrast::initVertexBuffer:
//
// Initialize vertex buffer.
//
void
Contrast::initVertexBuffer()
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
