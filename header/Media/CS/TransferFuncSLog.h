#ifndef _SM_MEDIA_CS_TRANSFERFUNCSLOG
#define _SM_MEDIA_CS_TRANSFERFUNCSLOG
#include "Media/CS/TransferFunc.h"

namespace Media
{
	namespace CS
	{
		class TransferFuncSLog : public TransferFunc
		{
		public:
			TransferFuncSLog();
			virtual ~TransferFuncSLog();

			virtual Double ForwardTransfer(Double val);
			virtual Double InverseTransfer(Double val);
		};
	};
};
#endif
