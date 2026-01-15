#ifndef _SM_MEDIA_DEINTERLACELR
#define _SM_MEDIA_DEINTERLACELR
#include "AnyType.h"
#include "Media/Deinterlacer.h"
#include "Sync/Event.h"

namespace Media
{
	class DeinterlaceLR : public Media::Deinterlacer
	{
	private:
		typedef struct
		{
			Sync::Event *evt;
			Sync::Event *evtMain;
			Int32 status; //0 = not running, 1 = idling, 2 = resizing, 3 = finish, 4 = to exit

			UnsafeArray<UInt8> inPt;
			UnsafeArray<UInt8> inPtCurr;
			UnsafeArray<UInt8> outPt;
			UIntOS width;
			UIntOS height;
			UIntOS tap;
			IntOS *index;
			Int64 *weight;
			IntOS sstep;
			IntOS dstep;
		} DITHREADSTAT;

		typedef struct
		{
			UIntOS length;
			Int64 *weight;
			IntOS *index;
			UIntOS tap;
		} DIPARAMETER;

	private:
		DIPARAMETER oddParam;
		DIPARAMETER evenParam;
		DITHREADSTAT *stats;
		UIntOS nCore;
		Sync::Event *evtMain;
		UIntOS fieldCnt;
		UIntOS fieldSep;


		static Double lanczos3_weight(Double phase);
		static void SetupInterpolationParameter(UIntOS source_length, UIntOS result_length, NN<DIPARAMETER> out, UIntOS indexSep, Double offsetCorr);

		static UInt32 __stdcall ProcThread(AnyType obj);
	public:
		DeinterlaceLR(UIntOS fieldCnt, UIntOS fieldSep);
		virtual ~DeinterlaceLR();

		virtual void Reinit(UIntOS fieldCnt, UIntOS fieldSep);
		virtual void Deinterlace(UnsafeArray<UInt8> src, UnsafeArray<UInt8> dest, Bool bottomField, UIntOS width, IntOS dstep);
	};
}
#endif
