#ifndef _SM_SSWR_AVIREAD_AVIRCOREWIN
#define _SM_SSWR_AVIREAD_AVIRCOREWIN
#include "SSWR/AVIRead/AVIRCore.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRCoreWin : public AVIRCore
		{
		public:
			AVIRCoreWin(NotNullPtr<UI::GUICore> ui);
			virtual ~AVIRCoreWin();

			virtual void OpenObject(NotNullPtr<IO::ParsedObject> pobj);
			virtual void SaveData(UI::GUIForm *ownerForm, NotNullPtr<IO::ParsedObject> pobj, const WChar *dialogName);
			virtual Media::Printer *SelectPrinter(UI::GUIForm *frm);
		};
	}
}
#endif
