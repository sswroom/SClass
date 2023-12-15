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
		static void AppendLineString(NotNullPtr<Text::StringBuilderUTF8> sb, NotNullPtr<Math::Geometry::LineString> pl, Bool reverseAxis);
		static void AppendPolygon(NotNullPtr<Text::StringBuilderUTF8> sb, NotNullPtr<Math::Geometry::Polygon> pg, Bool reverseAxis);
		static void AppendPolyline(NotNullPtr<Text::StringBuilderUTF8> sb, NotNullPtr<Math::Geometry::Polyline> pl, Bool reverseAxis);
		static void AppendCompoundCurve(NotNullPtr<Text::StringBuilderUTF8> sb, NotNullPtr<Math::Geometry::CompoundCurve> cc, Bool reverseAxis);
		static void AppendCurvePolygon(NotNullPtr<Text::StringBuilderUTF8> sb, NotNullPtr<Math::Geometry::CurvePolygon> cpg, Bool reverseAxis);
		static void AppendMultiPolygon(NotNullPtr<Text::StringBuilderUTF8> sb, NotNullPtr<Math::Geometry::MultiPolygon> mpg, Bool reverseAxis);
		static void AppendMultiSurface(NotNullPtr<Text::StringBuilderUTF8> sb, NotNullPtr<Math::Geometry::MultiSurface> ms, Bool reverseAxis);
		Bool AppendGeometryCollection(NotNullPtr<Text::StringBuilderUTF8> sb, NotNullPtr<Math::Geometry::GeometryCollection> geoColl);
	public:
		WKTWriter();
		virtual ~WKTWriter();

		virtual Text::CStringNN GetWriterName() const;
		virtual Bool ToText(NotNullPtr<Text::StringBuilderUTF8> sb, NotNullPtr<const Math::Geometry::Vector2D> vec);
		virtual Text::String *GetLastError();

		void SetReverseAxis(Bool reverseAxis);
	};
}
#endif
