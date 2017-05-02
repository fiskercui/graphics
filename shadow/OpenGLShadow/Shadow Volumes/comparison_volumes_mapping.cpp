/*
 * http://www.cnblogs.com/liangliangh/
 --------------------------------------------------------------------------------*/

#include "stdafx.h"

GLuint tex_walls, tex_shadow;
GLuint frame_buffer_s;
const int shadow_w=2048, shadow_h=2048;
glm::vec4 light_pos; // 世界坐标
bool light_rotate=false;
int dis_shadow_mode=1;
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

	glActiveTexture(GL_TEXTURE1); // 阴影纹理
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
	glEnable(GL_TEXTURE_GEN_Q);

	glGenTextures(1, &tex_shadow);
	glBindTexture(GL_TEXTURE_2D, tex_shadow);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat c[4] = {1,1,1, 1};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, c);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_w, shadow_h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	// frame buffer object
	glGenFramebuffers(1, &frame_buffer_s);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame_buffer_s);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex_shadow, 0);

	// 恢复状态
	glActiveTexture(GL_TEXTURE0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
	glPushMatrix(); glRotatef(90, -1, 0, 0); glTranslatef(-30, -30, 0);
		xoy_foor(60, 5, 100);
	glPopMatrix();
	if(!_tex) glDisable(GL_TEXTURE_2D);
}

// 模型
void draw_model()
{
	GLfloat _ca[4], _cd[4];
	glGetMaterialfv(GL_FRONT, GL_AMBIENT, _ca);
	glGetMaterialfv(GL_FRONT, GL_DIFFUSE, _cd);
	GLfloat c[4]; c[3]=1; float hsize = 360.0f/meshes.size();
	glMatrixMode(GL_MODELVIEW);
	for(size_t i=0; i<meshes.size(); ++i){
		glStaff::hsl_to_rgb(i*hsize, 1, 0.5f, c);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, c);
		glPushMatrix(); glMultMatrixf(&meshes_mat[i][0][0]);
			vcg_drawMesh(*meshes[i]);
		glPopMatrix();
	}
	glMaterialfv(GL_FRONT, GL_AMBIENT, _ca);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, _cd);
}

// 构造、绘制 Shadow Volume
void draw_model_volumes(GLMesh& mesh, glm::vec4& lpos)
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


void draw0(const glm::mat4& mat_model, const glm::mat4& mat_view)
{
	double tt = omp_get_wtime();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW); glLoadMatrixf(&mat_view[0][0]);
	glLightfv(GL_LIGHT0, GL_POSITION, &light_pos[0]);
		draw_world();
	glMatrixMode(GL_MODELVIEW);	glMultMatrixf(&mat_model[0][0]);
		draw_model();

	// 在光源处绘制一个黄色的球
	glMatrixMode(GL_MODELVIEW); glLoadMatrixf(&mat_view[0][0]);
		dlight(0.2f);

	if(light_rotate)
		light_pos = glm::rotate(glm::radians(1.0f), glm::vec3(0,1,0)) * light_pos;

	tt = omp_get_wtime()-tt;
	char ss[50]; sprintf(ss, "render time (ms): %.2f", tt*1000);
	glStaff::text_upperLeft(ss, 1);
	sprintf(ss, "no Shadow"); glStaff::text_upperLeft(ss);
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
		dlight(0.2f);

	if(light_rotate)
		light_pos = glm::rotate(glm::radians(1.0f), glm::vec3(0,1,0)) * light_pos;

	tt = omp_get_wtime()-tt;
	char ss[50]; sprintf(ss, "render time (ms): %.2f", tt*1000);
	glStaff::text_upperLeft(ss, 1);
	sprintf(ss, "Shadow Volumes"); glStaff::text_upperLeft(ss);
}

