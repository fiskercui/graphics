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



void testSharpen()
{
	Mat myImage = imread("main_bg3.png", IMREAD_UNCHANGED);
	Mat Result;

	CV_Assert(myImage.depth() == CV_8U);  // accept only uchar images   // CV_Assert 是cv定义的断言格式，如果条件不符合，会中断程序，抛出异常  
	const int nChannels = myImage.channels();
	Result.create(myImage.size(), myImage.type());

	for (int j = 1; j < myImage.rows - 1; ++j)                             // 第一行和最后一行无法计算  
	{
		const uchar* previous = myImage.ptr<uchar>(j - 1);      //   上一行数据的指针  
		const uchar* current = myImage.ptr<uchar>(j);        //   当前行数据的指针  
		const uchar* next = myImage.ptr<uchar>(j + 1);       //  下一行数据的指针  

		uchar* output = Result.ptr<uchar>(j);                            // 输出图像当前列数据的指针  

		for (int i = nChannels; i < nChannels*(myImage.cols - 1); ++i)   // 同理，忽略第一列和最后一列像素  
		{
			*output++ = saturate_cast<uchar>(5 * current[i]
				- current[i - nChannels] - current[i + nChannels] - previous[i] - next[i]);         // 计算新值  
		}
	}
	// 将边界设为0  
	Result.row(0).setTo(Scalar(0));
	Result.row(Result.rows - 1).setTo(Scalar(0));
	Result.col(0).setTo(Scalar(0));
	Result.col(Result.cols - 1).setTo(Scalar(0));
	//显示结果  
	namedWindow("锐化的图");
	imshow("锐化的图", Result);
	waitKey();
}


static void help()
{
	printf("\nThis program demonstrated the use of the discrete Fourier transform (dft)\n"
		"The dft of an image is taken and it's power spectrum is displayed.\n"
		"Usage:\n"
		"./dft [image_name -- default lena.jpg]\n");
}

const char* keys =
{
	"{1| |lena.jpg|input image file}"
};


int testDFT()
{
	std::string filename = "main_bg3.png";
	//Mat img = imread(filename.c_str(), CV_LOAD_IMAGE_GRAYSCALE);    //以灰度图像读入  
	Mat img = imread(filename.c_str(), CV_LOAD_IMAGE_GRAYSCALE);    //以灰度图像读入  
	if (img.empty())
	{
		help();
		printf("Cannot read image file: %s\n", filename.c_str());
		return -1;
	}
	int M = getOptimalDFTSize(img.rows);                // 获得最佳DFT尺寸，为2的次方  
	int N = getOptimalDFTSize(img.cols);                //同上  
	Mat padded;
	copyMakeBorder(img, padded, 0, M - img.rows, 0, N - img.cols, BORDER_CONSTANT, Scalar::all(0)); // opencv中的边界扩展函数，提供多种方式扩展  

	Mat mat1 = Mat_<float>(padded);
	Mat mat2 = Mat::zeros(padded.size(), CV_32F);
	Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };          // Mat 数组，第一个为扩展后的图像，一个为空图像，  
	Mat complexImg;
	merge(planes, 2, complexImg);                                                                              // 合并成一个Mat  

	dft(complexImg, complexImg);                                      // FFT变换， dft需要一个2通道的Mat  																									  // compute log(1 + sqrt(Re(DFT(img))**2 + Im(DFT(img))**2))  
	split(complexImg, planes);                                        //分离通道， planes[0] 为实数部分，planes[1]为虚数部分  
	magnitude(planes[0], planes[1], planes[0]);                       // 求模  
	Mat mag = planes[0];
	mag += Scalar::all(1);
	log(mag, mag);                                                   // 模的对数  																												// crop the spectrum, if it has an odd number of rows or columns  
	mag = mag(Rect(0, 0, mag.cols & -2, mag.rows & -2));             //保证偶数的边长  
	int cx = mag.cols / 2;
	int cy = mag.rows / 2;

	// rearrange the quadrants of Fourier image   //对傅立叶变换的图像进行重排，4个区块，从左到右，从上到下 分别为q0, q1, q2, q3  
	// so that the origin is at the image center       //  对调q0和q3, q1和q2  
	Mat tmp;
	Mat q0(mag, Rect(0, 0, cx, cy));
	Mat q1(mag, Rect(cx, 0, cx, cy));
	Mat q2(mag, Rect(0, cy, cx, cy));
	Mat q3(mag, Rect(cx, cy, cx, cy));
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);
	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);
	normalize(mag, mag, 0, 1, CV_MINMAX);           // 规范化值到 0~1 显示图片的需要  
	imshow("spectrum magnitude", mag);

	Mat ifft;
	idft(complexImg, ifft, DFT_REAL_OUTPUT);
	normalize(ifft, ifft, 0, 1, CV_MINMAX);
	imshow("idft", ifft);
	waitKey();
	return 0;
}


