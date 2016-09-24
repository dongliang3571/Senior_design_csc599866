//
//  Sharpen.cpp
//  hw
//
//  Created by dong liang on 8/28/16.
//
//

#include "Sharpen.h"

enum {
    SAMPLER,
    MVP,
    ISINPUT,
    ISRGB,
    DISTANCEX,
    DISTANCEY,
    FILTERWIDTH,
    FILTERHEIGHT,
    ISEDGE,
    FACTOR
};



// Sharpen::Sharpen
//
// Constructor for class Sharpen
//
Sharpen::Sharpen(QWidget *parent)
: GLWidget (parent)
{
    m_numberVertices = 4;
    m_u_filterWidth = 0.0; // This filter width indicate how many units away from the mid pixel
    m_u_filterHeight = 0.0; // This filter height indicate how many units away from the mid pixel
    m_u_isEdge = false;
    m_u_factor = 1.0;
}



// Sharpen::controlPanel
//
// Create control panel groupbox.
//
QGroupBox* Sharpen::controlPanel()
{
    // init group box
    m_ctrlGrp = new QGroupBox("Sharpen");
    QLabel* xrange = new QLabel(QString("Filter Width"));
    QLabel* yrange = new QLabel(QString("Filter Height"));
    QLabel* factor = new QLabel(QString("Factor"));
    
    m_radioButtons[0] = new QRadioButton("Edging", m_ctrlGrp);
    m_radioButtons[0]->setChecked(true);
    m_radioButtons[1] = new QRadioButton("Sharpen", m_ctrlGrp);
    m_radioButtons[1]->setChecked(false);
    
    m_sliderXrange = new QSlider(Qt::Horizontal, m_ctrlGrp);
    m_sliderXrange->setTickPosition(QSlider::TicksBelow);
    m_sliderXrange->setTickInterval(5);
    m_sliderXrange->setMinimum(1);
    m_sliderXrange->setMaximum(100);
    m_sliderXrange->setValue  (1);
    
    m_sliderYrange = new QSlider(Qt::Horizontal, m_ctrlGrp);
    m_sliderYrange->setTickPosition(QSlider::TicksBelow);
    m_sliderYrange->setTickInterval(5);
    m_sliderYrange->setMinimum(1);
    m_sliderYrange->setMaximum(100);
    m_sliderYrange->setValue  (1);
    m_sliderYrange->setDisabled(true);
    
    m_sliderFactor = new QSlider(Qt::Horizontal, m_ctrlGrp);
    m_sliderFactor->setTickPosition(QSlider::TicksBelow);
    m_sliderFactor->setTickInterval(5);
    m_sliderFactor->setMinimum(1);
    m_sliderFactor->setMaximum(50);
    m_sliderFactor->setValue  (1);
    
    m_spinBoxXrange = new QSpinBox(m_ctrlGrp);
    m_spinBoxXrange->setMinimum   (1);
    m_spinBoxXrange->setMaximum   (100);
    m_spinBoxXrange->setValue     (1);
    m_spinBoxXrange->setSingleStep(2);
    
    m_spinBoxYrange = new QSpinBox(m_ctrlGrp);
    m_spinBoxYrange->setMinimum   (1);
    m_spinBoxYrange->setMaximum   (100);
    m_spinBoxYrange->setValue     (1);
    m_spinBoxYrange->setSingleStep(2);
    m_sliderYrange->setDisabled(true);
    
    m_spinBoxFactor = new QSpinBox(m_ctrlGrp);
    m_spinBoxFactor->setMinimum   (1);
    m_spinBoxFactor->setMaximum   (100);
    m_spinBoxFactor->setValue     (1);
    m_spinBoxFactor->setSingleStep(1);
    
    m_checkboxLockXY = new QCheckBox(QString("Synchronize X Y"));
    m_checkboxLockXY->setCheckState(Qt::Checked);
    
    connect(m_radioButtons[0], SIGNAL(clicked(bool)),     this, SLOT(startEdging(bool)));
    connect(m_radioButtons[1], SIGNAL(clicked(bool)),     this, SLOT(startSharpening(bool)));
    connect(m_sliderXrange,    SIGNAL(valueChanged(int)), this, SLOT(changeXrange(int)));
    connect(m_spinBoxXrange,   SIGNAL(valueChanged(int)), this, SLOT(changeXrange(int)));
    connect(m_sliderYrange,    SIGNAL(valueChanged(int)), this, SLOT(changeYrange(int)));
    connect(m_spinBoxYrange,   SIGNAL(valueChanged(int)), this, SLOT(changeYrange(int)));
    connect(m_sliderFactor,    SIGNAL(valueChanged(int)), this, SLOT(changeFactor(int)));
    connect(m_spinBoxFactor,   SIGNAL(valueChanged(int)), this, SLOT(changeFactor(int)));
    connect(m_checkboxLockXY,  SIGNAL(stateChanged(int)), this, SLOT(lockXY(int)));
    
    QGridLayout *layout = new QGridLayout;
    
    layout->addWidget(xrange,            0, 0);
    layout->addWidget(m_sliderXrange,    0, 1);
    layout->addWidget(m_spinBoxXrange,   0, 2);
    
    layout->addWidget(yrange,            1, 0);
    layout->addWidget(m_sliderYrange,    1, 1);
    layout->addWidget(m_spinBoxYrange,   1, 2);
    
    layout->addWidget(factor,            2, 0);
    layout->addWidget(m_sliderFactor,    2, 1);
    layout->addWidget(m_spinBoxFactor,   2, 2);
    
    layout->addWidget(m_radioButtons[0], 4, 0);
    layout->addWidget(m_checkboxLockXY);
    layout->addWidget(m_radioButtons[1], 5, 0);
    
    m_ctrlGrp->setLayout(layout);
    
    return(m_ctrlGrp);
}



