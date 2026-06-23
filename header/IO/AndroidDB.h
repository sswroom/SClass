#ifndef _SM_IO_ANDROIDDB
#define _SM_IO_ANDROIDDB
#include "Text/CString.h"
namespace IO
{
	class AndroidDB
	{
	public:
		typedef struct
		{
			UnsafeArray<const UTF8Char> androidId;
			UIntOS androidIdLen;
			UnsafeArray<const Char> brandName;
			UnsafeArray<const Char> modelName;
			UnsafeArray<const Char> modelNum;
			UnsafeArray<const UTF8Char> cpuName;
			UIntOS cpuNameLen;
		} AndroidInfo;

	private:
		static AndroidInfo androids[];

	public:
		static Optional<const AndroidInfo> GetAndroidInfo(Text::CStringNN androidId);
	};
}
#endif
