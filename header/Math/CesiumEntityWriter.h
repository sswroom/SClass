#ifndef _SM_MATH_CESIUMENTITYWRITER
#define _SM_MATH_CESIUMENTITYWRITER
#include "Math/VectorTextWriter.h"

namespace Math
{
	class CesiumEntityWriter : public VectorTextWriter
	{
	private:
		Optional<Text::String> lastError;

		void SetLastError(Text::CStringNN lastError);
	public:
		CesiumEntityWriter();
		virtual ~CesiumEntityWriter();

		virtual Text::CStringNN GetWriterName() const;
		virtual Bool ToText(NN<Text::StringBuilderUTF8> sb, NN<const Math::Geometry::Vector2D> vec);
		virtual Optional<Text::String> GetLastError();
	};
}
#endif
