

/*================================================================
 *
 *
 *   文件名称：display_cache.c
 *   创 建 者：肖飞
 *   创建日期：2021年07月17日 星期六 09时42分40秒
 *   修改日期：2021年08月29日 星期日 17时48分20秒
 *   描    述：
 *
 *================================================================*/
#include "display_cache.h"

#include <stdio.h>

#include "app.h"
#include "channels.h"
#include "channel.h"
#include "log.h"

typedef enum {
	DISPLAY_CHARGER_STOP_NONE = 0,//未关机
	DISPLAY_CHARGER_STOP_BMS,//BMS停机
	DISPLAY_CHARGER_STOP_SCREEN,//手动点击触摸屏停机
	DISPLAY_CHARGER_STOP_NO_LOAD,//空载停机
	DISPLAY_CHARGER_STOP_ELCMETER_A_DISCONNECT,//电表1未连接停机
	DISPLAY_CHARGER_STOP_ELCMETER_B_DISCONNECT,//电表2未连接停机
	DISPLAY_CHARGER_STOP_CARD_DISCONNECT,//刷卡板未连接停机
	DISPLAY_CHARGER_STOP_GUN_DISCONNECT,//充电枪连接断开停机
	DISPLAY_CHARGER_STOP_OVERCURRENT,//过流停机
	DISPLAY_CHARGER_STOP_SYS_ERR,//系统故障停机
	DISPLAY_CHARGER_STOP_AC_U_OVER,//输入过压停机
	DISPLAY_CHARGER_STOP_AC_U_BELOW,//输入欠压停机
	DISPLAY_CHARGER_STOP_MONEY,//按金额充电停机
	DISPLAY_CHARGER_STOP_ENERGY,//按电量充电停机
	DISPLAY_CHARGER_STOP_TIME,//按时间充电停机
	DISPLAY_CHARGER_STOP_DURATION,//按时长充电停机
	DISPLAY_CHARGER_STOP_BACK_STAGE,//后台关机
	DISPLAY_CHARGER_STOP_NO_POWER,//断电停机
	DISPLAY_CHARGER_STOP_NO_MONEY,//余额不足
} display_charger_stop_reason_t;

uint16_t display_cache_get_stop_reason(channel_record_item_stop_reason_t reason, uint8_t channel_id)
{
	uint16_t display_charger_stop_reason = DISPLAY_CHARGER_STOP_NONE;

	switch(reason) {
		case CHANNEL_RECORD_ITEM_STOP_REASON_BMS: {
			display_charger_stop_reason = DISPLAY_CHARGER_STOP_BMS;
		}
		break;

		case CHANNEL_RECORD_ITEM_STOP_REASON_MANUAL: {
			display_charger_stop_reason = DISPLAY_CHARGER_STOP_SCREEN;
		}
		break;

		case CHANNEL_RECORD_ITEM_STOP_REASON_FAULT_ENERGYMETER: {
			if(channel_id == 0) {
				display_charger_stop_reason = DISPLAY_CHARGER_STOP_ELCMETER_A_DISCONNECT;
			} else if(channel_id == 1) {
				display_charger_stop_reason = DISPLAY_CHARGER_STOP_ELCMETER_B_DISCONNECT;
			}

		}
		break;

		case CHANNEL_RECORD_ITEM_STOP_REASON_FAULT_CARD_READER: {
			display_charger_stop_reason = DISPLAY_CHARGER_STOP_CARD_DISCONNECT;
		}
		break;

		case DISPLAY_CHARGER_STOP_GUN_DISCONNECT: {
			display_charger_stop_reason = DISPLAY_CHARGER_STOP_CARD_DISCONNECT;
		}
		break;

		case CHANNEL_RECORD_ITEM_STOP_REASON_BMS_OVER_CURRENT: {
			display_charger_stop_reason = DISPLAY_CHARGER_STOP_OVERCURRENT;
		}
		break;

		case CHANNEL_RECORD_ITEM_STOP_REASON_FAULT_INPUT_OVER_VOLTAGE: {
			display_charger_stop_reason = DISPLAY_CHARGER_STOP_AC_U_OVER;
		}
		break;

		case CHANNEL_RECORD_ITEM_STOP_REASON_FAULT_INPUT_LOW_VOLTAGE: {
			display_charger_stop_reason = DISPLAY_CHARGER_STOP_AC_U_BELOW;
		}
		break;

		case CHANNEL_RECORD_ITEM_STOP_REASON_AMOUNT: {
			display_charger_stop_reason = DISPLAY_CHARGER_STOP_MONEY;
		}
		break;

		case CHANNEL_RECORD_ITEM_STOP_REASON_ENERGY: {
			display_charger_stop_reason = DISPLAY_CHARGER_STOP_ENERGY;
		}
		break;

		case CHANNEL_RECORD_ITEM_STOP_REASON_DURATION: {
			display_charger_stop_reason = DISPLAY_CHARGER_STOP_TIME;
		}
		break;

		case CHANNEL_RECORD_ITEM_STOP_REASON_REMOTE: {
			display_charger_stop_reason = DISPLAY_CHARGER_STOP_BACK_STAGE;
		}
		break;

		default: {
			display_charger_stop_reason = DISPLAY_CHARGER_STOP_SCREEN;
			break;
		}
		break;
	}

	return display_charger_stop_reason;
}

