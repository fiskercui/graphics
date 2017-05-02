/*
 * http://www.cnblogs.com/liangliangh/
 --------------------------------------------------------------------------------*/

#include "stdafx.h"

GLuint tex_walls;
glm::vec4 light_pos; // 世界坐标
bool light_rotate=false;
bool dis_volumes=false;
int  dis_volumes_modeline=false;

std::vector<GLMesh*> meshes;
std::vector<glm::mat4> meshes_mat;

void tex_init()
{
	glActiveTexture(GL_TEXTURE0); // 普通贴图
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	void* data; int w, h;
	il_readImg(L"checker.png", &data, &w, &h);
	glGenTextures(1, &tex_walls);
	glBindTexture(GL_TEXTURE_2D, tex_walls);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	delete data;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void xoy_foor(float len, float tex_repeat, int subdivision)
{
	GLfloat color[]={.8f, 0.8f, 0.8f, 1};
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glScalef(len, len, 1);
	glNormal3f(0,0,1);
	float u = 1.0f/subdivision;
	float tu = tex_repeat/subdivision;
	for(int i=0; i<subdivision; ++i)
		for(int j=0; j<subdivision; ++j){
			float ox = u*i, oy = u*j;
			float tox = tu*i, toy = tu*j;
			glBegin(GL_POLYGON);
			glTexCoord2f(tox   , toy);		glVertex3f(ox  , oy  , 0);
			glTexCoord2f(tox+tu, toy);		glVertex3f(ox+u, oy  , 0);
			glTexCoord2f(tox+tu, toy+tu);	glVertex3f(ox+u, oy+u, 0);
			glTexCoord2f(tox   , toy+tu);	glVertex3f(ox  , oy+u, 0);
			glEnd();
		}
		glPopMatrix();
}

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

// 世界，地板
void draw_world()
{
	//glStaff::xyz_frame(2,2,2,false);
	GLboolean _tex = glIsEnabled(GL_TEXTURE_2D);
	if(!_tex) glEnable(GL_TEXTURE_2D);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix(); glRotatef(90, -1, 0, 0); glTranslatef(-10, -10, 0);
		xoy_foor(20, 3, 100);
	glPopMatrix();
	if(!_tex) glDisable(GL_TEXTURE_2D);
}

// 模型
void draw_model()
{
	GLfloat _ca[4], _cd[4];
	glGetMaterialfv(GL_FRONT, GL_AMBIENT, _ca);
	glGetMaterialfv(GL_FRONT, GL_DIFFUSE, _cd);
	GLfloat c[4]; c[3]=1;
	glMatrixMode(GL_MODELVIEW);
	for(size_t i=0; i<meshes.size(); ++i){
		if(i==0){c[0]=0.0f; c[1]=0.5f; c[2]=0.0f;}
		if(i==1){c[0]=1.0f; c[1]=0.2f; c[2]=0.2f;}
		if(i==2){c[0]=0.2f; c[1]=0.2f; c[2]=1.0f;}
		if(i==3){c[0]=0.8f; c[1]=0.8f; c[2]=0.8f;}
		if(glIsEnabled(GL_LIGHTING)){
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, c);
		}else{
			glColor4fv(c);
		}
		glPushMatrix(); glMultMatrixf(&meshes_mat[i][0][0]);
			vcg_drawMesh(*meshes[i]);
		glPopMatrix();
	}
	glMaterialfv(GL_FRONT, GL_AMBIENT, _ca);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, _cd);
}

