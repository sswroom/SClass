#ifndef _SM_MEDIA_CS_TRANSFERFUNCLUT
#define _SM_MEDIA_CS_TRANSFERFUNCLUT
#include "Media/CS/TransferFunc.h"
#include "Media/LUT.h"

namespace Media
{
	namespace CS
	{
		class TransferFuncLUT : public TransferFunc
		{
		private:
			OSInt srcCnt;
			Double *invLUT;
			Double *fwdLUT;

		public:
			TransferFuncLUT(Media::LUT *lut);
			virtual ~TransferFuncLUT();

			virtual Double ForwardTransfer(Double linearVal);
			virtual Double InverseTransfer(Double gammaVal);
		};
	};
};
#endif