static void load_device_id(uint16_t *id, uint16_t *cache, uint16_t size)
{
	u_uint16_bytes_t *u_uint16_bytes_id = (u_uint16_bytes_t *)id;
	u_uint16_bytes_t *u_uint16_bytes_cache = (u_uint16_bytes_t *)cache;
	int i;

	for(i = 0; i < size; i++) {
		uint8_t byte0 = u_uint16_bytes_id->s.byte0;
		uint8_t byte1 = u_uint16_bytes_id->s.byte1;

		if(byte0 == 0) {
			byte0 = 0xff;
		}

		if(byte1 == 0) {
			byte1 = 0xff;
		}

		u_uint16_bytes_cache->s.byte0 = byte1;
		u_uint16_bytes_cache->s.byte1 = byte0;
		u_uint16_bytes_id++;
		u_uint16_bytes_cache++;
	}
}

static void set_device_id(uint16_t *id, uint16_t *cache, uint16_t size)
{
	u_uint16_bytes_t *u_uint16_bytes_id = (u_uint16_bytes_t *)id;
	u_uint16_bytes_t *u_uint16_bytes_cache = (u_uint16_bytes_t *)cache;
	int i;

	for(i = 0; i < size; i++) {
		uint8_t byte0 = u_uint16_bytes_cache->s.byte0;
		uint8_t byte1 = u_uint16_bytes_cache->s.byte1;

		if(byte0 == 0xff) {
			byte0 = 0;
		}

		if(byte1 == 0xff) {
			byte1 = 0;
		}

		u_uint16_bytes_id->s.byte0 = byte1;
		u_uint16_bytes_id->s.byte1 = byte0;
		u_uint16_bytes_id++;
		u_uint16_bytes_cache++;
	}
}

void load_app_display_cache(app_info_t *app_info)
{
	int d0;
	int d1;
	int d2;
	int d3;
	int port;
	int ret = sscanf(app_info->mechine_info.uri, "tcp://%d.%d.%d.%d:%d",
	                 &d0,
	                 &d1,
	                 &d2,
	                 &d3,
	                 &port);

	if(ret == 5) {
		app_info->display_cache_app.ip[0] = d0;
		app_info->display_cache_app.ip[1] = d1;
		app_info->display_cache_app.ip[2] = d2;
		app_info->display_cache_app.ip[3] = d3;
		app_info->display_cache_app.ip[4] = port;
	}

	load_device_id((uint16_t *)app_info->mechine_info.device_id, (uint16_t *)app_info->display_cache_app.device_id, 16);
}