//设定对比度和亮度的初值  
int g_nContrastValue = 80;
int g_nBrightValue = 80;
Mat g_srcImage;
Mat g_dstImage;

static void ContrastAndBright(int , void*)
{

	//创建窗口  
	namedWindow("【原始图窗口】", 1);

	//三个for循环，执行运算 g_dstImage(i,j) =a*g_srcImage(i,j) + b  
	for (int y = 0; y < g_srcImage.rows; y++)
	{
		for (int x = 0; x < g_srcImage.cols; x++)
		{
			for (int c = 0; c < 3; c++)
			{
				g_dstImage.at<Vec3b>(y, x)[c] = saturate_cast<uchar>((g_nContrastValue*0.01)*(g_srcImage.at<Vec3b>(y, x)[c]) + g_nBrightValue);
			}
		}
	}

	//显示图像  
	imshow("【原始图窗口】", g_srcImage);
	imshow("【效果图窗口】", g_dstImage);
}

bool TestContrastAndBright()
{
	//读入用户提供的图像  
	g_srcImage = imread("girl.jpg");
	if (!g_srcImage.data) { printf("Oh，no，读取g_srcImage图片错误~！\n"); return false; }
	g_dstImage = Mat::zeros(g_srcImage.size(), g_srcImage.type());

	//创建窗口  
	namedWindow("【效果图窗口】", 1);

	//创建轨迹条  
	createTrackbar("对比度：", "【效果图窗口】", &g_nContrastValue, 300, ContrastAndBright);
	createTrackbar("亮   度：", "【效果图窗口】", &g_nBrightValue, 200, ContrastAndBright);

	//调用回调函数  
	ContrastAndBright(g_nContrastValue, 0);
	ContrastAndBright(g_nBrightValue, 0);

	//输出一些帮助信息  
	cout << endl << "\t嗯。好了，请调整滚动条观察图像效果~\n\n"
		<< "\t按下“q”键时，程序退出~!\n";

	//按下“q”键时，程序退出  
	while (char(waitKey(1)) != 'q') {}
	return true;
}

#include "testLineFilter.h"
#include "testdilate.h"
#include "TestMorphology.h"
#include "testPyUpDown.h"
#include "testHough.h"
#include "testFloodFill.h"
#include "testHarris.h"
#include "testRemap.h"
#include "testSurf.h"
#include "testTranslation.h"

int main(int argc, char** argv)
{
	//testAlpha();
	//testShowImage();
	//addRoi();
	//loadImage();
	//testLineBlend();

	//testMultiChannelBlending();
	//testSharpen();

	//testDFT();

	//TestContrastAndBright();

	//testLineFilter();
	//testFilter();

	//testDilate();
	//testMorphology();

	//testPyUpDown();
	//testHoughCircles();

	//testHoughLines();
	//testFloodFill();


	//testHarris();
	//testRemap(argc, argv);

	//testSurf();
	testSurfMatch();

	//testTranslate();

	return 0;
}

