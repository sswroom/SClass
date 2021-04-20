#ifndef _SM_IO_PCIINFO
#define _SM_IO_PCIINFO
#include "Data/ArrayList.h"

namespace IO
{
	class PCIInfo
	{
	private:
		void *clsData;
	public:
		PCIInfo(void *info);
		~PCIInfo();

		UInt16 GetVendorId();
		UInt16 GetProductId();
		const UTF8Char *GetDispName();

		static UOSInt GetPCIList(Data::ArrayList<PCIInfo*> *pciList);
	};
}

#endif
