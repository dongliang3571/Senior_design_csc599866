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

    // ----------------------------------------------------------------------
    // standard include files
    //

class Contrast : public GLWidget {
    Q_OBJECT
public:
    Contrast(QWidget *parent = 0);		// constructor
    QGroupBox*	controlPanel	();		// create control panel
    void		reset		();		// reset parameters
    void        setImage(QImage image);  // set image
    void		initVertexBuffer();		// init vertices
    void		initShaders	();		// init shaders
    void		initTexture	();		// init texture image
    
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
    QImage		  m_image;			// texture image
    GLuint		  m_texture;			// shader index to texture unit
    GLuint		  m_uniform[16];		// uniform vars for two shaders and <16 vars
    
    // shader variables
    QMatrix4x4    m_u_mvpMatrix;	// View Model Porjection matrix
};


#endif /* Contrast_h */
