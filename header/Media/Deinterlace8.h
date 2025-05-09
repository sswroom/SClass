#ifndef _SM_MEDIA_DEINTERLACE8
#define _SM_MEDIA_DEINTERLACE8
#include "AnyType.h"
#include "Media/Deinterlacer.h"
#include "Sync/Event.h"

namespace Media
{
	typedef struct
	{
		Sync::Event *evt;
		Sync::Event *evtMain;
		OSInt status; //0 = not running, 1 = idling, 2 = resizing, 3 = finish, 4 = to exit

		UnsafeArray<UInt8> inPt;
		UnsafeArray<UInt8> inPtCurr;
		UnsafeArray<UInt8> outPt;
		UOSInt width;
		UOSInt height;
		UOSInt tap;
		OSInt *index;
		Int64 *weight;
		UOSInt sstep;
		OSInt dstep;
	} DI8THREADSTAT;

	typedef struct
	{
		UOSInt length;
		Int64 *weight;
		OSInt *index;
		UOSInt tap;
	} DI8PARAMETER;


	class Deinterlace8 : public Media::Deinterlacer
	{
	private:
		DI8PARAMETER oddParam;
		DI8PARAMETER evenParam;
		DI8THREADSTAT *stats;
		UOSInt nCore;
		Sync::Event *evtMain;
		UOSInt fieldCnt;
		UOSInt fieldSep;


		static Double lanczos3_weight(Double phase);
		static void SetupInterpolationParameter(UOSInt source_length, UOSInt result_length, NN<DI8PARAMETER> out, UOSInt indexSep, Double offsetCorr);

		static UInt32 __stdcall ProcThread(AnyType obj);
	public:
		Deinterlace8(UOSInt fieldCnt, UOSInt fieldSep);
		virtual ~Deinterlace8();

		virtual void Reinit(UOSInt fieldCnt, UOSInt fieldSep);
		virtual void Deinterlace(UnsafeArray<UInt8> src, UnsafeArray<UInt8> dest, Bool bottomField, UOSInt width, OSInt dstep);
	};
}
#endif
