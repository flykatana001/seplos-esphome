#pragma once

#include "esphome/components/ble_client/ble_client.h"
#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace seplos_bms_ble {

class SeplosBmsBle : public ble_client::BLEClientNode, public Component {
 public:
  void dump_config() override;
  void loop() override;
  void gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param) override;

  void set_voltage_sensor(sensor::Sensor *sensor) { voltage_sensor_ = sensor; }
  void set_current_sensor(sensor::Sensor *sensor) { current_sensor_ = sensor; }
  void set_soc_sensor(sensor::Sensor *sensor) { soc_sensor_ = sensor; }
  void set_cycle_sensor(sensor::Sensor *sensor) { cycle_sensor_ = sensor; }
  void add_cell_sensor(sensor::Sensor *sensor) { cell_sensors_.push_back(sensor); }

 protected:
  void on_ble_data(const std::vector<uint8_t> &data);
  void poll_status();
  uint32_t last_poll_{0};
  bool connected_{false};
  
  sensor::Sensor *voltage_sensor_{nullptr};
  sensor::Sensor *current_sensor_{nullptr};
  sensor::Sensor *soc_sensor_{nullptr};
  sensor::Sensor *cycle_sensor_{nullptr};
  std::vector<sensor::Sensor*> cell_sensors_;
};

}  // namespace seplos_bms_ble
}  // namespace esphome
