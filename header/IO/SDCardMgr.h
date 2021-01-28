#ifndef _SM_IO_SDCARDMGR
#define _SM_IO_SDCARDMGR
#include "Data/ArrayList.h"
#include "IO/SDCardInfo.h"

namespace IO
{
	class SDCardMgr
	{
	public:
		static OSInt GetCardList(Data::ArrayList<IO::SDCardInfo*> *cardList);
	};
};
#endif
