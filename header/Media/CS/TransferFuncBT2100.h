#ifndef _SM_MEDIA_CS_TRANSFERFUNCBT2100
#define _SM_MEDIA_CS_TRANSFERFUNCBT2100
#include "Media/CS/TransferFunc.h"

namespace Media
{
	namespace CS
	{
		class TransferFuncBT2100 : public TransferFunc
		{
		public:
			TransferFuncBT2100();
			virtual ~TransferFuncBT2100();

			virtual Double ForwardTransfer(Double linearVal);
			virtual Double InverseTransfer(Double gammaVal);
		};
	};
};
#endif
