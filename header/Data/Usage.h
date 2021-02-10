#ifndef _SM_DATA_USAGE
#define _SM_DATA_USAGE
#include "Data/Usable.h"

namespace Data
{
	class Usage
	{
	private:
		Data::Usable *usable;

	public:
		Usage(Data::Usable *usable);
		~Usage();

		void EndUse();
	};
}
#endif
