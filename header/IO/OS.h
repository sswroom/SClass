#ifndef _SM_IO_OS
#define _SM_IO_OS

namespace IO
{
	class OS
	{
	public:
		static UTF8Char *GetDistro(UTF8Char *sbuff);
		static UTF8Char *GetVersion(UTF8Char *sbuff);
		static UOSInt GetBuildNumber();
	};
};
#endif
