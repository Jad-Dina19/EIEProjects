/*
 * main.c
 */

#include <inttypes.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>

#include "BTN.h"
#include "LED.h"

#define SLEEP_MS 1

int main(void) {
  int err;

  if (0 > BTN_init()) return 0;
  if (0 > LED_init()) return 0;

  err = bt_enable(NULL);
  printk("bt_enable: %d\n", err);
  if (err) return 0;

  /* This includes your CONFIG_BT_DEVICE_NAME in advertising */
  err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, NULL, 0, NULL, 0);
  printk("adv_start: %d\n", err);
  if (err) return 0;

  while (1) {
      k_msleep(SLEEP_MS);
  }
}
