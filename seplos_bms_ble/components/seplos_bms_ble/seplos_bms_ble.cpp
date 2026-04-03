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

  sensor::Sensor *voltage_sensor{nullptr};
  sensor::Sensor *current_sensor{nullptr};
  sensor::Sensor *soc_sensor{nullptr};
  sensor::Sensor *cycle_sensor{nullptr};
  std::vector<sensor::Sensor*> cell_sensors;

 protected:
  void on_ble_data(const std::vector<uint8_t> &data);
  void poll_status();
  uint32_t last_poll_{0};
};

}  // namespace seplos_bms_ble
}  // namespace esphome
