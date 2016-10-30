//
//  Blur.h
//  hw
//
//  Created by dong liang on 8/28/16.
//
//

#ifndef Blur_h
#define Blur_h

#include "GLWidget.h"


class Blur : public GLWidget {
    Q_OBJECT
public:
    Blur  (QWidget *parent = 0);		// constructor
    QGroupBox*  controlPanel();		// create control panel
    void        reset();            // reset parameters
    void        setImage(QImage image);  // set image
    void        reload();
    void        initVertexBuffer(); // initialize vertices
    void        initShaders();      // initialize shaders
    void        initTexture();      // initialize texture
    
public slots:
    void changeXrange   (int  value);
    void changeYrange   (int  value);
    void lockXY         (int  isChecked);
    
protected:
    void		initializeGL	();		// init GL state
    void		resizeGL	(int, int);	// resize GL widget
    void		paintGL		();         // render GL scene
    
private:
    int         m_winW;			// window width
    int         m_winH;			// window height
    int         m_numberVertices;   // number of vertices
    bool        m_isInitialized;    // Bool to save state of initialization
    
    
    QGLShaderProgram    m_program;  // GLSL program
    QImage		  m_image;          // texture image
    QImage        m_GLImage;
    GLsizei       m_width_GLImage;
    GLsizei       m_height_GLImage;
    
    GLuint		  m_texture;        // shader index to texture unit
    GLuint		  m_uniform[16];	// uniform var for a shader and <16 vars
    
    // shader variables
    QMatrix4x4    m_u_mvpMatrix;	// View Model Porjection matrix
    GLfloat       m_u_distanceX;    // Blur reference value
    GLfloat       m_u_distanceY;    // Blur reference value
    GLfloat        m_u_filterWidth;
    GLfloat        m_u_filterHeight;
    
    // GUI variables. widgets
    QGroupBox*      m_ctrlGrp;	// groupbox for panel
    QSlider*        m_sliderXrange;
    QSlider*        m_sliderYrange;
    QSpinBox*       m_spinBoxXrange;
    QSpinBox*       m_spinBoxYrange;
    QCheckBox*      m_checkboxLockXY;
};


#endif /* Blur_h */
