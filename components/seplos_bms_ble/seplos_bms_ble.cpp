#include "seplos_bms_ble.h"
#include "esphome/core/log.h"

namespace esphome {
namespace seplos_bms_ble {

static const char *TAG = "seplos_bms_ble";

// Seplos BMS BLE UUIDs (16-bit)
static const uint16_t SEPLOS_SERVICE_UUID = 0xFF00;
static const uint16_t SEPLOS_TX_CHAR_UUID = 0xFF01;
static const uint16_t SEPLOS_RX_CHAR_UUID = 0xFF02;

void SeplosBmsBle::dump_config() {
  ESP_LOGCONFIG(TAG, "Seplos V2 BLE BMS");
}

void SeplosBmsBle::loop() {
  const uint32_t now = millis();
  if (connected_ && rx_handle_ != 0 && now - last_poll_ > 3000) {
    poll_status();
    last_poll_ = now;
  }
}

void SeplosBmsBle::gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param) {
  switch (event) {
    case ESP_GATTC_CONNECT_EVT:
      connected_ = true;
      break;
    case ESP_GATTC_DISCONNECT_EVT:
      connected_ = false;
      tx_handle_ = 0;
      rx_handle_ = 0;
      break;
    case ESP_GATTC_SEARCH_CMPL_EVT: {
      // Find TX characteristic (notifications)
      auto *tx_chr = this->parent()->get_characteristic(SEPLOS_SERVICE_UUID, SEPLOS_TX_CHAR_UUID);
      if (tx_chr != nullptr) {
        tx_handle_ = tx_chr->handle;
        esp_ble_gattc_register_for_notify(gattc_if, this->parent()->remote_bda, tx_handle_);
      }
      // Find RX characteristic (write)
      auto *rx_chr = this->parent()->get_characteristic(SEPLOS_SERVICE_UUID, SEPLOS_RX_CHAR_UUID);
      if (rx_chr != nullptr) {
        rx_handle_ = rx_chr->handle;
      }
      break;
    }
    case ESP_GATTC_NOTIFY_EVT:
      if (param->notify.handle == tx_handle_) {
        this->on_ble_data(std::vector<uint8_t>(param->notify.value, param->notify.value + param->notify.value_len));
      }
      break;
    default:
      break;
  }
}

void SeplosBmsBle::poll_status() {
  uint8_t frame[] = {0xDD, 0xA5, 0x03, 0x00, 0xFF, 0xFD, 0x77};
  esp_ble_gattc_write_char(this->parent()->get_gattc_if(), this->parent()->get_conn_id(), 
                           rx_handle_, sizeof(frame), frame, 
                           ESP_GATT_WRITE_TYPE_NO_RSP, ESP_GATT_AUTH_REQ_NONE);
}

void SeplosBmsBle::on_ble_data(const std::vector<uint8_t> &data) {
  if (data.size() < 10) return;
  if (data[0] != 0xDD || data[1] != 0x03) return;

  const uint8_t *p = &data[4];
  float voltage = ((p[0] << 8) | p[1]) / 100.0f;
  float current = (int16_t)((p[2] << 8) | p[3]) / 100.0f;
  int soc = p[17];
  int cycles = (p[8] << 8) | p[9];
  int cell_count = p[19];

  if (voltage_sensor_) voltage_sensor_->publish_state(voltage);
  if (current_sensor_) current_sensor_->publish_state(current);
  if (soc_sensor_) soc_sensor_->publish_state(soc);
  if (cycle_sensor_) cycle_sensor_->publish_state(cycles);

  int cell_start = 21;
  for (int i = 0; i < cell_count && i < (int)cell_sensors_.size(); i++) {
    float cv = ((p[cell_start + i*2] << 8) | p[cell_start + i*2 + 1]) / 1000.0f;
    cell_sensors_[i]->publish_state(cv);
  }
}

}  // namespace seplos_bms_ble
}  // namespace esphome
