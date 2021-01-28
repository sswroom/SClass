#ifndef _SM_MEDIA_CS_TRANSFERFUNCSRGB
#define _SM_MEDIA_CS_TRANSFERFUNCSRGB
#include "Media/CS/TransferFunc.h"

namespace Media
{
	namespace CS
	{
		class TransferFuncSRGB : public TransferFunc
		{
		public:
			TransferFuncSRGB();
			virtual ~TransferFuncSRGB();

			virtual Double ForwardTransfer(Double val);
			virtual Double InverseTransfer(Double val);
		};
	};
};
#endif
