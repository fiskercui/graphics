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
	//【0】改变console字体颜色      
	system("color 2F");

	//【0】显示帮助文字    
	ShowHelpText();

	//【1】载入源图片并显示  
	Mat srcImage1 = imread("girl.jpg", 1);
	Mat srcImage2 = imread("girl5.jpg", 1);
	if (!srcImage1.data || !srcImage2.data)//检测是否读取成功  
	{
		printf("读取图片错误，请确定目录下是否有imread函数指定名称的图片存在~！ \n"); return false;
	}
	imshow("原始图1", srcImage1);
	imshow("原始图2", srcImage2);

	//【2】定义需要用到的变量和类  
	int minHessian = 400;//定义SURF中的hessian阈值特征点检测算子  
	//SurfFeatureDetector detector(minHessian);//定义一个SurfFeatureDetector（SURF） 特征检测类对象  
	//std::vector<KeyPoint> keypoints_1, keypoints_2;//vector模板类是能够存放任意类型的动态数组，能够增加和压缩数据  
	//【3】调用detect函数检测出SURF特征关键点，保存在vector容器中  
	//detector.detect(srcImage1, keypoints_1);
	//detector.detect(srcImage2, keypoints_2);

	Ptr<SURF> surf = SURF::create(minHessian);
	std::vector<KeyPoint> keypoints_1, keypoints_2;
	Mat desc;
	surf->detectAndCompute(srcImage1, noArray(), keypoints_1, desc);
	surf->detectAndCompute(srcImage2, noArray(), keypoints_2, desc);



	//【4】绘制特征关键点  
	Mat img_keypoints_1; Mat img_keypoints_2;
	drawKeypoints(srcImage1, keypoints_1, img_keypoints_1, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
	drawKeypoints(srcImage2, keypoints_2, img_keypoints_2, Scalar::all(-1), DrawMatchesFlags::DEFAULT);

	////【5】显示效果图  
	//imshow("特征点检测效果图1", img_keypoints_1);
	//imshow("特征点检测效果图2", img_keypoints_2);

	//Mat desc1, desc2;

	//BruteForceMatcher< L2<float> > matcher;
	//std::vector< DMatch > matches;
	////匹配两幅图中的描述子（descriptors）  
	//matcher.match(desc1, desc1, matches);

	////【6】绘制从两个图像中匹配出的关键点  
	//Mat imgMatches;
	//drawMatches(srcImage1, keyPoint1, srcImage2, keyPoints2, matches, imgMatches);//进行绘制  

	//
	//Ptr<cv::DescriptorMatcher> matcher(new cv::BFMatcher(cv::NORM_HAMMING, true));
	//vector<DMatch> matches;
	//matcher->match(descriptors1, descriptors2, matches);//【7】显示效果图  
	//imshow("匹配图", imgMatches);



	waitKey(0);
	return 0;
}



int testSurfMatch()
{
	//【0】改变console字体颜色      
	system("color 2F");
	ShowHelpText();

	Mat srcImage1 = imread("girl.jpg", 1);
	Mat srcImage2 = imread("girl.jpg", 1);


	Ptr<SURF> surf = SURF::create(400);

	BFMatcher matcher;         //实例化一个暴力匹配器  
	Mat c, d;
	vector<KeyPoint>key1, key2;
	vector<DMatch> matches;    //DMatch是用来描述匹配好的一对特征点的类，包含这两个点之间的相关信息  
							   //比如左图有个特征m，它和右图的特征点n最匹配，这个DMatch就记录它俩最匹配，并且还记录m和n的  
							   //特征向量的距离和其他信息，这个距离在后面用来做筛选  

	surf->detectAndCompute(srcImage1, Mat(), key1, c);//输入图像，输入掩码，输入特征点，输出Mat，存放所有特征点的描述向量  
	surf->detectAndCompute(srcImage1, Mat(), key2, d);//这个Mat行数为特征点的个数，列数为每个特征向量的尺寸，SURF是64（维）  

	matcher.match(c, d, matches);             //匹配，数据来源是特征向量，结果存放在DMatch类型里面    

											  //sort函数对数据进行升序排列  
	sort(matches.begin(), matches.end());     //筛选匹配点，根据match里面特征对的距离从小到大排序  
	vector< DMatch > good_matches;
	int ptsPairs = std::min(50, (int)(matches.size() * 0.15));
	cout << ptsPairs << endl;
	for (int i = 0; i < ptsPairs; i++)
	{
		good_matches.push_back(matches[i]);//距离最小的50个压入新的DMatch  
	}
	Mat outimg;                                //drawMatches这个函数直接画出摆在一起的图  
	drawMatches(srcImage1, key1, srcImage2, key2, good_matches, outimg, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);  //绘制匹配点    


	imshow("桌面", outimg);
	cvWaitKey(0);

	return 0;
}


//-----------------------------------【ShowHelpText( )函数】----------------------------------  
//          描述：输出一些帮助信息  
//----------------------------------------------------------------------------------------------  
void ShowHelpText()
{
	//输出一些帮助信息    
	printf("\n\n\n\t欢迎来到【SURF特征点检测】示例程序~\n\n");
	printf("\t当前使用的OpenCV版本为 OpenCV ");
	printf("\n\n\t按键操作说明: \n\n"
		"\t\t键盘按键任意键- 退出程序\n\n");

}