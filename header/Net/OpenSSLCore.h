#ifndef _SM_NET_OPENSSLCORE
#define _SM_NET_OPENSSLCORE

namespace Net
{
	class OpenSSLCore
	{
	private:
		static Int32 useCnt;
	public:
		static void Init();
		static void Deinit();
	};
}
#endif
