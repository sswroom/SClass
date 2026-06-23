#ifndef _SM_IO_STORAGEDEVICE
#define _SM_IO_STORAGEDEVICE

namespace IO
{
	class StorageDevice
	{
	private:
		void *hand;
		UnsafeArrayOpt<UInt8> storDesc;

		UInt8 lastEffect;

	protected:
		void InitDevice(WChar *devName);
	private:
		Bool GetStorDesc();
	public:
		static Int32 GetNumDevice();

		StorageDevice(Int32 devNo);
		~StorageDevice();

		Bool IsError();
		Bool IsRemovable();
		Bool SupportCommandQueue();
		UnsafeArrayOpt<UTF8Char> GetVendorID(UnsafeArray<UTF8Char> sbuff);
		UnsafeArrayOpt<UTF8Char> GetProductID(UnsafeArray<UTF8Char> sbuff);
		UnsafeArrayOpt<UTF8Char> GetProductRevision(UnsafeArray<UTF8Char> sbuff);
		UnsafeArrayOpt<UTF8Char> GetSerialNumber(UnsafeArray<UTF8Char> sbuff);
		Bool GetDiskGeometry(OutParam<UInt64> cylinder, OutParam<UInt32> trackPerCylinder, OutParam<UInt32> sectorPerTrack, OutParam<UInt32> bytesPerSector);

		Bool SMARTGetVersion(OutParam<UInt8> ver, OutParam<UInt8> rev, OutParam<Bool> supportATAID, OutParam<Bool> supportATAPIID, OutParam<Bool> supportSMART);
		Bool SMARTGetDiskID(UnsafeArray<UInt8> idSector);

		UnsafeArray<UTF8Char> IDSectorGetSN(UnsafeArray<UTF8Char> sbuff, UnsafeArray<UInt8> idSector);
	};
}

#endif
