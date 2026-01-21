/*
 * main.c
 */

#include <inttypes.h>

#include <errno.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdlib.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/printk.h>
#include <zephyr/types.h>

#include "BTN.h"
#include "LED.h"


//Defines and Macros
#define SLEEP_MS 1
#define TARGET_PHONE_NAME "Jad’s iPhone"

//set UUID's make sure they are unique
static struct bt_uuid_128 BLE_CUSTOM_SERVICE_UUID =
    BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x11111111, 0x2222, 0x3333, 0x4444, 0x000000000001)); //UUID is 128 bits encoded with 5 vars
static struct bt_uuid_128 BLE_CUSTOM_CHARACTERISTIC_UUID =
    BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x11111111, 0x2222, 0x3333, 0x4444, 0x000000000002));


//variables and functions
static struct bt_conn* my_connection = NULL;
static struct bt_uuid_16 discover_uuid = BT_UUID_INIT_16(0);
static struct bt_gatt_discover_params discover_params;
static struct bt_gatt_subscribe_params subscribe_params;

static uint8_t notify_func(struct bt_conn* conn, struct bt_gatt_subscribe_params* params,
                           const void* data, uint16_t length) {
  if (!data) {
    printk("[UNSUBSCRIBED]\n");
    params->value_handle = 0U;
    return BT_GATT_ITER_STOP;
  }

  printk("[NOTIFICATION] data %p length %u\n", data, length);
  for (int i = 0; i < MIN(length, 16); i++) {
    printk(" 0x%02X", ((uint8_t*)data)[i]);
  }
  printk("\n");

  return BT_GATT_ITER_CONTINUE;
}


static uint8_t discover_func(struct bt_conn* conn, const struct bt_gatt_attr* attr,
                             struct bt_gatt_discover_params* params) {
  int err;

  if (!attr) {
    printk("Discover complete\n");
    (void)memset(params, 0, sizeof(*params));
    return BT_GATT_ITER_STOP;
  }

  printk("[ATTRIBUTE] handle %u\n", attr->handle);

  if (!bt_uuid_cmp(discover_params.uuid, &BLE_CUSTOM_SERVICE_UUID.uuid)) {
    discover_params.uuid = &BLE_CUSTOM_CHARACTERISTIC_UUID.uuid;
    discover_params.start_handle = attr->handle + 1;
    discover_params.type = BT_GATT_DISCOVER_CHARACTERISTIC;

    err = bt_gatt_discover(conn, &discover_params);
    if (err) {
      printk("Discover failed (err %d)\n", err);
    }
  } else if (!bt_uuid_cmp(discover_params.uuid, &BLE_CUSTOM_CHARACTERISTIC_UUID.uuid)) {
    memcpy(&discover_uuid, BT_UUID_GATT_CCC, sizeof(discover_uuid));
    discover_params.uuid = &discover_uuid.uuid;
    discover_params.start_handle = attr->handle + 2;
    discover_params.type = BT_GATT_DISCOVER_DESCRIPTOR;
    subscribe_params.value_handle = bt_gatt_attr_value_handle(attr);

    err = bt_gatt_discover(conn, &discover_params);
    if (err) {
      printk("Discover failed (err %d)\n", err);
    }
  } else {
    subscribe_params.notify = notify_func;
    subscribe_params.value = BT_GATT_CCC_NOTIFY;
    subscribe_params.ccc_handle = attr->handle;

    err = bt_gatt_subscribe(conn, &subscribe_params);
    if (err && err != -EALREADY) {
      printk("Subscribe failed (err %d)\n", err);
    } else {
      printk("[SUBSCRIBED]\n");
    }

    return BT_GATT_ITER_STOP;
  }

  return BT_GATT_ITER_STOP;
}

