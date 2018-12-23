#include <iostream>
#include <vector>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "lane_detect.h"

int main(){
	//Create lane_detect object for the class
	lane_detect obj;
	//cv::Mat img = cv::imread("img51.png",CV_LOAD_IMAGE_COLOR);

	//cv::VideoCapture cap("../videos/project_video.mp4");
	cv::VideoCapture cap("../videos/challenge_video.mp4");
	if(!cap.isOpened()){
		std::cout<<"Error opening video stream"<<std::endl;
		return -1;
	}
	while(1){
		cv::Mat frame;
		cap>>frame;
		if(frame.empty())
			continue;

		//Denoise the Image
		cv::Mat preprocess_img = obj.preprocess(frame);
		//Detect the edges
		cv::Mat edge_img = obj.edge_detect(preprocess_img);
		//Get the ROI
		cv::Mat roi_img = obj.get_roi(edge_img);
		//Get hough lines
		std::vector<cv::Vec4i> lines = obj.hough_lines(roi_img);
		//Separate lines
		obj.separate_lines(lines, roi_img);
		//Extrapolate the lines
		std::vector<cv::Point> lanes = obj.lane_extrapolation(roi_img);
		//Get the turn prediction
		std::string turn = obj.turn_predict();
		//Plot output
		obj.plot_output(lanes, turn, frame);
		//cv::imshow("Original", roi_img);
		//cv::waitKey(25);
		char c = (char)cv::waitKey(25);
		if(c==27)
			break;
		//Display helpers
		/*cv::Mat cdst;
		for( std::size_t i = 0; i < lines.size(); i++ ){
  			cv::Vec4i l = lines[i];
  			cv::line( cdst, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0,0,255), 3, cv::LINE_AA);
		}
		cv::imshow("source", img);
		cv::waitKey();
		cv::imshow("detected lines", cdst);
		cv::waitKey();*/
		//cv::imshow("Original", img);
		//cv::waitKey(0);
		
		//cv::waitKey(0);
	}
	cap.release();
	cv::destroyAllWindows();
	return 0;
}
