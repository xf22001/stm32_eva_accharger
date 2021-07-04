

/*================================================================
 *
 *
 *   文件名称：app.c
 *   创 建 者：肖飞
 *   创建日期：2019年10月11日 星期五 16时54分03秒
 *   修改日期：2021年07月04日 星期日 21时56分00秒
 *   描    述：
 *
 *================================================================*/
#include "app.h"

#include <string.h>

#include "app_platform.h"
#include "cmsis_os.h"

#include "iwdg.h"

#include "os_utils.h"
#include "eeprom_layout.h"

#include "test_serial.h"
#include "test_event.h"
#include "file_log.h"
#include "uart_debug.h"
#include "probe_tool.h"
#include "net_client.h"
#include "ftp_client.h"
#include "usb_upgrade.h"

#include "log.h"

#include "channels.h"
#include "duty_cycle_pattern.h"

#include "sal_socket.h"
#include "sal_netdev.h"
#include "sal_netdev.h"
#include "wiz_ethernet.h"

extern IWDG_HandleTypeDef hiwdg;
extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart4;
extern SPI_HandleTypeDef hspi2;

static app_info_t *app_info = NULL;
static os_signal_t app_event = NULL;

app_info_t *get_app_info(void)
{
	return app_info;
}

static int app_load_config(void)
{
	eeprom_layout_t *eeprom_layout = get_eeprom_layout();
	size_t offset = (size_t)&eeprom_layout->mechine_info_seg.eeprom_mechine_info.mechine_info;
	debug("offset:%d", offset);
	return eeprom_load_config_item(app_info->eeprom_info, "eva", &app_info->mechine_info, sizeof(mechine_info_t), offset);
}

int app_save_config(void)
{
	eeprom_layout_t *eeprom_layout = get_eeprom_layout();
	size_t offset = (size_t)&eeprom_layout->mechine_info_seg.eeprom_mechine_info.mechine_info;
	debug("offset:%d", offset);
	return eeprom_save_config_item(app_info->eeprom_info, "eva", &app_info->mechine_info, sizeof(mechine_info_t), offset);
}

void app_init(void)
{
	app_event = signal_create(1);
}

void send_app_event(app_event_t event)
{
	signal_send(app_event, event, 0);
}

void app(void const *argument)
{

	poll_loop_t *poll_loop;

	app_info = (app_info_t *)os_calloc(1, sizeof(app_info_t));

	OS_ASSERT(app_info != NULL);

	app_info->eeprom_info = get_or_alloc_eeprom_info(get_or_alloc_spi_info(&hspi2),
	                        e2cs_GPIO_Port,
	                        e2cs_Pin,
	                        NULL,
	                        0);

	OS_ASSERT(app_info->eeprom_info != NULL);

	add_log_handler((log_fn_t)log_uart_data);
	add_log_handler((log_fn_t)log_udp_data);
	add_log_handler((log_fn_t)log_file_data);


	{
		uart_info_t *uart_info = get_or_alloc_uart_info(&huart4);

		if(uart_info == NULL) {
			app_panic();
		}

		set_log_uart_info(uart_info);

		osThreadDef(uart_debug, task_uart_debug, osPriorityNormal, 0, 128 * 2 * 2);
		osThreadCreate(osThread(uart_debug), uart_info);
	}

	//{
	//	uart_info_t *uart_info = get_or_alloc_uart_info(&huart4);

	//	if(uart_info == NULL) {
	//		app_panic();
	//	}

	//	osThreadDef(task_test_serial, task_test_serial, osPriorityNormal, 0, 128);
	//	osThreadCreate(osThread(task_test_serial), uart_info);
	//}

	sal_init();
	wiz_init();

	poll_loop = get_or_alloc_poll_loop(0);
	OS_ASSERT(poll_loop != NULL);
	probe_broadcast_add_poll_loop(poll_loop);
	probe_server_add_poll_loop(poll_loop);

	while(is_log_server_valid() == 0) {
		osDelay(1);
	}

	debug("===========================================start app============================================");

	if(app_load_config() == 0) {
		debug("app_load_config successful!");
		debug("device id:\'%s\', server host:\'%s\', server port:\'%s\'!", app_info->mechine_info.device_id, app_info->mechine_info.host, app_info->mechine_info.port);
		app_info->available = 1;
	} else {
		debug("app_load_config failed!");
		snprintf(app_info->mechine_info.device_id, sizeof(app_info->mechine_info.device_id), "%s", "0000000000");
		snprintf(app_info->mechine_info.host, sizeof(app_info->mechine_info.host), "%s", "10.42.0.1");
		snprintf(app_info->mechine_info.port, sizeof(app_info->mechine_info.port), "%s", "6003");
		snprintf(app_info->mechine_info.path, sizeof(app_info->mechine_info.path), "%s", "");
		debug("device id:\'%s\', server host:\'%s\', server port:\'%s\'!", app_info->mechine_info.device_id, app_info->mechine_info.host, app_info->mechine_info.port);
		app_info->mechine_info.upgrade_enable = 0;
		app_save_config();
		app_info->available = 1;
	}

	//ftpd_init();

	//test_event();

	start_channels();

	net_client_add_poll_loop(poll_loop);
	ftp_client_add_poll_loop(poll_loop);

	while(1) {
		uint32_t event;
		int ret = signal_wait(app_event, &event, 1000);

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
