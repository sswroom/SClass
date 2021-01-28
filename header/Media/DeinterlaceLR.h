#ifndef _SM_MEDIA_DEINTERLACELR
#define _SM_MEDIA_DEINTERLACELR
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
			OSInt width;
			OSInt height;
			OSInt tap;
			OSInt *index;
			Int64 *weight;
			OSInt sstep;
			OSInt dstep;
		} DITHREADSTAT;

		typedef struct
		{
			OSInt length;
			Int64 *weight;
			OSInt *index;
			OSInt tap;
		} DIPARAMETER;

	private:
		DIPARAMETER oddParam;
		DIPARAMETER evenParam;
		DITHREADSTAT *stats;
		UOSInt nCore;
		Sync::Event *evtMain;
		OSInt fieldCnt;
		OSInt fieldSep;


		static Double lanczos3_weight(Double phase);
		static void SetupInterpolationParameter(OSInt source_length, OSInt result_length, DIPARAMETER *out, OSInt indexSep, Double offsetCorr);

		static UInt32 __stdcall ProcThread(void *obj);
	public:
		DeinterlaceLR(OSInt fieldCnt, OSInt fieldSep);
		virtual ~DeinterlaceLR();

		virtual void Reinit(OSInt fieldCnt, OSInt fieldSep);
		virtual void Deinterlace(UInt8 *src, UInt8 *dest, OSInt isBottomField, OSInt width, OSInt dstep);
	};
};
#endif
