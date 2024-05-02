#ifndef _SM_IO_SDCARDMGR
#define _SM_IO_SDCARDMGR
#include "Data/ArrayListNN.h"
#include "IO/SDCardInfo.h"

namespace IO
{
	class SDCardMgr
	{
	public:
		static UOSInt GetCardList(NN<Data::ArrayListNN<IO::SDCardInfo>> cardList);
	};
}
#endif
