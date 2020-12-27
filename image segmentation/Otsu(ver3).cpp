#include <iostream>
#include<opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std;

int Otsu(Mat& src, Mat& src2, Mat& src3, Mat& dst, int &thresh) {
	cout << "test" << endl;
	cout << thresh << endl;
	int r = src.rows;
	int c = src.cols;
	if (thresh == 0)
	{
		const int Grayscale = 256;
		int graynum[Grayscale] = { 0 };
		for (int i = 0; i < r; ++i) {
			const uchar* ptr = src.ptr<uchar>(i);
			for (int j = 0; j < c; ++j) {        //ֱ��ͼͳ��
				graynum[ptr[j]]++;
			}
		}
		for (int i = 0; i < r; ++i) {
			const uchar* ptr = src2.ptr<uchar>(i);
			for (int j = 0; j < c; ++j) {        //ֱ��ͼͳ��
				graynum[ptr[j]]++;
			}
		}
		for (int i = 0; i < r; ++i) {
			const uchar* ptr = src3.ptr<uchar>(i);
			for (int j = 0; j < c; ++j) {        //ֱ��ͼͳ��
				graynum[ptr[j]]++;
			}
		}

		double P[Grayscale] = { 0 };
		double PK[Grayscale] = { 0 };
		double MK[Grayscale] = { 0 };
		double srcpixnum = r*c, sumtmpPK = 0, sumtmpMK = 0;
		for (int i = 0; i < Grayscale; ++i) {
			P[i] = graynum[i] / srcpixnum;   //ÿ���Ҷȼ�ռ�����ظ���֮��(Ҳ�ɳ�Ϊ����)
			PK[i] = sumtmpPK + P[i];         //ռ���ۼƺ� PK��ʾP(0->k)
			sumtmpPK = PK[i];
			MK[i] = sumtmpMK + i*P[i];       //�Ҷȼ����ۼӾ�ֵ    MK��ʾM(0->k)                                                                                                                                                                                                                                                                                                                                                                                                    
			sumtmpMK = MK[i];
		}

		//������䷽��
		double Var = 0;
		for (int k = 0; k < Grayscale; ++k) {
			if ((MK[Grayscale - 1] * PK[k] - MK[k])*(MK[Grayscale - 1] * PK[k] - MK[k]) / (PK[k] * (1 - PK[k])) > Var) {
				Var = (MK[Grayscale - 1] * PK[k] - MK[k])*(MK[Grayscale - 1] * PK[k] - MK[k]) / (PK[k] * (1 - PK[k]));
				thresh = k;
			}
		}
	}

	//��ֵ����(��ֵ��)
	src.copyTo(dst);
	for (int i = 0; i < r; ++i) {
		uchar* ptr = dst.ptr<uchar>(i);
		uchar* ptr_compare = src2.ptr<uchar>(i);
		for (int j = 0; j < c; ++j) {
			if (ptr_compare[j]> thresh)
				ptr[j] = ptr[j];
			else
				ptr[j] = 0;
		}
	}
	return thresh;
}


int main() {
	Mat src = imread("picture.jpg");
	if (src.empty()) {
		return -1;
	}
	vector<Mat> img_channels;
	//vector<Mat>:��Mat������������ʽ��ʾ
	split(src, img_channels);

	//if (src.channels() > 1)
	//	cvtColor(src, src, CV_RGB2GRAY);
	vector<Mat> img_channels_dst;
	Mat dst0, dst1, dst2, dst;
	int thresh = 0;
	Otsu(img_channels[0], img_channels[1], img_channels[2], dst0, thresh); //
	Otsu(img_channels[1], img_channels[1], img_channels[2], dst1, thresh); //Otsu
	Otsu(img_channels[2], img_channels[1], img_channels[2], dst2, thresh); //Otsu

	img_channels_dst.push_back(dst0);
	img_channels_dst.push_back(dst1);
	img_channels_dst.push_back(dst2);

	merge(img_channels_dst, dst);

	namedWindow("src", CV_WINDOW_NORMAL);
	imshow("src", src);
	namedWindow("dst", CV_WINDOW_NORMAL);
	imshow("dst", dst);
	waitKey(0);
}