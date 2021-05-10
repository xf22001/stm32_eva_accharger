

/*================================================================
 *
 *
 *   文件名称：channels.c
 *   创 建 者：肖飞
 *   创建日期：2021年01月18日 星期一 09时26分31秒
 *   修改日期：2021年04月13日 星期二 15时36分20秒
 *   描    述：
 *
 *================================================================*/
#include "channels.h"

#include <string.h>
#include <stdlib.h>

#include "object_class.h"
#include "os_utils.h"

#include "channel.h"
#include "channels_power_module.h"

#include "log.h"

static object_class_t *channels_class = NULL;

char *get_channel_event_type_des(channel_event_type_t type)
{
	char *des = "unknow";

	switch(type) {
			add_des_case(CHANNEL_EVENT_TYPE_UNKNOW);
			add_des_case(CHANNEL_EVENT_TYPE_START_CHANNEL);
			add_des_case(CHANNEL_EVENT_TYPE_STOP_CHANNEL);

		default: {
		}
		break;
	}

	return des;
}

static void free_channels_info(channels_info_t *channels_info)
{
	app_panic();
}

static void handle_channels_info_channels_event(void *_channels_info, void *_channels_event)
{
	int i;
	channels_info_t *channels_info = (channels_info_t *)_channels_info;
	channels_event_t *channels_event = (channels_event_t *)_channels_event;
	channel_info_t *channel_info = NULL;
	channel_event_t *channel_event;

	debug("");

	if(channels_event->type != CHANNELS_EVENT_CHANNEL_EVENT) {
		return;
	}

	channel_event = &channels_event->event.channel_event;

	if(channel_event->channel_id == 0xff) { //broadcast
		for(i = 0; i < channels_info->channel_number; i++) {
			channel_info = channels_info->channel_info + i;
			handle_channel_event(channel_info, channel_event);
		}
	} else {
		if(channel_event->channel_id < channels_info->channel_number) {
			channel_info = channels_info->channel_info + channel_event->channel_id;
			handle_channel_event(channel_info, channel_event);
		} else { //channel_id error
			debug("channel_event->channel_id:%d", channel_event->channel_id);
		}
	}
}

static int channels_info_set_channels_config(channels_info_t *channels_info, channels_config_t *channels_config)
{
	int ret = 0;
	debug("use channels channels_config %d!", channels_config->id);

	channels_info->event_pool = alloc_event_pool();

	OS_ASSERT(channels_info->event_pool != NULL);

	channels_info->channel_info = alloc_channels_channel_info(channels_info);

	channels_info->channels_power_module = alloc_channels_power_module(channels_info);

	channels_info->channels_channel_callback_item.fn = handle_channels_info_channels_event;
	channels_info->channels_channel_callback_item.fn_ctx = channels_info;
	OS_ASSERT(register_callback(channels_info->common_event_chain, &channels_info->channels_channel_callback_item) == 0);

	channels_info->configed = 1;

	return ret;
}

static channels_info_t *alloc_channels_info(channels_config_t *channels_config)
{
	channels_info_t *channels_info = NULL;

	OS_ASSERT(channels_config != NULL);

	channels_info = (channels_info_t *)os_calloc(1, sizeof(channels_info_t));

	OS_ASSERT(channels_info != NULL);

	channels_info->channels_config = channels_config;

	channels_info->common_periodic_chain = alloc_callback_chain();
	channels_info->common_event_chain = alloc_callback_chain();

	OS_ASSERT(channels_info_set_channels_config(channels_info, channels_config) == 0);

	return channels_info;
}

static int object_filter(void *o, void *ctx)
{
	int ret = -1;
	channels_info_t *channels_info = (channels_info_t *)o;
	channels_config_t *channels_config = (channels_config_t *)ctx;

	if(channels_info->channels_config == channels_config) {
		ret = 0;
	}

	return ret;
}

channels_info_t *get_or_alloc_channels_info(channels_config_t *channels_config)
{
	channels_info_t *channels_info = NULL;

	os_enter_critical();

	if(channels_class == NULL) {
		channels_class = object_class_alloc();
	}

	os_leave_critical();

	channels_info = (channels_info_t *)object_class_get_or_alloc_object(channels_class, object_filter, channels_config, (object_alloc_t)alloc_channels_info, (object_free_t)free_channels_info);

	return channels_info;
}

static void channels_info_process_event(channels_info_t *channels_info)
{
	channels_event_t *channels_event = NULL;
	int ret = -1;

	if(channels_info == NULL) {
		return;
	}

	ret = event_pool_wait_event(channels_info->event_pool, CHANNEL_TASK_PERIODIC);

	if(ret != 0) {
		return;
	}

	for(channels_event = event_pool_get_event(channels_info->event_pool);
	    channels_event != NULL;
	    channels_event = event_pool_get_event(channels_info->event_pool)) {
		do_callback_chain(channels_info->common_event_chain, channels_event);
		os_free(channels_event);
	}
}

static void channels_info_handle_common_periodic(channels_info_t *channels_info)
{
	do_callback_chain(channels_info->common_periodic_chain, channels_info);
}

static void channels_periodic(channels_info_t *channels_info)
{
	uint32_t ticks = osKernelSysTick();

	if(ticks_duration(ticks, channels_info->periodic_stamp) >= CHANNEL_TASK_PERIODIC) {
		channels_info->periodic_stamp = ticks;
		channels_info_handle_common_periodic(channels_info);
	}
}

int send_channels_event(channels_info_t *channels_info, channels_event_t *channels_event, uint32_t timeout)
{
	return event_pool_put_event(channels_info->event_pool, channels_event, timeout);
}

void task_channels(void const *argument)
{
	channels_info_t *channels_info = (channels_info_t *)argument;

	OS_ASSERT(channels_info != NULL);

	while(1) {
		if(channels_info->configed == 0) {
			osDelay(10);
			continue;
		}

		channels_info_process_event(channels_info);

		//处理周期性事件
		channels_periodic(channels_info);
	}
}
