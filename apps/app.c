

/*================================================================
 *
 *
 *   文件名称：app.c
 *   创 建 者：肖飞
 *   创建日期：2019年10月11日 星期五 16时54分03秒
 *   修改日期：2021年09月11日 星期六 16时30分28秒
 *   描    述：
 *
 *================================================================*/
#include "app.h"

#include <string.h>

#include "iwdg.h"

#include "os_utils.h"
#include "config_layout.h"

#include "test_serial.h"
#include "test_event.h"
#include "file_log.h"
#include "probe_tool.h"
#include "net_client.h"
#include "ftp_client.h"
#include "usb_upgrade.h"

#include "channels.h"
#include "duty_cycle_pattern.h"

#include "sal_socket.h"
#include "sal_netdev.h"
#include "sal_netdev.h"
#include "wiz_ethernet.h"
#include "display.h"
#include "sal_hook.h"
#include "channels_notify_voice.h"

#include "log.h"

extern IWDG_HandleTypeDef hiwdg;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern SPI_HandleTypeDef hspi2;

static app_info_t *app_info = NULL;
static os_signal_t app_event = NULL;

app_info_t *get_app_info(void)
{
	return app_info;
}

int app_load_config(void)
{
	config_layout_t *config_layout = get_config_layout();
	size_t offset = (size_t)&config_layout->mechine_info_seg.storage_mechine_info.mechine_info;
	debug("offset:%d", offset);
	return load_config_item(app_info->storage_info, "eva", &app_info->mechine_info, sizeof(mechine_info_t), offset);
}

int app_save_config(void)
{
	config_layout_t *config_layout = get_config_layout();
	size_t offset = (size_t)&config_layout->mechine_info_seg.storage_mechine_info.mechine_info;
	debug("offset:%d", offset);
	return save_config_item(app_info->storage_info, "eva", &app_info->mechine_info, sizeof(mechine_info_t), offset);
}

void app_init(void)
{
	app_event = signal_create(1);
}

void send_app_event(app_event_t event)
{
	signal_send(app_event, event, 0);
}

static void app_mechine_info_invalid(void *fn_ctx, void *chain_ctx)
{
	app_info_t *app_info = (app_info_t *)fn_ctx;
	modbus_data_ctx_t *modbus_data_ctx = (modbus_data_ctx_t *)chain_ctx;

	if(modbus_data_ctx->influence < (void *)&app_info->mechine_info) {
		return;
	}

	if(modbus_data_ctx->influence >= (void *)(&app_info->mechine_info + 1)) {
		return;
	}

	debug("[%p, %p, %p]", &app_info->mechine_info, modbus_data_ctx->influence, &app_info->mechine_info + 1);

	app_info->mechine_info_invalid = 1;
}

__weak void load_app_display_cache(app_info_t *app_info)
{
}

__weak void sync_app_display_cache(app_info_t *app_info)
{
}

static void app_mechine_info_changed(void *fn_ctx, void *chain_ctx)
{
	app_info_t *app_info = (app_info_t *)fn_ctx;

	sync_app_display_cache(app_info);

	if(app_info->mechine_info_invalid != 0) {
		app_info->mechine_info_invalid = 0;
		app_save_config();
	}
}

