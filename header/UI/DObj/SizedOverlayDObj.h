#ifndef _SM_UI_DOBJ_SIZEDOVERLAYDOBJ
#define _SM_UI_DOBJ_SIZEDOVERLAYDOBJ
#include "Manage/HiResClock.h"
#include "Math/Coord2DDbl.h"
#include "Media/ImageList.h"
#include "Media/Resizer/LanczosResizerRGB_C8.h"
#include "Parser/ParserList.h"
#include "UI/DObj/DirectObject.h"

namespace UI
{
	namespace DObj
	{
		class SizedOverlayDObj : public DirectObject
		{
		private:
			NN<Media::DrawEngine> deng;
			Sync::Mutex imgMut;
			Optional<Media::ImageList> imgList;
			Bool noRelease;
			IntOS frameDelay;
			Optional<Manage::HiResClock> clk;
			Double startTime;
			IntOS lastFrameNum;
			Math::Size2D<UIntOS> size;
			NN<Media::Resizer::LanczosResizerRGB_C8> resizer;
			Sync::Mutex dispMut;
			Optional<Media::StaticImage> dispImg;
			UIntOS dispFrameNum;
			Math::Coord2DDbl drawOfst;

		public:
			SizedOverlayDObj(NN<Media::DrawEngine> deng, Text::CString fileName, Math::Coord2D<IntOS> tl, Math::Size2D<UIntOS> size, NN<Parser::ParserList> parsers, NN<Media::Resizer::LanczosResizerRGB_C8> resizer);
			virtual ~SizedOverlayDObj();

			virtual Bool IsChanged();
			virtual Bool DoEvents();
			virtual void DrawObject(NN<Media::DrawImage> dimg);

			virtual Bool IsObject(Math::Coord2D<IntOS> scnPos);
			virtual void OnMouseDown();
			virtual void OnMouseUp();
			virtual void OnMouseClick();

			void SetFrameDelay(IntOS frameDelay);
			void SetSize(Math::Size2D<UIntOS> size);
			void SetImage(Text::CStringNN fileName, NN<Parser::ParserList> parsers);
		};
	}
}
#endif