// Sharpen::reset
//
// Reset all parameters
//
void Sharpen::reset()
{
    
}



// Sharpen::setImage
//
// set image
//
void Sharpen::setImage(QImage image)
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



void Sharpen::reload()
{
    glUniform1i(m_uniform[ISINPUT], m_isInput); // pass Sharpen reference to fragment shader
    glUniform1i(m_uniform[ISRGB], m_isRGB); // pass Sharpen reference to fragment shader
    updateGL();
}



void Sharpen::settingSliderAndSpinBox(QSlider* slider, QSpinBox* spinbox, int value, bool oddOnly) {
    if (oddOnly) {
        value += !(value%2);
    }
    slider->blockSignals(true );
    slider->setValue    (value);
    slider->blockSignals(false);
    
    spinbox->blockSignals(true );
    spinbox->setValue    (value);
    spinbox->blockSignals(false);
}


void Sharpen::changeXrange(int value) {
    bool isChecked = m_checkboxLockXY->isChecked();
    if (isChecked) {
        settingSliderAndSpinBox(m_sliderXrange, m_spinBoxXrange, value, true);
        settingSliderAndSpinBox(m_sliderYrange, m_spinBoxYrange, value, true);
    } else {
        settingSliderAndSpinBox(m_sliderXrange, m_spinBoxXrange, value, true);
    }
    
    if(value == 1) {
        m_u_filterWidth = 0.0;
        m_u_filterHeight = 0.0;
        glUniform1f(m_uniform[FILTERHEIGHT], m_u_filterHeight);
    } else if(value != 1 && isChecked) {
        m_u_filterWidth = (GLfloat)(value - 1) / 2;
        m_u_filterHeight = m_u_filterWidth;
        glUniform1f(m_uniform[FILTERHEIGHT], m_u_filterHeight);
    } else {
        m_u_filterWidth = (GLfloat)(value - 1) / 2;
    }
    glUniform1f(m_uniform[FILTERWIDTH], m_u_filterWidth);
    updateGL();
}

void Sharpen::changeYrange(int value) {
    settingSliderAndSpinBox(m_sliderYrange, m_spinBoxYrange, value, true);
    if(value == 1) {
        m_u_filterHeight = 0.0;
    } else {
        m_u_filterHeight = (GLfloat)(value - 1) / 2;
    }
    glUniform1f(m_uniform[FILTERHEIGHT], m_u_filterHeight);
    updateGL();
}



void Sharpen::lockXY(int isChecked) {
    int xValue = m_sliderXrange->value();
    if (isChecked == Qt::Checked) {
        settingSliderAndSpinBox(m_sliderYrange, m_spinBoxYrange, xValue, true);
        m_sliderYrange ->setEnabled(false);
        m_spinBoxYrange->setEnabled(false);
    } else {
        m_sliderYrange ->setEnabled(true);
        m_spinBoxYrange->setEnabled(true);
    }
    if(xValue == 1) {
        m_u_filterHeight = 0.0;
        m_u_filterWidth = 0.0;
    } else {
        m_u_filterWidth = (GLfloat)(xValue - 1) / 2;
        m_u_filterHeight = (GLfloat)(xValue - 1) / 2;
    }
    glUniform1f(m_uniform[FILTERHEIGHT], m_u_filterHeight);
    glUniform1f(m_uniform[FILTERWIDTH], m_u_filterWidth);
    updateGL();
}



void Sharpen::changeFactor(int value) {
    settingSliderAndSpinBox(m_sliderFactor, m_spinBoxFactor, value, false);
    m_u_factor = (GLfloat)value;
    glUniform1f(m_uniform[FACTOR], m_u_factor);
    updateGL();
}

void Sharpen::startEdging(bool isClicked) {
    if(isClicked) {
        m_u_isEdge = true;
        glUniform1i(m_uniform[ISEDGE], m_u_isEdge);
        updateGL();
    }
}

void Sharpen::startSharpening(bool isClicked) {
    if(isClicked) {
        m_u_isEdge = false;
        glUniform1i(m_uniform[ISEDGE], m_u_isEdge);
        updateGL();        
    }
}



