#ifndef _SM_MATH_WKTWRITER
#define _SM_MATH_WKTWRITER
#include "Math/VectorTextWriter.h"
#include "Math/Geometry/CompoundCurve.h"
#include "Math/Geometry/CurvePolygon.h"
#include "Math/Geometry/GeometryCollection.h"
#include "Math/Geometry/LineString.h"
#include "Math/Geometry/MultiSurface.h"
#include "Math/Geometry/Polygon.h"
#include "Math/Geometry/Polyline.h"

namespace Math
{
	class WKTWriter : public VectorTextWriter
	{
	private:
		Text::String *lastError;
		Bool reverseAxis;

		void SetLastError(Text::CString lastError);
		static void AppendLineString(NN<Text::StringBuilderUTF8> sb, NN<Math::Geometry::LineString> pl, Bool reverseAxis);
		static void AppendPolygon(NN<Text::StringBuilderUTF8> sb, NN<Math::Geometry::Polygon> pg, Bool reverseAxis);
		static void AppendPolyline(NN<Text::StringBuilderUTF8> sb, NN<Math::Geometry::Polyline> pl, Bool reverseAxis);
		static void AppendCompoundCurve(NN<Text::StringBuilderUTF8> sb, NN<Math::Geometry::CompoundCurve> cc, Bool reverseAxis);
		static void AppendCurvePolygon(NN<Text::StringBuilderUTF8> sb, NN<Math::Geometry::CurvePolygon> cpg, Bool reverseAxis);
		static void AppendMultiPolygon(NN<Text::StringBuilderUTF8> sb, NN<Math::Geometry::MultiPolygon> mpg, Bool reverseAxis);
		static void AppendMultiSurface(NN<Text::StringBuilderUTF8> sb, NN<Math::Geometry::MultiSurface> ms, Bool reverseAxis);
		Bool AppendGeometryCollection(NN<Text::StringBuilderUTF8> sb, NN<Math::Geometry::GeometryCollection> geoColl);
	public:
		WKTWriter();
		virtual ~WKTWriter();

		virtual Text::CStringNN GetWriterName() const;
		virtual Bool ToText(NN<Text::StringBuilderUTF8> sb, NN<const Math::Geometry::Vector2D> vec);
		virtual Text::String *GetLastError();

		void SetReverseAxis(Bool reverseAxis);
	};
}
#endif
