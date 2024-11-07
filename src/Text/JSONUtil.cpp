#include "Stdafx.h"
#include "Text/JSONUtil.h"

void Text::JSONUtil::ArrayGPSTrack(NN<JSONBuilder> json, NN<Map::GPSTrack> trk)
{
	UnsafeArray<Map::GPSTrack::GPSRecordFull> recs;
	UOSInt recordCnt;
	UOSInt i = 0;
	UOSInt j = trk->GetTrackCnt();
	UOSInt k;
	while (i < j)
	{
		if (trk->GetTrack(i, recordCnt).SetTo(recs))
		{
			k = 0;
			while (k < recordCnt)
			{
				json->ArrayBeginObject();
				json->ObjectAddFloat64(CSTR("lat"), recs[k].pos.GetLat());
				json->ObjectAddFloat64(CSTR("lon"), recs[k].pos.GetLon());
				json->ObjectAddInt64(CSTR("recTime"), recs[k].recTime.ToTicks());
				json->ObjectAddFloat64(CSTR("altitude"), recs[k].altitude);
				json->ObjectAddFloat64(CSTR("speed"), recs[k].speed);
				json->ObjectAddFloat64(CSTR("heading"), recs[k].heading);
				json->ObjectAddBool(CSTR("valid"), recs[k].valid);
				json->ObjectAddInt32(CSTR("sateUsed"), recs[k].nSateUsed);
				json->ObjectEnd();
				k++;
			}
		}
		i++;
	}
}