// 构造、绘制 Shadow Volume
void draw_model_volumes(GLMesh& mesh, glm::vec4& lpos, bool light=false)
{
	assert(mesh.FN()==mesh.face.size());
	assert(mesh.EN()==mesh.edge.size());
	// "可能轮廓边"挤出四边形
	for(int i=0; i<mesh.EN(); ++i){
		GLMesh::EdgeType& e = mesh.edge[i];
		GLMesh::FaceType* fa = e.EFp(); // fa,fb 为边 e 邻接的两个面
		GLMesh::FaceType* fb = fa->FFp(e.EFi());
		glm::vec3 l0 = lpos.w==0 ? glm::vec3(lpos) :
			glm::vec3(lpos)/lpos.w-vcg_to_glm(e.V(0)->P());
		int sa = glm::dot(l0, vcg_to_glm(fa->N()))>=0 ? 1 : -1; // 面对还是背对光源
		int sb = glm::dot(l0, vcg_to_glm(fb->N()))>=0 ? 1 : -1;
		if( sa*sb < 0 ){ // 一个面面对，一个面背对光源，“可能轮廓边”
			GLMesh::VertexType* va = fa->V(e.EFi());
			GLMesh::VertexType* vb = fa->V((e.EFi()+1)%3);
			if(sa<0) std::swap(va, vb); // 确定顶点顺序，是最终四边形环绕方向向外
			glm::vec4 e_far[2]; // 挤出的2个点，到无穷远
			e_far[0] = glm::vec4(
				va->P().X()*lpos.w-lpos.x,
				va->P().Y()*lpos.w-lpos.y,
				va->P().Z()*lpos.w-lpos.z, 0 );
			e_far[1] = glm::vec4(
				vb->P().X()*lpos.w-lpos.x,
				vb->P().Y()*lpos.w-lpos.y,
				vb->P().Z()*lpos.w-lpos.z, 0 );
			glBegin(GL_POLYGON); // 边挤出（extrude）的四边形
				if(light) glNormal3fv( &glm::normalize(glm::cross(
					glm::vec3(e_far[0]), vcg_to_glm(vb->P()-va->P()) ))[0] );
				glVertex3fv(&va->P()[0]);
				glVertex4fv(&e_far[0][0]);
				glVertex4fv(&e_far[1][0]);
				glVertex3fv(&vb->P()[0]);
			glEnd();
		}
	}
	// near cap，面对光源的三角形，// far cap，背对光源的三角形
	glBegin(GL_TRIANGLES);
	for(int i=0; i<mesh.FN(); ++i){
		GLMesh::FaceType& f = mesh.face[i];
		glm::vec3 l0 = lpos.w==0 ? glm::vec3(lpos) :
			glm::vec3(lpos)/lpos.w-vcg_to_glm(f.V(0)->P());
		int s = glm::dot(l0, vcg_to_glm(f.N()))>=0 ? 1 : -1; // 面对还是背对光源
		if( s>0 ){ // near cap
			if(light) glNormal3fv(&f.N()[0]);
			glVertex3fv(&f.V(0)->P()[0]);
			glVertex3fv(&f.V(1)->P()[0]);
			glVertex3fv(&f.V(2)->P()[0]);
		}else{ // far cap
			glm::vec4 v_far[3]; // 挤出的3个点，到无穷远
			for(int k=0; k<3; ++k){
				v_far[k] = glm::vec4(
					f.V(k)->P().X()*lpos.w-lpos.x,
					f.V(k)->P().Y()*lpos.w-lpos.y,
					f.V(k)->P().Z()*lpos.w-lpos.z, 0 );
			}
			glVertex4fv(&v_far[0][0]); // 原三角形本身就是背对光源的，所以是 012 的顺序
			glVertex4fv(&v_far[1][0]);
			glVertex4fv(&v_far[2][0]);
		}
	}
	glEnd();

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

	if(dis_volumes_modeline){
		GLint _modes[2]; glGetIntegerv(GL_POLYGON_MODE, _modes);
		GLboolean _li = glIsEnabled(GL_LIGHTING);
		GLfloat _lw; glGetFloatv(GL_LINE_WIDTH, &_lw);
		GLfloat _c[4]; glGetFloatv(GL_CURRENT_COLOR, _c);
		if(_li) glDisable(GL_LIGHTING);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glLineWidth(2);
		GLfloat c[4]; c[3]=1;
		glMatrixMode(GL_MODELVIEW);
		for(size_t i=0; i<meshes.size(); ++i){
			if(i==0){c[0]=0.0f; c[1]=0.8f; c[2]=0.0f;}
			if(i==1){c[0]=1.0f; c[1]=0.2f; c[2]=0.2f;}
			if(i==2){c[0]=0.2f; c[1]=0.2f; c[2]=1.0f;}
			if(i==3){c[0]=0.8f; c[1]=0.8f; c[2]=0.8f;}
			glColor4fv(c);
			glPushMatrix(); glMultMatrixf(&meshes_mat[i][0][0]);
				vcg_drawSilhEdges(*meshes[i], glm::affineInverse(mat_model*meshes_mat[i])*light_pos);
			glPopMatrix();
		}
		glLineWidth(_lw);

		glColor3f(0,0,0);

		for(size_t i=0; i<meshes.size(); ++i){
			glPushMatrix(); glMultMatrixf(&meshes_mat[i][0][0]);
			draw_model_volumes(*meshes[i],
				glm::affineInverse(mat_model*meshes_mat[i])*light_pos, true);
			glPopMatrix();
		}

		glPolygonMode(GL_FRONT, _modes[0]);
		glPolygonMode(GL_BACK, _modes[1]);
		if(_li) glEnable(GL_LIGHTING);
		glLineWidth(_lw); glColor4fv(_c);
	}else{
		draw_model();
		for(size_t i=0; i<meshes.size(); ++i){
			glPushMatrix(); glMultMatrixf(&meshes_mat[i][0][0]);
			draw_model_volumes(*meshes[i],
				glm::affineInverse(mat_model*meshes_mat[i])*light_pos, true);
			glPopMatrix();
		}
	}

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

	//glm::mat4 mat_pj = glStaff::get_mat_projection(); //将摄像机远裁剪面设置于无穷远处
	//mat_pj[2][2] = -1; mat_pj[3][2] = -2*1.0f;
	//glStaff::set_mat_projection(mat_pj);

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
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); glDepthMask(GL_FALSE);
	glStencilMask(~0); glEnable(GL_STENCIL_TEST); glStencilFunc(GL_ALWAYS, 0, ~0);
	GLboolean _dc = glIsEnabled(GL_DEPTH_CLAMP); if(!_dc) glEnable(GL_DEPTH_CLAMP);
	// 正面加1，背面减1
	glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_INCR_WRAP, GL_KEEP); // 改进后
	glStencilOpSeparate(GL_BACK, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
	glMatrixMode(GL_MODELVIEW); glLoadMatrixf(&mat_view[0][0]); glMultMatrixf(&mat_model[0][0]);
	for(size_t i=0; i<meshes.size(); ++i){
		glPushMatrix(); glMultMatrixf(&meshes_mat[i][0][0]);
		draw_model_volumes(*meshes[i], glm::affineInverse(mat_model*meshes_mat[i])*light_pos);
		glPopMatrix();
	}
	// 恢复状态
	if(_li) glEnable(GL_LIGHTING);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); glDepthMask(GL_TRUE);
	glDisable(GL_STENCIL_TEST); glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	if(!_dc) glDisable(GL_DEPTH_CLAMP);

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
	float*		   pixs = new float[w*h];
	glReadPixels(0,0, w,h, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, data);
	long long m=0, n=0; for(int i=0; i<w*h; ++i)if(data[i]){ m+=data[i]; ++n; }
	m /= n; float f = 0.5f/m; for(int i=0; i<w*h; ++i) pixs[i] = f * data[i];
	il_saveImgDep(L"stencil.png", pixs, w, h);
	delete[] data; delete[] pixs;
	std::cout << "stencil buffer saved" << '\n';
}

