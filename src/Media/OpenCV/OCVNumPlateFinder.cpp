#include "Stdafx.h"
#include "Math/Math.h"
#include "Media/OpenCV/OCVNumPlateFinder.h"
#include <opencv2/imgproc.hpp>

//#define VERBOSE
#if defined(VERBOSE)
#include <opencv2/highgui.hpp>
#endif

//https://circuitdigest.com/microcontroller-projects/license-plate-recognition-using-raspberry-pi-and-opencv

Media::OpenCV::OCVNumPlateFinder::OCVNumPlateFinder()
{
	this->maxTileAngle = 30;
	this->minArea = 2000;
	this->maxArea = 20000;
}

Media::OpenCV::OCVNumPlateFinder::~OCVNumPlateFinder()
{
}

void Media::OpenCV::OCVNumPlateFinder::SetMaxTileAngle(Double maxTileAngleDegree)
{
	this->maxTileAngle = maxTileAngleDegree;
}

void Media::OpenCV::OCVNumPlateFinder::SetAreaRange(Double minArea, Double maxArea)
{
	this->minArea = minArea;
	this->maxArea = maxArea;
}

void Media::OpenCV::OCVNumPlateFinder::Find(Media::OpenCV::OCVFrame *frame, PossibleAreaFunc func, void *userObj)
{
	cv::Mat *inp = (cv::Mat *)frame->GetFrame();
	cv::Mat *filtered = new cv::Mat();
	Media::OpenCV::OCVFrame filteredFrame(filtered);
	cv::bilateralFilter(*inp, *filtered, 11, 17, 17);
	cv::Mat edged;
	cv::Canny(*filtered, edged, 20, 200);
    std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Point> c;
	cv::findContours(edged.clone(), contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
	size_t i = 0;
	size_t j = contours.size();
	Double peri;
	std::vector<cv::Point> poly;
	cv::Scalar col(160);
	Double v1 = 90 - this->maxTileAngle;
	Double v2 = 90 + this->maxTileAngle;
	Double v3 = 270 - this->maxTileAngle;
	Double v4 = 270 + this->maxTileAngle;
	Double epsilon = 0.018; //0.018
	while (i < j)
	{
		c = contours[i];
		peri = cv::arcLength(c, true);
		cv::approxPolyDP(c, poly, epsilon * peri, true);
		if (poly.size() == 4)
		{
			Double area = cv::contourArea(poly);
			if (area >= this->minArea && area <= this->maxArea)
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
				Double leng[4];
				leng[0] = Math_Sqrt((poly[0].x - poly[1].x) * (poly[0].x - poly[1].x) + (poly[0].y - poly[1].y) * (poly[0].y - poly[1].y));
				leng[1] = Math_Sqrt((poly[1].x - poly[2].x) * (poly[1].x - poly[2].x) + (poly[1].y - poly[2].y) * (poly[1].y - poly[2].y));
				leng[2] = Math_Sqrt((poly[2].x - poly[3].x) * (poly[2].x - poly[3].x) + (poly[2].y - poly[3].y) * (poly[2].y - poly[3].y));
				leng[3] = Math_Sqrt((poly[3].x - poly[0].x) * (poly[3].x - poly[0].x) + (poly[3].y - poly[0].y) * (poly[3].y - poly[0].y));
				Double tiltAngle;
				Double maxTiltAngle = 0;
				Bool found = false;
				UOSInt k = 4;
				while (k-- > 0)
				{
					if (ang[k] < 0)
					{
						ang[k] += 360;
					}
					if (ang[k] <= v1 || ang[k] >= v4)
					{
						found = true;
						break;
					}
					if (ang[k] >= v2 && ang[k] <= v3)
					{
						found = true;
						break;
					}
					if (ang[k] < 180)
					{
						tiltAngle = ang[k] - 90;
					}
					else
					{
						tiltAngle = ang[k] - 270;
					}
					if (tiltAngle < 0)
					{
						tiltAngle = -tiltAngle;
					}
					if (maxTileAngle < tiltAngle)
					{
						maxTileAngle = tiltAngle;
					}
				}
				PlateSize pSize = PlateSize::SingleRow;
				if (!found)
				{
					found = true;
					Double lengRatio = leng[0] / leng[1];
					if (lengRatio >= 3.5 && lengRatio <= 4.5)
					{
						pSize = PlateSize::SingleRow;
						found = false;
					}
					else if (lengRatio >= 0.2 && lengRatio <= 0.3)
					{
						pSize = PlateSize::SingleRow;
						found = false;
					}
					else if (lengRatio >= 1.7 && lengRatio <= 2.1)
					{
						pSize = PlateSize::DoubleRow;
						found = false;
					}
					else if (lengRatio >= 0.45 && lengRatio <= 0.65)
					{
						pSize = PlateSize::DoubleRow;
						found = false;
					}
				}
				if (!found)
				{
//					std::vector<std::vector<cv::Point> > drawPoly;
//					drawPoly.push_back(poly);
//					cv::drawContours(edged, drawPoly, 0, cv::Scalar(160));
//					printf("Area dir: %lf %lf %lf %lf, ang: %lf, %lf, %lf, %lf\r\n", dir[0], dir[1], dir[2], dir[3], ang[0], ang[1], ang[2], ang[3]);
//					printf("Area leng: %lf %lf %lf %lf, ratio = %lf\r\n", leng[0], leng[1], leng[2], leng[3], leng[0] / leng[1]);
					Math::Coord2D<UOSInt> rect[4];
					rect[0] = Math::Coord2D<UOSInt>((UOSInt)poly[0].x, (UOSInt)poly[0].y);
					rect[1] = Math::Coord2D<UOSInt>((UOSInt)poly[1].x, (UOSInt)poly[1].y);
					rect[2] = Math::Coord2D<UOSInt>((UOSInt)poly[2].x, (UOSInt)poly[2].y);
					rect[3] = Math::Coord2D<UOSInt>((UOSInt)poly[3].x, (UOSInt)poly[3].y);
					func(userObj, &filteredFrame, rect, maxTileAngle, area, pSize);
				}
			}
		}
		i++;
	}
#if defined(VERBOSE)
	cv::imshow("Filtered", *filtered);
	cv::imshow("Edged", edged);
	
//	cv::waitKey(0);
//	cv::destroyAllWindows();
#endif
}
