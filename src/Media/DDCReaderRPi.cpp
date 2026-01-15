#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "Media/DDCReader.h"
#include "Text/MyString.h"

#include <interface/vcos/vcos_types.h>
#include <interface/vcos/vcos.h>
#include <interface/vchi/vchi.h>
#include <interface/vmcs_host/vc_tvservice.h>
#include <interface/vmcs_host/vchost.h>

typedef struct
{
	TVSERVICE_CALLBACK_T  notify_fn;
	void                 *notify_data;
} TVSERVICE_HOST_CALLBACK_T;

typedef struct {
	uint32_t is_valid;
	uint32_t max_modes; //How big the table have we allocated
	uint32_t num_modes; //How many valid entries are there
	TV_SUPPORTED_MODE_NEW_T *modes;
} TVSERVICE_MODE_CACHE_T;

typedef struct {
	//Generic service stuff
	VCHI_SERVICE_HANDLE_T client_handle[VCHI_MAX_NUM_CONNECTIONS]; //To connect to server on VC
	VCHI_SERVICE_HANDLE_T notify_handle[VCHI_MAX_NUM_CONNECTIONS]; //For incoming notification
	uint32_t              msg_flag[VCHI_MAX_NUM_CONNECTIONS];
	char                  command_buffer[TVSERVICE_MSGFIFO_SIZE];
	char                  response_buffer[TVSERVICE_MSGFIFO_SIZE];
	uint32_t              response_length;
	uint32_t              notify_buffer[TVSERVICE_MSGFIFO_SIZE/sizeof(uint32_t)];
	uint32_t              notify_length;
	uint32_t              num_connections;
	VCOS_MUTEX_T          lock;
	TVSERVICE_HOST_CALLBACK_T  callbacks[TVSERVICE_MAX_CALLBACKS];
	int                   initialised;
	int                   to_exit;

	//TV stuff
	uint32_t              copy_protect;

	//HDMI specific stuff
	HDMI_RES_GROUP_T      hdmi_current_group;
	HDMI_MODE_T           hdmi_current_mode;
	HDMI_DISPLAY_OPTIONS_T hdmi_options;

	//If client ever asks for supported modes, we store them for quick return
	HDMI_RES_GROUP_T      hdmi_preferred_group;
	uint32_t              hdmi_preferred_mode;
	TVSERVICE_MODE_CACHE_T dmt_cache;
	TVSERVICE_MODE_CACHE_T cea_cache;

	//SDTV specific stuff
	SDTV_COLOUR_T         sdtv_current_colour;
	SDTV_MODE_T           sdtv_current_mode;
	SDTV_OPTIONS_T        sdtv_options;
	SDTV_CP_MODE_T        sdtv_current_cp_mode;
} TVSERVICE_HOST_STATE_T;

VCHI_INSTANCE_T   vchi_instance;
VCHI_CONNECTION_T *vchi_connection;

static TVSERVICE_HOST_STATE_T tvservice_client;
static VCOS_EVENT_T tvservice_message_available_event;
static VCOS_EVENT_T tvservice_notify_available_event;
static VCOS_THREAD_T tvservice_notify_task;

#define  VCOS_LOG_CATEGORY (&tvservice_log_category)
static VCOS_LOG_CAT_T  tvservice_log_category;

static const char* tvservice_command_strings[] = {
	"get_state",
	"hdmi_on_preferred",
	"hdmi_on_best",
	"hdmi_on_explicit",
	"sdtv_on",
	"off",
	"query_supported_modes",
	"query_mode_support",
	"query_audio_support",
	"enable_copy_protect",
	"disable_copy_protect",
	"show_info",
	"get_av_latency",
	"hdcp_set_key",
	"hdcp_set_srm",
	"set_spd",
	"set_display_options",
	"test_mode_start",
	"test_mode_stop",
	"ddc_read",
	"set_attached",
	"set_property",
	"get_property",
	"get_display_state",
	"end_of_list"
};

static void tvservice_client_callback( void *callback_param, const VCHI_CALLBACK_REASON_T reason, void *msg_handle )
{
	VCOS_EVENT_T *event = (VCOS_EVENT_T *)callback_param;

	vcos_log_trace("[%s]", VCOS_FUNCTION);
	(void)msg_handle;

	if ( reason != VCHI_CALLBACK_MSG_AVAILABLE || event == NULL)
		return;

	vcos_event_signal(event);
}

