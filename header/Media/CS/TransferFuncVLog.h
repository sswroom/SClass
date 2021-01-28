#ifndef _SM_MEDIA_CS_TRANSFERFUNCVLOG
#define _SM_MEDIA_CS_TRANSFERFUNCVLOG
#include "Media/CS/TransferFunc.h"

namespace Media
{
	namespace CS
	{
		class TransferFuncVLog : public TransferFunc
		{
		public:
			TransferFuncVLog();
			virtual ~TransferFuncVLog();

			virtual Double ForwardTransfer(Double val);
			virtual Double InverseTransfer(Double val);
		};
	};
};
#endif
