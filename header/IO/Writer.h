#ifndef _SM_IO_WRITER
#define _SM_IO_WRITER

namespace IO
{
	class Writer
	{
	public:
		virtual ~Writer() {};

		virtual Bool Write(const UTF8Char *str, UOSInt nChar) = 0;
		virtual Bool Write(const UTF8Char *str) = 0;
		virtual Bool WriteLine(const UTF8Char *str, UOSInt nChar) = 0;
		virtual Bool WriteLine(const UTF8Char *str) = 0;
		virtual Bool WriteLine() = 0;
	};
}
#endif