// Sharpen::initializeGL
//
// Initialization routine before display loop.
// Gets called once before the first time resizeGL() or paintGL() is called.
//
void Sharpen::initializeGL()
{
    initializeGLFunctions();    // initialize GL function resolution for current context
    
    initTexture();
    initShaders();  // init vertex and fragment shaders
    initVertexBuffer(); // initialize vertex buffer and write positions to vertex shader
    glClearColor(0.0, 0.0, 0.0, 0.0);	// set background color
    glColor3f   (1.0, 1.0, 1.0);		// set foreground color
}



// Sharpen::resizeGL
//
// Resize event handler.
// The input parameters are the window width (w) and height (h).
//
void Sharpen::resizeGL(int w, int h)
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



// Sharpen::paintGL
//
// Update GL scene.
//
void Sharpen::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);   // clear canvas with background values
    if(!m_image.isNull()) glDrawArrays(GL_POLYGON, 0, m_numberVertices);   // draw a rectangle
}



// Thresold::initTexture
//
// Initialize setup for texture
//
void
Sharpen::initTexture()
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width_GLImage, m_height_GLImage, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_GLImage.bits());
}



// Sharpen::initShaders
//
// Initialize vertex and fragment shaders.
//
void Sharpen::initShaders()
{
    // compile vertex shader
    if(!m_program.addShaderFromSourceFile(QGLShader::Vertex, ":/vshaderSharpen.glsl")) {
        QMessageBox::critical(0, "Error", "Vertex shader error", QMessageBox::Ok);
        QApplication::quit();
    }
    
    // compile fragment shader
    if(!m_program.addShaderFromSourceFile(QGLShader::Fragment, ":/fshaderSharpen.glsl")) {
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
    
    // get location of u_reference in fragment shader
    m_uniform[DISTANCEX] = glGetUniformLocation(m_program.programId(), "u_DistanceX");
    if((int) m_uniform[DISTANCEX] < 0) {
        qDebug() << "Failed to get the storage location of u_DistanceX";
        exit(-1);
    }
    
    // get location of u_reference in fragment shader
    m_uniform[DISTANCEY] = glGetUniformLocation(m_program.programId(), "u_DistanceY");
    if((int) m_uniform[DISTANCEY] < 0) {
        qDebug() << "Failed to get the storage location of u_DistanceY";
        exit(-1);
    }
    
    // get location of u_reference in fragment shader
    m_uniform[FILTERWIDTH] = glGetUniformLocation(m_program.programId(), "u_FilterWidth");
    if((int) m_uniform[FILTERWIDTH] < 0) {
        qDebug() << "Failed to get the storage location of u_FilterWidth";
        exit(-1);
    }
    
    // get location of u_reference in fragment shader
    m_uniform[FILTERHEIGHT] = glGetUniformLocation(m_program.programId(), "u_FilterHeight");
    if((int) m_uniform[FILTERHEIGHT] < 0) {
        qDebug() << "Failed to get the storage location of u_FilterHeight";
        exit(-1);
    }
    
    // get location of u_reference in fragment shader
    m_uniform[ISEDGE] = glGetUniformLocation(m_program.programId(), "u_IsEdge");
    if((int) m_uniform[ISEDGE] < 0) {
        qDebug() << "Failed to get the storage location of u_IsEdge";
        exit(-1);
    }
    
    // get location of u_reference in fragment shader
    m_uniform[FACTOR] = glGetUniformLocation(m_program.programId(), "u_Factor");
    if((int) m_uniform[FACTOR] < 0) {
        qDebug() << "Failed to get the storage location of u_Factor";
        exit(-1);
    }
    
    // bind the glsl progam
    glUseProgram(m_program.programId());
    
    glUniform1i(m_uniform[SAMPLER], 0); // pass sampler aka. texture unit number(GL_TEXTURE0) to fragment shader
    
    m_u_mvpMatrix.setToIdentity();
    glUniformMatrix4fv(m_uniform[MVP], 1, GL_FALSE, m_u_mvpMatrix.constData()); // Pass MVP matrix to vertex shader
    
    glUniform1i(m_uniform[ISINPUT], m_isInput);
    glUniform1i(m_uniform[ISRGB], m_isRGB);
    
    m_u_distanceX = (GLfloat)1.0 / m_width_GLImage;
    m_u_distanceY = (GLfloat)1.0 / m_height_GLImage;
    glUniform1f(m_uniform[DISTANCEX], m_u_distanceX);
    glUniform1f(m_uniform[DISTANCEY], m_u_distanceY);
    
    glUniform1f(m_uniform[FILTERWIDTH], m_u_filterWidth);
    glUniform1f(m_uniform[FILTERHEIGHT], m_u_filterHeight);
    
    glUniform1i(m_uniform[ISEDGE], m_u_isEdge);
    glUniform1f(m_uniform[FACTOR], m_u_factor);
}



// Sharpen::initVertexBuffer:
//
// Initialize vertex buffer.
//
void Sharpen::initVertexBuffer()
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
