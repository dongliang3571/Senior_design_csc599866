//
//  HistogramStretching.cpp
//  hw
//
//  Created by dong liang on 8/26/16.
//
//

#include "HistogramStretching.h"

enum {
    SAMPLER,
    MVP,
    ISINPUT,
    ISRGB,
    MIN,
    MAX
};



// HistogramStretching::HistogramStretching
//
// Constructor for class HistogramStretching
//
HistogramStretching::HistogramStretching(QWidget *parent)
: GLWidget (parent)
{
    m_numberVertices = 4;
    m_u_minValue = 0.0;
    m_u_maxValue = 1.0;
}



// HistogramStretching::controlPanel
//
// Create control panel groupbox.
//
QGroupBox* HistogramStretching::controlPanel()
{
    // init group box
    m_ctrlGrp = new QGroupBox("HistogramStretching");
    
    QLabel* label_Max = new QLabel(QString("Max"));
    QLabel* label_Min = new QLabel(QString("Min"));
    
    m_sliderMax = new QSlider(Qt::Horizontal, m_ctrlGrp);
    m_sliderMax->setTickPosition(QSlider::TicksBelow);
    m_sliderMax->setTickInterval(50);
    m_sliderMax->setMinimum(1);
    m_sliderMax->setMaximum(MaxGray);
    m_sliderMax->setValue  (MaxGray);
    
    m_sliderMin = new QSlider(Qt::Horizontal, m_ctrlGrp);
    m_sliderMin->setTickPosition(QSlider::TicksBelow);
    m_sliderMin->setTickInterval(50);
    m_sliderMin->setMinimum(0);
    m_sliderMin->setMaximum(MaxGray-1);
    m_sliderMin->setValue  (0);
    
    m_spinboxMax = new QSpinBox(m_ctrlGrp);
    m_spinboxMax->setMinimum   (1);
    m_spinboxMax->setMaximum   (MaxGray);
    m_spinboxMax->setValue     (MaxGray);
    m_spinboxMax->setSingleStep(1);
    
    m_spinboxMin = new QSpinBox(m_ctrlGrp);
    m_spinboxMin->setMinimum   (0);
    m_spinboxMin->setMaximum   (MaxGray-1);
    m_spinboxMin->setValue     (0);
    m_spinboxMin->setSingleStep(1);
    
    m_checkBoxAutoMax = new QCheckBox("AutoMax");
    m_checkBoxAutoMax->setCheckState(Qt::Unchecked);
    
    m_checkBoxAutoMin = new QCheckBox("AutoMin");
    m_checkBoxAutoMin->setCheckState(Qt::Unchecked);
    
    connect(m_sliderMax ,    SIGNAL(valueChanged(int)), this, SLOT(changeStretchMax(int)));
    connect(m_spinboxMax,    SIGNAL(valueChanged(int)), this, SLOT(changeStretchMax(int)));
    connect(m_checkBoxAutoMax, SIGNAL(stateChanged(int)), this, SLOT(autoStretch(int)));
    
    connect(m_sliderMin ,    SIGNAL(valueChanged(int)), this, SLOT(changeStretchMin(int)));
    connect(m_spinboxMin,    SIGNAL(valueChanged(int)), this, SLOT(changeStretchMin(int)));
    connect(m_checkBoxAutoMin, SIGNAL(stateChanged(int)), this, SLOT(autoStretch(int)));
    
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(label_Max,    0, 0);
    layout->addWidget(m_sliderMax,     0, 1);
    layout->addWidget(m_spinboxMax,    0, 2);
//    layout->addWidget(m_checkBoxAutoMax, 0, 3);
    
    layout->addWidget(label_Min,    1, 0);
    layout->addWidget(m_sliderMin,     1, 1);
    layout->addWidget(m_spinboxMin,    1, 2);
//    layout->addWidget(m_checkBoxAutoMin, 1, 3);
    
    m_ctrlGrp->setLayout(layout);
    
    return m_ctrlGrp;
}



// HistogramStretching::reset
//
// Reset all parameters
//
void HistogramStretching::reset()
{
    
}



// HistogramStretching::setImage
//
// set image
//
void HistogramStretching::setImage(QImage image)
{
    m_image = QImage(image);
    updateGL();
    // convert jpg to GL formatted image
    m_GLImage = QGLWidget::convertToGLFormat(m_image);
    // init vars
    m_width_GLImage  = m_GLImage.width ();
    m_height_GLImage = m_GLImage.height();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width_GLImage, m_height_GLImage, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_GLImage.bits());
    
    updateGL();
}



void HistogramStretching::reload()
{
    glUniform1i(m_uniform[ISINPUT], m_isInput); // pass HistogramStretching reference to fragment shader
    glUniform1i(m_uniform[ISRGB], m_isRGB); // pass HistogramStretching reference to fragment shader
    updateGL();
}



void settingSliderAndSpinBox(QSlider* slider, QSpinBox* spinbox, int value) {
    slider->blockSignals(true );
    slider->setValue    (value);
    slider->blockSignals(false);
    
    spinbox->blockSignals(true );
    spinbox->setValue    (value);
    spinbox->blockSignals(false);
}


