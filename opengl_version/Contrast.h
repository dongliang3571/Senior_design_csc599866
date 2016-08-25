//
//  Contrast.h
//  hw
//
//  Created by dong liang on 8/16/16.
//
//

#ifndef Contrast_h
#define Contrast_h

#include "GLWidget.h"


class Contrast : public GLWidget {
    Q_OBJECT
public:
    Contrast(QWidget *parent = 0);		// constructor
    QGroupBox*	controlPanel();		// create control panel
    void		reset();		// reset parameters
    void        setImage(QImage image);  // set image
    void        reload();
    void		initVertexBuffer();		// init vertices
    void		initShaders();		// init shaders
    void		initTexture();		// init texture image
    
protected:
    void		initializeGL	();		// init GL state
    void		resizeGL	(int, int);	// resize GL widget
    void		paintGL		();		// render GL scene

private:
    int		  m_winW;			// window width
    int		  m_winH;			// window height
    int       m_numberVertices;   // number of vertices
    bool      m_isInitialized;    // Bool to save state of initialization
    
    QGLShaderProgram  m_program;			// GLSL programs
    QImage        m_image;			// texture image
    GLuint		  m_texture;			// shader index to texture unit
    GLuint		  m_uniform[16];		// uniform vars for two shaders and <16 vars
    
    // shader variables
    QMatrix4x4    m_u_mvpMatrix;	// View Model Porjection matrix
    GLfloat       m_u_contrast;
    GLfloat       m_u_brightness;
    
    // GUI components
    QSlider         *m_sliderB ;	// brightness slider
    QSlider         *m_sliderC ;	// contrast   slider
    QSpinBox        *m_spinBoxB;	// brightness spin box
    QDoubleSpinBox	*m_spinBoxC;	// contrast   spin box
    QGroupBox       *m_ctrlGrp;     // groupbox for panel
};


#endif /* Contrast_h */
