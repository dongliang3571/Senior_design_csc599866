//
//  Threshold.h
//  hw
//
//  Created by dong liang on 8/16/16.
//
//

#ifndef Threshold_h
#define Threshold_h

#include "HW.h"
#include <QGLShaderProgram>
#include <QtOpenGL>
    // ----------------------------------------------------------------------
    // standard include files
    //

class Threshold : public HW {
    Q_OBJECT
public:
    Threshold  (QWidget *parent = 0);		// constructor
    
    QGroupBox*  controlPanel();		// create control panel
    void        reset();            // reset parameters
    void        initVertexBuffer(); // initialize vertices
    void        initShaders();      // initialize shaders
    void        initTexture();      // initialize texture
    
protected:
    void		initializeGL	();		// init GL state
    void		resizeGL	(int, int);	// resize GL widget
    void		paintGL		();         // render GL scene
    
private:
    int         m_winW;			// window width
    int         m_winH;			// window height
    
    QGLShaderProgram    m_program;			// GLSL program
    
    QImage		  m_image;              // texture image
    GLuint		  m_texture;			// shader index to texture unit
    GLuint		  m_uniform[16];		// uniform var for a shader and <16 vars
    
    // shader variables
    QMatrix4x4		  m_u_mvpMatrix;		// shader index to transformation matrix
    
};



#endif /* Threshold_h */
