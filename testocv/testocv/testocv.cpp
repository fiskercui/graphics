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


	//��1������ͼƬ  
	logoImage = imread("girl.jpg", 0);
	srcImage = imread("main_bg3.png", IMREAD_UNCHANGED);


	//��2����һ��3ͨ��ͼ��ת����3����ͨ��ͼ��  
	split(srcImage, channels);//����ɫ��ͨ��  

	imwrite("blue.png", channels[0]);
	imwrite("green.png", channels[1]);
	imwrite("red.png", channels[2]);

	if (channels.size() >= 4)
	{
		imwrite("alpha.png", channels[3]);
	}

	//imwrite("alpha.png", channels[3]);


	//						  //��3����ԭͼ����ɫͨ�����÷��ظ�imageBlueChannel��ע�������ã��൱�����ߵȼۣ��޸�����һ����һ�����ű�  
	//imageBlueChannel = channels.at(0);
	////��4����ԭͼ����ɫͨ���ģ�500,250�����괦���·���һ�������logoͼ���м�Ȩ���������õ��Ļ�Ͻ���浽imageBlueChannel��  
	//
	//imageBlueChannel(Rect(500, 250, logoImage.cols, logoImage.rows));

	//addWeighted(imageBlueChannel(Rect(500, 250, logoImage.cols, logoImage.rows)), 1.0,
	//	logoImage, 0.5, 0, imageBlueChannel(Rect(500, 250, logoImage.cols, logoImage.rows)));

	////��5����������ͨ�����ºϲ���һ����ͨ��  
	//merge(channels, srcImage);

	////��6����ʾЧ��ͼ  
	//namedWindow("<1>");
	//imshow("<1>", srcImage);


	////=================����ɫͨ�����֡�=================  
	////     ��������ͨ�����-��ɫ��������  
	////============================================  

	////��0��������ر���  
	//Mat  imageGreenChannel;

	////��1�����¶���ͼƬ  
	//logoImage = imread("dota_logo.jpg", 0);
	//srcImage = imread("dota_jugg.jpg");

	//if (!logoImage.data) { printf("Oh��no����ȡlogoImage����~��\n"); return false; }
	//if (!srcImage.data) { printf("Oh��no����ȡsrcImage����~��\n"); return false; }

	////��2����һ����ͨ��ͼ��ת����������ͨ��ͼ��  
	//split(srcImage, channels);//����ɫ��ͨ��  

	//						  //��3����ԭͼ����ɫͨ�������÷��ظ�imageBlueChannel��ע�������ã��൱�����ߵȼۣ��޸�����һ����һ�����ű�  
	//imageGreenChannel = channels.at(1);
	////��4����ԭͼ����ɫͨ���ģ�500,250�����괦���·���һ�������logoͼ���м�Ȩ���������õ��Ļ�Ͻ���浽imageGreenChannel��  
	//addWeighted(imageGreenChannel(Rect(500, 250, logoImage.cols, logoImage.rows)), 1.0,
	//	logoImage, 0.5, 0., imageGreenChannel(Rect(500, 250, logoImage.cols, logoImage.rows)));

	////��5�������������ĵ�ͨ�����ºϲ���һ����ͨ��  
	//merge(channels, srcImage);

	////��6����ʾЧ��ͼ  
	//namedWindow("<2>��Ϸԭ��+logo��ɫͨ�� byǳī");
	//imshow("<2>��Ϸԭ��+logo��ɫͨ�� byǳī", srcImage);

	return true;
}



