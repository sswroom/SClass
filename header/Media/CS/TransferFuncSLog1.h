#ifndef _SM_MEDIA_CS_TRANSFERFUNCSLOG1
#define _SM_MEDIA_CS_TRANSFERFUNCSLOG1
#include "Media/CS/TransferFunc.h"

namespace Media
{
	namespace CS
	{
		class TransferFuncSLog1 : public TransferFunc
		{
		public:
			TransferFuncSLog1();
			virtual ~TransferFuncSLog1();

			virtual Double ForwardTransfer(Double val);
			virtual Double InverseTransfer(Double val);
		};
	};
};
#endif
