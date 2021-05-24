

/*================================================================
 *
 *
 *   文件名称：eeprom_layout.h
 *   创 建 者：肖飞
 *   创建日期：2021年03月30日 星期二 16时36分37秒
 *   修改日期：2021年05月24日 星期一 10时15分28秒
 *   描    述：
 *
 *================================================================*/
#ifndef _EEPROM_LAYOUT_H
#define _EEPROM_LAYOUT_H
#ifdef __cplusplus
extern "C"
{
#endif

#include "app_platform.h"
#include "cmsis_os.h"

#include "eeprom_config.h"
#include "app.h"
#include "channels.h"
#include "channel_record.h"

#ifdef __cplusplus
}
#endif


#pragma pack(push, 1)


typedef struct {
	eeprom_config_item_head_t head;
	mechine_info_t mechine_info;
} eeprom_mechine_info_t;

typedef struct {
	eeprom_config_item_head_t head;
	channels_settings_t channels_settings;
} eeprom_channels_settings_t;

typedef struct {
	eeprom_config_item_head_t head;
	channel_record_info_t channel_record_info;
} eeprom_channel_record_info_t;

typedef struct {
	eeprom_config_item_head_t head;
	channel_record_item_t channel_record_item;
} eeprom_channel_record_item_t;

typedef struct {
	union {
		eeprom_mechine_info_t eeprom_mechine_info;
		uint8_t seg[256];
	} mechine_info_seg;
	union {
		eeprom_channels_settings_t eeprom_channels_settings;
		uint8_t seg[2 * 1024];
	} channels_settings_seg;
	union {
		struct {
			eeprom_channel_record_info_t eeprom_channel_record_info;
			eeprom_channel_record_item_t eeprom_channel_record_item[CHANNEL_RECORD_NUMBER];
		} channel_record;
		uint8_t seg[100 * 1024];
	} channel_record_seg;
} eeprom_layout_t;

static inline eeprom_layout_t *get_eeprom_layout(void)
{
	return (eeprom_layout_t *)0;
}

#pragma pack(pop)

#endif //_EEPROM_LAYOUT_H
