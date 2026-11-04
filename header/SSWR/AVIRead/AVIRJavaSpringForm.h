#ifndef _SM_SSWR_AVIREAD_AVIRJAVASPRINGFORM
#define _SM_SSWR_AVIREAD_AVIRJAVASPRINGFORM
#include "IO/ConfigFile.h"
#include "IO/VirtualPackageFileFast.h"
#include "IO/Java/JavaClass.h"
#include "Net/WebUtil.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRJavaSpringForm : public UI::GUIForm
		{
		private:
			struct ControllerRequest
			{
				NN<Text::String> path;
				Net::WebUtil::RequestMethod reqMethod;
				NN<Text::String> returnType;
				NN<Text::String> sourceFile;
				NN<Text::String> declName;
			};
		private:
			Data::ArrayListNN<ControllerRequest> reqList;
			IO::VirtualPackageFileFast vpf;

			NN<UI::GUIPanel> pnlMain;
			NN<UI::GUILabel> lblPath;
			NN<UI::GUITextBox> txtPath;
			NN<UI::GUILabel> lblMainClass;
			NN<UI::GUITextBox> txtMainClass;
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpController;
			NN<UI::GUIPanel> pnlController;
			NN<UI::GUIButton> btnControllerSave;
			NN<UI::GUIListView> lvController;

			NN<UI::GUITabPage> tpLib;
			NN<UI::GUIListView> lvLib;

			NN<UI::GUITabPage> tpClasses;
			NN<UI::GUIListBox> lbClassPackages;
			NN<UI::GUIListBox> lbClasses;

			NN<SSWR::AVIRead::AVIRCore> core;

			static void __stdcall FreeControllerRequest(NN<ControllerRequest> req);
			static void __stdcall OnControllerSaveClicked(AnyType userObj);
			static void __stdcall OnDirectoryDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			static void __stdcall OnClassPackagesSelChg(AnyType userObj);
			static void __stdcall OnClassesDoubleClicked(AnyType userObj);
			static Optional<IO::ConfigFile> OpenManifest(NN<IO::PackageFile> pkg);
			Bool LoadJar(Text::CStringNN fileName, NN<IO::StreamData> fd);
			Bool LoadPath(Text::CStringNN path);
			Bool LoadClasses(NN<IO::PackageFile> pkg);
			Bool LoadClassesInner(NN<IO::PackageFile> pkg);
			void LoadFile(NN<IO::StreamData> fd);
			void DisplayClasses();
			void DisplayClassesInner(UnsafeArray<UTF8Char> sbuff, UnsafeArray<UTF8Char> sbuffEnd, NN<IO::PackageFile> pf);
		public:
			AVIRJavaSpringForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRJavaSpringForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
