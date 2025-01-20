#ifndef _SM_IO_PROTODEC_PROTODECLIST
#define _SM_IO_PROTODEC_PROTODECLIST
#include "Data/ArrayListNN.h"
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

			virtual UOSInt GetCount() const;
			virtual Optional<ProtocolDecoder> GetItem(UOSInt index) const;
			virtual NN<ProtocolDecoder> GetItemNoCheck(UOSInt index) const;
		};
	}
}
#endif