static void tvservice_notify_callback( void *callback_param, const VCHI_CALLBACK_REASON_T reason, void *msg_handle )
{
	VCOS_EVENT_T *event = (VCOS_EVENT_T *)callback_param;

	vcos_log_trace("[%s]", VCOS_FUNCTION);
	(void)msg_handle;

	if ( reason != VCHI_CALLBACK_MSG_AVAILABLE || event == NULL)
		return;

	vcos_event_signal(event);
}

static __inline int tvservice_lock_obtain (void)
{
	if(tvservice_client.initialised && vcos_mutex_lock(&tvservice_client.lock) == VCOS_SUCCESS)
	{
		if (tvservice_client.initialised)
		{
			vchi_service_use(tvservice_client.client_handle[0]);
			return 0;
		}
		else
			vcos_mutex_unlock(&tvservice_client.lock);
	}

	return -1;
}

static __inline void tvservice_lock_release (void)
{
	if (tvservice_client.initialised)
	{
		vchi_service_release(tvservice_client.client_handle[0]);
	}
	vcos_mutex_unlock(&tvservice_client.lock);
}

static int32_t tvservice_wait_for_reply(void *response, uint32_t max_length) {
	int32_t success = 0;
	uint32_t length_read = 0;
	vcos_log_trace("[%s]", VCOS_FUNCTION);
	do
	{
		//TODO : we need to deal with messages coming through on more than one connections properly
		//At the moment it will always try to read the first connection if there is something there
		//Check if there is something in the queue, if so return immediately
		//otherwise wait for the semaphore and read again
		success = vchi_msg_dequeue( tvservice_client.client_handle[0], response, max_length, &length_read, VCHI_FLAGS_NONE );
	} while( length_read == 0 && vcos_event_wait(&tvservice_message_available_event) == VCOS_SUCCESS);
	if(length_read)
	{
		vcos_log_trace("TV service got reply %d bytes", length_read);
	}
	else
	{
		vcos_log_warn("TV service wait for reply failed");
	}
	return success;
}

static int32_t tvservice_wait_for_bulk_receive(void *buffer, uint32_t max_length) {
	/*if(!vcos_verify(((uint32_t) buffer & 0xf) == 0)) //should be 16 byte aligned
		return -1;*/
	vcos_log_trace("[%s]", VCOS_FUNCTION);
	if(!vcos_verify(buffer))
	{
		vcos_log_error("TV service: NULL buffer passed to wait_for_bulk_receive");
		return -1;
	}

	return vchi_bulk_queue_receive( tvservice_client.client_handle[0],
		buffer,
		max_length,
		VCHI_FLAGS_BLOCK_UNTIL_OP_COMPLETE,
		NULL );
}

static int32_t tvservice_send_command(  uint32_t command, void *buffer, uint32_t length, uint32_t has_reply)
{
	VCHI_MSG_VECTOR_T vector[] = { {&command, sizeof(command)},
		{buffer, (Int32)length} };
	int32_t success = 0;
	TV_GENERAL_RESP_T response;
	response.ret = -1;

	if(vcos_verify(command < VC_TV_END_OF_LIST))
	{
		vcos_log_trace("[%s] command:%s param length %d %s", VCOS_FUNCTION,
			tvservice_command_strings[command], length,
			(has_reply)? "has reply" : " no reply");
	}
	else
	{
		vcos_log_error("[%s] not sending invalid command %d", VCOS_FUNCTION, command);
		return -1;
	}

	if(tvservice_lock_obtain() == 0)
	{
		success = vchi_msg_queuev( tvservice_client.client_handle[0],
				vector, sizeof(vector)/sizeof(vector[0]),
				VCHI_FLAGS_BLOCK_UNTIL_QUEUED, NULL );
		if(success == 0 && has_reply)
		{
	 		//otherwise only wait for a reply if we ask for one
			success = tvservice_wait_for_reply(&response, sizeof(response));
			response.ret = VC_VTOH32(response.ret);
		}
		else
		{
			if(success != 0)
				vcos_log_error("TV service failed to send command %s length %d, error code %d",
					tvservice_command_strings[command], length, success);
			//No reply expected or failed to send, send the success code back instead
			response.ret = success;
		}
		tvservice_lock_release();
	}
	return response.ret;
}

