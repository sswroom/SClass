#ifndef _SM_MATH_WKTWRITER
#define _SM_MATH_WKTWRITER
#include "Math/VectorTextWriter.h"
#include "Math/Geometry/CompoundCurve.h"
#include "Math/Geometry/CurvePolygon.h"
#include "Math/Geometry/GeometryCollection.h"
#include "Math/Geometry/LineString.h"
#include "Math/Geometry/MultiCurve.h"
#include "Math/Geometry/MultiSurface.h"
#include "Math/Geometry/Polygon.h"
#include "Math/Geometry/Polyline.h"

namespace Math
{
	class WKTWriter : public VectorTextWriter
	{
	private:
		Optional<Text::String> lastError;
		Bool reverseAxis;
		Bool no3D;

		void SetLastError(Text::CStringNN lastError);
		static void AppendLineString(NN<Text::StringBuilderUTF8> sb, NN<Math::Geometry::LineString> pl, Bool reverseAxis, Bool no3D);
		static void AppendPolygon(NN<Text::StringBuilderUTF8> sb, NN<Math::Geometry::Polygon> pg, Bool reverseAxis, Bool no3D);
		static void AppendPolyline(NN<Text::StringBuilderUTF8> sb, NN<Math::Geometry::Polyline> pl, Bool reverseAxis, Bool no3D);
		static void AppendCompoundCurve(NN<Text::StringBuilderUTF8> sb, NN<Math::Geometry::CompoundCurve> cc, Bool reverseAxis, Bool no3D);
		static void AppendCurvePolygon(NN<Text::StringBuilderUTF8> sb, NN<Math::Geometry::CurvePolygon> cpg, Bool reverseAxis, Bool no3D);
		static void AppendMultiPolygon(NN<Text::StringBuilderUTF8> sb, NN<Math::Geometry::MultiPolygon> mpg, Bool reverseAxis, Bool no3D);
		static void AppendMultiSurface(NN<Text::StringBuilderUTF8> sb, NN<Math::Geometry::MultiSurface> ms, Bool reverseAxis, Bool no3D);
		static void AppendMultiCurve(NN<Text::StringBuilderUTF8> sb, NN<Math::Geometry::MultiCurve> mc, Bool reverseAxis, Bool no3D);
		Bool AppendGeometryCollection(NN<Text::StringBuilderUTF8> sb, NN<Math::Geometry::GeometryCollection> geoColl);
	public:
		WKTWriter();
		virtual ~WKTWriter();

		virtual Text::CStringNN GetWriterName() const;
		virtual Bool ToText(NN<Text::StringBuilderUTF8> sb, NN<const Math::Geometry::Vector2D> vec);
		virtual Optional<Text::String> GetLastError();

		void SetReverseAxis(Bool reverseAxis) { this->reverseAxis = reverseAxis; }
		void SetNo3D(Bool no3D) { this->no3D = no3D; }
	};
}
#endif
