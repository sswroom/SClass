#ifndef _SM_IO_PROTODEC_PROTODECLIST
#define _SM_IO_PROTODEC_PROTODECLIST
#include "Data/ArrayList.h"
#include "IO/ProtoDec/IProtocolDecoder.h"

namespace IO
{
	namespace ProtoDec
	{
		class ProtoDecList : public Data::ReadingList<IProtocolDecoder*>
		{
		private:
			Data::ArrayList<IProtocolDecoder*> *decList;

		public:
			ProtoDecList();
			virtual ~ProtoDecList();

			virtual UOSInt GetCount();
			virtual IProtocolDecoder *GetItem(UOSInt index);
		};
	}
}
#endif
