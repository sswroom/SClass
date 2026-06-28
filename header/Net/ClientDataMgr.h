#ifndef _SM_NET_CLIENTDATAMGR
#define _SM_NET_CLIENTDATAMGR

namespace Net
{
	class ClientDataMgr
	{
	private:
		UnsafeArray<UInt8> buff;
		UIntOS currDataSize;
		UIntOS maxBuffSize;
	public:
		void *userData;

	public:
		ClientDataMgr(UIntOS maxBuffSize);
		~ClientDataMgr();

		void AddData(UnsafeArray<UInt8> buff, UIntOS buffSize);
		UnsafeArray<UInt8> GetData(OutParam<UIntOS> dataSize);
		void HandleData(UIntOS handleSize);
	};
}
#endif
