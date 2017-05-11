#include "testSurf.h"
#include "opencv2/core/core.hpp"  
#include "opencv2/features2d/features2d.hpp"  
#include "opencv2/highgui/highgui.hpp"  
#include "opencv2/xfeatures2d/nonfree.hpp"  
#include <iostream>  


using namespace cv;
using namespace cv::xfeatures2d;

static void ShowHelpText();
 
int testSurf()
{
	//��0���ı�console������ɫ      
	system("color 2F");

	//��0����ʾ��������    
	ShowHelpText();

	//��1������ԴͼƬ����ʾ  
	Mat srcImage1 = imread("girl.jpg", 1);
	Mat srcImage2 = imread("girl5.jpg", 1);
	if (!srcImage1.data || !srcImage2.data)//����Ƿ��ȡ�ɹ�  
	{
		printf("��ȡͼƬ������ȷ��Ŀ¼���Ƿ���imread����ָ�����Ƶ�ͼƬ����~�� \n"); return false;
	}
	imshow("ԭʼͼ1", srcImage1);
	imshow("ԭʼͼ2", srcImage2);

	//��2��������Ҫ�õ��ı�������  
	int minHessian = 400;//����SURF�е�hessian��ֵ������������  
	//SurfFeatureDetector detector(minHessian);//����һ��SurfFeatureDetector��SURF�� ������������  
	//std::vector<KeyPoint> keypoints_1, keypoints_2;//vectorģ�������ܹ�����������͵Ķ�̬���飬�ܹ����Ӻ�ѹ������  
	//��3������detect��������SURF�����ؼ��㣬������vector������  
	//detector.detect(srcImage1, keypoints_1);
	//detector.detect(srcImage2, keypoints_2);

	Ptr<SURF> surf = SURF::create(minHessian);
	std::vector<KeyPoint> keypoints_1, keypoints_2;
	Mat desc;
	surf->detectAndCompute(srcImage1, noArray(), keypoints_1, desc);
	surf->detectAndCompute(srcImage2, noArray(), keypoints_2, desc);



	//��4�����������ؼ���  
	Mat img_keypoints_1; Mat img_keypoints_2;
	drawKeypoints(srcImage1, keypoints_1, img_keypoints_1, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
	drawKeypoints(srcImage2, keypoints_2, img_keypoints_2, Scalar::all(-1), DrawMatchesFlags::DEFAULT);

	//��5����ʾЧ��ͼ  
	imshow("��������Ч��ͼ1", img_keypoints_1);
	imshow("��������Ч��ͼ2", img_keypoints_2);

	waitKey(0);
	return 0;
}


//-----------------------------------��ShowHelpText( )������----------------------------------  
//          ���������һЩ������Ϣ  
//----------------------------------------------------------------------------------------------  
void ShowHelpText()
{
	//���һЩ������Ϣ    
	printf("\n\n\n\t��ӭ������SURF�������⡿ʾ������~\n\n");
	printf("\t��ǰʹ�õ�OpenCV�汾Ϊ OpenCV ");
	printf("\n\n\t��������˵��: \n\n"
		"\t\t���̰��������- �˳�����\n\n");

}