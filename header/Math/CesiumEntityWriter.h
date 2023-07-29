#ifndef _SM_MATH_CESIUMENTITYWRITER
#define _SM_MATH_CESIUMENTITYWRITER
#include "Math/VectorTextWriter.h"

namespace Math
{
	class CesiumEntityWriter : public VectorTextWriter
	{
	private:
		Text::String *lastError;

		void SetLastError(Text::CString lastError);
	public:
		CesiumEntityWriter();
		virtual ~CesiumEntityWriter();

		virtual Text::CString GetWriterName();
		virtual Bool ToText(NotNullPtr<Text::StringBuilderUTF8> sb, Math::Geometry::Vector2D *vec);
		virtual Text::String *GetLastError();
	};
}
#endif