void HistogramStretching::changeStretchMax(int value) {
    
    m_checkBoxAutoMax->blockSignals(true);
    m_checkBoxAutoMax->setCheckState(Qt::Unchecked);
    m_checkBoxAutoMax->blockSignals(false);
    
    settingSliderAndSpinBox(m_sliderMax, m_spinboxMax, value);
    if (m_sliderMax->value() <= m_sliderMin->value()) {
        settingSliderAndSpinBox(m_sliderMax, m_spinboxMax, m_sliderMin->value() + 1);
    }
    m_u_maxValue = value/(GLfloat)MaxGray;
    glUniform1f(m_uniform[MAX], m_u_maxValue); // pass HistogramStretching max to fragment shader
    updateGL();
}

void HistogramStretching::changeStretchMin(int value) {
    
    m_checkBoxAutoMin->blockSignals(true);
    m_checkBoxAutoMin->setCheckState(Qt::Unchecked);
    m_checkBoxAutoMin->blockSignals(false);
    
    settingSliderAndSpinBox(m_sliderMin, m_spinboxMin, value);
    if (m_sliderMin->value() >= m_sliderMax->value()) {
        settingSliderAndSpinBox(m_sliderMin, m_spinboxMin, m_sliderMax->value() - 1);
    }
    m_u_minValue = value/(GLfloat)MaxGray;
    glUniform1f(m_uniform[MIN], m_u_minValue); // pass HistogramStretching max to fragment shader
    updateGL();
}

void HistogramStretching::autoStretch(int isAuto) {
    
}



// HistogramStretching::initializeGL
//
// Initialization routine before display loop.
// Gets called once before the first time resizeGL() or paintGL() is called.
//
void HistogramStretching::initializeGL()
{
    initializeGLFunctions();    // initialize GL function resolution for current context
    
    initTexture();
    initShaders();  // init vertex and fragment shaders
    initVertexBuffer(); // initialize vertex buffer and write positions to vertex shader
    glClearColor(0.0, 0.0, 0.0, 0.0);	// set background color
    glColor3f   (1.0, 1.0, 1.0);		// set foreground color
}



// HistogramStretching::resizeGL
//
// Resize event handler.
// The input parameters are the window width (w) and height (h).
//
void HistogramStretching::resizeGL(int w, int h)
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



// HistogramStretching::paintGL
//
// Update GL scene.
//
void HistogramStretching::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);   // clear canvas with background values
    if(!m_image.isNull()) glDrawArrays(GL_POLYGON, 0, m_numberVertices);   // draw a rectangle
}



// Thresold::initTexture
//
// Initialize setup for texture
//
void HistogramStretching::initTexture()
{
    // read image from file
    if(m_image.isNull()) {
        m_image.load(QString(":/mandrill.jpg"));
        
    }
    // convert jpg to GL formatted image
    m_GLImage = QGLWidget::convertToGLFormat(m_image);
    // init vars
    m_width_GLImage  = m_GLImage.width ();
    m_height_GLImage = m_GLImage.height();
    
    // generate texture object and bind it
    // GL_TEXTURE0 is texture unit, which is for managing a texture image, each has an associated GL_TEXTURE_2D,
    // which is the textuture target for specifying the type of texture
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    
    // set the texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width_GLImage, m_height_GLImage, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_GLImage.bits());
}



// HistogramStretching::initShaders
//
// Initialize vertex and fragment shaders.
//
void HistogramStretching::initShaders()
{
    // compile vertex shader
    if(!m_program.addShaderFromSourceFile(QGLShader::Vertex, ":/vshaderHistogramStretching.glsl")) {
        QMessageBox::critical(0, "Error", "Vertex shader error", QMessageBox::Ok);
        QApplication::quit();
    }
    
    // compile fragment shader
    if(!m_program.addShaderFromSourceFile(QGLShader::Fragment, ":/fshaderHistogramStretching.glsl")) {
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
    
    m_uniform[MIN] = glGetUniformLocation(m_program.programId(), "u_Min_Value");
    if((int) m_uniform[MIN] < 0) {
        qDebug() << "Failed to get the storage location of u_Min_Value";
        exit(-1);
    }
    
    m_uniform[MAX] = glGetUniformLocation(m_program.programId(), "u_Max_Value");
    if((int) m_uniform[MAX] < 0) {
        qDebug() << "Failed to get the storage location of u_Max_Value";
        exit(-1);
    }
    
    // bind the glsl progam
    glUseProgram(m_program.programId());
    
    glUniform1i(m_uniform[SAMPLER], 0); // pass sampler aka. texture unit number(GL_TEXTURE0) to fragment shader
    
    m_u_mvpMatrix.setToIdentity();
    glUniformMatrix4fv(m_uniform[MVP], 1, GL_FALSE, m_u_mvpMatrix.constData()); // Pass MVP matrix to vertex shader
    
    glUniform1i(m_uniform[ISINPUT], m_isInput);
    glUniform1i(m_uniform[ISRGB], m_isRGB);
    glUniform1f(m_uniform[MIN], m_u_minValue);
    glUniform1f(m_uniform[MAX], m_u_maxValue);
}



// HistogramStretching::initVertexBuffer:
//
// Initialize vertex buffer.
//
void HistogramStretching::initVertexBuffer()
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
