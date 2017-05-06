// testocv.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "common.h"

void createAlphaMat(Mat &mat)
{
	for (int i = 0; i < mat.rows; ++i) {
		for (int j = 0; j < mat.cols; ++j) {
			Vec4b&rgba = mat.at<Vec4b>(i, j);
			rgba[0] = UCHAR_MAX;
			rgba[1] = saturate_cast<uchar>((float(mat.cols - j)) / ((float)mat.cols) *UCHAR_MAX);
			rgba[2] = saturate_cast<uchar>((float(mat.rows - i)) / ((float)mat.rows) *UCHAR_MAX);
			rgba[3] = saturate_cast<uchar>(0.5 * (rgba[1] + rgba[2]));
		}
	}
}

void testAlpha()
{
	Mat mat(480, 640, CV_8UC4);
	createAlphaMat(mat);

	vector<int>compression_params;
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(9);
	try {
		imwrite("out-alpha.png", mat, compression_params);
	}
	catch (runtime_error& ex) {
		fprintf(stderr, "ͼ��ת����PNG��ʽ��������%s\n", ex.what());
	}
}

void testShowImage()
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
}

//-----------------------------------������˵����----------------------------------------------  
//  ��������:����OpenCV���Ž̳�֮����ͼ������룬��ʾ����� һվʽ��ȫ���� ��������Դ��  
// VS2010��   OpenCV�汾��2.4.8  
//      2014��3��5�� Create by ǳī  
//  ������ ͼ������룬��ʾ����� һվʽ����   ����Դ��  
//  ͼƬ�زĳ�����dota2ԭ��ʥ�ô̿� dota2 logo  ��������  
//------------------------------------------------------------------------------------------------  

//
//#include<opencv2/core/core.hpp>  
//#include<opencv2/highgui/highgui.hpp>  
//
//using namespace cv;
//
//
int loadImage()
{


						   //-----------------------------------����������ͼ���ϡ�--------------------------------------  
						   //     ��������������ͼ����  
						   //-----------------------------------------------------------------------------------------------  
						   //����ͼƬ  
	Mat image = imread("logo_s.png", 199);
	Mat logo = imread("main_bg3.png");

	//���������ʾ  
	namedWindow("��2��ԭ��ͼ");
	imshow("��2��ԭ��ͼ", image);

	namedWindow("��3��logoͼ");
	imshow("��3��logoͼ", logo);

	//����һ��Mat���ͣ����ڴ�ţ�ͼ���ROI  
	Mat imageROI;
	//����һ  
	imageROI = image;
	//������  
	//imageROI=image(Range(350,350+logo.rows),Range(800,800+logo.cols));  

	//��logo�ӵ�ԭͼ��  
	addWeighted(imageROI, 0.5, logo, 0.3, 0., imageROI);

	//��ʾ���  
	namedWindow("��4��ԭ��+logoͼ");
	imshow("��4��ԭ��+logoͼ", image);

	//-----------------------------------������ͼ��������--------------------------------------  
	//     ��������һ��Matͼ�������ͼ���ļ�  
	//-----------------------------------------------------------------------------------------------  
	//���һ��jpgͼƬ������Ŀ¼��  
	imwrite("��ϲ����dota2 byǳī.jpg", image);

	waitKey();

	return 0;
}
void addRoi()
{
	Mat logo = imread("logo_s.png");

	Mat image = imread("main_bg3.png", 199);

	//����һ��Mat���ͣ����ڴ�ţ�ͼ���ROI  
	Mat imageROI;
	//����һ  
	imageROI = image(Rect(1024, 1024, logo.cols, logo.rows));
	//������  
	//imageROI=image(Range(350,350+logo.rows),Range(800,800+logo.cols));  

	//��logo�ӵ�ԭͼ��  
	addWeighted(imageROI, 0.5, logo, 0.3, 0., imageROI);

}
int main()
{
	//testAlpha();
	//testShowImage();
	//addRoi();
	loadImage();
    return 0;
}

