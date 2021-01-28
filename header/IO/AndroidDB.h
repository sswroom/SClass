#ifndef _SM_IO_ANDROIDDB
#define _SM_IO_ANDROIDDB

namespace IO
{
	class AndroidDB
	{
	public:
		typedef struct
		{
			const Char *androidId;
			const Char *brandName;
			const Char *modelName;
			const Char *modelNum;
			const Char *cpuName;
		} AndroidInfo;

	private:
		static AndroidInfo androids[];

	public:
		static const AndroidInfo *GetAndroidInfo(const UTF8Char *androidId);
	};
}
#endif
