#ifndef _SM_MEDIA_CS_TRANSFERFUNCSLOG3
#define _SM_MEDIA_CS_TRANSFERFUNCSLOG3
#include "Media/CS/TransferFunc.h"

namespace Media
{
	namespace CS
	{
		class TransferFuncSLog3 : public TransferFunc
		{
		public:
			TransferFuncSLog3();
			virtual ~TransferFuncSLog3();

			virtual Double ForwardTransfer(Double val);
			virtual Double InverseTransfer(Double val);
		};
	};
};
#endif
