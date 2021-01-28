#ifndef _SM_MEDIA_CS_TRANSFERFUNCBT1361
#define _SM_MEDIA_CS_TRANSFERFUNCBT1361
#include "Media/CS/TransferFunc.h"

namespace Media
{
	namespace CS
	{
		class TransferFuncBT1361 : public TransferFunc
		{
		public:
			TransferFuncBT1361();
			virtual ~TransferFuncBT1361();

			virtual Double ForwardTransfer(Double val);
			virtual Double InverseTransfer(Double val);
		};
	};
};
#endif
