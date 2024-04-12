#ifndef _SM_IO_PROTODEC_PROTODECLIST
#define _SM_IO_PROTODEC_PROTODECLIST
#include "Data/ArrayListNN.h"
#include "IO/ProtoDec/IProtocolDecoder.h"

namespace IO
{
	namespace ProtoDec
	{
		class ProtoDecList : public Data::ReadingListNN<IProtocolDecoder>
		{
		private:
			Data::ArrayListNN<IProtocolDecoder> decList;

		public:
			ProtoDecList();
			virtual ~ProtoDecList();

			virtual UOSInt GetCount() const;
			virtual Optional<IProtocolDecoder> GetItem(UOSInt index) const;
			virtual NotNullPtr<IProtocolDecoder> GetItemNoCheck(UOSInt index) const;
		};
	}
}
#endif
