#ifndef _SM_IO_SDCARDINFO
#define _SM_IO_SDCARDINFO

namespace IO
{
	class SDCardInfo
	{
	private:
		Bool isEMMC;
		UInt8 cid[16];
		UInt8 csd[16];
		const UTF8Char *name;

	public:
		SDCardInfo(const UTF8Char *name, const UInt8 *cid, const UInt8 *csd);
		~SDCardInfo();

		OSInt GetCID(UInt8 *cid);
		OSInt GetCSD(UInt8 *csd);

		const UTF8Char *GetName();
		// CID
		UInt8 GetManufacturerID();
		UInt16 GetOEMID();
		Char *GetProductName(Char *name);
		UInt8 GetProductRevision();
		UInt32 GetSerialNo();
		UInt32 GetManufacturingYear();
		UInt8 GetManufacturingMonth();

		//CSD
		UInt8 GetCSDVersion();
		UInt8 GetTAAC();
		UInt8 GetNSAC();
		Int64 GetMaxTranRate(); //bit/s
		UInt16 GetCardCmdClass();
		UInt8 GetMaxReadBlkLen();
		Bool GetBlockReadPartial();
		Bool GetWriteBlockMisalign();
		Bool GetReadBlockMisalign();
		Bool GetDSRImplemented();
		UInt32 GetDeviceSize();
		UInt8 GetMaxVDDReadCurr();
		UInt8 GetMinVDDReadCurr();
		UInt8 GetMaxVDDWriteCurr();
		UInt8 GetMinVDDWriteCurr();
		UInt8 GetDeviceSizeMulitply();
		Bool GetEraseSingleBlockEnabled();
		UInt8 GetEraseSectorSize();
		UInt8 GetWriteProtectGroupSize();
		Bool GetWriteProtectGroupEnabled();
		UInt8 GetWriteSpeedFactor();
		UInt8 GetMaxWriteBlockLen();
		Bool GetBlockWritePartial();
		Bool GetFileFormatGroup();
		Bool GetCopyFlag();
		Bool GetPermWriteProtect();
		Bool GetTmpWriteProtect();
		UInt8 GetFileFormat();

		Int64 GetCardCapacity();

		static const UTF8Char *GetManufacturerName(UInt8 mid);
	};
};
#endif
