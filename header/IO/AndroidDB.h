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
			const UTF8Char *androidId;
			UIntOS androidIdLen;
			const Char *brandName;
			const Char *modelName;
			const Char *modelNum;
			const UTF8Char *cpuName;
			UIntOS cpuNameLen;
		} AndroidInfo;

	private:
		static AndroidInfo androids[];

	public:
		static const AndroidInfo *GetAndroidInfo(Text::CStringNN androidId);
	};
}
#endif
