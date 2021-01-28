#ifndef _SM_MEDIA_CS_TRANSFERFUNCLINEAR
#define _SM_MEDIA_CS_TRANSFERFUNCLINEAR
#include "Media/CS/TransferFunc.h"

namespace Media
{
	namespace CS
	{
		class TransferFuncLinear : public TransferFunc
		{
		public:
			TransferFuncLinear();
			virtual ~TransferFuncLinear();

			virtual Double ForwardTransfer(Double val);
			virtual Double InverseTransfer(Double val);
		};
	};
};
#endif
