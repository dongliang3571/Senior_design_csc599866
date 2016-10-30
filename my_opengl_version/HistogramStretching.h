//
//  HistogramStretching.h
//  hw
//
//  Created by dong liang on 8/26/16.
//
//

#ifndef HistogramStretching_h
#define HistogramStretching_h

#include "GLWidget.h"


class HistogramStretching : public GLWidget {
    Q_OBJECT
public:
    HistogramStretching  (QWidget *parent = 0);		// constructor
    QGroupBox*  controlPanel();		// create control panel
    void        reset();            // reset parameters
    void        setImage(QImage image);  // set image
    void        reload();
    void        initVertexBuffer(); // initialize vertices
    void        initShaders();      // initialize shaders
    void        initTexture();      // initialize texture
    
public slots:
    void changeStretchMax (int stretchValue);
    void changeStretchMin (int stretchValue);
    void autoStretch(int isAuto);
    
protected:
    void		initializeGL	();		// init GL state
    void		resizeGL	(int, int);	// resize GL widget
    void		paintGL		();         // render GL scene
    
private:
    int         m_winW;			// window width
    int         m_winH;			// window height
    int         m_numberVertices;   // number of vertices
    
    // GLSL Program and needed variables
    QGLShaderProgram    m_program;  // GLSL program
    QImage		  m_image;          // texture image
    QImage        m_GLImage;
    GLsizei       m_width_GLImage;
    GLsizei       m_height_GLImage;
    
    GLuint		  m_texture;        // shader index to texture unit
    GLuint		  m_uniform[16];	// uniform var for a shader and <16 vars
    
    // shader variables
    QMatrix4x4    m_u_mvpMatrix;	// View Model Porjection matrix
    GLfloat       m_u_minValue;
    GLfloat       m_u_maxValue;
    
    // GUI variables. widgets
    QGroupBox*      m_ctrlGrp;	// groupbox for panel
    QSlider*        m_sliderMax;
    QSlider*        m_sliderMin;
    QSpinBox*       m_spinboxMax;
    QSpinBox*       m_spinboxMin;
    QCheckBox*      m_checkBoxAutoMax;
    QCheckBox*      m_checkBoxAutoMin;
};


#endif /* HistogramStretching_h */
