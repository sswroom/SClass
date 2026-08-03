#ifndef _SM_SSWR_AVIREAD_AVIRJAVASPRINGFORM
#define _SM_SSWR_AVIREAD_AVIRJAVASPRINGFORM
#include "IO/Java/JavaClass.h"
#include "Net/WebUtil.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIListView.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"

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

			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpController;
			NN<UI::GUIPanel> pnlController;
			NN<UI::GUIButton> btnControllerSave;
			NN<UI::GUIListView> lvController;

			NN<SSWR::AVIRead::AVIRCore> core;

			static void __stdcall FreeControllerRequest(NN<ControllerRequest> req);
			static void __stdcall OnControllerSaveClicked(AnyType userObj);
			static void __stdcall OnDirectoryDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			Bool LoadPath(Text::CStringNN path);
			Bool LoadPackage(NN<IO::PackageFile> pkg);
			Bool LoadPackageInner(NN<IO::PackageFile> pkg);
			void LoadFile(NN<IO::StreamData> fd);
		public:
			AVIRJavaSpringForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRJavaSpringForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