static int32_t tvservice_send_command_reply(  uint32_t command, void *buffer, uint32_t length, void *response, uint32_t max_length)
{
	VCHI_MSG_VECTOR_T vector[] = { {&command, sizeof(command)},
		{buffer, (Int32)length} };
	int32_t success = 0;

	vcos_log_trace("[%s] sending command (with reply) %s param length %d", VCOS_FUNCTION,
		tvservice_command_strings[command], length);

	if(tvservice_lock_obtain() == 0)
	{
		success = vchi_msg_queuev( tvservice_client.client_handle[0],
			vector, sizeof(vector)/sizeof(vector[0]),
			VCHI_FLAGS_BLOCK_UNTIL_QUEUED, NULL );
		if(success == 0)
			success = tvservice_wait_for_reply(response, max_length);
		else
			vcos_log_error("TV service failed to send command %s param length %d, error code %d",
		tvservice_command_strings[command], length, success);

		tvservice_lock_release();
	}

	return success;
}

static void *tvservice_notify_func(void *arg) {
	int32_t success;
	TVSERVICE_HOST_STATE_T *state = (TVSERVICE_HOST_STATE_T *) arg;
	TV_DISPLAY_STATE_T tvstate;

	vcos_log_trace("TV service async thread started");
	/* Check starting state, and put service in use if necessary */
	tvservice_send_command_reply( VC_TV_GET_DISPLAY_STATE, NULL, 0, &tvstate, sizeof(TV_DISPLAY_STATE_T));
	if (tvstate.state & VC_HDMI_ATTACHED)
	{
		/* Connected */
		if (tvstate.state & (VC_HDMI_HDMI | VC_HDMI_DVI))
		{
			/* Mode already selected */
			vchi_service_use(state->notify_handle[0]);
		}
	}

	while(1)
	{
		VCOS_STATUS_T status = vcos_event_wait(&tvservice_notify_available_event);
		if(status != VCOS_SUCCESS || !state->initialised || state->to_exit)
			break;

		do
		{
			uint32_t reason, param1, param2;
			//Get all notifications in the queue
			success = vchi_msg_dequeue( state->notify_handle[0], state->notify_buffer, sizeof(state->notify_buffer), &state->notify_length, VCHI_FLAGS_NONE );
			if(success != 0 || state->notify_length < sizeof(uint32_t)*3 )
			{
				vcos_assert(state->notify_length == sizeof(uint32_t)*3);
				break;
			}

			if(tvservice_lock_obtain() != 0)
				break;

			//Check what notification it is and update ourselves accordingly before notifying the host app
			//All notifications are of format: reason, param1, param2 (all 32-bit unsigned int)
			reason = VC_VTOH32(state->notify_buffer[0]), param1 = VC_VTOH32(state->notify_buffer[1]), param2 = VC_VTOH32(state->notify_buffer[2]);
			vcos_log_trace("[%s] %s %d %d", VCOS_FUNCTION, vc_tv_notification_name((VC_HDMI_NOTIFY_T)reason),
				param1, param2);
			switch(reason)
			{
			case VC_HDMI_UNPLUGGED:
				if(tvstate.state & (VC_HDMI_HDMI|VC_HDMI_DVI|VC_HDMI_ATTACHED))
				{
					state->copy_protect = 0;
					if((tvstate.state & VC_HDMI_ATTACHED) == 0)
					{
						vchi_service_release(state->notify_handle[0]);
					}
				}
				tvstate.state &= ~(VC_HDMI_HDMI|VC_HDMI_DVI|VC_HDMI_ATTACHED|VC_HDMI_HDCP_AUTH);
				tvstate.state |= (VC_HDMI_UNPLUGGED | VC_HDMI_HDCP_UNAUTH);
				vcos_log_trace("[%s] invalidating caches", VCOS_FUNCTION);
				state->cea_cache.is_valid = state->cea_cache.num_modes = 0;
				state->dmt_cache.is_valid = state->dmt_cache.num_modes = 0;
				break;

			case VC_HDMI_ATTACHED:
				if(tvstate.state & (VC_HDMI_HDMI|VC_HDMI_DVI))
				{
					state->copy_protect = 0;
					vchi_service_release(state->notify_handle[0]);
				}
				tvstate.state &=  ~(VC_HDMI_HDMI|VC_HDMI_DVI|VC_HDMI_UNPLUGGED|VC_HDMI_HDCP_AUTH);
				tvstate.state |= VC_HDMI_ATTACHED;
				state->hdmi_preferred_group = (HDMI_RES_GROUP_T) param1;
				state->hdmi_preferred_mode = param2;
				break;

			case VC_HDMI_DVI:
				if(tvstate.state & (VC_HDMI_ATTACHED|VC_HDMI_UNPLUGGED))
				{
					vchi_service_use(state->notify_handle[0]);
				}
				tvstate.state &= ~(VC_HDMI_HDMI|VC_HDMI_ATTACHED|VC_HDMI_UNPLUGGED);
				tvstate.state |= VC_HDMI_DVI;
				state->hdmi_current_group = (HDMI_RES_GROUP_T) param1;
				state->hdmi_current_mode = (HDMI_MODE_T)param2;
				break;

			case VC_HDMI_HDMI:
				if(tvstate.state & (VC_HDMI_ATTACHED|VC_HDMI_UNPLUGGED))
				{
					vchi_service_use(state->notify_handle[0]);
				}
				tvstate.state &= ~(VC_HDMI_DVI|VC_HDMI_ATTACHED|VC_HDMI_UNPLUGGED);
				tvstate.state |= VC_HDMI_HDMI;
				state->hdmi_current_group = (HDMI_RES_GROUP_T) param1;
				state->hdmi_current_mode = (HDMI_MODE_T)param2;
				break;

			case VC_HDMI_HDCP_UNAUTH:
				tvstate.state &= ~VC_HDMI_HDCP_AUTH;
				tvstate.state |= VC_HDMI_HDCP_UNAUTH;
				state->copy_protect = 0;
				//Do we care about the reason for HDCP unauth in param1?
				break;

			case VC_HDMI_HDCP_AUTH:
				tvstate.state &= ~VC_HDMI_HDCP_UNAUTH;
				tvstate.state |= VC_HDMI_HDCP_AUTH;
				state->copy_protect = 1;
				break;

			case VC_HDMI_HDCP_KEY_DOWNLOAD:
			case VC_HDMI_HDCP_SRM_DOWNLOAD:
				//Nothing to do here, just tell the host app whether it is successful or not (in param1)
				break;

			case VC_SDTV_UNPLUGGED: //Currently we don't get this
				if(tvstate.state & (VC_SDTV_PAL | VC_SDTV_NTSC))
				{
					state->copy_protect = 0;
				}
				tvstate.state &= ~(VC_SDTV_ATTACHED | VC_SDTV_PAL | VC_SDTV_NTSC);
				tvstate.state |= (VC_SDTV_UNPLUGGED | VC_SDTV_CP_INACTIVE);
				state->sdtv_current_mode = SDTV_MODE_OFF;
				break;

			case VC_SDTV_ATTACHED: //Currently we don't get this either
				tvstate.state &= ~(VC_SDTV_UNPLUGGED | VC_SDTV_PAL | VC_SDTV_NTSC);
				tvstate.state |= VC_SDTV_ATTACHED;
				state->sdtv_current_mode = SDTV_MODE_OFF;
				break;

			case VC_SDTV_NTSC:
				tvstate.state &= ~(VC_SDTV_UNPLUGGED | VC_SDTV_ATTACHED | VC_SDTV_PAL);
				tvstate.state |= VC_SDTV_NTSC;
				state->sdtv_current_mode = (SDTV_MODE_T) param1;
				state->sdtv_options.aspect = (SDTV_ASPECT_T) param2;
				if(param1 & SDTV_COLOUR_RGB)
				{
					state->sdtv_current_colour = SDTV_COLOUR_RGB;
				}
				else if(param1 & SDTV_COLOUR_YPRPB)
				{
					state->sdtv_current_colour = SDTV_COLOUR_YPRPB;
				}
				else
				{
					state->sdtv_current_colour = SDTV_COLOUR_UNKNOWN;
				}
				break;

			case VC_SDTV_PAL:
				tvstate.state &= ~(VC_SDTV_UNPLUGGED | VC_SDTV_ATTACHED | VC_SDTV_NTSC);
				tvstate.state |= VC_SDTV_PAL;
				state->sdtv_current_mode = (SDTV_MODE_T) param1;
				state->sdtv_options.aspect = (SDTV_ASPECT_T) param2;
				if(param1 & SDTV_COLOUR_RGB)
				{
					state->sdtv_current_colour = SDTV_COLOUR_RGB;
				}
				else if(param1 & SDTV_COLOUR_YPRPB)
				{
					state->sdtv_current_colour = SDTV_COLOUR_YPRPB;
				}
				else
				{
					state->sdtv_current_colour = SDTV_COLOUR_UNKNOWN;
				}
				break;

			case VC_SDTV_CP_INACTIVE:
				tvstate.state &= ~VC_SDTV_CP_ACTIVE;
				tvstate.state |= VC_SDTV_CP_INACTIVE;
				state->copy_protect = 0;
				state->sdtv_current_cp_mode = SDTV_CP_NONE;
				break;

			case VC_SDTV_CP_ACTIVE:
				tvstate.state &= ~VC_SDTV_CP_INACTIVE;
				tvstate.state |= VC_SDTV_CP_ACTIVE;
				state->copy_protect = 1;
				state->sdtv_current_cp_mode = (SDTV_CP_MODE_T) param1;
				break;
			}

			tvservice_lock_release();

			//Now callback the host app(s)
			uint32_t i, called = 0;
			for(i = 0; i < TVSERVICE_MAX_CALLBACKS; i++)
			{
				if(state->callbacks[i].notify_fn != NULL)
				{
					called++;
					state->callbacks[i].notify_fn
					(state->callbacks[i].notify_data, reason, param1, param2);
				} // if
			} // for
			if(called == 0)
			{
				vcos_log_info("TV service: No callback handler specified, callback [%s] swallowed",
				vc_tv_notification_name((VC_HDMI_NOTIFY_T)reason));
			}
		} while(success == 0 && state->notify_length >= sizeof(uint32_t)*3); //read the next message if any
	} //while (1)

	if(state->to_exit)
		vcos_log_trace("TV service async thread exiting");

	return 0;
}

