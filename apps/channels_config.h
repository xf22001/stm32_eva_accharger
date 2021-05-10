

/*================================================================
 *
 *
 *   文件名称：channels_config.h
 *   创 建 者：肖飞
 *   创建日期：2021年01月18日 星期一 11时00分11秒
 *   修改日期：2021年05月10日 星期一 15时01分10秒
 *   描    述：
 *
 *================================================================*/
#ifndef _CHANNELS_CONFIG_H
#define _CHANNELS_CONFIG_H
#ifdef __cplusplus
extern "C"
{
#endif

#include "app_platform.h"
#include "cmsis_os.h"

#ifdef __cplusplus
}
#endif

typedef enum {
	CHANNEL_CHARGER_TYPE_NONE = 0,
	CHANNEL_CHARGER_TYPE_NATIVE,
	CHANNEL_CHARGER_TYPE_PROXY,
	CHANNEL_CHARGER_TYPE_PROXY_AND_NATIVE,
} channel_charger_type_t;

typedef enum {
	CHANNEL_CHARGER_BMS_TYPE_NONE = 0,
	CHANNEL_CHARGER_BMS_TYPE_GB,
	CHANNEL_CHARGER_BMS_TYPE_AC,
} channel_charger_bms_type_t;

typedef struct {
	channel_charger_type_t charger_type;
	channel_charger_bms_type_t charger_bms_type;
	CAN_HandleTypeDef *hcan_bms;
} channel_charger_config_t;

typedef enum {
	CHANNEL_ENERGY_METER_TYPE_NONE = 0,
	CHANNEL_ENERGY_METER_TYPE_NATIVE,
	CHANNEL_ENERGY_METER_TYPE_PROXY,
	CHANNEL_ENERGY_METER_TYPE_PROXY_AND_NATIVE,
} channel_energy_meter_type_t;

typedef struct {
	channel_energy_meter_type_t energy_meter_type;
	UART_HandleTypeDef *huart_energy_meter;
} channel_energy_meter_config_t;

typedef struct {
	channel_charger_config_t charger_config;
	channel_energy_meter_config_t energy_meter_config;
} channel_config_t;

typedef struct {
	uint8_t channels_number;
	channel_config_t **channel_config;
} channels_channels_config_t;

typedef enum {
	CHANNELS_POWER_MODULE_TYPE_NONE = 0,
	CHANNELS_POWER_MODULE_TYPE_NATIVE,
	CHANNELS_POWER_MODULE_TYPE_PROXY,
	CHANNELS_POWER_MODULE_TYPE_PROXY_AND_NATIVE,
	CHANNELS_POWER_MODULE_TYPE_NATIVE_AC,
} channels_power_module_type_t;

typedef struct {
	uint8_t channels_power_module_number;
	CAN_HandleTypeDef *hcan_power;
	channels_power_module_type_t channels_power_module_type;
} channels_power_module_config_t;

typedef struct {
	uint8_t id;
	channels_channels_config_t channels_config;
	channels_power_module_config_t power_module_config;
} channels_config_t;

char *get_channel_config_charger_type(channel_charger_type_t type);
char *get_channel_config_energy_meter_type(channel_energy_meter_type_t type);
channels_config_t *get_channels_config(uint8_t id);
#endif //_CHANNELS_CONFIG_H
