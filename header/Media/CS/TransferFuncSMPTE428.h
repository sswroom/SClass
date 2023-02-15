#ifndef _SM_MEDIA_CS_TRANSFERFUNCSMPTE428
#define _SM_MEDIA_CS_TRANSFERFUNCSMPTE428
#include "Media/CS/TransferFunc.h"

namespace Media
{
	namespace CS
	{
		class TransferFuncSMPTE428 : public TransferFunc
		{
		public:
			TransferFuncSMPTE428();
			virtual ~TransferFuncSMPTE428();

			virtual Double ForwardTransfer(Double val);
			virtual Double InverseTransfer(Double val);
		};
	}
}
#endif
