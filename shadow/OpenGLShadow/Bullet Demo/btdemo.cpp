/*
 * http://www.cnblogs.com/liangliangh/
 --------------------------------------------------------------------------------*/

#include "gl_staff.h"
#include <iostream>


void bt_start();
void bt_end();
void bt_rundraw(bool run);
bool bt_run;


void draw_world() { glStaff::xyz_frame(2,2,2,true); }
void draw_model() { bt_rundraw(bt_run); }

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
	wchar_t st[50]; swprintf(st, L"draw time (ms) :  %.2lf", t*1000);
	glStaff::text_upperLeft(st, 1);
}

void key_r() { bt_run=!bt_run; }

int main(void)
{
	glStaff::init_win(800, 800, "OpenGL", "C:\\Windows\\Fonts\\msyh.ttf");
	glStaff::init_gl(); // have to be called after glStaff::init_win

	glStaff::set_mat_view(
		glm::lookAt( glm::vec3(100,100,100), glm::vec3(1,1,1), glm::vec3(0,1,0) ) );

	glStaff::add_key_callback('R', key_r, L"run demo");

	bt_start();
	glStaff::renderLoop(draw);
	bt_end();
}



