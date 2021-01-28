#ifndef _SM_MEDIA_CS_TRANSFERFUNCNLOG
#define _SM_MEDIA_CS_TRANSFERFUNCNLOG
#include "Media/CS/TransferFunc.h"

namespace Media
{
	namespace CS
	{
		class TransferFuncNLog : public TransferFunc
		{
		public:
			TransferFuncNLog();
			virtual ~TransferFuncNLog();

			virtual Double ForwardTransfer(Double val);
			virtual Double InverseTransfer(Double val);
		};
	};
};
#endif
