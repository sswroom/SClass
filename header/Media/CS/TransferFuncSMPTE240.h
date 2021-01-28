#ifndef _SM_MEDIA_CS_TRANSFERFUNCSMPTE240
#define _SM_MEDIA_CS_TRANSFERFUNCSMPTE240
#include "Media/CS/TransferFunc.h"

namespace Media
{
	namespace CS
	{
		class TransferFuncSMPTE240 : public TransferFunc
		{
		public:
			TransferFuncSMPTE240();
			virtual ~TransferFuncSMPTE240();

			virtual Double ForwardTransfer(Double val);
			virtual Double InverseTransfer(Double val);
		};
	};
};
#endif
