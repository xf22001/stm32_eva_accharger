

/*================================================================
 *   
 *   
 *   文件名称：display_cache.h
 *   创 建 者：肖飞
 *   创建日期：2021年07月17日 星期六 09时42分47秒
 *   修改日期：2021年07月17日 星期六 13时38分31秒
 *   描    述：
 *
 *================================================================*/
#ifndef _DISPLAY_CACHE_H
#define _DISPLAY_CACHE_H
#ifdef __cplusplus
extern "C"
{
#endif

#include "app_platform.h"
#include "cmsis_os.h"

#ifdef __cplusplus
}
#endif

#pragma pack(push, 1)

typedef struct {
	uint8_t ip_sync;
	uint16_t ip[4];
	uint16_t port;
} display_cache_app_t;

typedef struct {
	uint16_t hour;
	uint16_t min;
	uint16_t price;
} price_item_cache_t;

typedef struct {
	uint16_t sec;
	uint16_t min;
	uint16_t hour;
	uint16_t day;
	uint16_t mon;
	uint16_t year;
	uint16_t wday;
} datetime_cache_t;

typedef struct {
	uint8_t price_sync;
	price_item_cache_t price_item_cache[20];

	uint8_t withholding_sync;
	uint16_t withholding_h;
	uint16_t withholding_l;

	uint8_t datetime_sync;
	datetime_cache_t datetime_cache;
} display_cache_channels_t;

typedef struct {
	uint8_t charge_sync;

	uint8_t charge_mode;
} display_cache_channel_t;

#pragma pack(pop)

#endif //_DISPLAY_CACHE_H
