#ifndef _SM_MEDIA_CS_TRANSFERFUNCHLG
#define _SM_MEDIA_CS_TRANSFERFUNCHLG
#include "Media/CS/TransferFunc.h"

namespace Media
{
	namespace CS
	{
		class TransferFuncHLG : public TransferFunc
		{
		public:
			TransferFuncHLG();
			virtual ~TransferFuncHLG();

			virtual Double ForwardTransfer(Double linearVal);
			virtual Double InverseTransfer(Double gammaVal);
		};
	};
};
#endif
