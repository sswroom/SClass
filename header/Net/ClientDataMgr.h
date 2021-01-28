#ifndef _SM_NET_CLIENTDATAMGR
#define _SM_NET_CLIENTDATAMGR

namespace Net
{
	class ClientDataMgr
	{
	private:
		UInt8 *buff;
		OSInt currDataSize;
		OSInt maxBuffSize;
	public:
		void *userData;

	public:
		ClientDataMgr(Int32 maxBuffSize);
		~ClientDataMgr();

		void AddData(UInt8 *buff, OSInt buffSize);
		UInt8 *GetData(OSInt *dataSize);
		void HandleData(OSInt handleSize);
	};
};
#endif
