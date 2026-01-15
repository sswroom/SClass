#ifndef _SM_SSWR_AVIREAD_AVIRDBMANAGER
#define _SM_SSWR_AVIREAD_AVIRDBMANAGER

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRDBManager
		{
		public:
			virtual UIntOS GetDataSourceCount() const = 0;
			virtual void GetDataSourceName(UIntOS index, NN<Text::StringBuilderUTF8> sb) const = 0;
			virtual Optional<DB::ReadingDB> OpenDataSource(UIntOS index) = 0;
		};
	}
}
#endif
