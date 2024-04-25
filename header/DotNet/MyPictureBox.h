#ifndef _SM_DOTNET_MYPICTUREBOX
#define _SM_DOTNET_MYPICTUREBOX
#include "Media/IVideoRenderer.h"
#include "Media/IImgResizer.h"
#include "Media/CS/CSConverter.h"
#include "Media/StaticImage.h"
#include "DotNet/DNColorManager.h"

namespace DotNet
{
	__gc class MyPictureBox : public DotNet::DNColorHandler
	{
	private:
		System::Windows::Forms::Control *formCtrl;
		System::EventHandler *sizeChgHdlr;

		Media::Image *currImage;
		UInt8 *imgBuff;
		Media::CS::CSConverter *csconv;
		Media::IImgResizer *resizer;
		DotNet::DNColorManager *dnColorMgr;

		void *hwnd;
		void *ddObj;
		void *pSurface;
		void *surfaceBuff;
		void *clipper;
		Int32 surfaceW;
		Int32 surfaceH;
		Bool allowEnlarge;

		void GetDrawingRect(void *rc);
		void DrawToScreen();
		void SizeChg(System::Object *sender, System::EventArgs *e);
		void OnPaint(System::Object *sender, System::Windows::Forms::PaintEventArgs *e);
		void CreateSurface();
		void ReleaseSurface();
		void CreateSubSurface();
		void ReleaseSubSurface();
		void UpdateSubSurface();

	public:
		MyPictureBox(System::Windows::Forms::Control *formCtrl, DotNet::DNColorManager *dnColorMgr, Bool allowEnlarge);
		~MyPictureBox();

		void SetImage(Media::Image *currImage);

		void YUVParamChanged(NN<const Media::IColorHandler::YUVPARAM> yuvParam);
		void RGBParamChanged(const Media::IColorHandler::RGBPARAM *rgbParam);
		void SetAllowEnlarge(Bool allowEnlarge);
	};
};
#endif
