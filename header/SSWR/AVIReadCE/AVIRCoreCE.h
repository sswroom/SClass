#ifndef _SM_SSWR_AVIREADCE_AVIRCORECE
#define _SM_SSWR_AVIREADCE_AVIRCORECE
#include "SSWR/AVIRead/AVIRCore.h"

namespace SSWR
{
	namespace AVIReadCE
	{
		class AVIRCoreCE : public SSWR::AVIRead::AVIRCore
		{
		public:
			AVIRCoreCE(NN<UI::GUICore> ui);
			virtual ~AVIRCoreCE();

			virtual void BeginLoad();
			virtual void EndLoad();
			virtual void OpenObject(NN<IO::ParsedObject> pobj);
			virtual void SaveData(NN<UI::GUIForm> ownerForm, NN<IO::ParsedObject> pobj, const WChar *dialogName);
			virtual Optional<Media::Printer> SelectPrinter(Optional<UI::GUIForm> frm);
		};
	}
}
#endif
