#ifndef _SM_MATH_GEOJSONWRITER
#define _SM_MATH_GEOJSONWRITER
#include "Math/VectorTextWriter.h"
#include "Text/JSONBuilder.h"

namespace Math
{
	class GeoJSONWriter : public VectorTextWriter
	{
	private:
		Optional<Text::String> lastError;

		void SetLastError(Text::CStringNN lastError);
	public:
		GeoJSONWriter();
		virtual ~GeoJSONWriter();

		virtual Text::CStringNN GetWriterName() const;
		virtual Bool ToText(NN<Text::StringBuilderUTF8> sb, NN<const Math::Geometry::Vector2D> vec);
		virtual Optional<Text::String> GetLastError();
		Bool ToGeometry(NN<Text::JSONBuilder> json, NN<const Math::Geometry::Vector2D> vec);
	};
}
#endif
