#ifndef _SM_MEDIA_CS_TRANSFERFUNCPROTUNE
#define _SM_MEDIA_CS_TRANSFERFUNCPROTUNE
#include "Media/CS/TransferFunc.h"

namespace Media
{
	namespace CS
	{
		class TransferFuncProtune : public TransferFunc
		{
		public:
			TransferFuncProtune();
			virtual ~TransferFuncProtune();

			virtual Double ForwardTransfer(Double val);
			virtual Double InverseTransfer(Double val);
		};
	};
};
#endif
