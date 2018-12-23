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
* The first step will be to denoise the image by applying a filter like a Gaussian mask that smooths the image and removes any undesired pixel values that could prevent the correct detection of the lanes. The picture below shows how the the filter blurres the same frame as the one above. While our image in currently in RBG format, we should explore whether visualising it in different color spaces such as HSL or HSV helps us in better isolating the lanes. Note that HSV is often referred to as HSB (Hue Saturation and Brightness). As can be seen while comparing images, HSL is better at contrasting lane lines than HSV. HSV is "blurring" our white lines too much, so it would not be suitable for us to opt for it in this case.
<p float="center">
<img width="460" height="300" src="https://github.com/anurag-b/Lane_Detection_ADAS/blob/master/test/gaussian.png" />
<img width="460" height="300" src="https://github.com/anurag-b/Lane_Detection_ADAS/blob/master/test/img_hsl.png" />
</p>

* We first isolate yellow and white from the original image. OpenCV, when converting from RGB to HSL, maps the components of the latter color space in the following range:
	* H is between 0 and 180 (i.e. H = H/2)
	* S & L are between 0 and 255
<p float="center">
<img width="460" height="300" src="https://github.com/anurag-b/Lane_Detection_ADAS/blob/master/test/white_lane.png" />
<img width="460" height="300" src="https://github.com/anurag-b/Lane_Detection_ADAS/blob/master/test/yellow_lane.png" />
</p>
We can observe how the yellow and the white of the lanes are very well isolated. Let's now combine those two masks using an OR operation and then combine with the original image using an AND operation to only retain the intersecting elements.
![bitwise](https://github.com/anurag-b/Lane_Detection_ADAS/blob/master/test/white_yellow_bitwise_lane.png)

* Next, an edge detection algorithm is used to detect the lines that form the boundaries of the lanes. For this, the image has to be converted to grayscale, and then to binary. Once this is done, the edge detection is achieved by applying a row kernel [-1 0 1] to each pixel of the image. This kernel is based on the one used in the Lane Departure Warning System developed by Mathworks.
![edge](https://github.com/anurag-b/Lane_Detection_ADAS/blob/master/test/edge_detect.png)

* As it can be seen in the image above, there are lots of unwanted edges being detected. To fix this, the image will be masked so that it only detects edges in a region of interest. This helps because the road is always going to be in the same location on the image. The following figure shows the result of masking the edges image:
![roi](https://github.com/anurag-b/Lane_Detection_ADAS/blob/master/test/roi.png)

* Once the desired edges are detected and the unwanted ones have been removed, the next step is to use the Hough Lines detection algorithm, which gives the location of all the lines in the image. The parameters of the function (rho and theta) were determined by trial and error so that only the most notable lines are detected. As seen in the image, the most obvious lines are the ones being detected.
![hough](https://github.com/anurag-b/Lane_Detection_ADAS/blob/master/test/hough.png)

* By applying basic linear algebra, all the detected Hough lines will be classified as left side lines or right side lines. The lines are classified depending on the value of their slope and where their initial and final points are approximately located with respect to the center of the image.
* Regression will be then applied to the stored left and right lines clusters to obtain only one line for each side. To accomplish this, the least squares method is used. The initial and final points of the lines are used as the data for the regression. The resultant lines will be the corresponding lane boundaries.
* Lastly, the turn prediction is determined by obtaining the location of the vanishing point (formed by the lane boundary lines) with respect to the center of the image.
* Once everything is detected, the results are represented in the input frame, captured at the begnning of the algorithm.
![result](https://github.com/anurag-b/Lane_Detection_ADAS/blob/master/test/hough.png)

## Result
Video to be added

The code can be found in the src folder.

## Dependencies
The only dependency for this project is OpenCV 3.1.0. To install it, follow the next steps:

```
sudo apt-get install build-essential
sudo apt-get install cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev
sudo apt-get install python-dev python-numpy libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev libjasper-dev libdc1394-22-dev
cd ~/<my_working_directory>
git clone https://github.com/opencv/opencv.git
git clone https://github.com/opencv/opencv_contrib.git
cd ~/opencv
mkdir build
cd build
cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr/local ..
make -j$(nproc)
sudo make install
sudo /bin/bash -c 'echo "/usr/local/lib" > /etc/ld.so.conf.d/opencv.conf'
sudo ldconfig
```

## How to build, run the demo and run the tests
To build and run the demo, follow the next steps (`project_video.mp4` is the video for the demo.):

* To build:
```
git clone https://github.com/anurag-b/Lane_Detection_ADAS.git
cd <path to repository>
mkdir build
cd build
cmake ..
make
```
* Run the demo. The argument of the executable `lanes` is the input video with its full path:
```
cd build
./lane_detect
```
(The saved output .avi demo video is in the output_video folder)

## Doxygen Documentation
To generate Doxygen Documentation in HTML and LaTEX, follow the next steps:

```
cd <path to repository>
mkdir <documentation_folder_name>
cd <documentation_folder_name>
doxygen -g <config_file_name>
```
Inside the configuration file, update:
```
PROJECT_NAME = 'your project name'
INPUT = ../src ../include
```
Run and generate the documents by running the next command:
```
doxygen <config_file_name>
```
## License

MIT License

Copyright (c) 2017 Anurag Bansal

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.