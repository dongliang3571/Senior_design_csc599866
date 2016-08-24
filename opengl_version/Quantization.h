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
    void		initVertexBuffer();		// init vertices
    void		initShaders	();		// init shaders
    void		initTexture	();		// init texture image

    
protected:
    void		initializeGL	();		// init GL state
    void		resizeGL	(int, int);	// resize GL widget
    void		paintGL		();		// render GL scene
    void		divideTriangle(vec2, vec2, vec2, int);	// subdivide triangle
    void		triangle(vec2, vec2, vec2);	// process single triangle
    
    
private:
    int		  m_winW;			// window width
    int		  m_winH;			// window height
    int       m_numberVertices;   // number of vertices
    bool      m_isInitialized;    // Bool to save state of initialization
    
    QGLShaderProgram  m_program[2];			// GLSL programs
    bool		  m_twist;			// twist flag
    bool		  m_wire;			// wireframe flag
    float		  m_theta;			// rotation angle
    int		  m_subdivisions;		// triangle subdivisions
    QSlider		 *m_sliderTheta;		// rotation slider
    QSlider		 *m_sliderSubdiv;		// subdivision slider
    QSpinBox	 *m_spinBoxTheta;		// rotation spinbox
    QSpinBox	 *m_spinBoxSubdiv;		// subdivision spinbox
    QCheckBox	 *m_checkBoxTwist;		// twist checkbox
    QCheckBox	 *m_checkBoxWire;		// wireframe checkbox
    std::vector<vec2> m_points;			// vector of 2D points (XY)
    std::vector<vec2> m_coords;			// vector of 2D coords (UV)
    int		  m_numPoints;			// number of 2D points
    QMatrix4x4	  m_ModelMatrix;		// 4x4 transformation matrix
    
    QMatrix4x4    m_ViewMatrix;
    
    QImage		  m_image;			// texture image
    GLuint		  m_texture;			// shader index to texture unit
    GLuint		  m_uniform[2][16];		// uniform vars for two shaders and <16 vars
};

#endif /* Quantization_h */