static void ble_on_device_connected(struct bt_conn* conn, uint8_t err) {
  //check if err is nor equal to 0, any other value means fail
  if(0 != err){
    printk("Connection failed (err %u)\n", err);
    bt_conn_unref(my_connection);
    my_connection = NULL;
    return;
  }
  //check if connection are same
  if(my_connection != conn){
    return;
  }
  //get address of connection
  char addr[BT_ADDR_LE_STR_LEN];
  bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
  printk("Connected: %s\n", addr);

  discover_params.uuid = &BLE_CUSTOM_SERVICE_UUID.uuid;
  discover_params.func = discover_func;
  discover_params.start_handle = BT_ATT_FIRST_ATTRIBUTE_HANDLE;
  discover_params.end_handle = BT_ATT_LAST_ATTRIBUTE_HANDLE;
  discover_params.type = BT_GATT_DISCOVER_PRIMARY;

  err = bt_gatt_discover(my_connection, &discover_params);
  if (err) {
    printk("Discover failed(err %d)\n", err);
    return;
  }
}

static void ble_on_device_disconnected(struct bt_conn* conn, uint8_t reason) {
  //check if connections match
  if(my_connection != conn){
    return;
  }
  //unref and clear connection
  bt_conn_unref(my_connection);
  my_connection = NULL;
  //reason for disconnect
  char* res = bt_hci_err_to_str(reason);
  printk("Disconnected: %s\n", res);

}

//tells OS when devices are connected or disconnected
BT_CONN_CB_DEFINE(conn_callbacks) = {
  .connected = ble_on_device_connected,
  .disconnected = ble_on_device_disconnected,
};


//checks advertising devices name
static bool ble_get_adv_device_name_cb(struct bt_data* data, void* user_data) {
  char* name = user_data;

  if (data->type == BT_DATA_NAME_COMPLETE || data->type == BT_DATA_NAME_SHORTENED) {
    memcpy(name, data->data, data->data_len);
    name[data->data_len] = 0; // Null-terminator
    return false; // Stop parsing
  }

  return true; // Continue looking through this advertising packet
}
static void ble_on_advertisement_received(const bt_addr_le_t* addr, int8_t rssi, uint8_t type,
                                          struct net_buf_simple* ad) {
  //check if connection exists
  if(my_connection != NULL){
    return;
  }

  if(type != BT_GAP_ADV_TYPE_ADV_IND && type != BT_GAP_ADV_TYPE_ADV_DIRECT_IND){
    return;
  }
  
  //parse name
  char name[32] = {0};
  bt_data_parse(ad, ble_get_adv_device_name_cb, name);
  printk("NAME: %s\n", name); 

  //convert MAC address to str and store in addr_str
  char addr_str[BT_ADDR_LE_STR_LEN];
  bt_addr_le_to_str(addr, addr_str, sizeof(addr_str));
  printk("ADDR: %s\n", addr_str);
  printk("RSSI: %d\n", rssi );
  

  if(strcmp("Jad", name)){
    printk("Wrong Name\n");
    return;
  }
  //no weak signals
  if(rssi < -50){
    return;
  }

  //stop scanning
  int err = bt_le_scan_stop();
  if(err){
    printk("Failed to stop scan (err %d)\n", err);
    return;
  }

  //create connection with device
  err = bt_conn_le_create(addr, BT_CONN_LE_CREATE_CONN, BT_LE_CONN_PARAM_DEFAULT, &my_connection);
  if(err){
    printk("Connection create failed (err %d)\n", err);
    return;
  }


}

int main(void) {
  //init led and buttons
  if (0 > BTN_init()) {
    return 0;
  }
  if (0 > LED_init()) {
    return 0;
  }

  //enable blue tooth
  int err = bt_enable(NULL);
  if (err) {
    printk("Bluetooth init failed (err %d)\n", err);
    return 0;
  } else {
    printk("Bluetooth initialized\n");
  }

  bt_le_scan_start(BT_LE_SCAN_PASSIVE, ble_on_advertisement_received);

  while(1) {
    k_msleep(SLEEP_MS);
  }
	return 0;
}
