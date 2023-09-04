#ifndef _SM_MEDIA_CS_TRANSFERFUNC
#define _SM_MEDIA_CS_TRANSFERFUNC
#include "Media/CS/TransferParam.h"
#include "Text/CString.h"

namespace Media
{
	namespace CS
	{
		class TransferFunc
		{
		protected:
			TransferParam param;
		public:
			TransferFunc(TransferType tranType, Double gamma);
			TransferFunc(NotNullPtr<const Media::LUT> lut);
			virtual ~TransferFunc();

			virtual Double ForwardTransfer(Double linearVal) = 0;
			virtual Double InverseTransfer(Double gammaVal) = 0;
			TransferType GetTransferType();
			Double GetTransferGamma();
			const TransferParam *GetTransferParam();

			static TransferFunc *CreateFunc(NotNullPtr<const TransferParam> param);
			static Double GetRefLuminance(NotNullPtr<const TransferParam> param);
		};
	}
}
#endif
