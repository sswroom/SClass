#ifndef _SM_MEDIA_CS_TRANSFERFUNCPARAM1
#define _SM_MEDIA_CS_TRANSFERFUNCPARAM1
#include "Media/CS/TransferFunc.h"
#include "Media/LUT.h"

namespace Media
{
	namespace CS
	{
		class TransferFuncParam1 : public TransferFunc
		{
		private:
			Double param2;

		public:
			TransferFuncParam1(Double *params);  //g a b c d e f
			virtual ~TransferFuncParam1();

			virtual Double ForwardTransfer(Double linearVal);
			virtual Double InverseTransfer(Double gammaVal);
		};
	};
};
#endif
