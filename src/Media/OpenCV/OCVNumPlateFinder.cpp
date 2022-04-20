#include "Stdafx.h"
#include "Math/Math.h"
#include "Media/OpenCV/OCVNumPlateFinder.h"
#include <opencv2/imgproc.hpp>

#include <opencv2/highgui.hpp>

//https://circuitdigest.com/microcontroller-projects/license-plate-recognition-using-raspberry-pi-and-opencv

void Media::OpenCV::OCVNumPlateFinder::Find(Media::OpenCV::OCVFrame *frame, PossibleAreaFunc func, void *userObj)
{
	cv::Mat *inp = (cv::Mat *)frame->GetFrame();
	cv::Mat *filtered = new cv::Mat();
	Media::OpenCV::OCVFrame filteredFrame(filtered);
	cv::bilateralFilter(*inp, *filtered, 11, 17, 17);
	cv::Mat edged;
	cv::Canny(*filtered, edged, 16, 200);
    std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Point> c;
	cv::findContours(edged.clone(), contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
	size_t i = 0;
	size_t j = contours.size();
	Double peri;
	std::vector<cv::Point> poly;
	cv::Scalar col(160);
	while (i < j)
	{
		c = contours[i];
		peri = cv::arcLength(c, true);
		cv::approxPolyDP(c, poly, 0.018 * peri, true);
		if (poly.size() == 4)
		{
			Double area = cv::contourArea(poly);
			if (area > 1000)
			{
				Double dir[4];
				dir[0] = Math_ArcTan2(poly[0].y - poly[1].y, poly[0].x - poly[1].x) * 180 / Math::PI;
				dir[1] = Math_ArcTan2(poly[1].y - poly[2].y, poly[1].x - poly[2].x) * 180 / Math::PI;
				dir[2] = Math_ArcTan2(poly[2].y - poly[3].y, poly[2].x - poly[3].x) * 180 / Math::PI;
				dir[3] = Math_ArcTan2(poly[3].y - poly[0].y, poly[3].x - poly[0].x) * 180 / Math::PI;
				Double ang[4];
				ang[0] = dir[0] - dir[1];
				ang[1] = dir[1] - dir[2];
				ang[2] = dir[2] - dir[3];
				ang[3] = dir[3] - dir[0];
				Bool found = false;
				UOSInt k = 4;
				while (k-- > 0)
				{
					if (ang[k] < 0)
					{
						ang[k] += 360;
					}
					if (ang[k] <= 70 || ang[k] >= 290)
					{
						found = true;
						break;
					}
					if (ang[k] >= 110 && ang[k] <= 250)
					{
						found = true;
						break;
					}
				}
				if (!found)
				{
//					printf("Area dir: %lf %lf %lf %lf, ang: %lf, %lf, %lf, %lf\r\n", dir[0], dir[1], dir[2], dir[3], ang[0], ang[1], ang[2], ang[3]);
					UOSInt rect[8];
					rect[0] = (UOSInt)poly[0].x;
					rect[1] = (UOSInt)poly[0].y;
					rect[2] = (UOSInt)poly[1].x;
					rect[3] = (UOSInt)poly[1].y;
					rect[4] = (UOSInt)poly[2].x;
					rect[5] = (UOSInt)poly[2].y;
					rect[6] = (UOSInt)poly[3].x;
					rect[7] = (UOSInt)poly[3].y;
					func(userObj, &filteredFrame, rect);
				}
			}
		}
		i++;
	}
	cv::imshow("Filtered", *filtered);
	cv::imshow("Edged", edged);
	
	cv::waitKey(0);
	cv::destroyAllWindows();
}
