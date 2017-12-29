/****************************************************************************

 Copyright (C) 2002-2014 Gilles Debunne. All rights reserved.

 This file is part of the QGLViewer library version 2.7.1.

 http://www.libqglviewer.com - contact@libqglviewer.com

 This file may be used under the terms of the GNU General Public License 
 versions 2.0 or 3.0 as published by the Free Software Foundation and
 appearing in the LICENSE file included in the packaging of this file.
 In addition, as a special exception, Gilles Debunne gives you certain 
 additional rights, described in the file GPL_EXCEPTION in this package.

 libQGLViewer uses dual licensing. Commercial/proprietary software must
 purchase a libQGLViewer Commercial License.

 This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

*****************************************************************************/

#include "drawLight.h"
#include <math.h>

#include <QGLViewer/manipulatedFrame.h>

using namespace std;
using namespace qglviewer;

void Viewer::draw() {
  float pos[4] = {1.0, 0.5, 1.0, 0.0};
  // Directionnal light 方向光
  //平行光源, GL_POSITION属性的最后一个参数为0
  glLightfv(GL_LIGHT0, GL_POSITION, pos);

  pos[3] = 1.0;
  // Spot light
  //点光源, GL_POSITION属性的最后一个参数为1
  Vec pos1 = light1->position();
  pos[0] = float(pos1.x);
  pos[1] = float(pos1.y);
  pos[2] = float(pos1.z);
  glLightfv(GL_LIGHT1, GL_POSITION, pos);
  glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION,
            light1->inverseTransformOf(Vec(0, 0, 1)));

  // Point light
  Vec pos2 = light2->position();
  pos[0] = float(pos2.x);
  pos[1] = float(pos2.y);
  pos[2] = float(pos2.z);
  glLightfv(GL_LIGHT2, GL_POSITION, pos);

  // Draws the spiral
  const float nbSteps = 200.0;
  glBegin(GL_QUAD_STRIP);//绘制螺旋模型
  for (float i = 0; i < nbSteps; ++i) {
    float ratio = i / nbSteps;
    float angle = 21.0 * ratio;
    float c = cos(angle);
    float s = sin(angle);
    float r1 = 1.0 - 0.8 * ratio;
    float r2 = 0.8 - 0.8 * ratio;
    float alt = ratio - 0.5;
    const float nor = .5;
    const float up = sqrt(1.0 - nor * nor);
    glColor3f(1 - ratio, 0.2f, ratio);
    glNormal3f(nor * c, up, nor * s);
    glVertex3f(r1 * c, alt, r1 * s);
    glVertex3f(r2 * c, alt + 0.05, r2 * s);
  }
  glEnd();

  drawLight(GL_LIGHT0);

  if (light1->grabsMouse())//grab 抓取 ,光线1被鼠标选中时,光线1的图像被放大到1.2倍
    drawLight(GL_LIGHT1, 1.2f);
  else
    drawLight(GL_LIGHT1);

  if (light2->grabsMouse())//光线2被鼠标选中时,光线2的图像被放大到1.2倍
    drawLight(GL_LIGHT2, 1.2f);
  else
    drawLight(GL_LIGHT2);
}

void Viewer::init() {
  /**glMatrixMode 是对接下来要做什么进行一下声明，也就是在要做下一步之前告诉计算机我要对“什么”进行操作了，
   *这个“什么”在glMatrixMode的“()”里的选项(参数)有3种模式:
   *GL_PROJECTION 投影, GL_MODELVIEW 模型视图, GL_TEXTURE 纹理
   */
  //参数是GL_MODELVIEW，这个是对模型视景的操作
  glMatrixMode(GL_MODELVIEW);
  //当前矩阵均恢复成一个单位矩阵
  //单位矩阵就是对角线上都是1，其余元素皆为0的矩阵
  glLoadIdentity();

  // Light0 is the default ambient light
  glEnable(GL_LIGHT0);

  // Light1 is a spot light
  glEnable(GL_LIGHT1);
  const GLfloat light_ambient[4] = {0.8f, 0.2f, 0.2f, 1.0};//环境光
  const GLfloat light_diffuse[4] = {1.0, 0.4f, 0.4f, 1.0};//漫反射光
  const GLfloat light_specular[4] = {1.0, 0.0, 0.0, 1.0};//镜面反射光

  //GL_SPOT_EXPONENT
  //!属性只有一个值，表示聚光的程度，为零时表示光照范围内向各方向发射的光线强度相同，
  //!为正数时表示光照向中央集中，正对发射方向的位置受到更多光照，其它位置受到较少光照。
  //!数值越大，聚光效果就越明显。
  glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 3.0);
  //GL_SPOT_CUTOFF
  //!属性也只有一个值，表示一个角度，它是光源发射光线所覆盖角度的一半，
  //!其取值范围在0到90之间，也可以取180这个特殊值。
  //!取值为180时表示光源发射光线覆盖360度，即不使用聚光灯，向全周围发射。即一个点光源。
  glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 20.0);
  //GL_CONSTANT_ATTENUATION,GL_LINEAR_ATTENUATION,GL_QUADRATIC_ATTENUATION。
  //通过设置这三个常数，就可以控制光线在传播过程中的减弱趋势
  //GL_CONSTANT_ATTENUATION
  //!表示光线按常数衰减(与距离无关)
  glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.5);
  //GL_LINEAR_ATTENUATION
  //!表示光线按距离线性衰减
  glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 1.0);
  //GL_QUADRATIC_ATTENUATION
  //!表示光线按距离以二次函数衰减
  glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 1.5);
  glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);

  // Light2 is a classical directionnal light
  glEnable(GL_LIGHT2);
  const GLfloat light_ambient2[4] = {0.2f, 0.2f, 2.0, 1.0};//环境光
  const GLfloat light_diffuse2[4] = {0.8f, 0.8f, 1.0, 1.0};//漫反射光
  const GLfloat light_specular2[4] = {0.0, 0.0, 1.0, 1.0};//镜面反射光

  glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient2);
  glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular2);
  glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse2);

  light1 = new ManipulatedFrame();
  light2 = new ManipulatedFrame();
  setMouseTracking(true);//设置鼠标跟随

  light1->setPosition(0.5, 0.5, 0);//设置光源1的位置
  // Align z axis with -position direction : look at scene center
  light1->setOrientation(Quaternion(Vec(0, 0, 1), -light1->position()));

  light2->setPosition(-0.5, 0.5, 0);

  restoreStateFromFile();
  help();
}

QString Viewer::helpString() const {
  QString text("<h2>D r a w L i g h t</h2>");
  text += "The <i>drawLight()</i> function displays a representation of the "
          "OpenGL lights ";
  text += "of your scene. This is convenient for debugging your light "
          "setup.<br><br>";
  text += "This scene features a directionnal ligth (arrow), a spot light "
          "(cone) and a point ";
  text += "light source (sphere). The representation color, position and shape "
          "matches the light setup.<br><br>";
  text += "Hover over the point light or the spot light to manipulate it using "
          "the mouse (right ";
  text += "button translates and left button rotates).";
  return text;
}
