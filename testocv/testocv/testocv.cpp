// testocv.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <opencv2/opencv.hpp>
#pragma comment (lib, "opencv_world320d")





int main()
{

	const char *pstrImageName = "bg.jpg";
	const char *pstrWindowsTitle = "O第一个程序";

	//从文件中读取图像  
	IplImage *pImage = cvLoadImage(pstrImageName, CV_LOAD_IMAGE_UNCHANGED);

	//创建窗口  
	cvNamedWindow(pstrWindowsTitle, CV_WINDOW_AUTOSIZE);

	//在指定窗口中显示图像  
	cvShowImage(pstrWindowsTitle, pImage);

	//等待按键事件  
	cvWaitKey();

	cvDestroyWindow(pstrWindowsTitle);
	cvReleaseImage(&pImage);



    return 0;
}

