#ifndef _SM_IO_SDCARDINFO
#define _SM_IO_SDCARDINFO
#include "Text/CString.h"
#include "Text/String.h"

namespace IO
{
	class SDCardInfo
	{
	private:
		Bool isEMMC;
		UInt8 cid[16];
		UInt8 csd[16];
		NN<Text::String> name;

	public:
		SDCardInfo(Text::CString name, const UInt8 *cid, const UInt8 *csd);
		~SDCardInfo();

		OSInt GetCID(UInt8 *cid);
		OSInt GetCSD(UInt8 *csd);

		NN<Text::String> GetName() const;
		// CID
		UInt8 GetManufacturerID();
		UInt16 GetOEMID();
		Char *GetProductName(Char *name);
		UInt8 GetProductRevision();
		UInt32 GetSerialNo();
		Int32 GetManufacturingYear();
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

		static Text::CString GetManufacturerName(UInt8 mid);
	};
}
#endif
