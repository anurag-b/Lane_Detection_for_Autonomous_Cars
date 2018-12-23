class lane_detect{
private:
	std::vector<cv::Vec4i> left_lines;
	std::vector<cv::Vec4i> right_lines;
	std::vector<cv::Vec4i> temp_right;
	std::vector<cv::Vec4i> temp_left;
	bool left, right;
	double slope;
	double img_center;
	// Members of both line equations of the lane boundaries:
	// y = mx + b
	cv::Point right_b;
  	double right_m;
  	cv::Point left_b;
  	double left_m;
  	int count = 0;

	cv::Mat color_hsl(cv::Mat src);
	cv::Mat color_hsv(cv::Mat src);
	cv::Vec4f fit_line(std::vector<cv::Vec4i> lines);
public:
	cv::Mat preprocess(cv::Mat img_raw);
	cv::Mat edge_detect(cv::Mat src);
	cv::Mat get_roi(cv::Mat src);
	std::vector<cv::Vec4i> hough_lines(cv::Mat src);
	void separate_lines(std::vector<cv::Vec4i> lines, cv::Mat src);
	std::vector<cv::Point> lane_extrapolation(cv::Mat src);
	std::string turn_predict();
	void plot_output(std::vector<cv::Point> lane, std::string turn, cv::Mat img);
};