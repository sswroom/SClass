#include "Stdafx.h"
#include "Text/JSONUtil.h"

void Text::JSONUtil::ArrayGPSTrack(NotNullPtr<JSONBuilder> json, NotNullPtr<Map::GPSTrack> trk)
{
	Map::GPSTrack::GPSRecord3 *recs;
	UOSInt recordCnt;
	UOSInt i = 0;
	UOSInt j = trk->GetTrackCnt();
	UOSInt k;
	while (i < j)
	{
		recs = trk->GetTrack(i, recordCnt);
		k = 0;
		while (k < recordCnt)
		{
			json->ArrayBeginObject();
			json->ObjectAddFloat64(CSTR("lat"), recs[k].pos.GetLat());
			json->ObjectAddFloat64(CSTR("lon"), recs[k].pos.GetLon());
			json->ObjectAddInt64(CSTR("t"), recs[k].recTime.ToTicks());
			json->ObjectAddFloat64(CSTR("a"), recs[k].altitude);
			json->ObjectAddFloat64(CSTR("s"), recs[k].speed);
			json->ObjectAddFloat64(CSTR("h"), recs[k].heading);
			json->ObjectAddBool(CSTR("v"), recs[k].valid);
			json->ObjectAddInt32(CSTR("sate"), recs[k].nSateUsed);
			json->ObjectEnd();
			k++;
		}
		i++;
	}
}
