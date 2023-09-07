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

		virtual Text::CStringNN GetWriterName() const;
		virtual Bool ToText(NotNullPtr<Text::StringBuilderUTF8> sb, Math::Geometry::Vector2D *vec);
		virtual Text::String *GetLastError();
		Bool ToGeometry(NotNullPtr<Text::StringBuilderUTF8> sb, Math::Geometry::Vector2D *vec);
	};
}
#endif
