#ifndef _SM_MEDIA_RESIZER_LANCZOSRESIZER8_8
#define _SM_MEDIA_RESIZER_LANCZOSRESIZER8_8
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Media/IImgResizer.h"

namespace Media
{
	namespace Resizer
	{
		class LanczosResizer8_8 : public Media::IImgResizer
		{
		private:
			typedef struct
			{
				Int32 status; // 0 = not running, 1 = waiting, 2 = to exit, 3 = h filter, 4 = end h filter, 5 = v filter, 6 = end v filter
				Sync::Event *evt;
				UInt8 *inPt;
				UInt8 *outPt;
				Int32 width;
				Int32 height;
				Int32 tap;
				Int32 *index;
				Int32 *weight;
				UInt32 sstep;
				UInt32 dstep;
			} LRTHREADSTAT;

			typedef struct
			{
				Int32 length;
				Int32 *weight;
				Int32 *index;
				Int32 tap;
			} PARAMETER;

		private:
			Int32 currId;
			Int32 nTap;
			Sync::Event *evtMain;
			Sync::Mutex *mut;
			LRTHREADSTAT *stats;
			Int32 nThread;

			Double hsSize;
			Int32 hdSize;
			Double hsOfst;
			Int32 *hIndex;
			Int32 *hWeight;
			Int32 hTap;

			Double vsSize;
			Int32 vdSize;
			Double vsOfst;
			Int32 *vIndex;
			Int32 *vWeight;
			Int32 vTap;

			Int32 buffW;
			Int32 buffH;
			UInt8 *buffPtr;

			Double lanczos3_weight(Double phase);
			void setup_interpolation_parameter(Double source_length, Int32 source_max_pos, Int32 result_length, PARAMETER *out, Int32 indexSep, Double offsetCorr);
			void setup_decimation_parameter(Double source_length, Int32 source_max_pos, Int32 result_length, PARAMETER *out, Int32 indexSep, Double offsetCorr);
			static void horizontal_filter(UInt8 *inPt, UInt8 *outPt,Int32 width, Int32 height, Int32 tap, Int32 *index, Int32 *weight, UInt32 sstep, UInt32 dstep);
			static void vertical_filter(UInt8 *inPt, UInt8 *outPt, Int32 width, Int32 height, Int32 tap, Int32 *index, Int32 *weight, UInt32 sstep, UInt32 dstep);

			void mt_horizontal_filter(UInt8 *inPt, UInt8 *outPt,Int32 width, Int32 height, Int32 tap, Int32 *index, Int32 *weight, UInt32 sstep, UInt32 dstep);
			void mt_vertical_filter(UInt8 *inPt, UInt8 *outPt, Int32 width, Int32 height, Int32 tap, Int32 *index, Int32 *weight, UInt32 sstep, UInt32 dstep);

			static UInt32 WorkerThread(void *obj);
			void DestoryHori();
			void DestoryVert();
		public:
			LanczosResizer8_8(Int32 nTap);
			virtual ~LanczosResizer8_8();

			virtual void Resize(UInt8 *src, OSInt sbpl, Double swidth, Double sheight, Double xOfst, Double yOfst, UInt8 *dest, OSInt dbpl, UOSInt dwidth, UOSInt dheight);

			virtual Bool IsSupported(Media::FrameInfo *srcInfo);
			virtual Media::StaticImage *ProcessToNewPartial(Media::StaticImage *srcImage, Double srcX1, Double srcY1, Double srcX2, Double srcY2);
		};
	};
};
#endif
