
#include<opencv2/core/core.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<iostream>

using namespace std;
using namespace cv;

void Scale(Mat&input, Mat&output, int index = 0)
{
	float max = 0, min = 0;
	output = Mat::zeros(input.rows, input.cols, CV_8UC3);
	for (int i = 0; i<input.rows; i++)
	{
		float *ptr = input.ptr<float>(i);
		for (int j = 0; j<input.cols * 3; j++)//ͼ��Ϊ3ͨ��
		{
			if (max<ptr[j])max = ptr[j];
			if (min>ptr[j])min = ptr[j];
		}

	}//ȡ��ͼ���е�������
	for (int i = 0; i<input.rows; i++)
	{
		float *ptr = input.ptr<float>(i);
		uchar *optr = output.ptr<uchar>(i);
		for (int j = 0; j<input.cols * 3; j++)
		{
			if (index)
			{
				if (ptr[j]>1) { optr[j] = 255; continue; }
				else if (ptr[j]<0) { optr[j] = 0; continue; }
				else optr[j] = (uchar)(ptr[j] * 255);
			}
			else
				optr[j] = (uchar)((ptr[j] - min) / (max - min) * 255);
		}
	}
}

void main()
{
	Mat srcImage = imread("blur.jpg");
	if (!srcImage.data) {
		cout << "falied to read" << endl;
		system("pause");
		return;
	}
	imshow("srcImage", srcImage);
	Mat blurImage;
	blur(srcImage, blurImage, Size(3, 3));
	Mat srcGray;
	cvtColor(blurImage, srcGray, CV_BGR2GRAY);
	//�����Եͼ��ˮƽ����ʹ�ֱ�����
	Mat edgeMat, edgeXMat, edgeYMat;
	//��x�����Sobel��Ե
	Sobel(srcGray, edgeXMat, CV_16S, 1, 0, 3, 1, 0, BORDER_DEFAULT);
	//��y�����Sobel��Ե
	Sobel(srcGray, edgeYMat, CV_16S, 0, 1, 3, 1, 0, BORDER_DEFAULT);
	//���Ա任��ת����������Ԫ��Ϊ8λ�޷�������
	convertScaleAbs(edgeXMat, edgeXMat);
	convertScaleAbs(edgeYMat, edgeYMat);
	//x��y�����Ե����
	addWeighted(edgeXMat, 0.5, edgeYMat, 0.5, 0, edgeMat);
	//imshow("X", edgeXMat);
	//imshow("Y", edgeYMat);
	addWeighted(edgeMat, 0.5, srcImage, 0.5, 0, edgeMat);
	imwrite("X+Y.jpg", edgeMat);
	waitKey(0);
}