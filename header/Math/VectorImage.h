#ifndef _SM_MATH_VECTORIMAGE
#define _SM_MATH_VECTORIMAGE
#include "Math/Vector2D.h"
#include "Media/SharedImage.h"

namespace Math
{
	class VectorImage : public Vector2D
	{
	private:
		Media::SharedImage *img;
		const UTF8Char *srcAddr;
		Double x1;
		Double y1;
		Double x2;
		Double y2;
		Double sizeX;
		Double sizeY;
		Bool scnCoord;
		Bool hasHeight;
		Double height;
		Int64 timeStart;
		Int64 timeEnd;
		Double srcAlpha;
		Bool hasZIndex;
		Int32 zIndex;
		
	public:
		VectorImage(UInt32 srid, Media::SharedImage *img, Double x1, Double y1, Double x2, Double y2, Bool scnCoord, const UTF8Char *srcAddr, Int64 timeStart, Int64 timeEnd);
		VectorImage(UInt32 srid, Media::SharedImage *img, Double x1, Double y1, Double x2, Double y2, Double sizeX, Double sizeY, Bool scnCoord, const UTF8Char *srcAddr, Int64 timeStart, Int64 timeEnd);
		virtual ~VectorImage();

		virtual VectorType GetVectorType();
		virtual void GetCenter(Double *x, Double *y);
		virtual Math::Vector2D *Clone();
		virtual void GetBounds(Double *minX, Double *minY, Double *maxX, Double *maxY);
		virtual Double CalSqrDistance(Double x, Double y, Double *nearPtX, Double *nearPtY);
		virtual Bool JoinVector(Math::Vector2D *vec);
		virtual Bool Support3D();
		virtual void ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys);
		virtual Bool Equals(Vector2D *vec);

		const UTF8Char *GetSourceAddr();
		void SetHeight(Double height);
		Double GetHeight();
		Int64 GetTimeStart();
		Int64 GetTimeEnd();
		void SetSrcAlpha(Double srcAlpha);
		Bool HasSrcAlpha();
		Double GetSrcAlpha();
		void SetZIndex(Int32 zIndex);
		Bool HasZIndex();
		Int32 GetZIndex();
		void GetScreenBounds(UOSInt scnWidth, UOSInt scnHeight, Double hdpi, Double vdpi, Double *x1, Double *y1, Double *x2, Double *y2);
		void GetVectorSize(Double *sizeX, Double *sizeY);
		Bool IsScnCoord();
		void SetBounds(Double minX, Double minY, Double maxX, Double maxY);
		Media::StaticImage *GetImage(UInt32 *imgTimeMS);
		Media::StaticImage *GetImage(Double width, Double height, UInt32 *imgTimeMS);
	};
}
#endif
