#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "DB/CSVFile.h"
#include "IO/FileStream.h"
#include "Manage/HiResClock.h"
#include <stdio.h>

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	Text::CStringNN fileName = CSTR("/home/sswroom/Progs/Temp/20221116 CAD/dataProcessed/adsb_movement_statistic/ADSB_Movement_Statistic_201904.csv");
	Manage::HiResClock clk;
	{
		IO::FileStream stm(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		{
			DB::CSVFile csv(stm, 65001);

			NotNullPtr<DB::DBReader> rdr;
			if (rdr.Set(csv.QueryTableData(CSTR_NULL, CSTR_NULL, 0, 0, 0, CSTR_NULL, 0)))
			{
				Int32 rowCnt = 0;
				while (rdr->ReadNext())
				{
		/*			if (rowCnt < 2)
					{
						UTF8Char buff[60];
						Int32 colCnt = rdr->ColCount();
						while (colCnt-- > 0)
						{
							rdr->GetStr(colCnt, buff, sizeof(buff));
							printf("%d->%d = %s\r\n", rowCnt, colCnt, buff);
						}
					}*/
					rowCnt++;
				}
				printf("RowCnt = %d\r\n", rowCnt);
				csv.CloseReader(rdr);
			}
			else
			{
				printf("Error in reading CSV\r\n");
			}
		}
	}
	printf("Time used: %lf\r\n", clk.GetTimeDiff());
	return 0;
}
