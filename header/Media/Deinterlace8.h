#ifndef _SM_MEDIA_DEINTERLACE8
#define _SM_MEDIA_DEINTERLACE8
#include "Media/IDeinterlacer.h"
#include "Sync/Event.h"

namespace Media
{
	typedef struct
	{
		Sync::Event *evt;
		Sync::Event *evtMain;
		OSInt status; //0 = not running, 1 = idling, 2 = resizing, 3 = finish, 4 = to exit

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
	} DI8THREADSTAT;

	typedef struct
	{
		OSInt length;
		Int64 *weight;
		OSInt *index;
		OSInt tap;
	} DI8PARAMETER;


	class Deinterlace8 : public Media::IDeinterlacer
	{
	private:
		DI8PARAMETER oddParam;
		DI8PARAMETER evenParam;
		DI8THREADSTAT *stats;
		UOSInt nCore;
		Sync::Event *evtMain;
		OSInt fieldCnt;
		OSInt fieldSep;


		static Double lanczos3_weight(Double phase);
		static void SetupInterpolationParameter(OSInt source_length, OSInt result_length, DI8PARAMETER *out, OSInt indexSep, Double offsetCorr);

		static UInt32 __stdcall ProcThread(void *obj);
	public:
		Deinterlace8(OSInt fieldCnt, OSInt fieldSep);
		virtual ~Deinterlace8();

		virtual void Reinit(OSInt fieldCnt, OSInt fieldSep);
		virtual void Deinterlace(UInt8 *src, UInt8 *dest, OSInt isBottomField, OSInt width, OSInt dstep);
	};
};
#endif
