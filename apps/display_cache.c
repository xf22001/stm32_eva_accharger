

/*================================================================
 *
 *
 *   文件名称：display_cache.c
 *   创 建 者：肖飞
 *   创建日期：2021年07月17日 星期六 09时42分40秒
 *   修改日期：2021年07月20日 星期二 17时46分04秒
 *   描    述：
 *
 *================================================================*/
#include "display_cache.h"

#include <stdio.h>

#include "app.h"
#include "channels.h"
#include "channel.h"

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
		app_info->display_cache_app.port = port;
	}
}

void sync_app_display_cache(app_info_t *app_info)
{
	if(app_info->display_cache_app.ip_sync != 0) {
		app_info->display_cache_app.ip_sync = 0;

		snprintf(app_info->mechine_info.uri, 256, "tcp://%d.%d:%d.%d:%d",
		         app_info->display_cache_app.ip[0],
		         app_info->display_cache_app.ip[1],
		         app_info->display_cache_app.ip[2],
		         app_info->display_cache_app.ip[3],
		         app_info->display_cache_app.port);
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
			tm.tm_mon = get_u8_from_bcd(channels_info->display_cache_channels.record_dt_cache.mon);
			tm.tm_mday = get_u8_from_bcd(channels_info->display_cache_channels.record_dt_cache.day) - 1;
			channel_record_task_info->page_load_time = mktime(&tm);

			channel_record_item_page_load_location(channel_record_task_info);
		} else {
			channel_record_item_page_load_current(channel_record_task_info);
		}

	}
}

void load_channel_display_cache(channel_info_t *channel_info)
{
}

void sync_channel_display_cache(channel_info_t *channel_info)
{
	if(channel_info->display_cache_channel.charger_start_sync == 1) {
		int hour;
		int min;
		struct tm tm = {0};

		channel_info->display_cache_channel.charger_start_sync = 0;

		hour = get_u8_from_bcd(channel_info->display_cache_channel.start_hour);
		min = get_u8_from_bcd(channel_info->display_cache_channel.start_min);
		tm.tm_hour = hour;
		tm.tm_min = min;
		channel_info->channel_record_item.start_time = mktime(&tm);

		hour = get_u8_from_bcd(channel_info->display_cache_channel.stop_hour);
		min = get_u8_from_bcd(channel_info->display_cache_channel.stop_min);
		tm.tm_hour = hour;
		tm.tm_min = min;
		channel_info->channel_record_item.stop_time = mktime(&tm);
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

	if(offset >= RECORD_ITEM_CACHE_NUMBER) {
		return;
	}

	memset(record_item_cache, 0, sizeof(record_item_cache_t));

	if(channel_record_item == NULL) {
		return;
	}

	record_item_cache->channel_id = channel_record_item->channel_id + 1;
	memcpy(record_item_cache->account, channel_record_item->account, 32);
	tm = localtime(&channel_record_item->start_time);
	hour = get_bcd_from_u8(tm->tm_hour);
	min = get_bcd_from_u8(tm->tm_min);
	record_item_cache->start_hour_min = get_u16_from_u8_lh(min, hour);
	tm = localtime(&channel_record_item->stop_time);
	hour = get_bcd_from_u8(tm->tm_hour);
	min = get_bcd_from_u8(tm->tm_min);
	record_item_cache->stop_hour_min = get_u16_from_u8_lh(min, hour);
	record_item_cache->energy_h = get_u16_1_from_u32(channel_record_item->energy);
	record_item_cache->energy_l = get_u16_0_from_u32(channel_record_item->energy);
	record_item_cache->amount_h = get_u16_1_from_u32(channel_record_item->amount);
	record_item_cache->amount_l = get_u16_0_from_u32(channel_record_item->amount);
	record_item_cache->start_reason = get_u16_0_from_u32(channel_record_item->start_reason);
	record_item_cache->stop_reason = get_u16_0_from_u32(channel_record_item->stop_reason);
}

