//
//  Threshold.h
//  HW
//
//  Created by dong liang on 8/16/16.
//
//

#ifndef Threshold_h
#define Threshold_h

#include "GLWidget.h"
#include <QGLShaderProgram>
#include <QtOpenGL>
#include <vector>
    // ----------------------------------------------------------------------
    // standard include files
    //

class Threshold : public GLWidget {
    Q_OBJECT
public:
    Threshold  (QWidget *parent = 0);		// constructor
    
    QGroupBox*  controlPanel();		// create control panel
    void        reset();            // reset parameters
    void        setImage(QImage image);  // set image
    void        reload();
    void        initVertexBuffer(); // initialize vertices
    void        initShaders();      // initialize shaders
    void        initTexture();      // initialize texture
    
public slots:
    void changeThr(int value);      // slot when m_slider changes
    
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
    GLuint		  m_texture;        // shader index to texture unit
    GLuint		  m_uniform[16];	// uniform var for a shader and <16 vars
    
    // shader variables
    QMatrix4x4    m_u_mvpMatrix;	// View Model Porjection matrix
    GLfloat       m_u_reference;    // Threshold reference value
    
    // GUI variables. widgets
    QSlider		*m_slider ;	// Threshold sliders
    QSpinBox	*m_spinBox;	// Threshold spin boxes
    QLabel		*m_label;	// Label for printing Otsu thresholds
    QGroupBox	*m_ctrlGrp;	// Groupbox for panel
};



#endif /* Threshold_h */
