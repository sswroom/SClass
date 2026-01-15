#ifndef _SM_MEDIA_RESIZER_LANCZOSRESIZERRGB_C8
#define _SM_MEDIA_RESIZER_LANCZOSRESIZERRGB_C8
#include "AnyType.h"
#include "Media/ColorManager.h"
#include "Media/ImageResizer.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/ParallelTask.h"

namespace Media
{
	namespace Resizer
	{
		class LanczosResizerRGB_C8 : public Media::ImageResizer, public Media::ColorHandler
		{
		private:
			enum class FuncType
			{
				NoFunction,
				HFilter,
				VFilter,
				Expand,
				Collapse,
				ImgCopy,
				HFilterPA,
				ExpandPA,
				ImgCopyPA
			};

			typedef struct
			{
				NN<LanczosResizerRGB_C8> me;
				FuncType funcType;
				UnsafeArray<const UInt8> inPt;
				UnsafeArray<UInt8> outPt;
				UIntOS swidth;
				UIntOS dwidth;
				UIntOS height;
				UIntOS tap;
				UnsafeArray<IntOS> index;
				UnsafeArray<Int64> weight;
				IntOS sstep;
				IntOS dstep;
				Media::PixelFormat srcPF;
				Media::PixelFormat destPF;
				UnsafeArrayOpt<UInt8> tmpbuff;
				UIntOS tmpbuffSize;
			} TaskParam;

		private:
			UIntOS hnTap;
			UIntOS vnTap;
			Sync::Mutex mut;
			UnsafeArray<TaskParam> params;
			UIntOS nThread;
			NN<Sync::ParallelTask> ptask;

			Double hsSize;
			Double hsOfst;
			UIntOS hdSize;
			UnsafeArrayOpt<IntOS> hIndex;
			UnsafeArrayOpt<Int64> hWeight;
			UIntOS hTap;

			Double vsSize;
			Double vsOfst;
			UIntOS vdSize;
			IntOS vsStep;
			UnsafeArrayOpt<IntOS> vIndex;
			UnsafeArrayOpt<Int64> vWeight;
			UIntOS vTap;

			UIntOS buffW;
			IntOS buffH;
			UnsafeArrayOpt<UInt8> buffPtr;

			Media::PixelFormat srcPF;
			Media::PixelFormat destPF;
			UnsafeArrayOpt<const UInt8> srcPal;
			Media::ColorProfile srcProfile;
			Media::ColorProfile destProfile;
			Optional<Media::ColorManagerSess> colorSess;
			Bool rgbChanged;
			UnsafeArrayOpt<UInt8> rgbTable;
			Bool rgb16Changed;
			UnsafeArrayOpt<UInt8> rgb16Table;

			void MTHorizontalFilterPA(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt,UIntOS width, UIntOS height, UIntOS tap, UnsafeArray<IntOS> index, UnsafeArray<Int64> weight, IntOS sstep, IntOS dstep, UIntOS swidth);
			void MTHorizontalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, UIntOS tap, UnsafeArray<IntOS> index, UnsafeArray<Int64> weight, IntOS sstep, IntOS dstep, UIntOS swidth);
			void MTVerticalFilter(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, UIntOS tap, UnsafeArray<IntOS> index, UnsafeArray<Int64> weight, IntOS sstep, IntOS dstep);
			void MTExpandPA(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, IntOS sstep, IntOS dstep);
			void MTExpand(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, IntOS sstep, IntOS dstep);
			void MTCollapse(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, IntOS sstep, IntOS dstep);
			void MTCopyPA(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, IntOS sstep, IntOS dstep);
			void MTCopy(UnsafeArray<const UInt8> inPt, UnsafeArray<UInt8> outPt, UIntOS width, UIntOS height, IntOS sstep, IntOS dstep);

			void UpdateRGBTable();
			UnsafeArray<UInt8> UpdateRGB16Table();
			void UpdatePalTable();

			static void __stdcall DoTask(AnyType obj);
			void DestoryHori();
			void DestoryVert();
		public:
			LanczosResizerRGB_C8(UIntOS hnTap, UIntOS vnTap, NN<const Media::ColorProfile> srcProfile, NN<const Media::ColorProfile> destProfile, Optional<Media::ColorManagerSess> colorSess, Media::AlphaType srcAlphaType);
			virtual ~LanczosResizerRGB_C8();

			virtual void Resize(UnsafeArray<const UInt8> src, IntOS sbpl, Double swidth, Double sheight, Double xOfst, Double yOfst, UnsafeArray<UInt8> dest, IntOS dbpl, UIntOS dwidth, UIntOS dheight);
			virtual Bool Resize(NN<const Media::StaticImage> srcImg, NN<Media::StaticImage> destImg);
			virtual void YUVParamChanged(NN<const Media::ColorHandler::YUVPARAM> yuvParam);
			virtual void RGBParamChanged(NN<const Media::ColorHandler::RGBPARAM2> rgbParam);
			void SetSrcProfile(NN<const Media::ColorProfile> srcProfile);
			void SetDestProfile(NN<const Media::ColorProfile> destProfile);
			Media::AlphaType GetDestAlphaType();
			void SetSrcPixelFormat(Media::PixelFormat srcPF, UnsafeArrayOpt<const UInt8> srcPal);
			void SetDestPixelFormat(Media::PixelFormat destPF);
			Bool IsSrcUInt16() const;
			virtual Bool IsSupported(NN<const Media::FrameInfo> srcInfo);
			virtual Optional<Media::StaticImage> ProcessToNewPartial(NN<const Media::RasterImage> srcImage, Math::Coord2DDbl srcTL, Math::Coord2DDbl srcBR);
		};
	}
}
#endif