void sync_app_display_cache(app_info_t *app_info)
{
	if(app_info->display_cache_app.ip_sync != 0) {
		app_info->display_cache_app.ip_sync = 0;

		snprintf(app_info->mechine_info.uri, 256, "tcp://%d.%d.%d.%d:%d",
		         app_info->display_cache_app.ip[0],
		         app_info->display_cache_app.ip[1],
		         app_info->display_cache_app.ip[2],
		         app_info->display_cache_app.ip[3],
		         app_info->display_cache_app.ip[4]);
		app_info->mechine_info_invalid = 1;
	}

	if(app_info->display_cache_app.device_id_sync != 0) {
		app_info->display_cache_app.device_id_sync = 0;
		set_device_id((uint16_t *)app_info->mechine_info.device_id, (uint16_t *)app_info->display_cache_app.device_id, 16);

		app_info->mechine_info_invalid = 1;
	}
}

void load_channels_display_cache(channels_info_t *channels_info)
{
	int i;
	int j = 0;
	uint8_t end_index = 0;
	uint32_t price = channels_info->channels_settings.price_info.price[0];

	for(i = 0; i < PRICE_SEGMENT_SIZE; i++) {
		if(channels_info->channels_settings.price_info.price[i] != price) {
			time_t ts;
			struct tm *tm;

			end_index = i;
			ts = get_ts_by_seg_index(end_index);
			tm = localtime(&ts);

			channels_info->display_cache_channels.price_item_cache[j].hour = tm->tm_hour;
			channels_info->display_cache_channels.price_item_cache[j].min = tm->tm_min;
			channels_info->display_cache_channels.price_item_cache[j].price = price;
			j++;

			price = channels_info->channels_settings.price_info.price[i];
		}
	}

	channels_info->display_cache_channels.price_item_cache[j].hour = 24;
	channels_info->display_cache_channels.price_item_cache[j].min = 0;
	channels_info->display_cache_channels.price_item_cache[j].price = price;
	j++;


	channels_info->display_cache_channels.withholding_l = get_u16_0_from_u32(channels_info->channels_settings.withholding);
	channels_info->display_cache_channels.withholding_h = get_u16_0_from_u32(channels_info->channels_settings.withholding);
}

void sync_channels_display_cache(channels_info_t *channels_info)
{
	if(channels_info->display_cache_channels.price_sync == 1) {
		int i;
		struct tm tm = {0};
		uint8_t start_index = 0;
		uint8_t end_index = 0;

		channels_info->display_cache_channels.price_sync = 0;

		for(i = 0; i < 20; i++) {
			int j;
			price_item_cache_t *price_item_cache = &channels_info->display_cache_channels.price_item_cache[i];
			tm.tm_hour = get_u8_from_bcd(price_item_cache->hour);
			tm.tm_min = get_u8_from_bcd(price_item_cache->min);

			end_index = get_seg_index_by_ts(mktime(&tm));

			if(end_index == 0) {
				end_index = PRICE_SEGMENT_SIZE;
			}

			for(j = start_index; j < end_index; j++) {
				channels_info->channels_settings.price_info.price[j] = price_item_cache->price;
			}

			if(end_index == PRICE_SEGMENT_SIZE) {
				break;
			}

			start_index = end_index;
		}

		channels_info->channels_settings_invalid = 1;
	}

	if(channels_info->display_cache_channels.withholding_sync == 1) {
		channels_info->display_cache_channels.withholding_sync = 0;

		channels_info->channels_settings.withholding = get_u32_from_u16_01(channels_info->display_cache_channels.withholding_l, channels_info->display_cache_channels.withholding_h);
		channels_info->channels_settings_invalid = 1;

	}

	if(channels_info->display_cache_channels.datetime_sync != 0) {
		struct tm tm = {0};

		channels_info->display_cache_channels.datetime_sync = 0;

		tm.tm_year = channels_info->display_cache_channels.datetime_cache.year - 1900;
		tm.tm_mon = channels_info->display_cache_channels.datetime_cache.mon - 1;
		tm.tm_mday = channels_info->display_cache_channels.datetime_cache.day;
		tm.tm_hour = channels_info->display_cache_channels.datetime_cache.hour;
		tm.tm_min = channels_info->display_cache_channels.datetime_cache.min;
		tm.tm_sec = channels_info->display_cache_channels.datetime_cache.sec;
		tm.tm_wday = channels_info->display_cache_channels.datetime_cache.wday;

		set_time(mktime(&tm));
	}

	if(channels_info->display_cache_channels.record_sync == 1) {
		channel_record_task_info_t *channel_record_task_info = get_or_alloc_channel_record_task_info(0);

		channels_info->display_cache_channels.record_sync = 0;

		if(channels_info->display_cache_channels.record_load_cmd == 1) {
			struct tm tm = {0};
			uint8_t year_h = get_u8_l_from_u16(channels_info->display_cache_channels.record_dt_cache.year);
			uint8_t year_l = get_u8_h_from_u16(channels_info->display_cache_channels.record_dt_cache.year);

			tm.tm_year = get_u16_from_bcd_b01(year_l, year_h) - 1900;
			tm.tm_mon = get_u8_from_bcd(channels_info->display_cache_channels.record_dt_cache.mon) - 1;
			tm.tm_mday = get_u8_from_bcd(channels_info->display_cache_channels.record_dt_cache.day);
			channel_record_task_info->page_load_time = mktime(&tm);

			channel_record_item_page_load_location(channel_record_task_info);
		} else {
			channel_record_item_page_load_current(channel_record_task_info);
		}

	}
}

