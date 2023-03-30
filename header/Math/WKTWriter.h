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
		static void AppendLineString(Text::StringBuilderUTF8 *sb, Math::Geometry::LineString *pl, Bool reverseAxis);
		static void AppendPolygon(Text::StringBuilderUTF8 *sb, Math::Geometry::Polygon *pg, Bool reverseAxis);
		static void AppendPolygonZ(Text::StringBuilderUTF8 *sb, Math::Geometry::Polygon *pg, Bool reverseAxis);
		static void AppendPolyline(Text::StringBuilderUTF8 *sb, Math::Geometry::Polyline *pl, Bool reverseAxis);
		static void AppendPolylineZ(Text::StringBuilderUTF8 *sb, Math::Geometry::Polyline *pl, Bool reverseAxis);
		static void AppendPolylineZM(Text::StringBuilderUTF8 *sb, Math::Geometry::Polyline *pl, Bool reverseAxis);
		static void AppendCompoundCurve(Text::StringBuilderUTF8 *sb, Math::Geometry::CompoundCurve *cc, Bool reverseAxis);
		static void AppendCurvePolygon(Text::StringBuilderUTF8 *sb, Math::Geometry::CurvePolygon *cpg, Bool reverseAxis);
		static void AppendMultiSurface(Text::StringBuilderUTF8 *sb, Math::Geometry::MultiSurface *ms, Bool reverseAxis);
		Bool AppendGeometryCollection(Text::StringBuilderUTF8 *sb, Math::Geometry::GeometryCollection *geoColl);
	public:
		WKTWriter();
		virtual ~WKTWriter();

		virtual Text::CString GetWriterName();
		virtual Bool ToText(Text::StringBuilderUTF8 *sb, Math::Geometry::Vector2D *vec);
		virtual Text::String *GetLastError();

		void SetReverseAxis(Bool reverseAxis);
	};
}
#endif
