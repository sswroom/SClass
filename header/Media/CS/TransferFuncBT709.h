#ifndef _SM_MEDIA_CS_TRANSFERFUNCBT709
#define _SM_MEDIA_CS_TRANSFERFUNCBT709
#include "Media/CS/TransferFunc.h"

namespace Media
{
	namespace CS
	{
		class TransferFuncBT709 : public TransferFunc
		{
		public:
			TransferFuncBT709();
			virtual ~TransferFuncBT709();

			virtual Double ForwardTransfer(Double val);
			virtual Double InverseTransfer(Double val);
		};
	};
};
#endif
