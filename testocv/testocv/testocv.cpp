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