int vc_vchi_tv_init(VCHI_INSTANCE_T initialise_instance, VCHI_CONNECTION_T **connections, uint32_t num_connections )
{
	int32_t success;
	uint32_t i;
	VCOS_STATUS_T status;
	VCOS_THREAD_ATTR_T attrs;
	static const HDMI_DISPLAY_OPTIONS_T hdmi_default_display_options =
	{
		HDMI_ASPECT_UNKNOWN,
		VC_FALSE, 0, 0, // No vertical bar information.
		VC_FALSE, 0, 0, // No horizontal bar information.
		0               // No overscan flags.
	};

	if (tvservice_client.initialised)
		return -2;

	vcos_log_set_level(VCOS_LOG_CATEGORY, VCOS_LOG_ERROR);
	vcos_log_register("tvservice-client", VCOS_LOG_CATEGORY);

	vcos_log_trace("[%s]", VCOS_FUNCTION);

	// record the number of connections
	memset( &tvservice_client, 0, sizeof(TVSERVICE_HOST_STATE_T) );
	tvservice_client.num_connections = num_connections;

	status = vcos_mutex_create(&tvservice_client.lock, "HTV");
	vcos_assert(status == VCOS_SUCCESS);
	status = vcos_event_create(&tvservice_message_available_event, "HTV");
	vcos_assert(status == VCOS_SUCCESS);
	status = vcos_event_create(&tvservice_notify_available_event, "HTV");
	vcos_assert(status == VCOS_SUCCESS);

	//Initialise any other non-zero bits of the TV service state here
	tvservice_client.sdtv_current_mode = SDTV_MODE_OFF;
	tvservice_client.sdtv_options.aspect = SDTV_ASPECT_4_3;
	memcpy(&tvservice_client.hdmi_options, &hdmi_default_display_options, sizeof(HDMI_DISPLAY_OPTIONS_T));

	for (i=0; i < tvservice_client.num_connections; i++)
	{
		// Create a 'Client' service on the each of the connections
		SERVICE_CREATION_T tvservice_parameters = { VCHI_VERSION(VC_TVSERVICE_VER),
			TVSERVICE_CLIENT_NAME,      // 4cc service code
			connections[i],             // passed in fn ptrs
			0,                          // tx fifo size (unused)
			0,                          // tx fifo size (unused)
			&tvservice_client_callback, // service callback
			&tvservice_message_available_event,  // callback parameter
			VC_TRUE,                    // want_unaligned_bulk_rx
			VC_TRUE,                    // want_unaligned_bulk_tx
			VC_FALSE,                   // want_crc
		};

		SERVICE_CREATION_T tvservice_parameters2 = { VCHI_VERSION(VC_TVSERVICE_VER),
			TVSERVICE_NOTIFY_NAME,     // 4cc service code
			connections[i],            // passed in fn ptrs
			0,                         // tx fifo size (unused)
			0,                         // tx fifo size (unused)
			&tvservice_notify_callback,// service callback
			&tvservice_notify_available_event,  // callback parameter
			VC_FALSE,                  // want_unaligned_bulk_rx
			VC_FALSE,                  // want_unaligned_bulk_tx
			VC_FALSE,                  // want_crc
		};

		//Create the client to normal TV service
		success = vchi_service_open( initialise_instance, &tvservice_parameters, &tvservice_client.client_handle[i] );

		//Create the client to the async TV service (any TV related notifications)
		if (success == 0)
		{
			success = vchi_service_open( initialise_instance, &tvservice_parameters2, &tvservice_client.notify_handle[i] );
			if (success == 0)
			{
				vchi_service_release(tvservice_client.client_handle[i]);
				vchi_service_release(tvservice_client.notify_handle[i]);
			}
			else
			{
				vchi_service_close(tvservice_client.client_handle[i]);
				vcos_log_error("Failed to connect to async TV service: %d", success);
			}
		}
		else
		{
			vcos_log_error("Failed to connect to TV service: %d", success);
		}

		if (success != 0)
		{
			while (i-- > 0)
			{
				vchi_service_close(tvservice_client.client_handle[i]);
				vchi_service_close(tvservice_client.notify_handle[i]);
			}
			return -1;
		}
	}

	//Create the notifier task
	vcos_thread_attr_init(&attrs);
	vcos_thread_attr_setstacksize(&attrs, 4096);
	vcos_thread_attr_settimeslice(&attrs, 1);

	status = vcos_thread_create(&tvservice_notify_task, "HTV Notify", &attrs, tvservice_notify_func, &tvservice_client);
	vcos_assert(status == VCOS_SUCCESS);

	tvservice_client.initialised = 1;
	vcos_log_trace("TV service initialised");

	return 0;
}

