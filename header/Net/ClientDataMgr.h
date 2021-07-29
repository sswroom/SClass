#ifndef _SM_NET_CLIENTDATAMGR
#define _SM_NET_CLIENTDATAMGR

namespace Net
{
	class ClientDataMgr
	{
	private:
		UInt8 *buff;
		UOSInt currDataSize;
		UOSInt maxBuffSize;
	public:
		void *userData;

	public:
		ClientDataMgr(UOSInt maxBuffSize);
		~ClientDataMgr();

		void AddData(UInt8 *buff, UOSInt buffSize);
		UInt8 *GetData(UOSInt *dataSize);
		void HandleData(UOSInt handleSize);
	};
}
#endif
