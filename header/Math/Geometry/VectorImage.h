#ifndef _SM_MATH_GEOMETRY_VECTORIMAGE
#define _SM_MATH_GEOMETRY_VECTORIMAGE
#include "Math/Geometry/Vector2D.h"
#include "Media/SharedImage.h"

namespace Math
{
	namespace Geometry
	{
		class VectorImage : public Math::Geometry::Vector2D
		{
		private:
			NN<Media::SharedImage> img;
			Optional<Text::String> srcAddr;
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
			VectorImage(UInt32 srid, NN<Media::SharedImage> img, Math::Coord2DDbl tl, Math::Coord2DDbl br, Bool scnCoord, Optional<Text::String> srcAddr, Int64 timeStart, Int64 timeEnd);
			VectorImage(UInt32 srid, NN<Media::SharedImage> img, Math::Coord2DDbl tl, Math::Coord2DDbl br, Bool scnCoord, Text::CString srcAddr, Int64 timeStart, Int64 timeEnd);
			VectorImage(UInt32 srid, NN<Media::SharedImage> img, Math::Coord2DDbl tl, Math::Coord2DDbl br, Math::Coord2DDbl size, Bool scnCoord, Optional<Text::String> srcAddr, Int64 timeStart, Int64 timeEnd);
			VectorImage(UInt32 srid, NN<Media::SharedImage> img, Math::Coord2DDbl tl, Math::Coord2DDbl br, Math::Coord2DDbl size, Bool scnCoord, Text::CString srcAddr, Int64 timeStart, Int64 timeEnd);
			virtual ~VectorImage();

			virtual VectorType GetVectorType() const;
			virtual Math::Coord2DDbl GetCenter() const;
			virtual NN<Math::Geometry::Vector2D> Clone() const;
			virtual Math::RectAreaDbl GetBounds() const;
			virtual Double CalBoundarySqrDistance(Math::Coord2DDbl pt, OutParam<Math::Coord2DDbl> nearPt) const;
			virtual Double CalSqrDistance(Math::Coord2DDbl pt, OutParam<Math::Coord2DDbl> nearPt) const;
			virtual Double CalArea() const;
			virtual Bool JoinVector(NN<const Math::Geometry::Vector2D> vec);
			virtual Bool HasZ() const;
			virtual Bool GetZBounds(OutParam<Double> min, OutParam<Double> max) const;
			virtual Bool GetMBounds(OutParam<Double> min, OutParam<Double> max) const;
			virtual void Convert(NN<Math::CoordinateConverter> converter);
			virtual Bool Equals(NN<const Vector2D> vec, Bool sameTypeOnly, Bool nearlyVal) const;
			virtual UOSInt GetCoordinates(NN<Data::ArrayListA<Math::Coord2DDbl>> coordList) const;
			virtual Bool InsideOrTouch(Math::Coord2DDbl coord) const;
			virtual void SwapXY();
			virtual void MultiplyCoordinatesXY(Double v);
			virtual UOSInt GetPointCount() const;

			Optional<Text::String> GetSourceAddr() const;
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
			Math::RectAreaDbl GetScreenBounds(UOSInt scnWidth, UOSInt scnHeight, Double hdpi, Double vdpi) const;
			Math::Size2DDbl GetVectorSize() const;
			Bool IsScnCoord() const;
			void SetBounds(Double minX, Double minY, Double maxX, Double maxY);
			Optional<Media::StaticImage> GetImage(OptOut<UInt32> imgTimeMS) const;
			Optional<Media::StaticImage> GetImage(Double width, Double height, OptOut<UInt32> imgTimeMS) const;

			static NN<Math::Geometry::VectorImage> CreateScreenImage(UInt32 srid, NN<Media::SharedImage> img, Math::Coord2DDbl tl, Math::Coord2DDbl size, Text::CString srcAddr);
		};
	}
}
#endif