void vc_vchi_tv_stop( void )
{
	// Wait for the current lock-holder to finish before zapping TV service
	uint32_t i;

	if (!tvservice_client.initialised)
		return;

	vcos_log_trace("[%s]", VCOS_FUNCTION);
	if(tvservice_lock_obtain() == 0)
	{
		void *dummy;
		vchi_service_release(tvservice_client.client_handle[0]); // to match the use in tvservice_lock_obtain()

		for (i=0; i < tvservice_client.num_connections; i++)
		{
			int32_t result;
			vchi_service_use(tvservice_client.client_handle[i]);
			vchi_service_use(tvservice_client.notify_handle[i]);
			result = vchi_service_close(tvservice_client.client_handle[i]);
			vcos_assert( result == 0 );
			result = vchi_service_close(tvservice_client.notify_handle[i]);
			vcos_assert( result == 0 );
		}

		// Unset the initialise flag so no other threads can obtain the lock
		tvservice_client.initialised = 0;

		tvservice_lock_release();
		tvservice_client.to_exit = 1; //Signal to quit
		vcos_event_signal(&tvservice_notify_available_event);
		vcos_thread_join(&tvservice_notify_task, &dummy);

		if (tvservice_client.cea_cache.modes)
			vcos_free(tvservice_client.cea_cache.modes);

		if (tvservice_client.dmt_cache.modes)
			vcos_free(tvservice_client.dmt_cache.modes);

		vcos_mutex_delete(&tvservice_client.lock);
		vcos_event_delete(&tvservice_message_available_event);
		vcos_event_delete(&tvservice_notify_available_event);
	}
}

