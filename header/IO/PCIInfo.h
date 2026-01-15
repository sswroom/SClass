#ifndef _SM_IO_PCIINFO
#define _SM_IO_PCIINFO
#include "Data/ArrayListNN.hpp"
#include "Text/CString.h"

namespace IO
{
	class PCIInfo
	{
	public:
		struct ClassData;
	private:
		NN<ClassData> clsData;
	public:
		PCIInfo(NN<ClassData> info);
		~PCIInfo();

		UInt16 GetVendorId();
		UInt16 GetProductId();
		Text::CStringNN GetDispName();

		static UIntOS GetPCIList(NN<Data::ArrayListNN<PCIInfo>> pciList);
	};
}

#endif
