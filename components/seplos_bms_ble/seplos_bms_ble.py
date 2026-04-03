import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import ble_client, sensor
from esphome.const import (
    UNIT_VOLT,
    UNIT_AMPERE,
    UNIT_PERCENT,
    UNIT_EMPTY,
)

from . import seplos_bms_ble_ns

SeplosBmsBle = seplos_bms_ble_ns.class_("SeplosBmsBle", ble_client.BLEClientNode, cg.Component)


CONF_BLE_CLIENT_ID = "ble_client_id"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(SeplosBmsBle),
    cv.Required(CONF_BLE_CLIENT_ID): cv.use_id(ble_client.BLEClient),

    cv.Optional("voltage"): sensor.sensor_schema(UNIT_VOLT),
    cv.Optional("current"): sensor.sensor_schema(UNIT_AMPERE),
    cv.Optional("soc"): sensor.sensor_schema(UNIT_PERCENT),
    cv.Optional("cycles"): sensor.sensor_schema(UNIT_EMPTY),

    cv.Optional("cells"): cv.ensure_list(sensor.sensor_schema(UNIT_VOLT)),
})

async def to_code(config):
    var = cg.new_Pvariable(config["id"])
    await cg.register_component(var, config)

    parent = await cg.get_variable(config[CONF_BLE_CLIENT_ID])
    cg.add(var.set_parent(parent))

    if "voltage" in config:
        sens = await sensor.new_sensor(config["voltage"])
        cg.add(var.set_voltage_sensor(sens))

    if "current" in config:
        sens = await sensor.new_sensor(config["current"])
        cg.add(var.set_current_sensor(sens))

    if "soc" in config:
        sens = await sensor.new_sensor(config["soc"])
        cg.add(var.set_soc_sensor(sens))

    if "cycles" in config:
        sens = await sensor.new_sensor(config["cycles"])
        cg.add(var.set_cycle_sensor(sens))

    if "cells" in config:
        for cell_cfg in config["cells"]:
            sens = await sensor.new_sensor(cell_cfg)
            cg.add(var.add_cell_sensor(sens))
