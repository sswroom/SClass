#ifndef _SM_DATA_USAGE
#define _SM_DATA_USAGE
#include "Data/Usable.h"

namespace Data
{
	class Usage
	{
	private:
		Optional<Data::Usable> usable;

	public:
		Usage(Optional<Data::Usable> usable);
		~Usage();

		void EndUse();
	};
}
#endif
