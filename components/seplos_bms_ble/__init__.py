import esphome.codegen as cg

CODEOWNERS = ["@flykatana001"]

seplos_bms_ble_ns = cg.esphome_ns.namespace("seplos_bms_ble")

from . import seplos_bms_ble
