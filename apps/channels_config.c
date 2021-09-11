

/*================================================================
 *
 *
 *   文件名称：channels_config.c
 *   创 建 者：肖飞
 *   创建日期：2021年01月18日 星期一 09时26分44秒
 *   修改日期：2021年09月11日 星期六 23时39分58秒
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
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart6;
extern TIM_HandleTypeDef htim3;
extern SPI_HandleTypeDef hspi2;

char *get_channel_config_channel_type(channel_type_t type)
{
	char *des = "unknow";

	switch(type) {
			add_des_case(CHANNEL_TYPE_NONE);
			add_des_case(CHANNEL_TYPE_NATIVE);

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
			add_des_case(CHANNEL_CHARGER_TYPE_BMS_GB_PROXY_REMOTE);
			add_des_case(CHANNEL_CHARGER_TYPE_BMS_AC);

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
			add_des_case(ENERGY_METER_TYPE_AC);
			add_des_case(ENERGY_METER_TYPE_AC_HLW8032);
			add_des_case(ENERGY_METER_TYPE_DC);
			add_des_case(ENERGY_METER_TYPE_AC_SDM_220);
			add_des_case(ENERGY_METER_TYPE_AC_SDM_630);
			add_des_case(ENERGY_METER_TYPE_PROXY);

		default: {
		}
		break;
	}

	return des;
}

static energy_meter_config_item_t energy_meter_config_item_0_0 = {
	.type = ENERGY_METER_TYPE_AC_SDM_220,
	.huart = &huart1,
	.con_gpio = con1_485_GPIO_Port,
	.con_pin = con1_485_Pin,
};

static energy_meter_config_item_t energy_meter_config_item_0_1 = {
	.type = ENERGY_METER_TYPE_AC_HLW8032,
	.huart = &huart3,
};

static energy_meter_config_item_t *energy_meter_config_item_0_sz[] = {
	&energy_meter_config_item_0_0,
	&energy_meter_config_item_0_1,
};

static channel_config_t channel0_config = {
	.channel_type = CHANNEL_TYPE_NATIVE,
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
		.default_type = ENERGY_METER_TYPE_AC_SDM_220,
		.size = ARRAY_SIZE(energy_meter_config_item_0_sz),
		.items = energy_meter_config_item_0_sz,
	},
	.hcan_channel_comm = &hcan1,
	.charger_temperature_adc = &hadc1,
	.charger_temperature_adc_rank = 1,
	.adhe_ad_adc = &hadc1,
	.adhe_ad_adc_rank = 2,
	.cp_ad_adc = &hadc1,
	.cp_ad_adc_rank = 4,
};

static channel_config_t channel1_config = {
	.channel_type = CHANNEL_TYPE_NATIVE,
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
		.items = NULL,
		.size = 0,
	},
	.hcan_channel_comm = &hcan1,
	.charger_temperature_adc = &hadc1,
	.charger_temperature_adc_rank = 1,
	.adhe_ad_adc = &hadc1,
	.adhe_ad_adc_rank = 3,
	.cp_ad_adc = &hadc1,
	.cp_ad_adc_rank = 5,
};

static channel_config_t *channel_config_sz[] = {
	&channel0_config,
	//&channel1_config,
};

static card_reader_config_item_t card_reader_config_item_0 = {
	.type = CARD_READER_TYPE_MT_318_626,
	.huart = &huart4,
};

static card_reader_config_item_t *card_reader_config_item_sz[] = {
	&card_reader_config_item_0,
};

static channels_config_t channels_config_0 = {
	.id = 0,
	.channel_number = ARRAY_SIZE(channel_config_sz),
	.channel_config = channel_config_sz,
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
		.size = ARRAY_SIZE(card_reader_config_item_sz),
		.items = card_reader_config_item_sz,
	},
	.display_config = {
		.huart = &huart2,
		.con_gpio = con2_485_GPIO_Port,
		.con_pin = con2_485_Pin,
	},
	.hcan_channel_comm = &hcan1,
	.board_temperature_adc = &hadc1,
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

energy_meter_config_item_t *get_energy_meter_config_item(channel_config_t *channel_config, energy_meter_type_t type)
{
	int i;
	energy_meter_config_t *energy_meter_config = &channel_config->energy_meter_config;
	energy_meter_config_item_t *energy_meter_config_item = NULL;

	for(i = 0; i < energy_meter_config->size; i++) {
		energy_meter_config_item_t *_energy_meter_config_item = energy_meter_config->items[i];

		if(_energy_meter_config_item->type == type) {
			energy_meter_config_item = _energy_meter_config_item;
			break;
		}
	}

	return energy_meter_config_item;
}

card_reader_config_item_t *get_card_reader_config_item(channels_config_t *channels_config, card_reader_type_t type)
{
	int i;
	card_reader_config_t *card_reader_config = &channels_config->card_reader_config;
	card_reader_config_item_t *card_reader_config_item = NULL;

	for(i = 0; i < card_reader_config->size; i++) {
		card_reader_config_item_t *_card_reader_config_item = card_reader_config->items[i];

		if(_card_reader_config_item->type == type) {
			card_reader_config_item = _card_reader_config_item;
			break;
		}
	}

	return card_reader_config_item;
}
