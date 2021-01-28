#ifndef _SM_MEDIA_CS_TRANSFERFUNCNTSC
#define _SM_MEDIA_CS_TRANSFERFUNCNTSC
#include "Media/CS/TransferFunc.h"

namespace Media
{
	namespace CS
	{
		class TransferFuncNTSC : public TransferFunc
		{
		public:
			TransferFuncNTSC();
			virtual ~TransferFuncNTSC();

			virtual Double ForwardTransfer(Double val);
			virtual Double InverseTransfer(Double val);
		};
	};
};
#endif