void draw2(const glm::mat4& mat_model, const glm::mat4& mat_view)
{
	double tt = omp_get_wtime();

	//-------------------------------- 第1次绘制，生成深度纹理 -------------------------
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame_buffer_s);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glViewport(0, 0, shadow_w, shadow_h);
	//GLenum re = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	glClear(GL_DEPTH_BUFFER_BIT);

	// 不需要光照和纹理
	GLboolean li = glIsEnabled(GL_LIGHTING);
	if(li) glDisable(GL_LIGHTING);
	glActiveTexture(GL_TEXTURE1); glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0); glDisable(GL_TEXTURE_2D);

	glm::mat4 shadow_mat_p = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, 1.0e10f);
	glm::vec3 cen = glm::vec3(glStaff::get_mat_model()*glm::vec4(0,0,0,1));
	glm::mat4 shadow_mat_v = glm::lookAt(glm::vec3(light_pos), cen, glm::vec3(0,1,0));
	glEnable(GL_CULL_FACE); glCullFace(GL_FRONT);
	glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadMatrixf(&shadow_mat_p[0][0]);
	glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadMatrixf(&shadow_mat_v[0][0]);
		draw_world();
	glMultMatrixf(&mat_model[0][0]);
		draw_model();

	glMatrixMode(GL_PROJECTION); glPopMatrix();
	glMatrixMode(GL_MODELVIEW); glPopMatrix();
	glDisable(GL_CULL_FACE); glCullFace(GL_BACK);

	glActiveTexture(GL_TEXTURE1);
	// When the eye planes are specified, the GL will automatically post-multiply them
	// with the inverse of the current modelview matrix.
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&mat_view[0][0]);
	glm::mat4 mat = glm::translate(glm::vec3(0.5f,0.5f,0.5f))
		* glm::scale(glm::vec3(0.5f,0.5f,0.5f)) * shadow_mat_p * shadow_mat_v;
	mat = glm::transpose(mat);
	glTexGenfv(GL_S, GL_EYE_PLANE, &mat[0][0]);
	glTexGenfv(GL_T, GL_EYE_PLANE, &mat[1][0]);
	glTexGenfv(GL_R, GL_EYE_PLANE, &mat[2][0]);
	glTexGenfv(GL_Q, GL_EYE_PLANE, &mat[3][0]);

	// 恢复视口、颜色模板、光照
	glViewport(0, 0, glStaff::get_frame_width(), glStaff::get_frame_height());
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	if(li) glEnable(GL_LIGHTING);

	//-------------------------------- 第2次绘制，绘制场景 ----------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// 1 环境光
	glDisable(GL_LIGHT0);
	glActiveTexture(GL_TEXTURE1); glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0); glEnable(GL_TEXTURE_2D);
	//float gac2[4]={0,0,0,1}; glLightModelfv(GL_LIGHT_MODEL_AMBIENT, gac2); // black

	glMatrixMode(GL_MODELVIEW); glLoadMatrixf(&mat_view[0][0]);
		draw_world();
	glMultMatrixf(&mat_model[0][0]);
		draw_model();

	// 2 点光源
	GLfloat la[4]; glGetFloatv(GL_LIGHT_MODEL_AMBIENT, la);
	float gac[4]={0,0,0,1}; glLightModelfv(GL_LIGHT_MODEL_AMBIENT, gac); // black
	glEnable(GL_LIGHT0);
	glActiveTexture(GL_TEXTURE1); glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0); glEnable(GL_TEXTURE_2D);

	glDepthFunc(GL_EQUAL); glBlendFunc(GL_ONE, GL_ONE);

	glMatrixMode(GL_MODELVIEW); glLoadMatrixf(&mat_view[0][0]);
	glLightfv(GL_LIGHT0, GL_POSITION, &light_pos[0]); // 位置式光源
		draw_world();
	glMultMatrixf(&mat_model[0][0]); glDisable(GL_TEXTURE_2D);
		draw_model();

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, la); // 恢复环境光
	glDepthFunc(GL_LESS); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// 3 绘制光源处的球
	glActiveTexture(GL_TEXTURE1); glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0); glDisable(GL_TEXTURE_2D);
	glMatrixMode(GL_MODELVIEW); glLoadMatrixf(&mat_view[0][0]);
		dlight(0.2f);

	tt = omp_get_wtime()-tt;
	char ss[50]; sprintf(ss, "render time (ms): %.2f", tt*1000);
	glStaff::text_upperLeft(ss, 1);
	sprintf(ss, "Shadow Mapping"); glStaff::text_upperLeft(ss);
}

