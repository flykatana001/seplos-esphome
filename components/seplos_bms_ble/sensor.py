import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import ble_client, sensor
from esphome.const import UNIT_VOLT, UNIT_AMPERE, UNIT_PERCENT, UNIT_EMPTY

seplos_ns = cg.esphome_ns.namespace("seplos_bms_ble")
SeplosBmsBle = seplos_ns.class_("SeplosBmsBle", ble_client.BLEClientNode, cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(SeplosBmsBle),

    cv.Optional("voltage"): sensor.sensor_schema(UNIT_VOLT),
    cv.Optional("current"): sensor.sensor_schema(UNIT_AMPERE),
    cv.Optional("soc"): sensor.sensor_schema(UNIT_PERCENT),
    cv.Optional("cycles"): sensor.sensor_schema(UNIT_EMPTY),

    cv.Optional("cells"): cv.ensure_list(sensor.sensor_schema(UNIT_VOLT)),
})

async def to_code(config):
    var = cg.new_Pvariable(config["id"])
    await cg.register_component(var, config)
    await ble_client.register_ble_node(var, config)

    if "voltage" in config:
        sens = await sensor.new_sensor(config["voltage"])
        cg.add(var.voltage_sensor, sens)

    if "current" in config:
        sens = await sensor.new_sensor(config["current"])
        cg.add(var.current_sensor, sens)

    if "soc" in config:
        sens = await sensor.new_sensor(config["soc"])
        cg.add(var.soc_sensor, sens)

    if "cycles" in config:
        sens = await sensor.new_sensor(config["cycles"])
        cg.add(var.cycle_sensor, sens)

    if "cells" in config:
        for cell_cfg in config["cells"]:
            sens = await sensor.new_sensor(cell_cfg)
            cg.add(var.cell_sensors.push_back(sens))
