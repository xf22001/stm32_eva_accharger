

/*================================================================
 *
 *
 *   文件名称：channel.c
 *   创 建 者：肖飞
 *   创建日期：2021年04月08日 星期四 09时51分12秒
 *   修改日期：2021年04月13日 星期二 14时25分01秒
 *   描    述：
 *
 *================================================================*/
#include "channel.h"

#include "charger.h"
#include "energy_meter.h"

#include "log.h"

static void periodic(void *_channel_info, void *_channels_info)
{
	//debug("channel_id %d periodic!", ((channel_info_t *)_channel_info)->channel_id);
}

static int channel_init(channel_info_t *channel_info)
{
	int ret = 0;
	channels_info_t *channels_info = (channels_info_t *)channel_info->channels_info;
	channel_config_t *channel_config = channel_info->channel_config;

	channel_info->periodic_callback_item.fn = periodic;
	channel_info->periodic_callback_item.fn_ctx = channel_info;

	OS_ASSERT(register_callback(channels_info->common_periodic_chain, &channel_info->periodic_callback_item) == 0);

	debug("channel %d alloc charger %s", channel_info->channel_id, get_channel_config_charger_type(channel_config->charger_config.charger_type));
	channel_info->charger_info = alloc_charger_info(channel_info);
	debug("channel %d alloc energy_meter %s", channel_info->channel_id, get_channel_config_energy_meter_type(channel_config->energy_meter_config.energy_meter_type));
	channel_info->energy_meter_info = alloc_energy_meter_info(channel_info);

	return ret;
}

int handle_channel_event(channel_info_t *channel_info, channel_event_t *channel_event)
{
	int ret = -1;

	debug("channel_id %d process event %s!", channel_info->channel_id, get_channel_event_type_des(channel_event->type));

	switch(channel_event->type) {
		case CHANNEL_EVENT_TYPE_START_CHANNEL: {
		}
		break;

		case CHANNEL_EVENT_TYPE_STOP_CHANNEL: {
		}
		break;

		default: {
		}
		break;
	}

	return ret;
}

channel_info_t *alloc_channels_channel_info(channels_info_t *channels_info)
{
	channels_config_t *channels_config = channels_info->channels_config;
	channel_info_t *channel_info = NULL;
	int i;

	channels_info->channel_number = channels_config->channels_config.channels_number;
	channel_info = (channel_info_t *)os_calloc(channels_info->channel_number, sizeof(channel_info_t));
	OS_ASSERT(channel_info != NULL);

	for(i = 0; i < channels_info->channel_number; i++) {
		channel_info_t *channel_info_item = channel_info + i;

		channel_info_item->channels_info = channels_info;
		channel_info_item->channel_config = channels_config->channels_config.channel_config[i];
		channel_info_item->channel_id = i;

		channel_init(channel_info_item);
	}


	return channel_info;
}
