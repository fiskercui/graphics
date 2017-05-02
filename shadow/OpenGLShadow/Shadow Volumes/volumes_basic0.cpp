/*
 * http://www.cnblogs.com/liangliangh/
 --------------------------------------------------------------------------------*/

#include "stdafx.h"


glm::vec4 light_pos; // 世界坐标
bool light_rotate=false;
bool dis_volumes=false;

void dlight(float size=0.1f) // 在灯上绘制一个球
{ 
	GLboolean li = glIsEnabled(GL_LIGHTING);
	if(li) glDisable(GL_LIGHTING);
	GLfloat cc[4]; glGetFloatv(GL_CURRENT_COLOR, cc);
	glColor3f(1.0f, 1.0f, 0.2f);
	glMatrixMode(GL_MODELVIEW); glPushMatrix();
		glTranslatef(light_pos[0], light_pos[1], light_pos[2]);
		glutSolidSphere(size, 50, 50);
	glPopMatrix();
	if(li) glEnable(GL_LIGHTING);
	glColor4fv(cc);
}

// 世界，四边形地板
void draw_world()
{
	glStaff::xyz_frame(2, 2, 2, false);
	glBegin(GL_POLYGON);
		glNormal3f(0, 1, 0);
		glVertex3f(-5, 0,-5); glVertex3f(-5, 0, 5);
		glVertex3f(5, 0, 5); glVertex3f(5, 0,-5);
	glEnd();
}

glm::vec3 tri1[3] = { glm::vec3(0, 3, 0), glm::vec3( 0, 3, 2), glm::vec3(2, 3, 0) };
glm::vec3 tri2[3] = { glm::vec3(1, 2,-1), glm::vec3(-1, 2,-1), glm::vec3(1, 2, 1) };
// 模型，两个三角形
void draw_model()
{
	GLfloat _ca[4], _cd[4];
	glGetMaterialfv(GL_FRONT, GL_AMBIENT, _ca);
	glGetMaterialfv(GL_FRONT, GL_DIFFUSE, _cd);
	GLfloat c[4];
	glBegin(GL_TRIANGLES);
		c[0]=1; c[1]=0; c[2]=0; c[3]=1; glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, c);
		glNormal3fv(&glm::normalize(glm::cross(tri1[1]-tri1[0], tri1[2]-tri1[0]))[0]);
		for(int i=0; i<3; ++i) glVertex3fv(&tri1[i][0]); // tri1，红色
		c[0]=0; c[1]=1; c[2]=0; c[3]=1; glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, c);
		glNormal3fv(&glm::normalize(glm::cross(tri2[1]-tri2[0], tri2[2]-tri2[0]))[0]);
		for(int i=0; i<3; ++i) glVertex3fv(&tri2[i][0]); // tri2，绿色
	glEnd();
	glMaterialfv(GL_FRONT, GL_AMBIENT, _ca);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, _cd);
}

static float d_far = 10;
// 构造、绘制 Shadow Volume，仅考虑位置光源
void draw_model_volumes(bool light=false)
{
	GLfloat _ca[4], _cd[4];
	glGetMaterialfv(GL_FRONT, GL_AMBIENT, _ca);
	glGetMaterialfv(GL_FRONT, GL_DIFFUSE, _cd);
	for(int t=0; t<2; ++t){
		if(light){ GLfloat c[4];
			if(t==0) c[0]=1, c[1]=0, c[2]=0, c[3]=1;
			else     c[0]=0, c[1]=1, c[2]=0, c[3]=1;
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, c);
		}
		glm::vec3* tri = t==0 ? tri1 : tri2; // tri1 or tri2
		glm::vec3 tri_far[3];
		for(int i=0; i<3; ++i){
			tri_far[i] = tri[i] + glm::normalize(tri[i]-glm::vec3(light_pos))*d_far;
		}
		for(int i=0; i<3; ++i){
			glBegin(GL_POLYGON); // 三个边挤出（extrude）的四边形
				if(light) glNormal3fv(&glm::normalize(glm::cross(tri_far[i]-tri[i], tri[(i+1)%3]-tri[i]))[0]);
				glVertex3fv(&tri[i][0]);
				glVertex3fv(&tri_far[i][0]);
				glVertex3fv(&tri_far[(i+1)%3][0]);
				glVertex3fv(&tri[(i+1)%3][0]);
			glEnd();
		}
		glBegin(GL_TRIANGLES); // 顶部（near cap），原三角形，对 Zpass 来说可选
			if(light) glNormal3fv(&glm::normalize(glm::cross(tri[1]-tri[0], tri[2]-tri[0]))[0]);
			for(int i=0; i<3; ++i) glVertex3fv(&tri[i][0]);
		glEnd();
		glBegin(GL_TRIANGLES); // 底部（far cap），挤出三角形，对 Zpass 来说可选
			if(light) glNormal3fv(&glm::normalize(glm::cross(tri_far[2]-tri_far[0], tri_far[1]-tri_far[0]))[0]);
			for(int i=0; i<3; ++i) glVertex3fv(&tri_far[2-i][0]);
		glEnd();
	}
	glMaterialfv(GL_FRONT, GL_AMBIENT, _ca);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, _cd);
}

// 观察 Shadow Volumes，光源位于视觉坐标原点
void draw2(const glm::mat4& mat_model, const glm::mat4& mat_view)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	GLfloat pl[4]={0,0,0, 1}; glLightfv(GL_LIGHT0, GL_POSITION, pl); // 位置式光源
	glMatrixMode(GL_MODELVIEW); glLoadMatrixf(&mat_view[0][0]);
		draw_world();
	glMultMatrixf(&mat_model[0][0]);
		draw_model_volumes(true);

	glMatrixMode(GL_MODELVIEW); glLoadMatrixf(&mat_view[0][0]);
		dlight(0.05f);

	if(light_rotate)
		light_pos = glm::rotate(glm::radians(1.0f), glm::vec3(0,1,0)) * light_pos;
}

