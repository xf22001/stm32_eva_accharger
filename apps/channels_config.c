

/*================================================================
 *   
 *   
 *   文件名称：channels_config.c
 *   创 建 者：肖飞
 *   创建日期：2021年01月18日 星期一 09时26分44秒
 *   修改日期：2021年05月10日 星期一 15时00分44秒
 *   描    述：
 *
 *================================================================*/
#include "channels_config.h"
#include "os_utils.h"
#include "channels_power_module.h"

extern CAN_HandleTypeDef hcan1;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart6;

char *get_channel_config_charger_type(channel_charger_type_t type)
{
	char *des = "unknow";

	switch(type) {
			add_des_case(CHANNEL_CHARGER_TYPE_NONE);
			add_des_case(CHANNEL_CHARGER_TYPE_NATIVE);
			add_des_case(CHANNEL_CHARGER_TYPE_PROXY);

		default: {
		}
		break;
	}

	return des;
}

char *get_channel_config_energy_meter_type(channel_energy_meter_type_t type)
{
	char *des = "unknow";

	switch(type) {
			add_des_case(CHANNEL_ENERGY_METER_TYPE_NONE);
			add_des_case(CHANNEL_ENERGY_METER_TYPE_NATIVE);
			add_des_case(CHANNEL_ENERGY_METER_TYPE_PROXY);
			add_des_case(CHANNEL_ENERGY_METER_TYPE_PROXY_AND_NATIVE);

		default: {
		}
		break;
	}

	return des;
}

static channel_config_t channel0_config = {
	.charger_config = {
		.charger_type = CHANNEL_CHARGER_TYPE_NATIVE,
		.charger_bms_type = CHANNEL_CHARGER_BMS_TYPE_AC,
		.hcan_bms = NULL,
	},
	.energy_meter_config = {
		.energy_meter_type = CHANNEL_ENERGY_METER_TYPE_NATIVE,
		.huart_energy_meter = &huart3,
	},
};

static channel_config_t channel1_config = {
	.charger_config = {
		.charger_type = CHANNEL_CHARGER_TYPE_NATIVE,
		.charger_bms_type = CHANNEL_CHARGER_BMS_TYPE_AC,
		.hcan_bms = NULL,
	},
	.energy_meter_config = {
		.energy_meter_type = CHANNEL_ENERGY_METER_TYPE_NATIVE,
		.huart_energy_meter = &huart6,
	},
};

static channel_config_t *channel_config_sz[] = {
	&channel0_config,
	&channel1_config,
};

static channels_config_t channels_config_0 = {
	.id = 0,
	.channels_config = {
		.channels_number = ARRAY_SIZE(channel_config_sz),
		.channel_config = channel_config_sz,
	},
	.power_module_config = {
		.channels_power_module_number = 0,
		.hcan_power = NULL,
		.channels_power_module_type = CHANNELS_POWER_MODULE_TYPE_NATIVE,
	},
};

static channels_config_t *channels_config_sz[] = {
	&channels_config_0,
};

channels_config_t *get_channels_config(uint8_t id)
{
	int i;
	channels_config_t *channels_config = NULL;
	channels_config_t *channels_config_item = NULL;

	for(i = 0; i < ARRAY_SIZE(channels_config_sz); i++) {
		channels_config_item = channels_config_sz[i];

		if(channels_config_item->id == id) {
			channels_config = channels_config_item;
			break;
		}
	}

	return channels_config;
}