void update_network_ip_config(app_info_t *app_info)
{
	int exit = 0;

	while(exit == 0) {
		if(set_dhcp_enable(app_info->mechine_info.dhcp_enable) != 0) {
			debug("");
			osDelay(1000);
		} else {
			if(app_info->mechine_info.dhcp_enable == 0) {
				ip_addr_t ip;
				ip_addr_t sn;
				ip_addr_t gw;

				if(ipaddr_aton(app_info->mechine_info.ip, &ip) == 0) {
					debug("ip:%s", app_info->mechine_info.ip);
				}

				if(ipaddr_aton(app_info->mechine_info.sn, &sn) == 0) {
					debug("sn:%s", app_info->mechine_info.sn);
				}

				if(ipaddr_aton(app_info->mechine_info.gw, &gw) == 0) {
					debug("gw:%s", app_info->mechine_info.gw);
				}

				if(set_default_ipaddr(&ip) != 0) {
					debug("");
					osDelay(1000);
					continue;
				}

				if(set_default_netmask(&sn) != 0) {
					debug("");
					osDelay(1000);
					continue;
				}

				if(set_default_gw(&gw) != 0) {
					debug("");
					osDelay(1000);
					continue;
				}

				if(set_default_dns_server(&gw) != 0) {
					debug("");
					osDelay(1000);
					continue;
				}
			}

			exit = 1;
		}
	}
}

void app(void const *argument)
{

	poll_loop_t *poll_loop;
	channels_info_t *channels_info = NULL;
	display_info_t *display_info = NULL;
	int ret;

	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 1000);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 1000);

	app_info = (app_info_t *)os_calloc(1, sizeof(app_info_t));

	OS_ASSERT(app_info != NULL);

	app_info->storage_info = get_or_alloc_storage_info(&hspi2);
	OS_ASSERT(app_info->storage_info != NULL);

	ret = app_load_config();

	//ret = -1;

	if(ret == 0) {
		debug("app_load_config successful!");
		debug("device id:\'%s\', server uri:\'%s\'!", app_info->mechine_info.device_id, app_info->mechine_info.uri);
	} else {
		debug("app_load_config failed!");
		snprintf(app_info->mechine_info.device_id, sizeof(app_info->mechine_info.device_id), "%s", "0000000000");
		snprintf(app_info->mechine_info.uri, sizeof(app_info->mechine_info.uri), "%s", "tcp://112.74.40.227:12345");
		debug("device id:\'%s\', server uri:\'%s\'!", app_info->mechine_info.device_id, app_info->mechine_info.uri);
		snprintf(app_info->mechine_info.ip, sizeof(app_info->mechine_info.ip), "%d.%d.%d.%d", 10, 42, 0, 122);
		snprintf(app_info->mechine_info.sn, sizeof(app_info->mechine_info.sn), "%d.%d.%d.%d", 255, 255, 255, 0);
		snprintf(app_info->mechine_info.gw, sizeof(app_info->mechine_info.gw), "%d.%d.%d.%d", 10, 42, 0, 1);
		app_info->mechine_info.dhcp_enable = 0;
		app_info->mechine_info.upgrade_enable = 0;
		app_save_config();
	}

	load_app_display_cache(app_info);

	sal_init();
	wiz_init();

	//at_device_init();//数据结构
	//sim76xx_device_class_register();//驱动
	//sim76xx_device_register();//数据
	
	update_network_ip_config(app_info);

	poll_loop = get_or_alloc_poll_loop(0);
	OS_ASSERT(poll_loop != NULL);

	probe_broadcast_add_poll_loop(poll_loop);
	probe_server_add_poll_loop(poll_loop);

	while(is_log_server_valid() == 0) {
		osDelay(1);
	}

	add_log_handler((log_fn_t)log_udp_data);
	add_log_handler((log_fn_t)log_file_data);

	debug("===========================================start app============================================");

	//ftpd_init();

	//test_event();

	channels_info = start_channels();
	OS_ASSERT(channels_info != NULL);

	net_client_add_poll_loop(poll_loop);
	//ftp_client_add_poll_loop(poll_loop);

	display_info = (display_info_t *)channels_info->display_info;
	OS_ASSERT(display_info != NULL);

	app_info->display_data_invalid_callback_item.fn = app_mechine_info_invalid;
	app_info->display_data_invalid_callback_item.fn_ctx = app_info;
	OS_ASSERT(register_callback(display_info->modbus_slave_info->data_invalid_chain, &app_info->display_data_invalid_callback_item) == 0);

	app_info->display_data_changed_callback_item.fn = app_mechine_info_changed;
	app_info->display_data_changed_callback_item.fn_ctx = app_info;
	OS_ASSERT(register_callback(display_info->modbus_slave_info->data_changed_chain, &app_info->display_data_changed_callback_item) == 0);

	if(init_channels_notify_voice(channels_info) != 0) {
		debug("");
	}

	while(1) {
		uint32_t event;
		ret = signal_wait(app_event, &event, 1000);

		if(ret == 0) {
			switch(event) {
				case APP_EVENT_USB: {
					start_usb_upgrade();
				}
				break;

				default: {
				}
				break;
			}
		}

		handle_open_log();
		handle_usb_upgrade();
		continue;
		{
#include <time.h>
#include "hw_rtc.h"
#include "hw_adc.h"
#include "ntc_temperature.h"
			extern ADC_HandleTypeDef hadc1;
			adc_info_t *adc_info = get_or_alloc_adc_info(&hadc1);
			struct tm tm;
			uint8_t data[10];
			rtc_get_datetime(&tm);
			uint64_t u = 0xffffffffffffffff;
			debug("tm %04d-%02d-%02d %02d:%02d:%02d",
			      tm.tm_year + 1900,
			      tm.tm_mon + 1,
			      tm.tm_mday,
			      tm.tm_hour,
			      tm.tm_min,
			      tm.tm_sec);

			OS_ASSERT(adc_info != NULL);
			debug("adc[0]:%d, temperature:%d", get_adc_value(adc_info, 0), get_ntc_temperature(10000, get_adc_value(adc_info, 0), 4095));
			debug("adc[1]:%d", get_adc_value(adc_info, 1));
			debug("adc[2]:%d", get_adc_value(adc_info, 2));
			debug("adc[3]:%d", get_adc_value(adc_info, 3));
			debug("adc[4]:%d", get_adc_value(adc_info, 4));
			debug("adc[5]:%d", get_adc_value(adc_info, 5));
			get_bcd_b0123456789_from_u64(&data[0], &data[1], &data[2], &data[3], &data[4], &data[5], &data[6], &data[7], &data[8], &data[9], u);
			debug("0xffffffffffffffff is:%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
			      data[9],
			      data[8],
			      data[7],
			      data[6],
			      data[5],
			      data[4],
			      data[3],
			      data[2],
			      data[1],
			      data[0]);
		}
	}
}

