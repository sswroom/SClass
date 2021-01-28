#ifndef _SM_MEDIA_CS_TRANSFERFUNCLOGSQRT10
#define _SM_MEDIA_CS_TRANSFERFUNCLOGSQRT10
#include "Media/CS/TransferFunc.h"

namespace Media
{
	namespace CS
	{
		class TransferFuncLogSqrt10 : public TransferFunc
		{
		public:
			TransferFuncLogSqrt10();
			virtual ~TransferFuncLogSqrt10();

			virtual Double ForwardTransfer(Double val);
			virtual Double InverseTransfer(Double val);
		};
	};
};
#endif
