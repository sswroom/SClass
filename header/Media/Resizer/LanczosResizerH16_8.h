#ifndef _SM_MEDIA_RESIZER_LANCZOSRESIZERH16_8
#define _SM_MEDIA_RESIZER_LANCZOSRESIZERH16_8
#include "AnyType.h"
//MMX Accelerated Lanczos Resizer
namespace Media
{
	namespace Resizer
	{
		class LanczosResizerH16_8 : public Media::ImageResizer
		{
		private:
			typedef struct
			{
				Int32 status; // 0 = not running, 1 = waiting, 2 = to exit, 3 = h filter, 4 = end h filter, 5 = v filter, 6 = end v filter, 7 = expand, 8 = end expand, 9 = collapse, 10 = end collapse
				Sync::Event *evt;
				UInt8 *inPt;
				UInt8 *outPt;
				UOSInt width;
				UOSInt height;
				Int32 tap;
				Int32 *index;
				Int64 *weight;
				UInt32 sstep;
				UInt32 dstep;
			} LRH16THREADSTAT;

			typedef struct
			{
				UOSInt length;
				Int64 *weight;
				Int32 *index;
				Int32 tap;
			} LRH16PARAMETER;

		private:
			Int32 currId;
			UOSInt hnTap;
			UOSInt vnTap;
			Sync::Event *evtMain;
			LRH16THREADSTAT *stats;
			UOSInt nThread;

			Int32 hsSize;
			Int32 hdSize;
			Int32 *hIndex;
			Int64 *hWeight;
			Int32 hTap;

			Int32 vsSize;
			Int32 vdSize;
			Int32 *vIndex;
			Int64 *vWeight;
			Int32 vTap;

			Int32 buffW;
			Int32 buffH;
			UInt8 *buffPtr;

			Double lanczos3_weight(Double phase, UOSInt nTap);
			void setup_interpolation_parameter(Int32 source_length, UOSInt result_length, LRH16PARAMETER *out, Int32 indexSep, Double offsetCorr);
			void setup_decimation_parameter(Int32 source_length, UOSInt result_length, LRH16PARAMETER *out, Int32 indexSep, Int32 offsetCorr);
			static void horizontal_filter(UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, Int32 tap, Int32 *index, Int64 *weight, UInt32 sstep, UInt32 dstep);
			static void vertical_filter(UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, Int32 tap, Int32 *index, Int64 *weight, UInt32 sstep, UInt32 dstep);
			static void expand(UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, Int32 sstep, Int32 dstep);
			static void collapse(UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, Int32 sstep, Int32 dstep);

			void mt_horizontal_filter(UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, Int32 tap, Int32 *index, Int64 *weight, UInt32 sstep, UInt32 dstep);
			void mt_vertical_filter(UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, Int32 tap, Int32 *index, Int64 *weight, UInt32 sstep, UInt32 dstep);
			void mt_expand(UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, Int32 sstep, Int32 dstep);
			void mt_collapse(UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, Int32 sstep, Int32 dstep);

			static UInt32 WorkerThread(AnyType obj);
			void DestoryHori();
			void DestoryVert();
		public:
			LanczosResizerH16_8(UOSInt hnTap, UOSInt vnTap);
			virtual ~LanczosResizerH16_8();

			virtual void Resize(UInt8 *src, OSInt sbpl, Double swidth, Double sheight, Double xOfst, Double yOfst, UInt8 *dest, OSInt dbpl, UOSInt dwidth, UOSInt dheight);
			virtual Bool Resize(Media::StaticImage *srcImg, Media::StaticImage *destImg);

			virtual Bool IsSupported(Media::FrameInfo *srcInfo);
			virtual Media::StaticImage *ProcessToNewPartial(Media::StaticImage *srcImage, Double srcX1, Double srcY1, Double srcX2, Double srcY2);

		};
	}
}
#endif
