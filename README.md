Image filter
============
Image filter is something that you can see often in the image processing software, for example PhotoShop, it can enhance contrast, brightness for you images. This project is going to implement some basic image filters using **OpenGL** and **Qt**.

### Filter List
- Point Operations
  * [x] Threshold
  * [x] Contrast
  * [x] Quantization(Dithering isn't good)
  * [x] Histogram Stretching
  * [ ] Histogram Matching

- Neighborhood Operations
  * [x] Blur
  * [ ] Sharpen
  * [ ] Median Filter

**Note:This is a Qt project, in order to run this you have to have Qt installed!**

**OpenGL:** Open Graphics Library (OpenGL) is a cross-language, cross-platform application programming interface (API) for rendering 2D and 3D vector graphics. The API is typically used to interact with a graphics processing unit (GPU), to achieve hardware-accelerated rendering.

**Qt:** Qt is a cross-platform application framework that is widely used for developing application software that can be run on various software and hardware platforms with little or no change in the underlying codebase, while still being a native application with the capabilities and speed thereof

ScreenShots
===========

### Input:
![alt text](https://github.com/dongliang3571/Senior_design_csc599866/blob/master/screenshots/input.png?raw=true "Threshold input")

### Output:
- Threshold
![alt text](https://github.com/dongliang3571/Senior_design_csc599866/blob/master/screenshots/threshold.png?raw=true "Logo Title Text 1")

- Contrast
![alt text](https://github.com/dongliang3571/Senior_design_csc599866/blob/master/screenshots/contrast.png?raw=true "Logo Title Text 1")

Important Notes
===============

### Include lib "QtOpenGL"

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
  After `#include <QtOpenGL>`, you can use `<QGLFunctions>`, `<QGLShaderProgram>`, `<QGLWidgets>` and so on. Alternatively, you can do `#include <QGLShaderProgram>` or `#include <QGLFunctions>`to include each of the class definition in `<QtOpenGL>`, that way you can be aware of what methods come from what classes.

### gl_ModelViewProjectionMatrix
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
  
### Only declaring shader variables


- Declaring shader variable(uniform or attribute varibles) but not use it in the `main()` will cause failure to get location of the variable, eg. `int location = glGetUniformLocation(m_program.programId(), "u_Lut");` the location to be returned will be -1, which indicate failed to get location for name `u_Lut`

### Array type in shader

- This is how to declare array type in shader: `uniform int u_Lut[256];`
- To get location of uniform variable of array type: `glGetUniformLocation(m_program.programId(), "u_Lut")`, or `glGetUniformLocation(m_program.programId(), "u_Lut[0]")`


### glUniform  
(note: bool is 1 or 0)

- glUniform{1|2|3|4}{f|i|ui} 
  * `glUniform1i(GLint location, GLint x)` send data of type **int**
  * `glUniform2f(GLint location, GLfloat x, GLfloat y)` send data of type **float**
  * `glUniform2i(GLint location, GLint x, GLint y)` send data of type **ivec2**
  * `glUniform2f(GLint location, GLfloat x, GLfloat y)` send data of type **vec2**
  * `glUniform3i(GLint location, GLint x, GLint y, GLint z)` send data of type **ivec3**
  * `glUniform3f(GLint location, GLfloat x, GLfloat y, GLfloat z)` send data of type **vec3**
  *  Example: `glUniform1i(m_uniform[SAMPLER], 0);`
- glUniform{1|2|3|4}{f|i|ui}v 
  * `glUniform1iv(GLint location, GLsizei count(size of array), const GLint* v(pointer of first element of array))` send data of type int[]
  * `glUniform1fv` send data of type **float[]**
  * `glUniform2iv` send data of type **ivec2[]**
  * `glUniform2fv` send data of type **vec2[]**
  * `glUniform3iv` send data of type **ivec3[]**
  * `glUniform3fv` send data of type **vec3[]**
  * Example: `glUniform1iv(m_uniform[LUT], 256, lut);`

- glUniformMatrix{2|3|4|2x3|3x2|2x4|4x2|3x4|4x3}fv
  * `glUniformMatrix3x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)` send data of type **mat3x4**
  * `glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)` send data of type **mat4**
  * `glUniformMatrix4fv(m_uniform[MVP], 1, GL_FALSE, m_u_mvpMatrix.constData());`

### Sampler2D and texture2D in fragment shader
- Sampler indicates texture unit number(`GL_TEXTURE0`). Example: `glUniform1i(m_uniform[SAMPLER], 0);`
- `vec4 texture2D(sampler2D sampler, vec2 coord)`, `vec4 texture2D(sampler2D sampler, vec2 coord, float bias)`
The texture2D function returns a texel, i.e. the (color) value of the texture for the given coordinates. The function has one input parameter of the type sampler2D and one input parameter of the type vec2 : sampler, the uniform the texture is bound to, and coord, the 2-dimensional coordinates of the texel to look up.
  * There is an optional third input parameter of the type float: bias. After calculating the appropriate level of detail for a texture with mipmaps the bias is added before the actual texture lookup operation is executed.
  * Side note: On iOS devices texture lookup functionality is only available in the fragment shader.
