#ifndef _SM_IO_FILEMODE
#define _SM_IO_FILEMODE

namespace IO
{
	enum class FileMode
	{
		Create,
		Append,
		ReadOnly,
		ReadWriteExisting,
		Device,
		CreateWrite
	};
	
	enum class FileShare
	{
		DenyNone,
		DenyRead,
		DenyWrite,
		DenyAll
	};
}
#endif
