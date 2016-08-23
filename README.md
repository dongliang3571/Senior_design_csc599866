**Note:This is a Qt project, in order to run this you have to have Qt installed!**
Open Graphics Library (OpenGL) is a cross-language, cross-platform application programming interface (API) for rendering 2D and 3D vector graphics. The API is typically used to interact with a graphics processing unit (GPU), to achieve hardware-accelerated rendering.

Important Notes
===============
- In order to use opengl libray in Qt, we need to add `QT += opengl` to .pro file to link against the module. Add `#include <QtOpenGL>` to include the definitions of the module's classes. *Note:* A **module** is just a bunch of header files reside together in one file, example of `#include <QtOpenGL>`:
  ```
  #ifndef QT_QTOPENGL_MODULE_H
  #define QT_QTOPENGL_MODULE_H
  #include <QtOpenGL/QtOpenGLDepends>
  #include "qgl.h"
  #include "qglbuffer.h"
  #include "qglcolormap.h"
  #include "qglframebufferobject.h"
  #include "qglfunctions.h"
  #include "qglpixelbuffer.h"
  #include "qglshaderprogram.h"
  #include "qtopenglglobal.h"
  #include "qtopenglversion.h"
  #endif

  ```
  After `#include <QtOpenGL>`, you can use `<QGLFunctions>`, `<QGLShaderProgram>` and so on. Alternatively, you can do `#include <QGLShaderProgram>` or `#include <QGLFunctions>`to include each of the class definition in `<QtOpenGL>`, that way you can be aware of what methods come from what classes.
- `gl_ModelViewProjectionMatrix` in shader file(normally is multiplied by a Position attribute variable, eg. `gl_Position = gl_ModelViewProjectionMatrix * a.Position`) is controlled by glfunctions, eg. `glLoadIdentity()`,
	`glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)`, `glRotated(GLdouble angle, GLdouble x, GLdouble y, GLdouble z)` and so on, these are outside shader files, normally they reside in program methods, like
  ```
  void HW3a::resizeGL(int w, int h)
  {
	  // save window dimensions
	  m_winW = w;
	  m_winH = h;

	  // compute aspect ratio
	  float ar = (float) w / h;
	  float xmax, ymax;
	  if(ar > 1.0) {		// wide screen
		  xmax = ar;
		  ymax = 1.;
	  }
	  else {			// tall screen
		  xmax = 1.;
		  ymax = 1 / ar;
	  }

	  // set viewport to occupy full canvas
	  glViewport(0, 0, w, h);

	  // init viewing coordinates for orthographic projection
	  glLoadIdentity();                               ////////////////Control gl_ModelViewProjectionMatrix//////////// 
	  glOrtho(-xmax, xmax, -ymax, ymax, -1.0, 1.0);   ////////////////Control gl_ModelViewProjectionMatrix//////////// 
  }
  ```


