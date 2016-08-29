//
//  MedianFilter.cpp
//  hw
//
//  Created by dong liang on 8/28/16.
//
//

#include "MedianFilter.h"

enum {
    SAMPLER,
    MVP,
    ISINPUT,
    ISRGB,
    DISTANCEX,
    DISTANCEY,
    NEIGHBORSIZE
};



// MedianFilter::MedianFilter
//
// Constructor for class MedianFilter
//
MedianFilter::MedianFilter(QWidget *parent)
: GLWidget (parent)
{
    m_numberVertices = 4;
    m_u_neighborSize = 1.0;
}



// MedianFilter::controlPanel
//
// Create control panel groupbox.
//
QGroupBox* MedianFilter::controlPanel()
{
    m_ctrlGrp = new QGroupBox("Median Filter");
    
    QLabel* labelFilterSize = new QLabel(QString("Filter Size"));
    QLabel* labelAvgK = new QLabel(QString("Avg K"));
    
    m_sliderFilterSize = new QSlider(Qt::Horizontal, m_ctrlGrp);
    m_sliderFilterSize->setTickPosition(QSlider::TicksBelow);
    m_sliderFilterSize->setTickInterval(5);
    m_sliderFilterSize->setMinimum(1);
    m_sliderFilterSize->setMaximum(99);
    m_sliderFilterSize->setValue  (1);
    
    m_sliderAvgK = new QSlider(Qt::Horizontal, m_ctrlGrp);
    m_sliderAvgK->setTickPosition(QSlider::TicksBelow);
    m_sliderAvgK->setTickInterval(5);
    m_sliderAvgK->setMinimum(0);
    m_sliderAvgK->setMaximum(49);
    m_sliderAvgK->setValue  (0);
    
    m_spinBoxFilterSize = new QSpinBox(m_ctrlGrp);
    m_spinBoxFilterSize->setMinimum   (1);
    m_spinBoxFilterSize->setMaximum   (99);
    m_spinBoxFilterSize->setValue     (1);
    m_spinBoxFilterSize->setSingleStep(2);
    
    m_spinBoxAvgK = new QSpinBox(m_ctrlGrp);
    m_spinBoxAvgK->setMinimum   (0);
    m_spinBoxAvgK->setMaximum   (49);
    m_spinBoxAvgK->setValue     (0);
    m_spinBoxAvgK->setSingleStep(1);
    
    connect(m_sliderFilterSize,    SIGNAL(valueChanged(int)), this, SLOT(changeFilterSize(int)));
    connect(m_spinBoxFilterSize,   SIGNAL(valueChanged(int)), this, SLOT(changeFilterSize(int)));
    connect(m_sliderAvgK,    SIGNAL(valueChanged(int)), this, SLOT(changeAvgK(int)));
    connect(m_spinBoxAvgK,   SIGNAL(valueChanged(int)), this, SLOT(changeAvgK(int)));
    
    QGridLayout *layout = new QGridLayout;
    
    layout->addWidget(labelFilterSize, 0, 0);
    layout->addWidget(m_sliderFilterSize, 0, 1);
    layout->addWidget(m_spinBoxFilterSize, 0, 2);
    
    layout->addWidget(labelAvgK, 1, 0);
    layout->addWidget(m_sliderAvgK, 1, 1);
    layout->addWidget(m_spinBoxAvgK, 1, 2);
    
    m_ctrlGrp->setLayout(layout);
    
    return m_ctrlGrp;
}



// MedianFilter::reset
//
// Reset all parameters
//
void MedianFilter::reset()
{
    
}



// MedianFilter::setImage
//
// set image
//
void MedianFilter::setImage(QImage image)
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



void MedianFilter::reload()
{
    glUniform1i(m_uniform[ISINPUT], m_isInput); // pass MedianFilter reference to fragment shader
    glUniform1i(m_uniform[ISRGB], m_isRGB); // pass MedianFilter reference to fragment shader
    updateGL();
}



void MedianFilter::settingSliderAndSpinBox(QSlider* slider, QSpinBox* spinbox, int value, bool oddOnly) {
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



void MedianFilter::changeFilterSize(int value) {
    value += !(value%2);
    settingSliderAndSpinBox(m_sliderFilterSize, m_spinBoxFilterSize, value, true);
}



void MedianFilter::changeAvgK(int value) {
    int limit = m_spinBoxFilterSize->value() * m_spinBoxFilterSize->value() / 2;
    if(value > limit) {
        value = limit;
    }
    settingSliderAndSpinBox(m_sliderAvgK, m_spinBoxAvgK, value, false);
}


// MedianFilter::initializeGL
//
// Initialization routine before display loop.
// Gets called once before the first time resizeGL() or paintGL() is called.
//
void MedianFilter::initializeGL()
{
    initializeGLFunctions();    // initialize GL function resolution for current context
    
    initTexture();
    initShaders();  // init vertex and fragment shaders
    initVertexBuffer(); // initialize vertex buffer and write positions to vertex shader
    glClearColor(0.0, 0.0, 0.0, 0.0);	// set background color
    glColor3f   (1.0, 1.0, 1.0);		// set foreground color
}



// MedianFilter::resizeGL
//
// Resize event handler.
// The input parameters are the window width (w) and height (h).
//
void MedianFilter::resizeGL(int w, int h)
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



// MedianFilter::paintGL
//
// Update GL scene.
//
void MedianFilter::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);   // clear canvas with background values
    if(!m_image.isNull()) glDrawArrays(GL_POLYGON, 0, m_numberVertices);   // draw a rectangle
}



// Thresold::initTexture
//
// Initialize setup for texture
//
void
MedianFilter::initTexture()
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



// MedianFilter::initShaders
//
// Initialize vertex and fragment shaders.
//
void MedianFilter::initShaders()
{
    // compile vertex shader
    if(!m_program.addShaderFromSourceFile(QGLShader::Vertex, ":/vshaderMedianFilter.glsl")) {
        QMessageBox::critical(0, "Error", "Vertex shader error", QMessageBox::Ok);
        QApplication::quit();
    }
    
    // compile fragment shader
    if(!m_program.addShaderFromSourceFile(QGLShader::Fragment, ":/fshaderMedianFilter.glsl")) {
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
    m_uniform[NEIGHBORSIZE] = glGetUniformLocation(m_program.programId(), "u_NeighborSize");
    if((int) m_uniform[NEIGHBORSIZE] < 0) {
        qDebug() << "Failed to get the storage location of u_NeighborSize";
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
    
    glUniform1f(m_uniform[NEIGHBORSIZE], m_u_neighborSize);
    qDebug() << m_u_neighborSize;
}



// MedianFilter::initVertexBuffer:
//
// Initialize vertex buffer.
//
void MedianFilter::initVertexBuffer()
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