void key_t()
{
	dis_volumes = !dis_volumes;
}

void key_r()
{
	dis_volumes_modeline = !dis_volumes_modeline;
}

int main(void) 
{
	meshes.push_back(new GLMesh()); meshes_mat.push_back(glm::translate(glm::vec3( 0,3, 2))*glm::rotate(glm::radians(45.0f),glm::vec3(-1,0,0)));
	vcg_loadFromPLY(*meshes.back(), "monkey.ply");
	meshes.push_back(new GLMesh()); meshes_mat.push_back(glm::translate(glm::vec3(-2,2,-1)));
	vcg_loadFromPLY(*meshes.back(), "torus.ply");
	meshes.push_back(new GLMesh()); meshes_mat.push_back(glm::translate(glm::vec3( 2,2,-1)));
	vcg_loadFromPLY(*meshes.back(), "cube.ply");
	meshes.push_back(new GLMesh()); meshes_mat.push_back(glm::translate(glm::vec3( -4,9,4)));
	vcg_loadFromPLY(*meshes.back(), "cube.ply");
	
	//vcg::Matrix44f mat(&glm::scale(glm::vec3(1.7f, 1.7f, 1.7f))[0][0]);
	//vcg::Matrix44f mat(&glm::rotate(glm::radians(90.0f), glm::vec3(1,0,0))[0][0]);
	//vcg::tri::UpdatePosition<GLMesh>::Matrix(*meshes[1], mat, true); // 更新顶点位置，并更新法向量
	//vcg::tri::io::ExporterPLY<GLMesh>::Save(*meshes[1], "file_to_save.ply");

	glStaff::init_win(1000, 800,
		"OpenGL Shadow    - cnblogs.com/liangliangh", "C:\\Windows\\Fonts\\msyh.ttf");
	glStaff::init_gl();

	tex_init();

	glStaff::set_mat_view(
		glm::lookAt( glm::vec3(-4, 12, 12), glm::vec3(0,0,0), glm::vec3(0,1,0) ) );
	glStaff::set_mat_model( glm::translate( glm::vec3(0,0,0) ) );
	glStaff::load_mat_view("matrix_view_zfail");

	light_pos = glm::vec4(-2,8,2, 1);

	glStaff::add_key_callback('P', key_p, L"改变光源位置");
	glStaff::add_key_callback('U', key_u, L"保存模板缓冲区");
	glStaff::add_key_callback('T', key_t, L"切换显示阴影体");
	glStaff::add_key_callback('R', key_r, L"阴影体多边形模式");

	//GLfloat c[4]={0.5,0.5,0.5, 1}; glLightModelfv(GL_LIGHT_MODEL_AMBIENT, c); 
	glStaff::renderLoop(draw);

	// deallocate
	for(size_t i=0; i<meshes.size(); ++i){
		delete meshes[i];
	}

}
