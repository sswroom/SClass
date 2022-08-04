#ifndef _SM_MATH_WKTWRITER
#define _SM_MATH_WKTWRITER
#include "Math/Polygon.h"
#include "Math/Polyline.h"
#include "Math/VectorTextWriter.h"

namespace Math
{
	class WKTWriter : public VectorTextWriter
	{
	private:
		Text::String *lastError;

		void SetLastError(Text::CString lastError);
		static void AppendPolygon(Text::StringBuilderUTF8 *sb, Math::Polygon *pg);
		static void AppendPolyline(Text::StringBuilderUTF8 *sb, Math::Polyline *pl);
		static void AppendPolyline3D(Text::StringBuilderUTF8 *sb, Math::Polyline *pl);
	public:
		WKTWriter();
		virtual ~WKTWriter();

		virtual Text::CString GetWriterName();
		virtual Bool ToText(Text::StringBuilderUTF8 *sb, Math::Vector2D *vec);
		virtual Text::String *GetLastError();
	};
}
#endif
