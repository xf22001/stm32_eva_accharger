

/*================================================================
 *   
 *   
 *   文件名称：eeprom_layout.h
 *   创 建 者：肖飞
 *   创建日期：2021年03月30日 星期二 16时36分37秒
 *   修改日期：2021年03月30日 星期二 16时43分54秒
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

#ifdef __cplusplus
}
#endif


#pragma pack(push, 1)


typedef struct {
	eeprom_config_item_head_t head;
	mechine_info_t mechine;
} eeprom_mechine_info_t;

typedef struct {
	eeprom_config_item_head_t head;
	channels_settings_t channels_settings;
} eeprom_channels_settings_t;

typedef struct {
	eeprom_mechine_info_t mechine_info;
	eeprom_channels_settings_t channels_settings;
} eeprom_layout_t;

#pragma pack(pop)

#endif //_EEPROM_LAYOUT_H
