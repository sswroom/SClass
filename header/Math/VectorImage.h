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
		Text::String *srcAddr;
		Math::Coord2DDbl tl;
		Math::Coord2DDbl br;
		Math::Coord2DDbl size;
		Bool scnCoord;
		Bool hasHeight;
		Double height;
		Int64 timeStart;
		Int64 timeEnd;
		Double srcAlpha;
		Bool hasZIndex;
		Int32 zIndex;
		
	public:
		VectorImage(UInt32 srid, Media::SharedImage *img, Math::Coord2DDbl tl, Math::Coord2DDbl br, Bool scnCoord, Text::String *srcAddr, Int64 timeStart, Int64 timeEnd);
		VectorImage(UInt32 srid, Media::SharedImage *img, Math::Coord2DDbl tl, Math::Coord2DDbl br, Bool scnCoord, Text::CString srcAddr, Int64 timeStart, Int64 timeEnd);
		VectorImage(UInt32 srid, Media::SharedImage *img, Math::Coord2DDbl tl, Math::Coord2DDbl br, Math::Coord2DDbl size, Bool scnCoord, Text::String *srcAddr, Int64 timeStart, Int64 timeEnd);
		VectorImage(UInt32 srid, Media::SharedImage *img, Math::Coord2DDbl tl, Math::Coord2DDbl br, Math::Coord2DDbl size, Bool scnCoord, Text::CString srcAddr, Int64 timeStart, Int64 timeEnd);
		virtual ~VectorImage();

		virtual VectorType GetVectorType() const;
		virtual Math::Coord2DDbl GetCenter() const;
		virtual Math::Vector2D *Clone() const;
		virtual void GetBounds(Math::RectAreaDbl *bounds) const;
		virtual Double CalSqrDistance(Math::Coord2DDbl pt, Math::Coord2DDbl *nearPt) const;
		virtual Bool JoinVector(Math::Vector2D *vec);
		virtual Bool HasZ() const;
		virtual void ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys);
		virtual Bool Equals(Vector2D *vec) const;

		Text::String *GetSourceAddr() const;
		void SetHeight(Double height);
		Double GetHeight() const;
		Int64 GetTimeStart() const;
		Int64 GetTimeEnd() const;
		void SetSrcAlpha(Double srcAlpha);
		Bool HasSrcAlpha() const;
		Double GetSrcAlpha() const;
		void SetZIndex(Int32 zIndex);
		Bool HasZIndex() const;
		Int32 GetZIndex() const;
		void GetScreenBounds(UOSInt scnWidth, UOSInt scnHeight, Double hdpi, Double vdpi, Double *x1, Double *y1, Double *x2, Double *y2) const;
		void GetVectorSize(Double *sizeX, Double *sizeY) const;
		Bool IsScnCoord() const;
		void SetBounds(Double minX, Double minY, Double maxX, Double maxY);
		Media::StaticImage *GetImage(UInt32 *imgTimeMS) const;
		Media::StaticImage *GetImage(Double width, Double height, UInt32 *imgTimeMS) const;
	};
}
#endif
