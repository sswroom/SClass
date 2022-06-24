#ifndef _SM_MATH_GEOJSONWRITER
#define _SM_MATH_GEOJSONWRITER
#include "Math/VectorTextWriter.h"

namespace Math
{
	class GeoJSONWriter : public VectorTextWriter
	{
	private:
		Text::String *lastError;

		void SetLastError(Text::CString lastError);
	public:
		GeoJSONWriter();
		virtual ~GeoJSONWriter();

		virtual Text::CString GetWriterName();
		virtual Bool ToText(Text::StringBuilderUTF8 *sb, Math::Vector2D *vec);
		virtual Text::String *GetLastError();
		Bool ToGeometry(Text::StringBuilderUTF8 *sb, Math::Vector2D *vec);
	};
}
#endif