void testSharpen()
{
	Mat myImage = imread("main_bg3.png", IMREAD_UNCHANGED);
	Mat Result;

	CV_Assert(myImage.depth() == CV_8U);  // accept only uchar images   // CV_Assert ��cv����Ķ��Ը�ʽ��������������ϣ����жϳ����׳��쳣  
	const int nChannels = myImage.channels();
	Result.create(myImage.size(), myImage.type());

	for (int j = 1; j < myImage.rows - 1; ++j)                             // ��һ�к����һ���޷�����  
	{
		const uchar* previous = myImage.ptr<uchar>(j - 1);      //   ��һ�����ݵ�ָ��  
		const uchar* current = myImage.ptr<uchar>(j);        //   ��ǰ�����ݵ�ָ��  
		const uchar* next = myImage.ptr<uchar>(j + 1);       //  ��һ�����ݵ�ָ��  

		uchar* output = Result.ptr<uchar>(j);                            // ���ͼ��ǰ�����ݵ�ָ��  

		for (int i = nChannels; i < nChannels*(myImage.cols - 1); ++i)   // ͬ�����Ե�һ�к����һ������  
		{
			*output++ = saturate_cast<uchar>(5 * current[i]
				- current[i - nChannels] - current[i + nChannels] - previous[i] - next[i]);         // ������ֵ  
		}
	}
	// ���߽���Ϊ0  
	Result.row(0).setTo(Scalar(0));
	Result.row(Result.rows - 1).setTo(Scalar(0));
	Result.col(0).setTo(Scalar(0));
	Result.col(Result.cols - 1).setTo(Scalar(0));
	//��ʾ���  
	namedWindow("�񻯵�ͼ");
	imshow("�񻯵�ͼ", Result);
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
	//Mat img = imread(filename.c_str(), CV_LOAD_IMAGE_GRAYSCALE);    //�ԻҶ�ͼ�����  
	Mat img = imread(filename.c_str(), CV_LOAD_IMAGE_GRAYSCALE);    //�ԻҶ�ͼ�����  
	if (img.empty())
	{
		help();
		printf("Cannot read image file: %s\n", filename.c_str());
		return -1;
	}
	int M = getOptimalDFTSize(img.rows);                // ������DFT�ߴ磬Ϊ2�Ĵη�  
	int N = getOptimalDFTSize(img.cols);                //ͬ��  
	Mat padded;
	copyMakeBorder(img, padded, 0, M - img.rows, 0, N - img.cols, BORDER_CONSTANT, Scalar::all(0)); // opencv�еı߽���չ�������ṩ���ַ�ʽ��չ  

	Mat mat1 = Mat_<float>(padded);
	Mat mat2 = Mat::zeros(padded.size(), CV_32F);
	Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };          // Mat ���飬��һ��Ϊ��չ���ͼ��һ��Ϊ��ͼ��  
	Mat complexImg;
	merge(planes, 2, complexImg);                                                                              // �ϲ���һ��Mat  

	dft(complexImg, complexImg);                                      // FFT�任�� dft��Ҫһ��2ͨ����Mat  																									  // compute log(1 + sqrt(Re(DFT(img))**2 + Im(DFT(img))**2))  
	split(complexImg, planes);                                        //����ͨ���� planes[0] Ϊʵ�����֣�planes[1]Ϊ��������  
	magnitude(planes[0], planes[1], planes[0]);                       // ��ģ  
	Mat mag = planes[0];
	mag += Scalar::all(1);
	log(mag, mag);                                                   // ģ�Ķ���  																												// crop the spectrum, if it has an odd number of rows or columns  
	mag = mag(Rect(0, 0, mag.cols & -2, mag.rows & -2));             //��֤ż���ı߳�  
	int cx = mag.cols / 2;
	int cy = mag.rows / 2;

	// rearrange the quadrants of Fourier image   //�Ը���Ҷ�任��ͼ��������ţ�4�����飬�����ң����ϵ��� �ֱ�Ϊq0, q1, q2, q3  
	// so that the origin is at the image center       //  �Ե�q0��q3, q1��q2  
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
	normalize(mag, mag, 0, 1, CV_MINMAX);           // �淶��ֵ�� 0~1 ��ʾͼƬ����Ҫ  
	imshow("spectrum magnitude", mag);

	Mat ifft;
	idft(complexImg, ifft, DFT_REAL_OUTPUT);
	normalize(ifft, ifft, 0, 1, CV_MINMAX);
	imshow("idft", ifft);
	waitKey();
	return 0;
}


//�趨�ԱȶȺ����ȵĳ�ֵ  
int g_nContrastValue = 80;
int g_nBrightValue = 80;
Mat g_srcImage;
Mat g_dstImage;

static void ContrastAndBright(int , void*)
{

	//��������  
	namedWindow("��ԭʼͼ���ڡ�", 1);

	//����forѭ����ִ������ g_dstImage(i,j) =a*g_srcImage(i,j) + b  
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

	//��ʾͼ��  
	imshow("��ԭʼͼ���ڡ�", g_srcImage);
	imshow("��Ч��ͼ���ڡ�", g_dstImage);
}

bool TestContrastAndBright()
{
	//�����û��ṩ��ͼ��  
	g_srcImage = imread("girl.jpg");
	if (!g_srcImage.data) { printf("Oh��no����ȡg_srcImageͼƬ����~��\n"); return false; }
	g_dstImage = Mat::zeros(g_srcImage.size(), g_srcImage.type());

	//��������  
	namedWindow("��Ч��ͼ���ڡ�", 1);

	//�����켣��  
	createTrackbar("�Աȶȣ�", "��Ч��ͼ���ڡ�", &g_nContrastValue, 300, ContrastAndBright);
	createTrackbar("��   �ȣ�", "��Ч��ͼ���ڡ�", &g_nBrightValue, 200, ContrastAndBright);

	//���ûص�����  
	ContrastAndBright(g_nContrastValue, 0);
	ContrastAndBright(g_nBrightValue, 0);

	//���һЩ������Ϣ  
	cout << endl << "\t�š����ˣ�������������۲�ͼ��Ч��~\n\n"
		<< "\t���¡�q����ʱ�������˳�~!\n";

	//���¡�q����ʱ�������˳�  
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

