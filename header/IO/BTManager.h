#ifndef _SM_IO_BTMANAGER
#define _SM_IO_BTMANAGER
#include "Data/ArrayList.h"
#include "IO/BTController.h"

namespace IO
{
	class BTManager
	{
	private:
		void *internalData;
	public:
		BTManager();
		~BTManager();
	public:
		OSInt CreateControllers(Data::ArrayList<IO::BTController*> *ctrlList);
	};
}
#endif
