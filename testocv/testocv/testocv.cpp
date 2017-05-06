// testocv.cpp : 定义控制台应用程序的入口点。
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
		fprintf(stderr, "图像转换成PNG格式发生错误：%s\n", ex.what());
	}
}

void testShowImage()
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
}

//-----------------------------------【程序说明】----------------------------------------------  
//  程序名称:：【OpenCV入门教程之三】图像的载入，显示与输出 一站式完全解析 博文配套源码  
// VS2010版   OpenCV版本：2.4.8  
//      2014年3月5日 Create by 浅墨  
//  描述： 图像的载入，显示与输出 一站式剖析   配套源码  
//  图片素材出处：dota2原画圣堂刺客 dota2 logo  动漫人物  
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


						   //-----------------------------------【二、初级图像混合】--------------------------------------  
						   //     描述：二、初级图像混合  
						   //-----------------------------------------------------------------------------------------------  
						   //载入图片  
	Mat image = imread("logo_s.png", 199);
	Mat logo = imread("main_bg3.png");

	//载入后先显示  
	namedWindow("【2】原画图");
	imshow("【2】原画图", image);

	namedWindow("【3】logo图");
	imshow("【3】logo图", logo);

	//定义一个Mat类型，用于存放，图像的ROI  
	Mat imageROI;
	//方法一  
	imageROI = image;
	//方法二  
	//imageROI=image(Range(350,350+logo.rows),Range(800,800+logo.cols));  

	//将logo加到原图上  
	addWeighted(imageROI, 0.5, logo, 0.3, 0., imageROI);

	//显示结果  
	namedWindow("【4】原画+logo图");
	imshow("【4】原画+logo图", image);

	//-----------------------------------【三、图像的输出】--------------------------------------  
	//     描述：将一个Mat图像输出到图像文件  
	//-----------------------------------------------------------------------------------------------  
	//输出一张jpg图片到工程目录下  
	imwrite("我喜欢打dota2 by浅墨.jpg", image);

	waitKey();

	return 0;
}
void addRoi()
{
	Mat logo = imread("logo_s.png");

	Mat image = imread("main_bg3.png", 199);

	//定义一个Mat类型，用于存放，图像的ROI  
	Mat imageROI;
	//方法一  
	imageROI = image(Rect(1024, 1024, logo.cols, logo.rows));
	//方法二  
	//imageROI=image(Range(350,350+logo.rows),Range(800,800+logo.cols));  

	//将logo加到原图上  
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

