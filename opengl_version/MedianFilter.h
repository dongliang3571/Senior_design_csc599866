//
//  MedianFilter.h
//  hw
//
//  Created by dong liang on 8/28/16.
//
//

#ifndef MedianFilter_h
#define MedianFilter_h

#include "GLWidget.h"


class MedianFilter : public GLWidget {
    Q_OBJECT
public:
    MedianFilter  (QWidget *parent = 0);		// constructor
    QGroupBox*  controlPanel();		// create control panel
    void        reset();            // reset parameters
    void        setImage(QImage image);  // set image
    void        reload();
    void        initVertexBuffer(); // initialize vertices
    void        initShaders();      // initialize shaders
    void        initTexture();      // initialize texture
    
public slots:
    void changeFilterSize(int value);
    void changeAvgK(int value);
    
protected:
    void		initializeGL	();		// init GL state
    void		resizeGL	(int, int);	// resize GL widget
    void		paintGL		();         // render GL scene
    
private:
    int         m_winW;			// window width
    int         m_winH;			// window height
    int         m_numberVertices;   // number of vertices
    
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
    GLfloat       m_u_neighborSize;
    
    // GUI variables. widgets
    QGroupBox*      m_ctrlGrp;	// groupbox for panel
    QSlider*        m_sliderFilterSize;
    QSlider*        m_sliderAvgK;
    QSpinBox*       m_spinBoxFilterSize;
    QSpinBox*       m_spinBoxAvgK;
    
    // Util function
    void settingSliderAndSpinBox(QSlider* slider, QSpinBox* spinbox, int value, bool oddOnly);
};


#endif /* MedianFilter_h */