void load_channel_display_cache(channel_info_t *channel_info)
{
	switch(channel_info->channel_settings.energy_meter_settings.type) {
		case ENERGY_METER_TYPE_AC_HLW8032: {
			channel_info->display_cache_channel.energy_meter_type = 0;
		}
		break;

		case ENERGY_METER_TYPE_AC: {
			channel_info->display_cache_channel.energy_meter_type = 1;
		}
		break;

		case ENERGY_METER_TYPE_AC_SDM_220: {
			channel_info->display_cache_channel.energy_meter_type = 3;
		}
		break;

		default: {
			channel_info->display_cache_channel.energy_meter_type = 2;
		}
		break;
	}
}

void sync_channel_display_cache(channel_info_t *channel_info)
{
	if(channel_info->display_cache_channel.charger_start_sync == 1) {
		channel_event_type_t type = CHANNEL_EVENT_TYPE_UNKNOW;
		channel_event_t *channel_event;
		channels_event_t *channels_event;
		channels_info_t *channels_info = (channels_info_t *)channel_info->channels_info;

		channel_info->display_cache_channel.charger_start_sync = 0;

		if(channel_info->display_cache_channel.onoff == 1) {//开机
			if(channel_info->state != CHANNEL_STATE_IDLE) {
				debug("");
				return;
			}

			channel_info->channel_event_start_display.charge_mode = channel_info->display_cache_channel.charge_mode;
			channel_info->channel_event_start_display.start_reason = channel_info->display_cache_channel.start_reason;
			type = CHANNEL_EVENT_TYPE_START_CHANNEL;

			switch(channel_info->display_cache_channel.charge_mode) {
				case CHANNEL_RECORD_CHARGE_MODE_UNLIMIT: {
				}
				break;

				case CHANNEL_RECORD_CHARGE_MODE_DURATION: {
					int hour;
					int min;
					struct tm tm = {0};

					hour = get_u8_from_bcd(channel_info->display_cache_channel.start_hour);
					min = get_u8_from_bcd(channel_info->display_cache_channel.start_min);
					tm.tm_hour = hour;
					tm.tm_min = min;
					channel_info->channel_event_start_display.start_time = mktime(&tm);

					hour = get_u8_from_bcd(channel_info->display_cache_channel.stop_hour);
					min = get_u8_from_bcd(channel_info->display_cache_channel.stop_min);
					tm.tm_hour = hour;
					tm.tm_min = min;

					channel_info->channel_event_start_display.charge_duration = mktime(&tm) - channel_info->channel_event_start_display.start_time;
				}
				break;

				case CHANNEL_RECORD_CHARGE_MODE_AMOUNT: {
					channel_info->channel_event_start_display.charge_amount = channel_info->display_cache_channel.charge_amount;
				}
				break;

				case CHANNEL_RECORD_CHARGE_MODE_ENERGY: {
					channel_info->channel_event_start_display.charge_energy = channel_info->display_cache_channel.charge_energy;
				}
				break;

				default: {

				}
				break;
			}
		} else {
			channel_info->channel_event_stop.stop_reason = CHANNEL_RECORD_ITEM_STOP_REASON_MANUAL;
			type = CHANNEL_EVENT_TYPE_STOP_CHANNEL;
		}

		channel_event = os_calloc(1, sizeof(channel_event_t));
		channels_event = os_calloc(1, sizeof(channels_event_t));

		OS_ASSERT(channel_event != NULL);
		OS_ASSERT(channels_event != NULL);

		channel_event->channel_id = channel_info->channel_id;
		channel_event->type = type;
		channel_event->ctx = &channel_info->channel_event_start_display;

		channels_event->type = CHANNELS_EVENT_CHANNEL;
		channels_event->event = channel_event;

		if(send_channels_event(channels_info, channels_event, 100) != 0) {
		}
	}

	if(channel_info->display_cache_channel.energy_meter_type_sync == 1) {
		channel_info->display_cache_channel.energy_meter_type_sync = 0;

		switch(channel_info->display_cache_channel.energy_meter_type) {
			case 0: {
				channel_info->channel_settings.energy_meter_settings.type = ENERGY_METER_TYPE_AC_HLW8032;
			}
			break;

			case 1: {
				channel_info->channel_settings.energy_meter_settings.type = ENERGY_METER_TYPE_AC;
			}
			break;

			case 3: {
				debug("");
				channel_info->channel_settings.energy_meter_settings.type = ENERGY_METER_TYPE_AC_SDM_220;
			}
			break;

			default: {
				channel_info->channel_settings.energy_meter_settings.type = ENERGY_METER_TYPE_NONE;
			}
			break;
		}

		channel_info->channel_settings_invalid = 1;
	}
}

