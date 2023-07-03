#ifndef _SM_UI_DOBJ_ROLLINGMESSAGEDOBJ
#define _SM_UI_DOBJ_ROLLINGMESSAGEDOBJ
#include "Data/DateTime.h"
#include "Data/FastMap.h"
#include "UI/DObj/DirectObject.h"

namespace UI
{
	namespace DObj
	{
		class RollingMessageDObj : public DirectObject
		{
		protected:
			struct MessageInfo
			{
				UInt32 id;
				Text::String *message;
				Media::DrawImage *img;
				Bool deleted;
			};
		private:
			Math::Size2D<UOSInt> size;
			Media::DrawEngine *deng;
			Double rollSpeed;
			OSInt lastRollPos;
			Data::DateTime startTime;
			UInt32 nextMsgId;
			Sync::Mutex msgMut;
			MessageInfo *lastMessage;
			MessageInfo *thisMessage;
			Data::FastMap<UInt32, MessageInfo*> msgMap;
			UOSInt nextMsgIndex;
			OSInt lastMsgOfst;

		protected:
			virtual Media::DrawImage *GenerateImage(Media::DrawEngine *deng, Text::String *message, Math::Size2D<UOSInt> drawSize, Media::DrawImage *scnImg) = 0;
		private:
			void FreeMessage(MessageInfo *msg);
		public:
			RollingMessageDObj(Media::DrawEngine *deng, Math::Coord2D<OSInt> tl, Math::Size2D<UOSInt> size, Double rollSpeed);
			virtual ~RollingMessageDObj();

			virtual Bool IsChanged();
			virtual Bool DoEvents();
			virtual void DrawObject(Media::DrawImage *dimg);

			virtual Bool IsObject(Math::Coord2D<OSInt> scnPos);
			virtual void OnMouseDown();
			virtual void OnMouseUp();
			virtual void OnMouseClick();

			UInt32 AddMessage(Text::CString message);
			UInt32 AddMessage(Text::String *message);
			void RemoveMessage(UInt32 msgId);

			void SetSize(Math::Size2D<UOSInt> size);
		};
	}
}
#endif
