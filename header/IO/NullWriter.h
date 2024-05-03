#ifndef _SM_IO_NULLWRITER
#define _SM_IO_NULLWRITER
#include "IO/Writer.h"

namespace IO
{
	class NullWriter : public IO::Writer
	{
	public:
		NullWriter();
		virtual ~NullWriter();

		virtual Bool Write(Text::CStringNN str);
		virtual Bool WriteLine(Text::CStringNN str);
		virtual Bool WriteLine();
	};
}
#endif