int get_channel_record_page_load_item_number(void)
{
	return RECORD_ITEM_CACHE_NUMBER;
}

void channel_record_item_page_item_refresh(channel_record_item_t *channel_record_item, uint16_t offset, uint16_t id)
{
	channels_info_t *channels_info = get_channels();
	record_item_cache_t *record_item_cache = &channels_info->display_cache_channels.record_item_cache[offset];
	uint8_t hour;
	uint8_t min;
	struct tm *tm;
	time_t ts;

	if(offset >= RECORD_ITEM_CACHE_NUMBER) {
		return;
	}

	memset(record_item_cache, 0, sizeof(record_item_cache_t));

	if(channel_record_item == NULL) {
		return;
	}

	record_item_cache->channel_id = channel_record_item->channel_id + 1;
	memcpy(record_item_cache->account, channel_record_item->account, 32);
	ts = channel_record_item->start_time;
	tm = localtime(&ts);
	hour = get_bcd_from_u8(tm->tm_hour);
	min = get_bcd_from_u8(tm->tm_min);
	record_item_cache->start_hour_min = get_u16_from_u8_lh(min, hour);
	ts = channel_record_item->stop_time;
	tm = localtime(&ts);
	hour = get_bcd_from_u8(tm->tm_hour);
	min = get_bcd_from_u8(tm->tm_min);
	record_item_cache->stop_hour_min = get_u16_from_u8_lh(min, hour);
	record_item_cache->energy_h = get_u16_1_from_u32(channel_record_item->energy);
	record_item_cache->energy_l = get_u16_0_from_u32(channel_record_item->energy);
	record_item_cache->amount_h = get_u16_1_from_u32(channel_record_item->amount);
	record_item_cache->amount_l = get_u16_0_from_u32(channel_record_item->amount);
	record_item_cache->start_reason = channel_record_item->start_reason;
	record_item_cache->stop_reason = display_cache_get_stop_reason(channel_record_item->stop_reason, channel_record_item->channel_id);
}