int vc_tv_hdmi_ddc_read(uint32_t offset, uint32_t length, uint8_t *buffer)
{
	int success;
	TV_DDC_READ_PARAM_T param = {VC_HTOV32(offset), VC_HTOV32(length)};

	vcos_log_trace("[%s]", VCOS_FUNCTION);

	vchi_service_use(tvservice_client.client_handle[0]);
	success = tvservice_send_command( VC_TV_DDC_READ, &param, sizeof(TV_DDC_READ_PARAM_T), 1);

	if(success == 0)
	{
		success = tvservice_wait_for_bulk_receive(buffer, length);
	}
	vchi_service_release(tvservice_client.client_handle[0]);
	return (success == 0)? length : 0; //Either return the whole block or nothing
}

const char* vc_tv_notification_name(VC_HDMI_NOTIFY_T reason)
{
	switch (reason)
	{
	case VC_HDMI_UNPLUGGED:
		return "VC_HDMI_UNPLUGGED";
	case VC_HDMI_ATTACHED:
		return "VC_HDMI_ATTACHED";
	case VC_HDMI_DVI:
		return "VC_HDMI_DVI";
	case VC_HDMI_HDMI:
		return "VC_HDMI_HDMI";
	case VC_HDMI_HDCP_UNAUTH:
		return "VC_HDMI_HDCP_UNAUTH";
	case VC_HDMI_HDCP_AUTH:
		return "VC_HDMI_HDCP_AUTH";
	case VC_HDMI_HDCP_KEY_DOWNLOAD:
		return "VC_HDMI_HDCP_KEY_DOWNLOAD";
	case VC_HDMI_HDCP_SRM_DOWNLOAD:
		return "VC_HDMI_HDCP_SRM_DOWNLOAD";
	case VC_HDMI_CHANGING_MODE:
		return "VC_HDMI_CHANGING_MODE";
	default:
		return "VC_HDMI_UNKNOWN";
	}
}

