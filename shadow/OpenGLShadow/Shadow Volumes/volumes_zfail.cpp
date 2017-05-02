/*
 * http://www.cnblogs.com/liangliangh/
 --------------------------------------------------------------------------------*/

#include "stdafx.h"

GLuint tex_walls;
glm::vec4 light_pos; // ��������
bool light_rotate=false;
bool dis_volumes=false;
int  dis_volumes_modeline=false;

std::vector<GLMesh*> meshes;
std::vector<glm::mat4> meshes_mat;

void tex_init()
{
	glActiveTexture(GL_TEXTURE0); // ��ͨ��ͼ
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

void dlight(float size=0.1f) // �ڵ��ϻ���һ����
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

// ���磬�ذ�
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

// ģ��
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

// ���졢���� Shadow Volume
void draw_model_volumes(GLMesh& mesh, glm::vec4& lpos, bool light=false)
{
	assert(mesh.FN()==mesh.face.size());
	assert(mesh.EN()==mesh.edge.size());
	// "����������"�����ı���
	for(int i=0; i<mesh.EN(); ++i){
		GLMesh::EdgeType& e = mesh.edge[i];
		GLMesh::FaceType* fa = e.EFp(); // fa,fb Ϊ�� e �ڽӵ�������
		GLMesh::FaceType* fb = fa->FFp(e.EFi());
		glm::vec3 l0 = lpos.w==0 ? glm::vec3(lpos) :
			glm::vec3(lpos)/lpos.w-vcg_to_glm(e.V(0)->P());
		int sa = glm::dot(l0, vcg_to_glm(fa->N()))>=0 ? 1 : -1; // ��Ի��Ǳ��Թ�Դ
		int sb = glm::dot(l0, vcg_to_glm(fb->N()))>=0 ? 1 : -1;
		if( sa*sb < 0 ){ // һ������ԣ�һ���汳�Թ�Դ�������������ߡ�
			GLMesh::VertexType* va = fa->V(e.EFi());
			GLMesh::VertexType* vb = fa->V((e.EFi()+1)%3);
			if(sa<0) std::swap(va, vb); // ȷ������˳���������ı��λ��Ʒ�������
			glm::vec4 e_far[2]; // ������2���㣬������Զ
			e_far[0] = glm::vec4(
				va->P().X()*lpos.w-lpos.x,
				va->P().Y()*lpos.w-lpos.y,
				va->P().Z()*lpos.w-lpos.z, 0 );
			e_far[1] = glm::vec4(
				vb->P().X()*lpos.w-lpos.x,
				vb->P().Y()*lpos.w-lpos.y,
				vb->P().Z()*lpos.w-lpos.z, 0 );
			glBegin(GL_POLYGON); // �߼�����extrude�����ı���
				if(light) glNormal3fv( &glm::normalize(glm::cross(
					glm::vec3(e_far[0]), vcg_to_glm(vb->P()-va->P()) ))[0] );
				glVertex3fv(&va->P()[0]);
				glVertex4fv(&e_far[0][0]);
				glVertex4fv(&e_far[1][0]);
				glVertex3fv(&vb->P()[0]);
			glEnd();
		}
	}
	// near cap����Թ�Դ�������Σ�// far cap�����Թ�Դ��������
	glBegin(GL_TRIANGLES);
	for(int i=0; i<mesh.FN(); ++i){
		GLMesh::FaceType& f = mesh.face[i];
		glm::vec3 l0 = lpos.w==0 ? glm::vec3(lpos) :
			glm::vec3(lpos)/lpos.w-vcg_to_glm(f.V(0)->P());
		int s = glm::dot(l0, vcg_to_glm(f.N()))>=0 ? 1 : -1; // ��Ի��Ǳ��Թ�Դ
		if( s>0 ){ // near cap
			if(light) glNormal3fv(&f.N()[0]);
			glVertex3fv(&f.V(0)->P()[0]);
			glVertex3fv(&f.V(1)->P()[0]);
			glVertex3fv(&f.V(2)->P()[0]);
		}else{ // far cap
			glm::vec4 v_far[3]; // ������3���㣬������Զ
			for(int k=0; k<3; ++k){
				v_far[k] = glm::vec4(
					f.V(k)->P().X()*lpos.w-lpos.x,
					f.V(k)->P().Y()*lpos.w-lpos.y,
					f.V(k)->P().Z()*lpos.w-lpos.z, 0 );
			}
			glVertex4fv(&v_far[0][0]); // ԭ�����α�����Ǳ��Թ�Դ�ģ������� 012 ��˳��
			glVertex4fv(&v_far[1][0]);
			glVertex4fv(&v_far[2][0]);
		}
	}
	glEnd();

}

// �۲� Shadow Volumes����Դλ���Ӿ�����ԭ��
void draw2(const glm::mat4& mat_model, const glm::mat4& mat_view)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	GLfloat pl[4]={0,0,0, 1}; glLightfv(GL_LIGHT0, GL_POSITION, pl); // λ��ʽ��Դ
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

	// ------------------------------------------ ���������������ģ�建��
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//glm::mat4 mat_pj = glStaff::get_mat_projection(); //�������Զ�ü�������������Զ��
	//mat_pj[2][2] = -1; mat_pj[3][2] = -2*1.0f;
	//glStaff::set_mat_projection(mat_pj);

	// ------------------------------------------ ��1�飬��Ⱦ�����⣬���ֵ
	// �رչ�Դ���򿪻�����
	GLboolean _li0 = glIsEnabled(GL_LIGHT0); if(_li0) glDisable(GL_LIGHT0);
	glMatrixMode(GL_MODELVIEW); glLoadMatrixf(&mat_view[0][0]);
		draw_world();
	glMultMatrixf(&mat_model[0][0]);
		draw_model();
	if(_li0) glEnable(GL_LIGHT0);

	// ------------------------------------------ ��2�飬��Ⱦģ��ֵ
	// ����Ҫ���գ���������ɫ����Ȼ���
	GLboolean _li = glIsEnabled(GL_LIGHTING); if(_li) glDisable(GL_LIGHTING);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); glDepthMask(GL_FALSE);
	glStencilMask(~0); glEnable(GL_STENCIL_TEST); glStencilFunc(GL_ALWAYS, 0, ~0);
	GLboolean _dc = glIsEnabled(GL_DEPTH_CLAMP); if(!_dc) glEnable(GL_DEPTH_CLAMP);
	// �����1�������1
	glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_INCR_WRAP, GL_KEEP); // �Ľ���
	glStencilOpSeparate(GL_BACK, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
	glMatrixMode(GL_MODELVIEW); glLoadMatrixf(&mat_view[0][0]); glMultMatrixf(&mat_model[0][0]);
	for(size_t i=0; i<meshes.size(); ++i){
		glPushMatrix(); glMultMatrixf(&meshes_mat[i][0][0]);
		draw_model_volumes(*meshes[i], glm::affineInverse(mat_model*meshes_mat[i])*light_pos);
		glPopMatrix();
	}
	// �ָ�״̬
	if(_li) glEnable(GL_LIGHTING);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); glDepthMask(GL_TRUE);
	glDisable(GL_STENCIL_TEST); glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	if(!_dc) glDisable(GL_DEPTH_CLAMP);

	// ------------------------------------------ ��3�飬��Ⱦ��Դ���գ�����ģ��ֵ�ж���Ӱ
	// �رջ����⣬�򿪹�Դ
	GLfloat _lia[4]; glGetFloatv(GL_LIGHT_MODEL_AMBIENT, _lia);
	GLfloat ca[4]={0}; glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ca);
	// ģ�����Ϊ������0ͨ���� ��Ȳ���Ϊ�����ͨ������ɫ���Ϊֱ���ۼ�
	glEnable(GL_STENCIL_TEST); glStencilFunc(GL_EQUAL, 0, ~0);
	glDepthFunc(GL_EQUAL); glBlendFunc(GL_ONE, GL_ONE);

	glMatrixMode(GL_MODELVIEW); glLoadMatrixf(&mat_view[0][0]);
	glLightfv(GL_LIGHT0, GL_POSITION, &light_pos[0]); // λ��ʽ��Դ
		draw_world();
	glMultMatrixf(&mat_model[0][0]);
		draw_model(); 
	// �ָ�״̬
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, _lia);
	glDisable(GL_STENCIL_TEST); glStencilFunc(GL_ALWAYS, 0, ~0);
	glDepthFunc(GL_LESS); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// �ڹ�Դ������һ����ɫ����
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
	//vcg::tri::UpdatePosition<GLMesh>::Matrix(*meshes[1], mat, true); // ���¶���λ�ã������·�����
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

	glStaff::add_key_callback('P', key_p, L"�ı��Դλ��");
	glStaff::add_key_callback('U', key_u, L"����ģ�建����");
	glStaff::add_key_callback('T', key_t, L"�л���ʾ��Ӱ��");
	glStaff::add_key_callback('R', key_r, L"��Ӱ������ģʽ");

	//GLfloat c[4]={0.5,0.5,0.5, 1}; glLightModelfv(GL_LIGHT_MODEL_AMBIENT, c); 
	glStaff::renderLoop(draw);

	// deallocate
	for(size_t i=0; i<meshes.size(); ++i){
		delete meshes[i];
	}

}
