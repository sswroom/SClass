#ifndef _SM_MEDIA_COLORMANAGER
#define _SM_MEDIA_COLORMANAGER
#include "Handles.h"
#include "Data/ArrayListNN.h"
#include "Data/FastStringMapNN.h"
#include "Media/ColorSess.h"
#include "Sync/Mutex.h"
#include "Sync/RWMutex.h"
#include "Text/String.h"

namespace Media
{
	class ColorManagerSess;

	class MonitorColorManager
	{
	private:
		Optional<Text::String> profileName;

		Media::IColorHandler::YUVPARAM yuv;
		Media::IColorHandler::RGBPARAM2 rgb;
		Optional<Text::String> monProfileFile;
		Bool color10Bit;

		Data::ArrayListNN<Media::ColorManagerSess> sessList;
		Sync::Mutex sessMut;
	public:
		static void SetDefaultYUV(NN<Media::IColorHandler::YUVPARAM> yuv);
		static void SetDefaultRGB(NN<Media::IColorHandler::RGBPARAM2> rgb);

	public:
		MonitorColorManager(Optional<Text::String> profileName);
		MonitorColorManager(Text::CString profileName);
		~MonitorColorManager();

		Optional<Text::String> GetProfileName();
		Bool Load();
		Bool Save();
		void SetDefault();
		NN<const Media::IColorHandler::YUVPARAM> GetYUVParam();
		NN<const Media::IColorHandler::RGBPARAM2> GetRGBParam();

		void SetMonVBright(Double newVal);
		void SetMonPBright(Double newVal);
		void SetMonRBright(Double newVal);
		void SetMonGBright(Double newVal);
		void SetMonBBright(Double newVal);
		void SetRMonBright(Double newVal);
		void SetRMonContr(Double newVal);
		void SetRMonGamma(Double newVal);
		void SetGMonBright(Double newVal);
		void SetGMonContr(Double newVal);
		void SetGMonGamma(Double newVal);
		void SetBMonBright(Double newVal);
		void SetBMonContr(Double newVal);
		void SetBMonGamma(Double newVal);
		void SetYUVBright(Double newVal);
		void SetYUVContr(Double newVal);
		void SetYUVSat(Double newVal);
		void SetYGamma(Double newVal);
		void SetCGamma(Double newVal);
		void SetMonProfileType(Media::ColorProfile::CommonProfileType newVal);
		Bool SetMonProfileFile(NN<Text::String> fileName);
		void SetMonProfile(NN<const Media::ColorProfile> color);
		Optional<Text::String> GetMonProfileFile();
		void SetMonLuminance(Double newVal);
		Bool Get10BitColor();
		void Set10BitColor(Bool color10Bit);


		void AddSess(NN<Media::ColorManagerSess> colorSess);
		void RemoveSess(NN<Media::ColorManagerSess> colorSess);
	private:
		Bool SetFromProfileFile(NN<Text::String> fileName);
		void SetOSProfile();
		void SetEDIDProfile();

		void RGBUpdated();
		void YUVUpdated();
	};

	class ColorManager
	{
	private:
		Data::FastStringMapNN<MonitorColorManager> monColor;
		Sync::Mutex mut;

		Media::ColorProfile::YUVType defYUVType;
		Media::ColorProfile defVProfile;
		Media::ColorProfile::CommonProfileType defVProfileType;
		Media::ColorProfile defPProfile;
		Media::ColorProfile::CommonProfileType defPProfileType;

	public:
		ColorManager();
		~ColorManager();

		Bool LoadDef();
		Bool SaveDef();
		void SetDefVProfile(Media::ColorProfile::CommonProfileType newVal);
		void SetDefPProfile(Media::ColorProfile::CommonProfileType newVal);
		NN<Media::ColorProfile> GetDefVProfile();
		NN<Media::ColorProfile> GetDefPProfile();
		Media::ColorProfile::CommonProfileType GetDefVProfileType();
		Media::ColorProfile::CommonProfileType GetDefPProfileType();
		void SetYUVType(Media::ColorProfile::YUVType newVal);
		Media::ColorProfile::YUVType GetDefYUVType();

		NN<MonitorColorManager> GetMonColorManager(Optional<Text::String> profileName);
		NN<MonitorColorManager> GetMonColorManager(MonitorHandle *hMon);
		NN<ColorManagerSess> CreateSess(MonitorHandle *hMon);
		void DeleteSess(NN<ColorManagerSess> sess);
	};

	class ColorManagerSess : public Media::ColorSess
	{
	private:
		NN<ColorManager> colorMgr;
		NN<MonitorColorManager> monColor;
		Sync::RWMutex mut;
		Data::ArrayListNN<Media::IColorHandler> hdlrs;
		Sync::Mutex hdlrMut;

	public:
		ColorManagerSess(NN<ColorManager> colorMgr, NN<MonitorColorManager> monColor);
		virtual ~ColorManagerSess();

		virtual void AddHandler(NN<Media::IColorHandler> hdlr);
		virtual void RemoveHandler(NN<Media::IColorHandler> hdlr);
		NN<const Media::IColorHandler::YUVPARAM> GetYUVParam();
		virtual NN<const Media::IColorHandler::RGBPARAM2> GetRGBParam();
		virtual NN<Media::ColorProfile> GetDefVProfile();
		virtual NN<Media::ColorProfile> GetDefPProfile();
		Media::ColorProfile::YUVType GetDefYUVType();
		Bool Get10BitColor();

		void ChangeMonitor(MonitorHandle *hMon);

		void RGBUpdated(NN<const Media::IColorHandler::RGBPARAM2> rgbParam);
		void YUVUpdated(NN<const Media::IColorHandler::YUVPARAM> yuvParam);
	};
}
#endif
