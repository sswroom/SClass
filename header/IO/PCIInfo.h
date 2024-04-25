#ifndef _SM_IO_PCIINFO
#define _SM_IO_PCIINFO
#include "Data/ArrayList.h"
#include "Text/CString.h"

namespace IO
{
	class PCIInfo
	{
	public:
		struct ClassData;
	private:
		ClassData *clsData;
	public:
		PCIInfo(ClassData *info);
		~PCIInfo();

		UInt16 GetVendorId();
		UInt16 GetProductId();
		Text::CString GetDispName();

		static UOSInt GetPCIList(NN<Data::ArrayList<PCIInfo*>> pciList);
	};
}

#endif
