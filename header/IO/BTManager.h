#ifndef _SM_IO_BTMANAGER
#define _SM_IO_BTMANAGER
#include "Data/ArrayListNN.hpp"
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
		UIntOS CreateControllers(NN<Data::ArrayListNN<IO::BTController>> ctrlList);
	};
}
#endif