Media::DDCReader::DDCReader(void *hMon)
{
	this->edid = 0;
	this->edidSize = 0;
	this->hMon = hMon;

	IntOS ret;
	ret = vchi_initialise(&vchi_instance);
	if (ret != 0)
	{
		return;
	}

	ret = vchi_connect(NULL, 0, vchi_instance);
	if (ret != 0)
	{
		return;
	}

	// Initialize the tvservice
	vc_vchi_tv_init(vchi_instance, &vchi_connection, 1);

	UInt8 buffer[128];
	IntOS offset = 0;
	IntOS i;
	IntOS extensions;
	IntOS siz = vc_tv_hdmi_ddc_read(offset, 128, buffer);
	offset += 128;
	if (siz == 128)
	{
		extensions = buffer[0x7e];
		this->edidSize = (extensions + 1) * 128;
		this->edid = MemAlloc(UInt8, this->edidSize);
		MemCopyNO(this->edid, buffer, 128);
		i = 0;
		while (i < extensions)
		{
			siz = vc_tv_hdmi_ddc_read(offset, 128, &this->edid[offset]);
			offset += 128;
			i++;
		 }
	}
}

Media::DDCReader::DDCReader(UnsafeArray<const UTF8Char> monitorId)
{
	this->edid = 0;
	this->edidSize = 0;
	this->hMon = hMon;

	IntOS ret;
	ret = vchi_initialise(&vchi_instance);
	if (ret != 0)
	{
		return;
	}

	ret = vchi_connect(NULL, 0, vchi_instance);
	if (ret != 0)
	{
		return;
	}

	// Initialize the tvservice
	vc_vchi_tv_init(vchi_instance, &vchi_connection, 1);

	UInt8 buffer[128];
	IntOS offset = 0;
	IntOS i;
	IntOS extensions;
	IntOS siz = vc_tv_hdmi_ddc_read(offset, 128, buffer);
	offset += 128;
	if (siz == 128)
	{
		extensions = buffer[0x7e];
		this->edidSize = (extensions + 1) * 128;
		this->edid = MemAlloc(UInt8, this->edidSize);
		MemCopyNO(this->edid, buffer, 128);
		i = 0;
		while (i < extensions)
		{
			siz = vc_tv_hdmi_ddc_read(offset, 128, &this->edid[offset]);
			offset += 128;
			i++;
		 }
	}
}

Media::DDCReader::~DDCReader()
{
	vc_vchi_tv_stop();
	vchi_disconnect(vchi_instance);
	if (this->edid)
	{
		MemFree(this->edid);
		this->edid = 0;
	}
}

UInt8 *Media::DDCReader::GetEDID(OutParam<UIntOS> size)
{
	size.Set(this->edidSize);
	return this->edid;
}

UIntOS Media::DDCReader::CreateDDCReaders(NN<Data::ArrayListNN<DDCReader>> readerList)
{
	NN<Media::DDCReader> reader;
	NEW_CLASSNN(reader, Media::DDCReader((void*)0));
	readerList->Add(reader);
	return 1;
}