static pattern_state_t work_pattern_state = {
	.type = PWM_COMPARE_COUNT_UP,
	.duty_cycle = 0,
};

static void update_work_led(void)
{
	//计数值小于duty_cycle,输出1;大于duty_cycle输出0
	uint16_t duty_cycle = get_duty_cycle_pattern(&work_pattern_state, 1000, 0, 20);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, duty_cycle);
}

void idle(void const *argument)
{
	MX_IWDG_Init();
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);

	while(1) {
		HAL_IWDG_Refresh(&hiwdg);
		update_work_led();
		osDelay(10);
	}
}

int force_bootloader(void)
{
	int ret = -1;
	u_uint8_bits_t u_uint8_bits;
	u_uint8_bits.v = 0;

	HAL_Init();
	MX_GPIO_Init();

	//u_uint8_bits.s.bit0 = (HAL_GPIO_ReadPin(d1_GPIO_Port, d1_Pin) == GPIO_PIN_SET) ? 1 : 0;
	//u_uint8_bits.s.bit1 = (HAL_GPIO_ReadPin(d2_GPIO_Port, d2_Pin) == GPIO_PIN_SET) ? 1 : 0;
	//u_uint8_bits.s.bit2 = (HAL_GPIO_ReadPin(d3_GPIO_Port, d3_Pin) == GPIO_PIN_SET) ? 1 : 0;

	if(u_uint8_bits.v == 0x07) {
		ret = 0;
	}

	HAL_DeInit();

	return ret;
}
