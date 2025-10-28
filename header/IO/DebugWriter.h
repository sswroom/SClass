#ifndef _SM_IO_DEBUGWRITER
#define _SM_IO_DEBUGWRITER
#include "IO/Writer.h"

namespace IO
{
	class DebugWriter : public IO::Writer
	{
	private:
		struct ClassData;
		NN<ClassData> clsData;
	public:
		DebugWriter();
		virtual ~DebugWriter();

		void Flush();

		virtual Bool Write(Text::CStringNN str);
		virtual Bool WriteLine(Text::CStringNN str);
		virtual Bool WriteLine();
	};
}
#endif
