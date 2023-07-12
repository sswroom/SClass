#ifndef _SM_MEDIA_COLORMANAGER
#define _SM_MEDIA_COLORMANAGER
#include "Handles.h"
#include "Data/ArrayList.h"
#include "Data/FastStringMap.h"
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
		Text::String *profileName;

		Media::IColorHandler::YUVPARAM yuv;
		Media::IColorHandler::RGBPARAM2 rgb;
		Text::String *monProfileFile;
		Bool color10Bit;

		Data::ArrayList<Media::ColorManagerSess *> sessList;
		Sync::Mutex sessMut;
	public:
		static void SetDefaultYUV(Media::IColorHandler::YUVPARAM *yuv);
		static void SetDefaultRGB(Media::IColorHandler::RGBPARAM2 *rgb);

	public:
		MonitorColorManager(Text::String *profileName);
		MonitorColorManager(Text::CString profileName);
		~MonitorColorManager();

		Text::String *GetProfileName();
		Bool Load();
		Bool Save();
		void SetDefault();
		const Media::IColorHandler::YUVPARAM *GetYUVParam();
		const Media::IColorHandler::RGBPARAM2 *GetRGBParam();

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
		Bool SetMonProfileFile(NotNullPtr<Text::String> fileName);
		void SetMonProfile(Media::ColorProfile *color);
		Text::String *GetMonProfileFile();
		void SetMonLuminance(Double newVal);
		Bool Get10BitColor();
		void Set10BitColor(Bool color10Bit);


		void AddSess(Media::ColorManagerSess *colorSess);
		void RemoveSess(Media::ColorManagerSess *colorSess);
	private:
		Bool SetFromProfileFile(NotNullPtr<Text::String> fileName);
		void SetOSProfile();
		void SetEDIDProfile();

		void RGBUpdated();
		void YUVUpdated();
	};

	class ColorManager
	{
	private:
		Data::FastStringMap<MonitorColorManager*> monColor;
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
		Media::ColorProfile *GetDefVProfile();
		Media::ColorProfile *GetDefPProfile();
		Media::ColorProfile::CommonProfileType GetDefVProfileType();
		Media::ColorProfile::CommonProfileType GetDefPProfileType();
		void SetYUVType(Media::ColorProfile::YUVType newVal);
		Media::ColorProfile::YUVType GetDefYUVType();

		MonitorColorManager *GetMonColorManager(Text::String *profileName);
		MonitorColorManager *GetMonColorManager(MonitorHandle *hMon);
		ColorManagerSess *CreateSess(MonitorHandle *hMon);
		void DeleteSess(ColorManagerSess *sess);
	};

	class ColorManagerSess : public Media::ColorSess
	{
	private:
		ColorManager *colorMgr;
		MonitorColorManager *monColor;
		Sync::RWMutex mut;
		Data::ArrayList<Media::IColorHandler *> hdlrs;
		Sync::Mutex hdlrMut;

	public:
		ColorManagerSess(ColorManager *colorMgr, MonitorColorManager *monColor);
		virtual ~ColorManagerSess();

		virtual void AddHandler(Media::IColorHandler *hdlr);
		virtual void RemoveHandler(Media::IColorHandler *hdlr);
		const Media::IColorHandler::YUVPARAM *GetYUVParam();
		virtual const Media::IColorHandler::RGBPARAM2 *GetRGBParam();
		virtual Media::ColorProfile *GetDefVProfile();
		virtual Media::ColorProfile *GetDefPProfile();
		Media::ColorProfile::YUVType GetDefYUVType();
		Bool Get10BitColor();

		void ChangeMonitor(MonitorHandle *hMon);

		void RGBUpdated(const Media::IColorHandler::RGBPARAM2 *rgbParam);
		void YUVUpdated(const Media::IColorHandler::YUVPARAM *yuvParam);
	};
};
#endif
