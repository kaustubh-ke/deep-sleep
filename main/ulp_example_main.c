#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sys/time.h"
#include "esp_sleep.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"
#include "soc/rtc_periph.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "esp32/ulp.h"
#include "ulp_main.h"
#include "sdkconfig.h"

#define GPIO_INPUT_IO_TRIGGER     25

#define GPIO_DEEP_SLEEP_DURATION     30
RTC_DATA_ATTR unsigned int counter = 0;
RTC_DATA_ATTR static time_t last;
RTC_DATA_ATTR static time_t interrupt= 0;
struct timeval now;
RTC_DATA_ATTR unsigned int  time1= 1000000LL * GPIO_DEEP_SLEEP_DURATION ;
RTC_DATA_ATTR unsigned int  in = 1000000LL * GPIO_DEEP_SLEEP_DURATION ;

void WakeupCause()
{
	esp_sleep_wakeup_cause_t cause;
			cause = esp_sleep_get_wakeup_cause();
	if(cause==ESP_SLEEP_WAKEUP_EXT0)
		{
			interrupt = now.tv_sec - last;
			in = time1 -(1000000LL * interrupt);
			printf(" interrupt at %ld \n", interrupt);
			printf("external source wakeup \n ");
			printf("counter value = %d \n", counter);
		}

	else if(cause==ESP_SLEEP_WAKEUP_TIMER)
		{
			last = now.tv_sec;
			in = time1;
			printf("%ld \n" ,last);
			printf("Timer source wakeup \n");
			printf("counter value = %d \n", counter);

		}
}


void app_main()
{


	gettimeofday(&now, NULL);
	printf("starting ESP32\n");
	printf("config Timer\n");
	WakeupCause();
	esp_sleep_enable_timer_wakeup(in);
	esp_sleep_enable_ext0_wakeup(GPIO_INPUT_IO_TRIGGER, 1);
	esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
	rtc_gpio_pullup_en(GPIO_INPUT_IO_TRIGGER);
	rtc_gpio_pulldown_en(GPIO_INPUT_IO_TRIGGER);
	counter++;

	printf("deep sleep\n");
	esp_deep_sleep_start();





}