void draw(const glm::mat4& mat_model, const glm::mat4& mat_view)
{
	if(dis_shadow_mode==0){
		draw0(mat_model, mat_view);
	}
	if(dis_shadow_mode==1){
		draw1(mat_model, mat_view);
	}
	if(dis_shadow_mode==2){
		draw2(mat_model, mat_view);
	}
}

void key_p()
{
	light_rotate = !light_rotate;
}

void key_s()
{
	dis_shadow_mode = (++dis_shadow_mode)%3;
}


int main(void) 
{
#if 0 // 场景1
	GLMesh m; vcg_loadFromPLY(m, "monkey.ply");
	for(int i=0; i<25; ++i){
		meshes.push_back(new GLMesh());
		meshes_mat.push_back(
			glm::translate(glm::vec3((i/5-2)*6-3, 3, (i%5-2)*6+3))*glm::rotate(glm::radians(45.0f),glm::vec3(-1,0,0)));
		vcg::tri::Append<GLMesh,GLMesh>::MeshCopy(*meshes.back(), m, false, true);
	}
	for(int i=0; i<25; ++i){
		meshes.push_back(new GLMesh());
		meshes_mat.push_back(
			glm::translate(glm::vec3((i/5-2)*6, 6, (i%5-2)*6))*glm::rotate(glm::radians(45.0f),glm::vec3(-1,0,0)));
		vcg::tri::Append<GLMesh,GLMesh>::MeshCopy(*meshes.back(), m, false, true);
	}

	glStaff::set_mat_view(
		glm::lookAt( glm::vec3(4, 40, 20), glm::vec3(0,0,0), glm::vec3(0,1,0) ) );
	light_pos = glm::vec4(-5,30,10, 1);
#endif

#if 1 // 场景2
	GLMesh m; vcg_loadFromPLY(m, "cube.ply");
	vcg::Matrix44f mat(&glm::scale(glm::vec3(0.3f, 0.3f, 0.3f))[0][0]);
	vcg::tri::UpdatePosition<GLMesh>::Matrix(m, mat, true); // 更新顶点位置，并更新法向量
	for(int i=0; i<10; ++i)for(int k=0; k<5; ++k){
		meshes.push_back(new GLMesh());
		meshes_mat.push_back(
			glm::translate(glm::vec3((i/20-10)*1.8f, 3+k*2, (i%20-10)*1.8f))
			*glm::rotate(glm::radians(45.0f),glm::vec3(-1,0,0)) );
		vcg::tri::Append<GLMesh,GLMesh>::MeshCopy(*meshes.back(), m, false, true);
	}

	glStaff::set_mat_view(
		glm::lookAt( glm::vec3(20, 50, 60), glm::vec3(0,10,0), glm::vec3(0,1,0) ) );
	light_pos = glm::vec4(-5,36,10, 1);
#endif

	glStaff::init_win(1000, 800,
		"OpenGL Shadow    - cnblogs.com/liangliangh", "C:\\Windows\\Fonts\\msyh.ttf");
	glStaff::init_gl();

	tex_init();

	glStaff::add_key_callback('P', key_p, L"改变光源位置");
	glStaff::add_key_callback('S', key_s, L"切换阴影");

	glStaff::renderLoop(draw);

	// deallocate
	for(size_t i=0; i<meshes.size(); ++i){
		delete meshes[i];
	}

}
