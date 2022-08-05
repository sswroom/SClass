#ifndef _SM_MATH_WKTWRITER
#define _SM_MATH_WKTWRITER
#include "Math/VectorTextWriter.h"
#include "Math/Geometry/Polygon.h"
#include "Math/Geometry/Polyline.h"

namespace Math
{
	class WKTWriter : public VectorTextWriter
	{
	private:
		Text::String *lastError;

		void SetLastError(Text::CString lastError);
		static void AppendPolygon(Text::StringBuilderUTF8 *sb, Math::Geometry::Polygon *pg);
		static void AppendPolyline(Text::StringBuilderUTF8 *sb, Math::Geometry::Polyline *pl);
		static void AppendPolyline3D(Text::StringBuilderUTF8 *sb, Math::Geometry::Polyline *pl);
	public:
		WKTWriter();
		virtual ~WKTWriter();

		virtual Text::CString GetWriterName();
		virtual Bool ToText(Text::StringBuilderUTF8 *sb, Math::Geometry::Vector2D *vec);
		virtual Text::String *GetLastError();
	};
}
#endif
