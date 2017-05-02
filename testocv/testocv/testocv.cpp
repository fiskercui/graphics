// testocv.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include <opencv2/opencv.hpp>
#pragma comment (lib, "opencv_world320d")





int main()
{

	const char *pstrImageName = "bg.jpg";
	const char *pstrWindowsTitle = "O��һ������";

	//���ļ��ж�ȡͼ��  
	IplImage *pImage = cvLoadImage(pstrImageName, CV_LOAD_IMAGE_UNCHANGED);

	//��������  
	cvNamedWindow(pstrWindowsTitle, CV_WINDOW_AUTOSIZE);

	//��ָ����������ʾͼ��  
	cvShowImage(pstrWindowsTitle, pImage);

	//�ȴ������¼�  
	cvWaitKey();

	cvDestroyWindow(pstrWindowsTitle);
	cvReleaseImage(&pImage);



    return 0;
}

