#ifndef _SM_NET_NETBIOSUTIL
#define _SM_NET_NETBIOSUTIL

namespace Net
{
	class NetBIOSUtil
	{
	public:
		static UTF8Char *GetName(UTF8Char *sbuff, const UTF8Char *nbName);
		static UTF8Char *SetName(UTF8Char *nbBuff, const UTF8Char *name);
		static const UTF8Char *NameTypeGetName(UInt8 nameType);
	};
}
#endif