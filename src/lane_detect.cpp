#include <iostream>
#include <vector>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "lane_detect.h"

cv::Mat lane_detect::color_hsl(cv::Mat src){
	cv::Mat output;
	cv::cvtColor(src,output,CV_RGB2HLS);
	return output;
}
cv::Mat lane_detect::color_hsv(cv::Mat src){
	cv::Mat output;
	cv::cvtColor(src,output,CV_RGB2HSV);
	return output;
}

cv::Mat lane_detect::preprocess(cv::Mat img_raw){
	cv::Mat output, denoised, img_hsl, yellow_lane, white_lane;

	//remove the noise using the gaussian 5x5 filter kernel
	cv::GaussianBlur(img_raw, denoised, cv::Size(5,5), 0, 0);
	//std::string name = "test/gaussian.png";
    //cv::imwrite(name, denoised);
	
	//change the color scale to HSL (Hue light saturation)
	img_hsl = color_hsl(denoised);
	//name = "test/img_hsl.png";
   	//cv::imwrite(name, img_hsl);
	
	//Detect the yellow lines
	cv::inRange(img_hsl, cv::Scalar(95,90,90), cv::Scalar(110,200,255), yellow_lane);//low = ()15,38,115, high = ()35,204,255
	//name = "test/yellow_lane.png";
    //cv::imwrite(name, yellow_lane);
	
	//change the color scale to HSV (Hue Saturation Value)
	//img_hsv = color_hsv(img_raw);
	
	//Detect the white lines
	cv::inRange(img_hsl,cv::Scalar(60,190,10), cv::Scalar(115,255,255), white_lane);
	//name = "test/white_lane.png";
    //cv::imwrite(name, white_lane);
	
	//Or the white and yellow lanes
	cv::bitwise_or(yellow_lane,white_lane,output);
	//name = "test/white_yellow_bitwise_lane.png";
    //cv::imwrite(name, output);
	//cv::imshow("2", output);
	//cv::waitKey(0);
	return output;
}

cv::Mat lane_detect::edge_detect(cv::Mat src){
	cv::Mat output;
	// Vertical edge detetction using a [-1,0,1] kernel, 
	//obtained from Matlab lane detection library
  	cv::Point anchor = cv::Point(-1, -1);
    cv::Mat kernel = cv::Mat(1, 3, CV_32F);
    kernel.at<float>(0, 0) = -1;
    kernel.at<float>(0, 1) = 0;
    kernel.at<float>(0, 2) = 1;

    // Filter the binary image to obtain the edges
    cv::filter2D(src, output, -1, kernel, anchor, 0, cv::BORDER_DEFAULT);
    //std::string name = "test/edge_detect.png";
    //cv::imwrite(name, output);
	//cv::imshow("Color Scale", output);
	//cv::waitKey(0);
	return output;
}

cv::Mat lane_detect::get_roi(cv::Mat src){
	cv::Mat output;
  	cv::Mat mask = cv::Mat::zeros(src.size(), src.type());
  	//std::cout<<src.size()<<" "<<src.rows<<" "<<src.cols<<std::endl;
  	//std::cout<<src.size()<<" "<<src.size().width<<" "<<src.size().height<<std::endl;
  	cv::Point pts[4] = { cv::Point(250, 720),
     					 cv::Point(550, 450),
     					 cv::Point(717, 450),
     					 cv::Point(1280, 720)
  						};//450

  	// Create a binary polygon mask
  	cv::fillConvexPoly(mask, pts, 4, cv::Scalar(255, 255, 255));
  	// Multiply the edges image and the mask to get the output
  	cv::bitwise_and(src, mask, output);
  	//std::string name = "test/roi.png";
  	//cv::imwrite(name, output);
  	return output;
}

std::vector<cv::Vec4i> lane_detect::hough_lines(cv::Mat src){
	std::vector<cv::Vec4i> lines;
	// rho and theta are selected by trial and error
	HoughLinesP(src, lines, 1, CV_PI/180, 15, 20, 10);
	/*for(auto i : lines){
		std::cout<<i<<std::endl;
	}*/

	return lines;
}

void lane_detect::separate_lines(std::vector<cv::Vec4i> lines, cv::Mat src){
	img_center = static_cast<double>((src.cols / 2));
	const double thresh = 0.3;
	if(!lines.empty()){
		right_lines.clear();
		left_lines.clear();
		for( std::size_t i = 0; i < lines.size(); i++ ){
  			cv::Vec4i l = lines[i];
  			//dx = x2 - x1
  			float dx = l[2] - l[0];
  			float dy = l[3] - l[1];
  			if(dx == 0 && dy == 0)
  				continue;
  			slope = dy/dx;
  			if(std::abs(slope) < thresh)
  				continue;
  			else if(slope < 0 && l[2] < img_center && l[0] < img_center){
  				right_lines.push_back(l);
  				temp_right.push_back(l); 
  			}
  			else if(slope > 0 && l[2] > img_center && l[0] > img_center){
  				left_lines.push_back(l);
  				temp_left.push_back(l); 
  			}
  			//cv::line( cdst, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0,0,255), 3, cv::LINE_AA);
		}
	}
	else
		std::cout<<"The lines were not detected"<<std::endl;
	if(right_lines.empty()){
		right_lines = temp_right;
		//temp_right.clear();
	}

	if(left_lines.empty()){
		left_lines = temp_left;
		//temp_left.clear();
	}

	//Helper to check the function working
	/*std::cout<<"Right lines below"<<std::endl;
	for(auto j : right_lines){
		std::cout<<j<<std::endl;
	}
	std::cout<<"Left lines below"<<std::endl;
	for(auto k : left_lines){
		std::cout<<k<<std::endl;
	}*/
}

