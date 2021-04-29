#ifndef _SM_MEDIA_ICOLORHANDLER
#define _SM_MEDIA_ICOLORHANDLER
#include "MyMemory.h"
#include "Media/ColorProfile.h"

namespace Media
{
	class IColorHandler
	{
	public:
		typedef struct
		{
			Double Brightness;
			Double Contrast;
			Double Saturation;
			Double YGamma;
			Double CGamma;

			Double RAdd;
			Double RMul;

			Double GAdd;
			Double GMul;

			Double BAdd;
			Double BMul;

			void SetDefault()
			{
				this->Brightness = 1;
				this->Contrast = 1;
				this->Saturation = 1;
				this->YGamma = 1;
				this->CGamma = 1;

				this->RAdd = 0;
				this->RMul = 1;

				this->GAdd = 0;
				this->GMul = 1;
				
				this->BAdd = 0;
				this->BMul = 1;
			}
		} YUVPARAM;

		class RGBPARAM2
		{
		public:
			Double MonRBright;
			Double MonRContr;
			Double MonRGamma;

			Double MonGBright;
			Double MonGContr;
			Double MonGGamma;

			Double MonBBright;
			Double MonBContr;
			Double MonBGamma;

			Double MonVBrightness;
			Double MonPBrightness;
			Double MonRBrightness;
			Double MonGBrightness;
			Double MonBBrightness;

			Media::ColorProfile *monProfile;
			Media::ColorProfile::CommonProfileType monProfileType;
			Double monLuminance;

			RGBPARAM2()
			{
				NEW_CLASS(this->monProfile, Media::ColorProfile());
			}

			~RGBPARAM2()
			{
				DEL_CLASS(this->monProfile);
			}

			void SetDefault()
			{
				this->MonRBright = 1;
				this->MonRContr = 1;
				this->MonRGamma = 1;

				this->MonGBright = 1;
				this->MonGContr = 1;
				this->MonGGamma = 1;

				this->MonBBright = 1;
				this->MonBContr = 1;
				this->MonBGamma = 1;

				this->MonVBrightness = 1;
				this->MonPBrightness = 1;
				this->MonRBrightness = 1;
				this->MonGBrightness = 1;
				this->MonBBrightness = 1;

				this->monProfileType = Media::ColorProfile::CPT_SRGB;
				this->monProfile->SetCommonProfile(Media::ColorProfile::CPT_SRGB);
				this->monLuminance = 250.0;
			}

			void Set(const RGBPARAM2 *param)
			{
				this->MonRBright = param->MonRBright;
				this->MonRContr = param->MonRContr;
				this->MonRGamma = param->MonRGamma;
				this->MonGBright = param->MonGBright;
				this->MonGContr = param->MonGContr;
				this->MonGGamma = param->MonGGamma;
				this->MonBBright = param->MonBBright;
				this->MonBContr = param->MonBContr;
				this->MonBGamma = param->MonBGamma;
				this->MonVBrightness = param->MonVBrightness;
				this->MonPBrightness = param->MonPBrightness;
				this->MonRBrightness = param->MonRBrightness;
				this->MonGBrightness = param->MonGBrightness;
				this->MonBBrightness = param->MonBBrightness;
				this->monProfile->Set(param->monProfile);
				this->monProfileType = param->monProfileType;
				this->monLuminance = param->monLuminance;
			}
		};
	public:
		virtual void YUVParamChanged(const Media::IColorHandler::YUVPARAM *yuvParam) = 0;
		virtual void RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgbParam) = 0;
	};
}
#endif
