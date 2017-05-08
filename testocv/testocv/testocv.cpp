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
int testLineBlend()
{
	double alpha = 0.5; double beta; double input;

	Mat src1, src2, dst;

	/// Ask the user enter alpha
	std::cout << " Simple Linear Blender " << std::endl;
	std::cout << "-----------------------" << std::endl;
	std::cout << "* Enter alpha [0-1]: ";
	std::cin >> input;

	// We use the alpha provided by the user iff it is between 0 and 1
	if (alpha >= 0 && alpha <= 1)
	{
		alpha = input;
	}

	/// Read image ( same size, same type )
	src1 = imread("girl.jpg");
	src2 = imread("girl3.jpg");

	if (!src1.data) { printf("Error loading src1 \n"); return -1; }
	if (!src2.data) { printf("Error loading src2 \n"); return -1; }

	/// Create Windows
	namedWindow("Linear Blend", 1);

	beta = (1.0 - alpha);
	addWeighted(src1, alpha, src2, beta, 0.0, dst);

	imshow("Linear Blend", dst);

	waitKey(0);
	return 0;
}


bool testMultiChannelBlending()
{
	Mat srcImage;
	Mat logoImage;
	vector<Mat>channels;
	Mat  imageBlueChannel;


	//【1】读入图片  
	logoImage = imread("girl.jpg", 0);
	srcImage = imread("main_bg3.png", IMREAD_UNCHANGED);


	//【2】把一个3通道图像转换成3个单通道图像  
	split(srcImage, channels);//分离色彩通道  

	imwrite("blue.png", channels[0]);
	imwrite("green.png", channels[1]);
	imwrite("red.png", channels[2]);

	if (channels.size() >= 4)
	{
		imwrite("alpha.png", channels[3]);
	}

	//imwrite("alpha.png", channels[3]);


	//						  //【3】将原图的蓝色通道引用返回给imageBlueChannel，注意是引用，相当于两者等价，修改其中一个另一个跟着变  
	//imageBlueChannel = channels.at(0);
	////【4】将原图的蓝色通道的（500,250）坐标处右下方的一块区域和logo图进行加权操作，将得到的混合结果存到imageBlueChannel中  
	//
	//imageBlueChannel(Rect(500, 250, logoImage.cols, logoImage.rows));

	//addWeighted(imageBlueChannel(Rect(500, 250, logoImage.cols, logoImage.rows)), 1.0,
	//	logoImage, 0.5, 0, imageBlueChannel(Rect(500, 250, logoImage.cols, logoImage.rows)));

	////【5】将三个单通道重新合并成一个三通道  
	//merge(channels, srcImage);

	////【6】显示效果图  
	//namedWindow("<1>");
	//imshow("<1>", srcImage);


	////=================【绿色通道部分】=================  
	////     描述：多通道混合-绿色分量部分  
	////============================================  

	////【0】定义相关变量  
	//Mat  imageGreenChannel;

	////【1】重新读入图片  
	//logoImage = imread("dota_logo.jpg", 0);
	//srcImage = imread("dota_jugg.jpg");

	//if (!logoImage.data) { printf("Oh，no，读取logoImage错误~！\n"); return false; }
	//if (!srcImage.data) { printf("Oh，no，读取srcImage错误~！\n"); return false; }

	////【2】将一个三通道图像转换成三个单通道图像  
	//split(srcImage, channels);//分离色彩通道  

	//						  //【3】将原图的绿色通道的引用返回给imageBlueChannel，注意是引用，相当于两者等价，修改其中一个另一个跟着变  
	//imageGreenChannel = channels.at(1);
	////【4】将原图的绿色通道的（500,250）坐标处右下方的一块区域和logo图进行加权操作，将得到的混合结果存到imageGreenChannel中  
	//addWeighted(imageGreenChannel(Rect(500, 250, logoImage.cols, logoImage.rows)), 1.0,
	//	logoImage, 0.5, 0., imageGreenChannel(Rect(500, 250, logoImage.cols, logoImage.rows)));

	////【5】将三个独立的单通道重新合并成一个三通道  
	//merge(channels, srcImage);

	////【6】显示效果图  
	//namedWindow("<2>游戏原画+logo绿色通道 by浅墨");
	//imshow("<2>游戏原画+logo绿色通道 by浅墨", srcImage);

	return true;
}


int main()
{
	//testAlpha();
	//testShowImage();
	//addRoi();
	//loadImage();
	//testLineBlend();

	testMultiChannelBlending();
    return 0;
}

