#include "testSurf.h"
#include "opencv2/core/core.hpp"  
#include "opencv2/features2d/features2d.hpp"  
#include "opencv2/highgui/highgui.hpp"  
#include "opencv2/xfeatures2d/nonfree.hpp"  
//#include<opencv2/legacy/legacy.hpp>  
#include <iostream>  


using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;

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

	////��5����ʾЧ��ͼ  
	//imshow("��������Ч��ͼ1", img_keypoints_1);
	//imshow("��������Ч��ͼ2", img_keypoints_2);

	//Mat desc1, desc2;

	//BruteForceMatcher< L2<float> > matcher;
	//std::vector< DMatch > matches;
	////ƥ������ͼ�е������ӣ�descriptors��  
	//matcher.match(desc1, desc1, matches);

	////��6�����ƴ�����ͼ����ƥ����Ĺؼ���  
	//Mat imgMatches;
	//drawMatches(srcImage1, keyPoint1, srcImage2, keyPoints2, matches, imgMatches);//���л���  

	//
	//Ptr<cv::DescriptorMatcher> matcher(new cv::BFMatcher(cv::NORM_HAMMING, true));
	//vector<DMatch> matches;
	//matcher->match(descriptors1, descriptors2, matches);//��7����ʾЧ��ͼ  
	//imshow("ƥ��ͼ", imgMatches);



	waitKey(0);
	return 0;
}



int testSurfMatch()
{
	//��0���ı�console������ɫ      
	system("color 2F");
	ShowHelpText();

	Mat srcImage1 = imread("girl.jpg", 1);
	Mat srcImage2 = imread("girl.jpg", 1);


	Ptr<SURF> surf = SURF::create(400);

	BFMatcher matcher;         //ʵ����һ������ƥ����  
	Mat c, d;
	vector<KeyPoint>key1, key2;
	vector<DMatch> matches;    //DMatch����������ƥ��õ�һ����������࣬������������֮��������Ϣ  
							   //������ͼ�и�����m��������ͼ��������n��ƥ�䣬���DMatch�ͼ�¼������ƥ�䣬���һ���¼m��n��  
							   //���������ľ����������Ϣ����������ں���������ɸѡ  

	surf->detectAndCompute(srcImage1, Mat(), key1, c);//����ͼ���������룬���������㣬���Mat������������������������  
	surf->detectAndCompute(srcImage1, Mat(), key2, d);//���Mat����Ϊ������ĸ���������Ϊÿ�����������ĳߴ磬SURF��64��ά��  

	matcher.match(c, d, matches);             //ƥ�䣬������Դ��������������������DMatch��������    

											  //sort���������ݽ�����������  
	sort(matches.begin(), matches.end());     //ɸѡƥ��㣬����match���������Եľ����С��������  
	vector< DMatch > good_matches;
	int ptsPairs = std::min(50, (int)(matches.size() * 0.15));
	cout << ptsPairs << endl;
	for (int i = 0; i < ptsPairs; i++)
	{
		good_matches.push_back(matches[i]);//������С��50��ѹ���µ�DMatch  
	}
	Mat outimg;                                //drawMatches�������ֱ�ӻ�������һ���ͼ  
	drawMatches(srcImage1, key1, srcImage2, key2, good_matches, outimg, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);  //����ƥ���    


	imshow("����", outimg);
	cvWaitKey(0);

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