#include <iostream>
#include<opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std;

int Otsu(cv::Mat& src, cv::Mat& dst, int thresh) {
	const int Grayscale = 256;
	int graynum[Grayscale] = { 0 };
	int r = src.rows;
	int c = src.cols;
	for (int i = 0; i < r; ++i) {
		const uchar* ptr = src.ptr<uchar>(i);
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

	//��ֵ����(��ֵ��)
	src.copyTo(dst);
	for (int i = 0; i < r; ++i) {
		uchar* ptr = dst.ptr<uchar>(i);
		for (int j = 0; j < c; ++j) {
			if (ptr[j]> thresh)
				ptr[j] = 255;
			else
				ptr[j] = 0;
		}
	}
	return thresh;
}

int main()
{
	//��ȡ��Ƶ������ͷ
	VideoCapture capture("video.mp4");
	clock_t startTime, endTime;
	double time, FPS;
	double FPS_AVE = 0;
	int count = 0;

	while (true)
	{
		Mat frame;
		capture >> frame;
		if (frame.empty())//���ĳ֡Ϊ�����˳�ѭ��
			break;

		startTime = clock();

		//vector<Mat> img_channels;
		Mat frame_gry;
		//vector<Mat>:��Mat������������ʽ��ʾ
		//split(frame, img_channels);
		if (frame.channels() > 1)
			cv::cvtColor(frame, frame, CV_RGB2GRAY);

		//if (src.channels() > 1)
		//	cvtColor(src, src, CV_RGB2GRAY);
		Mat dst;
		int thresh = 0;
		Otsu(frame, dst, thresh);

		endTime = clock();

		imshow("��ȡ��Ƶ", dst);
		imshow("Ԫ��Ƶ", frame);
		waitKey(30);	//��ʱ30
		time = (double)(endTime - startTime) / CLOCKS_PER_SEC;
		FPS = 1 / time;
		cout << "FPS = " << FPS << endl;
		FPS_AVE += FPS;
		count += 1;
	}
	FPS_AVE = FPS_AVE / count;
	cout << "FPS_AVE = " << FPS_AVE << endl;
	system("pause");
	return 0;
}