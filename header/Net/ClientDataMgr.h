#ifndef _SM_NET_CLIENTDATAMGR
#define _SM_NET_CLIENTDATAMGR

namespace Net
{
	class ClientDataMgr
	{
	private:
		UInt8 *buff;
		UIntOS currDataSize;
		UIntOS maxBuffSize;
	public:
		void *userData;

	public:
		ClientDataMgr(UIntOS maxBuffSize);
		~ClientDataMgr();

		void AddData(UInt8 *buff, UIntOS buffSize);
		UInt8 *GetData(UIntOS *dataSize);
		void HandleData(UIntOS handleSize);
	};
}
#endif
