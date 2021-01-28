#ifndef _SM_MEDIA_CS_TRANSFERFUNCSLOG2
#define _SM_MEDIA_CS_TRANSFERFUNCSLOG2
#include "Media/CS/TransferFunc.h"

namespace Media
{
	namespace CS
	{
		class TransferFuncSLog2 : public TransferFunc
		{
		public:
			TransferFuncSLog2();
			virtual ~TransferFuncSLog2();

			virtual Double ForwardTransfer(Double val);
			virtual Double InverseTransfer(Double val);
		};
	};
};
#endif