cv::Vec4f lane_detect::fit_line(std::vector<cv::Vec4i> lines){
	cv::Point start;
	cv::Point end;
	std::vector<cv::Point> pts;
	cv::Vec4f line;

	for( std::size_t i = 0; i < lines.size(); i++ ){
  		cv::Vec4i l = lines[i];
  		start = cv::Point(l[0],l[1]);
  		end = cv::Point(l[2],l[3]);
  		pts.push_back(start);
  		pts.push_back(end);
  	}
  	cv::fitLine(pts,line, CV_DIST_L2, 0, 0.01, 0.01);
  	return line;
}

std::vector<cv::Point> lane_detect::lane_extrapolation(cv::Mat src){
	
	cv::Vec4f right_lane;
	cv::Vec4f left_lane;
	std::vector<cv::Point> output(4);

	//if left line is being detected, fit a line
	if(!right_lines.empty()){
		right_lane = fit_line(right_lines);
	} 
	else
		std::cout<<"###########Right lines not detected###########"<<std::endl;
	if(!left_lines.empty()){
		left_lane = fit_line(left_lines);
	} 
	else
		std::cout<<"#############Left lines not detected##########3"<<std::endl;
	right_m = right_lane[1]/right_lane[0];
	right_b = cv::Point(right_lane[2],right_lane[3]);
	left_m = left_lane[1]/left_lane[0];
	left_b = cv::Point(left_lane[2],left_lane[3]);

	int start_y = src.rows;
	int end_y = 470;

	double right_start_x = ((start_y - right_b.y) / right_m) + right_b.x;
  	double right_end_x = ((end_y - right_b.y) / right_m) + right_b.x;

  	double left_start_x = ((start_y - left_b.y) / left_m) + left_b.x;
  	double left_end_x = ((end_y - left_b.y) / left_m) + left_b.x;

  	output[0] = cv::Point(right_start_x, start_y);
  	output[1] = cv::Point(right_end_x, end_y);
  	output[2] = cv::Point(left_start_x, start_y);
  	output[3] = cv::Point(left_end_x, end_y);
 	
 	return output;
}

std::string lane_detect::turn_predict(){
	std::string output;
	double vanish_x;
	double thresh = 8;

	//Vanishing point is the point where both lane lines intersecr
	 vanish_x = ((right_m*right_b.x) - (left_m * left_b.x) - right_b.y + left_b.y)/(right_m - left_m);
	//The vanishing point location decides the road turn/curvature
	if(vanish_x < (img_center - thresh))
		output = "Left Curve";
	else if(vanish_x > (img_center + thresh))
	 	output = "Right Curve";
	else if(vanish_x >= (img_center - thresh) && vanish_x <= (img_center + thresh))
		output = "Straight";

	return output; 
}	

void lane_detect::plot_output(std::vector<cv::Point> lane, std::string turn, cv::Mat img){
	std::vector<cv::Point> poly_points;
  	cv::Mat output;

  	// Create the transparent polygon for a better visualization of the lane
  	img.copyTo(output);
  	poly_points.push_back(lane[2]);
  	poly_points.push_back(lane[0]);
  	poly_points.push_back(lane[1]);
  	poly_points.push_back(lane[3]);
  	cv::fillConvexPoly(img, poly_points, cv::Scalar(0, 255, 0), CV_AA, 0);
  	cv::addWeighted(output, 0.3, img, 1.0 - 0.3, 0, img);

  	// Plot both lines of the lane boundary
  	cv::line(img, lane[0], lane[1], cv::Scalar(0, 0, 255), 5, CV_AA);
  	cv::line(img, lane[2], lane[3], cv::Scalar(0, 0, 255), 5, CV_AA);

  	// Plot the turn message
  	cv::putText(img, turn, cv::Point(50, 90), cv::FONT_HERSHEY_COMPLEX_SMALL, 3, cv::Scalar(255, 0, 0), 1, CV_AA);

  	// Show the final output image
  	cv::namedWindow("Lane", CV_WINDOW_AUTOSIZE);
  	cv::imshow("Lane", img);
  	
  	// Save images to generate video
    //std::string name = "output/output_" + std::to_string(count) + ".png";
    //cv::imwrite(name, img);
    count++;
}