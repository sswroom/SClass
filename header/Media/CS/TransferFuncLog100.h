#ifndef _SM_MEDIA_CS_TRANSFERFUNCLOG100
#define _SM_MEDIA_CS_TRANSFERFUNCLOG100
#include "Media/CS/TransferFunc.h"

namespace Media
{
	namespace CS
	{
		class TransferFuncLog100 : public TransferFunc
		{
		public:
			TransferFuncLog100();
			virtual ~TransferFuncLog100();

			virtual Double ForwardTransfer(Double val);
			virtual Double InverseTransfer(Double val);
		};
	};
};
#endif