void draw1(const glm::mat4& mat_model, const glm::mat4& mat_view)
{
	double tt = omp_get_wtime();
	
	// ------------------------------------------ 清除缓冲区，包括模板缓冲
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// ------------------------------------------ 第1遍，渲染环境光，深度值
	// 关闭光源，打开环境光
	GLboolean _li0 = glIsEnabled(GL_LIGHT0); if(_li0) glDisable(GL_LIGHT0);
	glMatrixMode(GL_MODELVIEW); glLoadMatrixf(&mat_view[0][0]);
		draw_world();
	glMultMatrixf(&mat_model[0][0]);
		draw_model();
	if(_li0) glEnable(GL_LIGHT0);

	// ------------------------------------------ 第2遍，渲染模板值
	// 不需要光照，不更新颜色和深度缓冲
	GLboolean _li = glIsEnabled(GL_LIGHTING); if(_li) glDisable(GL_LIGHTING);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE); glStencilMask(~0);
	glEnable(GL_CULL_FACE);
	glEnable(GL_STENCIL_TEST); glStencilFunc(GL_ALWAYS, 0, ~0);
	// 剔除背面留下正面，穿入，模板值 加 1
	glCullFace(GL_BACK); glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
	glMatrixMode(GL_MODELVIEW); glLoadMatrixf(&mat_view[0][0]); glMultMatrixf(&mat_model[0][0]);
		draw_model_volumes();
	// 剔除正面留下背面，穿出，模板值 减 1
	glCullFace(GL_FRONT); glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
	glMatrixMode(GL_MODELVIEW); glLoadMatrixf(&mat_view[0][0]); glMultMatrixf(&mat_model[0][0]);
		draw_model_volumes();
	// 恢复状态
	if(_li) glEnable(GL_LIGHTING);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE); glStencilMask(~0);
	glDisable(GL_CULL_FACE); glDisable(GL_STENCIL_TEST); glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	// ------------------------------------------ 第3遍，渲染光源光照，依据模板值判断阴影
	// 关闭环境光，打开光源
	GLfloat _lia[4]; glGetFloatv(GL_LIGHT_MODEL_AMBIENT, _lia);
	GLfloat ca[4]={0}; glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ca);
	// 模板测试为，等于0通过， 深度测试为，相等通过，颜色混合为直接累加
	glEnable(GL_STENCIL_TEST); glStencilFunc(GL_EQUAL, 0, ~0);
	glDepthFunc(GL_EQUAL); glBlendFunc(GL_ONE, GL_ONE);

	glMatrixMode(GL_MODELVIEW); glLoadMatrixf(&mat_view[0][0]);
	glLightfv(GL_LIGHT0, GL_POSITION, &light_pos[0]); // 位置式光源
		draw_world();
	glMultMatrixf(&mat_model[0][0]);
		draw_model();
	// 恢复状态
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, _lia);
	glDisable(GL_STENCIL_TEST); glStencilFunc(GL_ALWAYS, 0, ~0);
	glDepthFunc(GL_LESS); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// 在光源处绘制一个黄色的球
	glMatrixMode(GL_MODELVIEW); glLoadMatrixf(&mat_view[0][0]);
		dlight(0.05f);

	if(light_rotate)
		light_pos = glm::rotate(glm::radians(1.0f), glm::vec3(0,1,0)) * light_pos;

	tt = omp_get_wtime()-tt;
	char ss[50]; sprintf(ss, "render time (ms): %.2f", tt*1000);
	glStaff::text_upperLeft(ss, 1);
}

void draw(const glm::mat4& mat_model, const glm::mat4& mat_view)
{
	if(dis_volumes){
		draw2(mat_model, mat_view);
	}else{
		draw1(mat_model, mat_view);
	}
}

void key_p()
{
	light_rotate = !light_rotate;
}

void key_u()
{
	int w, h; glStaff::get_frame_size(&w, &h);
	unsigned char* data = new unsigned char[w*h];
	float* pixs = new float[w*h];
	glReadPixels(0,0, w,h, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, data);
	unsigned char m=0; for(int i=0; i<w*h; ++i) m = std::max(m, data[i]);
	float f = 1.0f/m/2; for(int i=0; i<w*h; ++i) pixs[i] = f * data[i];
	il_saveImgDep(L"stencil.png", pixs, w, h);
	delete[] data; delete[] pixs;
	std::cout << "stencil buffer saved" << '\n';
}

void key_t()
{
	dis_volumes = !dis_volumes;
}

int main(void) 
{

	glStaff::init_win(800, 800,
		"OpenGL Shadow    - cnblogs.com/liangliangh", "C:\\Windows\\Fonts\\msyh.ttf");
	glStaff::init_gl();

	glStaff::set_mat_view(
		glm::lookAt( glm::vec3(8,8,8), glm::vec3(0,0,0), glm::vec3(0,1,0) ) );
	glStaff::set_mat_model( glm::translate( glm::vec3(0,0,0) ) );
	glStaff::load_mat_view("matrix_view_basic0");

	light_pos = glm::vec4(2,5,1, 1);

	glStaff::add_key_callback('P', key_p, L"改变光源位置");
	glStaff::add_key_callback('U', key_u, L"保存模板缓冲区");
	glStaff::add_key_callback('T', key_t, L"切换显示阴影体");

	//GLfloat c[4]={1,1,1, 1}; glLightModelfv(GL_LIGHT_MODEL_AMBIENT, c); 
	glStaff::renderLoop(draw);
}
