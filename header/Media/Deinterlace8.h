#ifndef _SM_MEDIA_DEINTERLACE8
#define _SM_MEDIA_DEINTERLACE8
#include "AnyType.h"
#include "Media/Deinterlacer.h"
#include "Sync/Event.h"

namespace Media
{
	typedef struct
	{
		NN<Sync::Event> evt;
		IntOS status; //0 = not running, 1 = idling, 2 = resizing, 3 = finish, 4 = to exit
		NN<Sync::Event> evtMain;

		UnsafeArray<UInt8> inPt;
		UnsafeArray<UInt8> inPtCurr;
		UnsafeArray<UInt8> outPt;
		UIntOS width;
		UIntOS height;
		UIntOS tap;
		UnsafeArray<IntOS> index;
		UnsafeArray<Int64> weight;
		UIntOS sstep;
		IntOS dstep;
	} DI8THREADSTAT;

	typedef struct
	{
		UIntOS length;
		UnsafeArrayOpt<Int64> weight;
		UnsafeArrayOpt<IntOS> index;
		UIntOS tap;
	} DI8PARAMETER;


	class Deinterlace8 : public Media::Deinterlacer
	{
	private:
		DI8PARAMETER oddParam;
		DI8PARAMETER evenParam;
		UnsafeArray<DI8THREADSTAT> stats;
		UIntOS nCore;
		NN<Sync::Event> evtMain;
		UIntOS fieldCnt;
		UIntOS fieldSep;


		static Double lanczos3_weight(Double phase);
		static void SetupInterpolationParameter(UIntOS source_length, UIntOS result_length, NN<DI8PARAMETER> out, UIntOS indexSep, Double offsetCorr);

		static UInt32 __stdcall ProcThread(AnyType obj);
	public:
		Deinterlace8(UIntOS fieldCnt, UIntOS fieldSep);
		virtual ~Deinterlace8();

		virtual void Reinit(UIntOS fieldCnt, UIntOS fieldSep);
		virtual void Deinterlace(UnsafeArray<UInt8> src, UnsafeArray<UInt8> dest, Bool bottomField, UIntOS width, IntOS dstep);
	};
}
#endif
