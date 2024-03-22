#ifndef _SM_MEDIA_DEINTERLACELR
#define _SM_MEDIA_DEINTERLACELR
#include "AnyType.h"
#include "Media/IDeinterlacer.h"
#include "Sync/Event.h"

namespace Media
{
	class DeinterlaceLR : public Media::IDeinterlacer
	{
	private:
		typedef struct
		{
			Sync::Event *evt;
			Sync::Event *evtMain;
			Int32 status; //0 = not running, 1 = idling, 2 = resizing, 3 = finish, 4 = to exit

			UInt8 *inPt;
			UInt8 *inPtCurr;
			UInt8 *outPt;
			UOSInt width;
			UOSInt height;
			UOSInt tap;
			OSInt *index;
			Int64 *weight;
			OSInt sstep;
			OSInt dstep;
		} DITHREADSTAT;

		typedef struct
		{
			UOSInt length;
			Int64 *weight;
			OSInt *index;
			UOSInt tap;
		} DIPARAMETER;

	private:
		DIPARAMETER oddParam;
		DIPARAMETER evenParam;
		DITHREADSTAT *stats;
		UOSInt nCore;
		Sync::Event *evtMain;
		UOSInt fieldCnt;
		UOSInt fieldSep;


		static Double lanczos3_weight(Double phase);
		static void SetupInterpolationParameter(UOSInt source_length, UOSInt result_length, DIPARAMETER *out, UOSInt indexSep, Double offsetCorr);

		static UInt32 __stdcall ProcThread(AnyType obj);
	public:
		DeinterlaceLR(UOSInt fieldCnt, UOSInt fieldSep);
		virtual ~DeinterlaceLR();

		virtual void Reinit(UOSInt fieldCnt, UOSInt fieldSep);
		virtual void Deinterlace(UInt8 *src, UInt8 *dest, Bool bottomField, UOSInt width, OSInt dstep);
	};
}
#endif
