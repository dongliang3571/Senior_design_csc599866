//
//  Quantization.h
//  hw
//
//  Created by dong liang on 8/23/16.
//
//

#ifndef Quantization_h
#define Quantization_h


#include "GLWidget.h"


class Quantization : public GLWidget {
    Q_OBJECT
public:
    Quantization		(QWidget *parent = 0);		// constructor
    QGroupBox*	controlPanel	();		// create control panel
    void		reset		();		// reset parameters
    void        setImage(QImage image);  // set image
    void        reload();
    void		initVertexBuffer();		// init vertices
    void		initShaders	();		// init shaders
    void		initTexture	();		// init texture image

protected slots:
    void changeLevel (int level);
    void changeDither(int checked);
    
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
    GLfloat       m_u_bias;
    GLboolean     m_u_isDither;
    GLfloat       m_u_scale;
    
    // GUI Components
    QGroupBox*      m_ctrlGrp;	// groupbox for panel
    QSlider*        m_sliderLevel;
    QSpinBox*       m_spinboxLevel;
    QCheckBox*      m_checkBox_dither;
};

#endif /* Quantization_h */
