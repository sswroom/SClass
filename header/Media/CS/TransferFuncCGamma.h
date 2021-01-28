#ifndef _SM_MEDIA_CS_TRANSFERFUNCCGAMMA
#define _SM_MEDIA_CS_TRANSFERFUNCCGAMMA
#include "Media/CS/TransferFunc.h"

namespace Media
{
	namespace CS
	{
		class TransferFuncCGamma : public TransferFunc
		{
		public:
			TransferFuncCGamma(Double rgbGamma);
			virtual ~TransferFuncCGamma();

			virtual Double ForwardTransfer(Double val);
			virtual Double InverseTransfer(Double val);
		};
	};
};
#endif
