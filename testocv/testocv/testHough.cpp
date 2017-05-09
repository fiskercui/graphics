#include <opencv2/opencv.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  

#include <vector>

//-----------------------------------�������ռ��������֡�---------------------------------------  
//      ����������������ʹ�õ������ռ�  
//-----------------------------------------------------------------------------------------------   
using namespace cv;
using namespace std;
//-----------------------------------��main( )������--------------------------------------------  
//      ����������̨Ӧ�ó������ں��������ǵĳ�������￪ʼ  
//-----------------------------------------------------------------------------------------------  
int testHoughCircles()
{
	//��1������ԭʼͼ��Mat��������     
	Mat srcImage = imread("circle.jpg");  //����Ŀ¼��Ӧ����һ����Ϊ1.jpg���ز�ͼ  
	Mat midImage, dstImage;//��ʱ������Ŀ��ͼ�Ķ���  

						   //��2����ʾԭʼͼ  
	imshow("��ԭʼͼ��", srcImage);

	//��3��תΪ�Ҷ�ͼ������ͼ��ƽ��  
	cvtColor(srcImage, midImage, CV_BGR2GRAY);//ת����Ե�����ͼΪ�Ҷ�ͼ  
	GaussianBlur(midImage, midImage, Size(9, 9), 2, 2);

	//��4�����л���Բ�任  
	vector<Vec3f> circles;
	HoughCircles(midImage, circles, CV_HOUGH_GRADIENT, 1.5, 10, 200, 100, 0, 0);

	//��5��������ͼ�л��Ƴ�Բ  
	for (size_t i = 0; i < circles.size(); i++)
	{
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
		//����Բ��  
		circle(srcImage, center, 3, Scalar(0, 255, 0), -1, 8, 0);
		//����Բ����  
		circle(srcImage, center, radius, Scalar(155, 50, 255), 3, 8, 0);
	}

	//��6����ʾЧ��ͼ    
	imshow("��Ч��ͼ��", srcImage);

	waitKey(0);

	return 0;
}


#include <opencv2/opencv.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  

//-----------------------------------�������ռ��������֡�--------------------------------------  
//      ����������������ʹ�õ������ռ�  
//-----------------------------------------------------------------------------------------------   
using namespace std;
using namespace cv;



static Mat g_srcImage, g_dstImage, g_midImage;//ԭʼͼ���м�ͼ��Ч��ͼ  
vector<Vec4i> g_lines;//����һ��ʸ���ṹg_lines���ڴ�ŵõ����߶�ʸ������  
					  //�������յ�TrackBarλ�ò���  
static int g_nthreshold = 100;

static void on_HoughLines(int, void*);//�ص�����  
static void ShowHelpText();


int testHoughLines()
{
	//�ı�console������ɫ  
	system("color 3F");

	ShowHelpText();

	//����ԭʼͼ��Mat��������     
	Mat g_srcImage = imread("girl.jpg");  //����Ŀ¼��Ӧ����һ����Ϊ1.jpg���ز�ͼ  

									   //��ʾԭʼͼ    
	imshow("��ԭʼͼ��", g_srcImage);

	//����������  
	namedWindow("��Ч��ͼ��", 1);
	createTrackbar("ֵ", "��Ч��ͼ��", &g_nthreshold, 200, on_HoughLines);

	//���б�Ե����ת��Ϊ�Ҷ�ͼ  
	Canny(g_srcImage, g_midImage, 50, 200, 3);//����һ��canny��Ե���  
	cvtColor(g_midImage, g_dstImage, CV_GRAY2BGR);//ת����Ե�����ͼΪ�Ҷ�ͼ  

												  //����һ�λص�����������һ��HoughLinesP����  
	on_HoughLines(g_nthreshold, 0);
	HoughLinesP(g_midImage, g_lines, 1, CV_PI / 180, 80, 50, 10);

	//��ʾЧ��ͼ    
	imshow("��Ч��ͼ��", g_dstImage);


	waitKey(0);

	return 0;

}

//-----------------------------------��on_HoughLines( )������--------------------------------  
//      ����������ñ����/��ñ���㡿���ڵĻص�����  
//----------------------------------------------------------------------------------------------  
static void on_HoughLines(int, void*)
{
	//����ֲ���������ȫ�ֱ���  
	Mat dstImage = g_dstImage.clone();
	Mat midImage = g_midImage.clone();

	//����HoughLinesP����  
	vector<Vec4i> mylines;
	HoughLinesP(midImage, mylines, 1, CV_PI / 180, g_nthreshold + 1, 50, 10);

	//ѭ����������ÿһ���߶�  
	for (size_t i = 0; i < mylines.size(); i++)
	{
		Vec4i l = mylines[i];
		line(dstImage, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(23, 180, 55), 1, CV_AA);
	}
	//��ʾͼ��  
	imshow("��Ч��ͼ��", dstImage);
}


//-----------------------------------��ShowHelpText( )������----------------------------------  
//      ���������һЩ������Ϣ  
//----------------------------------------------------------------------------------------------  
static void ShowHelpText()
{
	//���һЩ������Ϣ  
	printf("\n\n\n\t������������۲�ͼ��Ч��~\n\n");
	printf("\n\n\t\t\t\t\t\t\t\t byǳī"
	);
}