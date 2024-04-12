#ifndef _SM_IO_BTMANAGER
#define _SM_IO_BTMANAGER
#include "Data/ArrayListNN.h"
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
		UOSInt CreateControllers(NotNullPtr<Data::ArrayListNN<IO::BTController>> ctrlList);
	};
}
#endif
