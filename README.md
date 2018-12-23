# Lane_Detection_ADAS

##Overview
Identifying lanes on the road is a common task performed by all human drivers to ensure their vehicles are within lane constraints when driving, so as to make sure traffic is smooth and minimize chances of collisions with other cars due to lane misalignment.

Similarly, it is a critical task for an autonomous vehicle to perform. It turns out that recognising lane markings on roads is possible using well known computer vision techniques. So for this project, a lane detection algorithm is proposed as part of the perception component for a self driving vehicle. By using a video feed input of a car driving on the highway, the algorithm will detect where the lane is so that the car can use its location to avoid getting out of the it. It will also be able to predict any turn on the road to secure a good tracking of the lane.

The project was developed using C++, CMake and the OpenCV library. The input video was taken from Self-Driving Car Engineer Nanodegree. The following subsection of the Overview will explain the pipeline of the algorithm step by step.

####Algorithm

######Pipeline
In this part, we will cover in detail the different steps needed to create our pipeline, which will enable us to identify and classify lane lines. The pipeline itself will look as follows:

* Convert original image to HSL
* Isolate yellow and white from HSL image
* Convert image to grayscale for easier manipulation
* Apply Gaussian Blur to smoothen edges
* Apply Canny Edge Detection on smoothed gray image
* Trace Region Of Interest and discard all other lines identified by our previous step that are outside this region
* Perform a Hough Transform to find lanes within our region of interest and trace them in red
* Separate left and right lanes
* Extrapolate them to create two smooth lines

######Steps
* The frames of a video are captured. This image inputs are from a video of a car driving on the highway.
![diagram](https://github.com/anurag-b/Lane_Detection_ADAS/blob/master/test/img102.png)
* 

