

/*================================================================
 *
 *
 *   文件名称：channels_config.c
 *   创 建 者：肖飞
 *   创建日期：2021年01月18日 星期一 09时26分44秒
 *   修改日期：2021年07月19日 星期一 14时46分19秒
 *   描    述：
 *
 *================================================================*/
#include "channels_config.h"
#include "os_utils.h"
#include "channels_power_module.h"
#include "main.h"

extern CAN_HandleTypeDef hcan1;
extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart4;
extern TIM_HandleTypeDef htim3;
extern SPI_HandleTypeDef hspi2;

char *get_channel_config_channel_type(channel_type_t type)
{
	char *des = "unknow";

	switch(type) {
			add_des_case(CHANNEL_TYPE_NONE);
			add_des_case(CHANNEL_TYPE_PROXY);
			add_des_case(CHANNEL_TYPE_DC);
			add_des_case(CHANNEL_TYPE_AC);

		default: {
		}
		break;
	}

	return des;
}

char *get_channel_config_charger_type(channel_charger_type_t type)
{
	char *des = "unknow";

	switch(type) {
			add_des_case(CHANNEL_CHARGER_TYPE_BMS_NONE);
			add_des_case(CHANNEL_CHARGER_TYPE_BMS_GB);
			add_des_case(CHANNEL_CHARGER_TYPE_BMS_AC);
			add_des_case(CHANNEL_CHARGER_TYPE_BMS_PROXY);

		default: {
		}
		break;
	}

	return des;
}

char *get_channel_config_energy_meter_type(energy_meter_type_t type)
{
	char *des = "unknow";

	switch(type) {
			add_des_case(ENERGY_METER_TYPE_NONE);
			add_des_case(ENERGY_METER_TYPE_PROXY);
			add_des_case(ENERGY_METER_TYPE_DC);
			add_des_case(ENERGY_METER_TYPE_AC);
			add_des_case(ENERGY_METER_TYPE_AC_HLW8032);

		default: {
		}
		break;
	}

	return des;
}

static channel_config_t channel0_config = {
	.channel_type = CHANNEL_TYPE_AC,
	.charger_config = {
		.channel_charger_type = CHANNEL_CHARGER_TYPE_BMS_AC,
		.hcan_bms = NULL,
		.cp_pwm_timer = &htim3,
		.cp_pwm_channel = TIM_CHANNEL_3,
		.kl_gpio = kl1_GPIO_Port,
		.kl_pin = kl1_Pin,
		.kn_gpio = kn1_GPIO_Port,
		.kn_pin = kn1_Pin,
		.rey3_gpio = rey3_GPIO_Port,
		.rey3_pin = rey3_Pin,
		.rey4_gpio = rey4_GPIO_Port,
		.rey4_pin = rey4_Pin,
	},
	.energy_meter_config = {
		.energy_meter_type = ENERGY_METER_TYPE_AC,
		.huart = &huart1,
		.con_gpio = con1_485_GPIO_Port,
		.con_pin = con1_485_Pin,
	},
	.hcan_channel_comm = &hcan1,
	.adhe_ad_adc = &hadc1,
	.adhe_ad_adc_rank = 2,
	.cp_ad_adc = &hadc1,
	.cp_ad_adc_rank = 4,
};

static channel_config_t channel1_config = {
	.channel_type = CHANNEL_TYPE_AC,
	.charger_config = {
		.channel_charger_type = CHANNEL_CHARGER_TYPE_BMS_AC,
		.hcan_bms = NULL,
		.cp_pwm_timer = &htim3,
		.cp_pwm_channel = TIM_CHANNEL_4,
		.kl_gpio = kl2_GPIO_Port,
		.kl_pin = kl2_Pin,
		.kn_gpio = kn2_GPIO_Port,
		.kn_pin = kn2_Pin,
		.rey3_gpio = rey3_GPIO_Port,
		.rey3_pin = rey3_Pin,
		.rey4_gpio = rey4_GPIO_Port,
		.rey4_pin = rey4_Pin,
	},
	.energy_meter_config = {
		.energy_meter_type = ENERGY_METER_TYPE_AC,
		.huart = &huart1,
		.con_gpio = con1_485_GPIO_Port,
		.con_pin = con1_485_Pin,
	},
	.hcan_channel_comm = &hcan1,
	.adhe_ad_adc = &hadc1,
	.adhe_ad_adc_rank = 3,
	.cp_ad_adc = &hadc1,
	.cp_ad_adc_rank = 5,
};

static channel_config_t *channel_config_sz[] = {
	&channel0_config,
	&channel1_config,
};

static channels_config_t channels_config_0 = {
	.id = 0,
	.channel_number = ARRAY_SIZE(channel_config_sz),
	.channel_config = channel_config_sz,
	.channels_eeprom_config = {
		.hspi_eeprom = &hspi2,
		.eeprom_cs_port = e2cs_GPIO_Port,
		.eeprom_cs_pin = e2cs_Pin,
	},
	.power_module_config = {
		.channels_power_module_number = 0,
		.hcan = NULL,
		.channels_power_module_type = CHANNELS_POWER_MODULE_TYPE_NONE,
	},
	.voice_config = {
		.data_port = voi_data_GPIO_Port,
		.data_pin = voi_data_Pin,
		.cs_port = voi_cs_GPIO_Port,
		.cs_pin = voi_cs_Pin,
		.clk_port = voi_clk_GPIO_Port,
		.clk_pin = voi_clk_Pin,
	},
	.card_reader_config = {
		.card_reader_type = CARD_READER_TYPE_ZLG,
		.huart_card_reader = &huart4,
	},
	.display_config = {
		.huart = &huart2,
		.con_gpio = con2_485_GPIO_Port,
		.con_pin = con2_485_Pin,
	},
	.hcan_channel_comm = &hcan1,
	.board_temperature_adc = NULL,
	.board_temperature_adc_rank = 0,
	.force_stop_port = esb_GPIO_Port,
	.force_stop_pin = esb_Pin,
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
