#include<opencv2/opencv.hpp>
#include<iostream>

using namespace std;
using namespace cv;

cv::Mat image_make_border(cv::Mat &src)
{
	int w = getOptimalDFTSize(src.cols);
	int h = getOptimalDFTSize(src.rows);
	Mat padded;
	copyMakeBorder(src, padded, 0, h - src.rows, 0, w - src.cols, BORDER_CONSTANT, Scalar::all(0));
	padded.convertTo(padded, CV_32FC1);
	return padded;
}

//Ƶ�����˲�
Mat frequency_filter(Mat &scr, Mat &blur)
{
	//***********************DFT*******************
	Mat plane[] = { scr, Mat::zeros(scr.size() , CV_32FC1) }; //����ͨ�����洢dft���ʵ�����鲿��CV_32F������Ϊ��ͨ������
	Mat complexIm;
	merge(plane, 2, complexIm);//�ϲ�ͨ�� ������������ϲ�Ϊһ��2ͨ����Mat��������
	dft(complexIm, complexIm);//���и���Ҷ�任���������������

	//***************���Ļ�********************
	split(complexIm, plane);//����ͨ����������룩
							//    plane[0] = plane[0](Rect(0, 0, plane[0].cols & -2, plane[0].rows & -2));//����Ϊʲô&��-2����鿴opencv�ĵ�
							//    //��ʵ��Ϊ�˰��к��б��ż�� -2�Ķ�������11111111.......10 ���һλ��0
	int cx = plane[0].cols / 2; 
	int cy = plane[0].rows / 2;//���µĲ������ƶ�ͼ��  (��Ƶ�Ƶ�����)
	Mat part1_r(plane[0], Rect(0, 0, cx, cy));  //Ԫ�������ʾΪ(cx,cy)
	Mat part2_r(plane[0], Rect(cx, 0, cx, cy));
	Mat part3_r(plane[0], Rect(0, cy, cx, cy));
	Mat part4_r(plane[0], Rect(cx, cy, cx, cy));

	Mat temp;
	part1_r.copyTo(temp);  //���������½���λ��(ʵ��)
	part4_r.copyTo(part1_r);
	temp.copyTo(part4_r);

	part2_r.copyTo(temp);  //���������½���λ��(ʵ��)
	part3_r.copyTo(part2_r);
	temp.copyTo(part3_r);

	Mat part1_i(plane[1], Rect(0, 0, cx, cy));  //Ԫ������(cx,cy)
	Mat part2_i(plane[1], Rect(cx, 0, cx, cy));
	Mat part3_i(plane[1], Rect(0, cy, cx, cy));
	Mat part4_i(plane[1], Rect(cx, cy, cx, cy));

	part1_i.copyTo(temp);  //���������½���λ��(�鲿)
	part4_i.copyTo(part1_i);
	temp.copyTo(part4_i);

	part2_i.copyTo(temp);  //���������½���λ��(�鲿)
	part3_i.copyTo(part2_i);
	temp.copyTo(part3_i);

	//*****************�˲���������DFT����ĳ˻�****************
	Mat blur_r, blur_i, BLUR;
	multiply(plane[0], blur, blur_r); //�˲���ʵ�����˲���ģ���ӦԪ����ˣ�
	multiply(plane[1], blur, blur_i);//�˲����鲿���˲���ģ���ӦԪ����ˣ�
	Mat plane1[] = { blur_r, blur_i };
	merge(plane1, 2, BLUR);//ʵ�����鲿�ϲ�

	magnitude(plane1[0], plane1[1], plane1[0]);//��ȡ����ͼ��0ͨ��Ϊʵ��ͨ����1Ϊ�鲿����Ϊ��ά����Ҷ�任����Ǹ���
	plane1[0] += Scalar::all(1);  //����Ҷ�任���ͼƬ���÷��������ж�����������ȽϺÿ�
	log(plane1[0], plane1[0]);    // float�͵ĻҶȿռ�Ϊ[0��1])
	normalize(plane1[0], plane1[0], 1, 0, CV_MINMAX);  //��һ��������ʾ
	imshow("�˲���Ƶ��ͼ", plane1[0]);

	//*********************�õ�ԭͼƵ��ͼ***********************************
	magnitude(plane[0], plane[1], plane[0]);//��ȡ����ͼ��0ͨ��Ϊʵ��ͨ����1Ϊ�鲿����Ϊ��ά����Ҷ�任����Ǹ���
	plane[0] += Scalar::all(1);  //����Ҷ�任���ͼƬ���÷��������ж�����������ȽϺÿ�
	log(plane[0], plane[0]);    // float�͵ĻҶȿռ�Ϊ[0��1])
	normalize(plane[0], plane[0], 1, 0, CV_MINMAX);  //��һ��������ʾ
	imshow("ԭͼƵ��ͼ", plane[0]);

	//*********************ͼ��IDFT***********************************
	idft(BLUR, BLUR);    //idft���ҲΪ����
	split(BLUR, plane);//����ͨ������Ҫ��ȡͨ��
	magnitude(plane[0], plane[1], plane[0]);  //���ֵ(ģ)
	normalize(plane[0], plane[0], 1, 0, CV_MINMAX);  //��һ��������ʾ
	return plane[0];//���ز���
}

//*****************������˹�����˲���***********************
Mat butterworth_band_stop_kernel(Mat &scr, float sigma, float w, int n)
{
	Mat butterworth_low_pass(scr.size(), CV_32FC1); //��CV_32FC1
	double D0 = sigma;//�뾶D0ԽС��ģ��Խ�󣻰뾶D0Խ��ģ��ԽС
	double W = w;
	for (int i = 0; i<scr.rows; i++) {
		for (int j = 0; j<scr.cols; j++) {
			double d = sqrt(pow((i - scr.rows / 2), 2) + pow((j - scr.cols / 2), 2));//����,����pow����Ϊfloat��
			butterworth_low_pass.at<float>(i, j) = 1.0 / (1 + pow((d * W)/(pow(d, 2) - pow(D0, 2)), 2 * n));
		}
	}

	string name = "������˹�����˲���d0=" + std::to_string(sigma) + "n=" + std::to_string(n);
	imshow(name, butterworth_low_pass);
	return butterworth_low_pass;
}

//������˹�����˲���
Mat butterworth_band_stop_filter(Mat &src, float d0, float w, int n)
{
	//H = 1 / (1+(D0/D)^2n)    n��ʾ������˹�˲����Ĵ���
	Mat padded = image_make_border(src);
	Mat butterworth_kernel = butterworth_band_stop_kernel(padded, d0, w, n);
	Mat result = frequency_filter(padded, butterworth_kernel);
	return result;
}

int main(int argc, char *argv[])
{
	Mat input = imread("bad.jpg", IMREAD_GRAYSCALE);
	if (input.empty())
		return -1;
	imshow("ԭͼ", input);//��ʾԭͼ

	cv::Mat bw_high = butterworth_band_stop_filter(input, 150, 300, 4);
	bw_high = bw_high(cv::Rect(0, 0, input.cols, input.rows));
	imshow("������˹����", bw_high);


	waitKey();
	return 0;
}
