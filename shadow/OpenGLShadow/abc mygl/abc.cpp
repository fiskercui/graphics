/*
 * http://www.cnblogs.com/liangliangh/
 --------------------------------------------------------------------------------*/

//#include "gl_staff.h"
//#include "bt_inc.h"
//#include "vcg_inc.h"
//#include <iostream>

#include "stdafx.h"


GLMesh mesh, mesh2;


void draw_world() { glStaff::xyz_frame(2,2,2,true); }
void draw_model() { vcg_drawMesh(mesh); }

void draw(const glm::mat4& mat_model, const glm::mat4& mat_view)
{
	double t = omp_get_wtime();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&mat_view[0][0]);
	draw_world();
	glMatrixMode(GL_MODELVIEW);
	glMultMatrixf(&mat_model[0][0]);
	draw_model();
	t = omp_get_wtime()-t;
	char st[50]; sprintf(st, "draw time (ms) :  %.2lf", t*1000);
	glStaff::text_upperLeft(st, 1);
}

void key_p() { std::cout << "key p pressed\n"; }

int main(void)
{
	//double tt = omp_get_wtime();
	//vcg_loadFromPly(mesh, "hand-smple.ply");
	//std::cout << "PLY time: " << omp_get_wtime()-tt << '\n';

	//vcg_loadFromPly(mesh2, "hand-smple.ply");
	//vcg::tri::Smooth<GLMesh>::VertexNormalLaplacian(mesh2, 5);
	//for(int i=0; i<mesh2.VN(); ++i){
	//	GLMesh::VertexType& v = mesh2.vert[i];
	//	for(int k=0; k<3; ++k) v.P()[k] += v.N()[k]*(-0.015f);
	//}
	//vcg::tri::io::ExporterPLY<GLMesh>::Save(mesh2, "out.ply");

	vcg_loadFromPLY(mesh, "hand-smple.ply");
    
	glStaff::init_win(800, 800, "OpenGL", "C:\\Windows\\Fonts\\msyh.ttf");
	glStaff::init_gl(); // have to be called after glStaff::init_win

	glStaff::set_mat_view(
		glm::lookAt( glm::vec3(5,5,5), glm::vec3(1,1,1), glm::vec3(0,1,0) ) );
	glStaff::set_mat_model(
		glm::translate( glm::vec3(1,1,1) ) );
	glStaff::add_key_callback('P', key_p, L"print");

	glStaff::renderLoop(draw);
}
