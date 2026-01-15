#ifndef _SM_IO_SDCARDMGR
#define _SM_IO_SDCARDMGR
#include "Data/ArrayListNN.hpp"
#include "IO/SDCardInfo.h"

namespace IO
{
	class SDCardMgr
	{
	public:
		static UIntOS GetCardList(NN<Data::ArrayListNN<IO::SDCardInfo>> cardList);
	};
}
#endif
