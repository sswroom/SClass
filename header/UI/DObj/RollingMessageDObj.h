#ifndef _SM_UI_DOBJ_ROLLINGMESSAGEDOBJ
#define _SM_UI_DOBJ_ROLLINGMESSAGEDOBJ
#include "Data/DateTime.h"
#include "Data/FastMapNN.hpp"
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
				NN<Text::String> message;
				Optional<Media::DrawImage> img;
				Bool deleted;
			};
		private:
			Math::Size2D<UIntOS> size;
			NN<Media::DrawEngine> deng;
			Double rollSpeed;
			IntOS lastRollPos;
			Data::DateTime startTime;
			UInt32 nextMsgId;
			Sync::Mutex msgMut;
			Optional<MessageInfo> lastMessage;
			Optional<MessageInfo> thisMessage;
			Data::FastMapNN<UInt32, MessageInfo> msgMap;
			UIntOS nextMsgIndex;
			IntOS lastMsgOfst;

		protected:
			virtual Optional<Media::DrawImage> GenerateImage(NN<Media::DrawEngine> deng, NN<Text::String> message, Math::Size2D<UIntOS> drawSize, NN<Media::DrawImage> scnImg) = 0;
		private:
			void FreeMessage(NN<MessageInfo> msg);
		public:
			RollingMessageDObj(NN<Media::DrawEngine> deng, Math::Coord2D<IntOS> tl, Math::Size2D<UIntOS> size, Double rollSpeed);
			virtual ~RollingMessageDObj();

			virtual Bool IsChanged();
			virtual Bool DoEvents();
			virtual void DrawObject(NN<Media::DrawImage> dimg);

			virtual Bool IsObject(Math::Coord2D<IntOS> scnPos);
			virtual void OnMouseDown();
			virtual void OnMouseUp();
			virtual void OnMouseClick();

			UInt32 AddMessage(Text::CStringNN message);
			UInt32 AddMessage(NN<Text::String> message);
			void RemoveMessage(UInt32 msgId);

			void SetSize(Math::Size2D<UIntOS> size);
		};
	}
}
#endif
