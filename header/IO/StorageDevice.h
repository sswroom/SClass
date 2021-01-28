#ifndef _SM_IO_STORAGEDEVICE
#define _SM_IO_STORAGEDEVICE

namespace IO
{
	class StorageDevice
	{
	private:
		void *hand;
		UInt8 *storDesc;

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
		UTF8Char *GetVendorID(UTF8Char *sbuff);
		UTF8Char *GetProductID(UTF8Char *sbuff);
		UTF8Char *GetProductRevision(UTF8Char *sbuff);
		UTF8Char *GetSerialNumber(UTF8Char *sbuff);
		Bool GetDiskGeometry(UInt64 *cylinder, UInt32 *trackPerCylinder, UInt32 *sectorPerTrack, UInt32 *bytesPerSector);

		Bool SMARTGetVersion(UInt8 *ver, UInt8 *rev, Bool *supportATAID, Bool *supportATAPIID, Bool *supportSMART);
		Bool SMARTGetDiskID(UInt8 *idSector);

		UTF8Char *IDSectorGetSN(UTF8Char *sbuff, UInt8 *idSector);
	};
};

#endif
