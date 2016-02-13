#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

using namespace cv;
using namespace std;

Mat float2byte(const Mat& If) {
	double minVal, maxVal;
	minMaxLoc(If,&minVal,&maxVal);
	Mat Ib;
	If.convertTo(Ib, CV_8U, 255.0/(maxVal - minVal),-minVal * 255.0/(maxVal - minVal));
	return Ib;
}

void onTrackbar(int sigma, void* p)
{
	Mat A=*(Mat*)p;
	Mat B;
	if (sigma) {
		GaussianBlur(A,B,Size(0,0),sigma);
		imshow("images",B);
	} else
		imshow("images",A);
}

int main_()
{
    Mat A=imread("../fruits.jpg");
	namedWindow("images");
	Vec3b c=A.at<Vec3b>(12,12);
	cout << c << endl;
	imshow("images",A);	waitKey();

	Mat I;
	cvtColor(A,I,CV_BGR2GRAY);
	cout << int(I.at<uchar>(12,12)) << endl;
	imshow("images",I);waitKey();

	int m=I.rows,n=I.cols;
	Mat Ix(m,n,CV_32F),Iy(m,n,CV_32F),G(m,n,CV_32F);
	for (int i=0;i<m;i++) {
		for (int j=0;j<n;j++){
			float ix,iy;
			if (i==0 || i==m-1)
				iy=0;
			else
				iy=(float(I.at<uchar>(i+1,j))-float(I.at<uchar>(i-1,j)))/2;
			if (j==0 || j==n-1)
				ix=0;
			else
				ix=(float(I.at<uchar>(i,j+1))-float(I.at<uchar>(i,j-1)))/2;
			Ix.at<float>(i,j)=ix;
			Iy.at<float>(i,j)=iy;
			G.at<float>(i,j)=sqrt(ix*ix+iy*iy);
		}
	}
	imshow("images",float2byte(Ix));waitKey();
	imshow("images",float2byte(Iy));waitKey();
	imshow("images",float2byte(G));waitKey();

	Mat C;
	threshold(G,C,10,1,THRESH_BINARY);
	imshow("images",float2byte(C));waitKey();


	createTrackbar("sigma","images",0,20,onTrackbar,&A);
	imshow("images",A);	waitKey();

	return 0;
}
