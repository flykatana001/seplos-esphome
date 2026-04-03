#include "seplos_bms_ble.h"
#include "esphome/core/log.h"
#include "esphome/core/component.h"

namespace esphome {
namespace seplos_bms_ble {

static const char *TAG = "seplos_bms_ble";

void SeplosBmsBle::dump_config() {
  ESP_LOGCONFIG(TAG, "Seplos V2 BLE BMS");
}

void SeplosBmsBle::loop() {
  const uint32_t now = millis();
  if (now - last_poll_ > 3000) {
    poll_status();
    last_poll_ = now;
  }
}

void SeplosBmsBle::poll_status() {
  uint8_t frame[] = {0xDD, 0xA5, 0x03, 0x00, 0xFF, 0xFD, 0x77};
  this->parent()->write_value(frame, sizeof(frame));
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

  if (voltage_sensor) voltage_sensor->publish_state(voltage);
  if (current_sensor) current_sensor->publish_state(current);
  if (soc_sensor) soc_sensor->publish_state(soc);
  if (cycle_sensor) cycle_sensor->publish_state(cycles);

  int cell_start = 21;
  for (int i = 0; i < cell_count && i < cell_sensors.size(); i++) {
    float cv = ((p[cell_start + i*2] << 8) | p[cell_start + i*2 + 1]) / 1000.0f;
    cell_sensors[i]->publish_state(cv);
  }
}

}  // namespace seplos_bms_ble
}  // namespace esphome

REGISTER_COMPONENT(esphome::seplos_bms_ble::SeplosBmsBle, "seplos_bms_ble");
