

/*================================================================
 *
 *
 *   文件名称：uart_debug_handler.c
 *   创 建 者：肖飞
 *   创建日期：2020年05月13日 星期三 13时18分00秒
 *   修改日期：2021年07月09日 星期五 08时50分17秒
 *   描    述：
 *
 *================================================================*/
#include "uart_debug_handler.h"

#include "iap.h"
#include "app.h"

#define LOG_UART
#include "log.h"

static void fn1(char *arguments)
{
	_printf("%s:%s:%d arguments:\'%s\'\n", __FILE__, __func__, __LINE__, arguments);
}

static void fn2(char *arguments)
{
	if(is_app() == 1) {
		app_info_t *app_info = get_app_info();
		app_info->mechine_info.upgrade_enable = 1;
		app_save_config();
		_printf("enable upgrade! resetting...\n");
		HAL_NVIC_SystemReset();
	}
}

uint16_t osGetCPUUsage(void);
static void fn5(char *arguments)
{
	int size = xPortGetFreeHeapSize();
	uint8_t *os_thread_info;
	uint32_t ticks = osKernelSysTick();
	uint16_t cpu_usage = osGetCPUUsage();
	size_t total_heap_size = get_total_heap_size();
	size_t heap_size;
	size_t heap_count;
	size_t heap_max_size;

	get_mem_info(&heap_size, &heap_count,  &heap_max_size);

	_printf("cpu usage:%d\n", cpu_usage);
	_printf("free os heap size:%d\n", size);
	_printf("total heap size:%d, free heap size:%d, used:%d, heap count:%d, max heap size:%d\n",
	        total_heap_size,
	        total_heap_size - heap_size,
	        heap_size,
	        heap_count,
	        heap_max_size);
	_printf("current ticks:%lu\n", ticks);
	_printf("%lu day %lu hour %lu min %lu sec\n",
	        ticks / (1000 * 60 * 60 * 24),//day
	        (ticks % (1000 * 60 * 60 * 24)) / (1000 * 60 * 60),//hour
	        (ticks % (1000 * 60 * 60)) / (1000 * 60),//min
	        (ticks % (1000 * 60)) / (1000)//sec
	       );

	if(size < 4 * 1024) {
		return;
	}

	size = 1024;

	os_thread_info = (uint8_t *)os_alloc(size);

	if(os_thread_info == NULL) {
		return;
	}

	osThreadList(os_thread_info);

	_puts((const char *)os_thread_info);

	os_free(os_thread_info);

	if(is_app() == 1) {
		_printf("in app!\n");
	} else {
		_printf("in bootloader!\n");
	}
}

#include "net_client.h"
static void fn6(char *arguments)
{
	char protocol[8];
	int catched;
	int ret = 0;
	net_client_info_t *net_client_info = get_net_client_info();

	if(net_client_info == NULL) {
		return;
	}

	set_client_state(net_client_info, CLIENT_SUSPEND);

	ret = sscanf(arguments, "%10s%n", protocol, &catched);

	if(ret == 1) {
		_printf("protocol:%s!\n", protocol);

		if(memcmp(protocol, "default", 7) == 0) {
			set_net_client_request_type(net_client_info, REQUEST_TYPE_DEFAULT);
		} else if(memcmp(protocol, "sse", 3) == 0) {
			set_net_client_request_type(net_client_info, REQUEST_TYPE_SSE);
		} else if(memcmp(protocol, "ocpp", 4) == 0) {
			set_net_client_request_type(net_client_info, REQUEST_TYPE_OCPP_1_6);
		}
	} else {
		_printf("no protocol!\n");
	}

	set_client_state(net_client_info, CLIENT_REINIT);
}

static uart_fn_item_t uart_fn_map[] = {
	{1, fn1},
	{2, fn2},
	{5, fn5},
	{6, fn6},
};

uart_fn_map_info_t uart_fn_map_info = {
	.uart_fn_map = uart_fn_map,
	.uart_fn_map_size = sizeof(uart_fn_map) / sizeof(uart_fn_item_t),
};
