#ifndef _SM_IO_IWRITER
#define _SM_IO_IWRITER


namespace IO
{
	class IWriter
	{
	public:
		virtual ~IWriter() {};

		virtual Bool Write(const UTF8Char *str, UOSInt nChar) = 0;
		virtual Bool Write(const UTF8Char *str) = 0;
		virtual Bool WriteLine(const UTF8Char *str, UOSInt nChar) = 0;
		virtual Bool WriteLine(const UTF8Char *str) = 0;
		virtual Bool WriteLine() = 0;
	};
}
#endif
