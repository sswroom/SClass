#ifndef _SM_IO_DEVICE_QUECTELGSMMODEMCONTROLLER
#define _SM_IO_DEVICE_QUECTELGSMMODEMCONTROLLER
#include "IO/GSMModemController.h"

namespace IO
{
	namespace Device
	{
		class QuectelGSMModemController : public GSMModemController
		{
		public:
			
			struct ServingCell
			{

			};

			struct NeighbourCell
			{

			};
		public:
			QuectelGSMModemController(NN<IO::ATCommandChannel> channel, Bool needRelease);
			virtual ~QuectelGSMModemController();

			virtual UnsafeArrayOpt<UTF8Char> GetICCID(UnsafeArray<UTF8Char> sbuff);
			virtual UOSInt QueryCells(NN<Data::ArrayListNN<CellSignal>> cells);

			// (U)SIM Related Commands
			UnsafeArrayOpt<UTF8Char> QuectelGetICCID(UnsafeArray<UTF8Char> iccid); //AT+QCCID

			// Network Service Command
			Bool QuectelQueryServingCell(NN<ServingCell> cell); // AT+QENG="servingcell"
			Bool QuectelQueryNeighbourCells(NN<Data::ArrayListNN<NeighbourCell>> cells); // AT+QENG="neighbourcell"
		};
	}
}
#endif
