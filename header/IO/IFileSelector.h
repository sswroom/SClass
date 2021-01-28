#ifndef _SM_IO_IFILESELECTOR
#define _SM_IO_IFILESELECTOR

namespace IO
{
	class IFileSelector
	{
	public:
		virtual void AddFilter(const UTF8Char *pattern, const UTF8Char *name) = 0;
	};
};
#endif

