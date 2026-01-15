#ifndef _SM_IO_PROTODEC_PROTODECLIST
#define _SM_IO_PROTODEC_PROTODECLIST
#include "Data/ArrayListNN.hpp"
#include "IO/ProtoDec/ProtocolDecoder.h"

namespace IO
{
	namespace ProtoDec
	{
		class ProtoDecList : public Data::ReadingListNN<ProtocolDecoder>
		{
		private:
			Data::ArrayListNN<ProtocolDecoder> decList;

		public:
			ProtoDecList();
			virtual ~ProtoDecList();

			virtual UIntOS GetCount() const;
			virtual Optional<ProtocolDecoder> GetItem(UIntOS index) const;
			virtual NN<ProtocolDecoder> GetItemNoCheck(UIntOS index) const;
		};
	}
}
#endif
