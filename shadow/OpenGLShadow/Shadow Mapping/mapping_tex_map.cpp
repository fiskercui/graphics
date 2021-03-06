/*
 * http://www.cnblogs.com/liangliangh/
 --------------------------------------------------------------------------------*/

#include "gl_staff.h"
#include "bt_inc.h"

#include <iostream>

GLuint tex_lena; // 纹理名字
glm::vec4 light_pos; // 世界坐标系中光源位置
glm::mat4 shadow_mat_p; // 光源视角的投影矩阵
glm::mat4 shadow_mat_v; // 光源视角的视图矩阵

void tex_init() // 纹理初始化
{
	// 纹理如何影响颜色，和光照计算结果相乘
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	// 分配纹理对象，并绑定为当前纹理
	glGenTextures(1, &tex_lena);
	glBindTexture(GL_TEXTURE_2D, tex_lena);
	// 纹理坐标超出[0,1]时如何处理
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	// 边框颜色
	GLfloat c[4] = {0,0,0, 1};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, c);
	// 非整数纹理坐标处理方式，线性插值
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// 纹理坐标自动生成
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
	glEnable(GL_TEXTURE_GEN_Q);
	// 纹理数据
	void* data; int w, h;
	//il_readImg(L"Lena Soderberg.jpg", &data, &w, &h);
	il_readImg(L"glass.jpg", &data, &w, &h);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	delete data;
}

void dlight() // 在灯上绘制一个球
{
	GLboolean li = glIsEnabled(GL_LIGHTING);
	if(li) glDisable(GL_LIGHTING);
	GLfloat cc[4]; glGetFloatv(GL_CURRENT_COLOR, cc);
	glColor3f(0.8f, 0.8f, 0.1f);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(light_pos[0], light_pos[1], light_pos[2]);
	glutSolidSphere(0.05f, 50, 50);
	glPopMatrix();
	if(li) glEnable(GL_LIGHTING);
	glColor4fv(cc);
}

void draw_world() // 绘制世界，一个地板
{
	glm::vec4 v1(-3, 0,-3, 1), v2(-3, 0, 3, 1), v3( 3, 0, 3, 1), v4( 3, 0,-3, 1);
	glBegin(GL_POLYGON);
		glNormal3f(0, 1, 0);
		glVertex3fv(&v1[0]);
		glVertex3fv(&v2[0]);
		glVertex3fv(&v3[0]);
		glVertex3fv(&v4[0]);
	glEnd();//*/
}

void draw_model() // 绘制模型，一个茶壶
{	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(0, 1, 0);
		glutSolidTeapot(1);
	glPopMatrix();
}

void draw(const glm::mat4& mat_model, const glm::mat4& mat_view)
{
	double tt = omp_get_wtime();

	// 将摄像机放置在光源位置，投影矩阵和视图矩阵
	shadow_mat_p = glm::perspective(glm::radians(60.0f), 1.0f, 1.0f, 1.0e10f);
	shadow_mat_v = glm::lookAt(glm::vec3(light_pos), glm::vec3(0), glm::vec3(0,1,0));
	// When the eye planes are specified, the GL will automatically post-multiply them
	// with the inverse of the current modelview matrix.
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&mat_view[0][0]); // glLoadIdentity();
	glm::mat4 mat =
		glm::translate(glm::vec3(0.5f,0.5f,0.5f)) * glm::scale(glm::vec3(0.5f,0.5f,0.5f))
		* shadow_mat_p * shadow_mat_v/* * glm::affineInverse(mat_view)*/;
	mat = glm::transpose(mat);
	glTexGenfv(GL_S, GL_EYE_PLANE, &mat[0][0]);
	glTexGenfv(GL_T, GL_EYE_PLANE, &mat[1][0]);
	glTexGenfv(GL_R, GL_EYE_PLANE, &mat[2][0]);
	glTexGenfv(GL_Q, GL_EYE_PLANE, &mat[3][0]);
	glEnable(GL_TEXTURE_2D);

	/*
	GLfloat* data = new GLfloat[glStaff::get_frame_width()*glStaff::get_frame_height()];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, data); // 获取纹理数据
	il_saveImgDep(L"d0.png", data, glStaff::get_frame_width(), glStaff::get_frame_height());
	delete[] data;//*/

	//-------------------------------------------第2次绘制，绘制场景------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&mat_view[0][0]);
		draw_world();
	glMultMatrixf(&mat_model[0][0]);
		draw_model();

	glDisable(GL_TEXTURE_2D);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&mat_view[0][0]);
	dlight();

	tt = omp_get_wtime()-tt;
	char ss[50];
	sprintf(ss, "render time (ms): %.2f", tt*1000);
	glStaff::text_upperLeft(ss, 1);
}

int main(void)
{
	glStaff::init_win(800, 800,
		"OpenGL Shadow    - cnblogs.com/liangliangh", "C:\\Windows\\Fonts\\msyh.ttf");
	glStaff::init_gl();
	glStaff::set_mat_view( glm::lookAt( glm::vec3(2,10,6), glm::vec3(0,0,0), glm::vec3(0,1,0) ) );
	light_pos = glm::vec4(2,5,0.5f, 1);
	tex_init();
	glStaff::renderLoop(draw);
}
