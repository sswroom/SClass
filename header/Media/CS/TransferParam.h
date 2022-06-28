#ifndef _SM_MEDIA_CS_TRANSFERPARAM
#define _SM_MEDIA_CS_TRANSFERPARAM
#include "Media/LUT.h"

namespace Media
{
	namespace CS
	{
		enum TransferType
		{
			TRANT_sRGB,
			TRANT_BT709,
			TRANT_GAMMA,
			TRANT_SMPTE240,
			TRANT_LINEAR,
			TRANT_BT1361,
			TRANT_LOG100,
			TRANT_LOGSQRT10,
			TRANT_NTSC,
			TRANT_SLOG,
			TRANT_SLOG1,
			TRANT_SLOG2,
			TRANT_SLOG3,
			TRANT_VLOG,
			TRANT_PROTUNE,
			TRANT_BT2100,
			TRANT_HLG,
			TRANT_NLOG,
			TRANT_LAST = TRANT_NLOG,
			TRANT_PDISPLAY = 256,
			TRANT_VDISPLAY = 257,
			TRANT_VUNKNOWN = 258,
			TRANT_PUNKNOWN = 259,
			TRANT_LUT = 260,
			TRANT_PARAM1 = 261
		};

		class TransferParam
		{
		private:
			TransferType tranType;
			Double gamma;

			Media::LUT *lut;
		public:
			Double *params;
			UOSInt paramCnt;

		public:
			TransferParam();
			TransferParam(const TransferParam *tran);
			TransferParam(TransferType tranType, Double gamma);
			TransferParam(const Media::LUT *lut);
			~TransferParam();

			void Set(TransferType tranType, Double gamma);
			void Set(TransferType tranType, Double *params, UOSInt paramCnt);
			void Set(Media::LUT *lut);
			void Set(const TransferParam *tran);
			TransferType GetTranType() const { return this->tranType; }
			Double GetGamma() const { return this->gamma; }
			Media::LUT *GetLUT() { return this->lut; }
			const Media::LUT *GetLUTRead() const { return this->lut; }
			Bool Equals(const TransferParam *tran) const;
		};

		Text::CString TransferTypeGetName(Media::CS::TransferType ttype);
	}
}
#endif
