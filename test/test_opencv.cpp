#include <algorithm>
#include <string>
#include <vector>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>


#include "tjs.h"
#include "tjsError.h"

#include <locale.h>
#include <boost/filesystem.hpp>

#include <iostream>
#include <opencv2/opencv.hpp>
 
using namespace cv;
using namespace std;
 
void on_ContrastAndBright(double contrast,int bright);//轨迹条的回调函数，参数必须是（int,void*）,第一个是滑动条的位置，第二个是额外参数，creatTrackbar的参数六
Mat src, dst;
// int bright ;
// int contrast ;
const char* outname = NULL;
 
int main(int argc, const char* argv[])
{
 
	src = imread("lena.jpg");	
	if (!src.data) { printf("read file fail!"); return false; }
	dst = Mat::zeros(src.size(), src.type());
	int bright = 0;
	double contrast = 2.2;
	if(argc > 1)
	{
		outname = argv[1];
	}
	if(argc > 2)
	{

		sscanf(argv[2],"%lf",&contrast);
	}
	if(argc > 3)
	{
		sscanf(argv[3],"%d",&bright);
	}
	printf("outname:%s,contrast:%.2f,bright:%d\n",outname,contrast,bright);
	on_ContrastAndBright(contrast,bright);
	printf("finish!\n");
	return 0;
}
void on_ContrastAndBright(double contrast,int bright)
{
	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			for (int c = 0; c < 3; c++)
			{
				dst.at<Vec3b>(i, j)[c] = saturate_cast<uchar>(src.at<Vec3b>(i, j)[c] * contrast + bright );
			}
		}
	}
	imwrite("out1.jpg",dst);
}