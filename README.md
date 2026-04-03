# ESPHome Seplos V2 BLE Component

This repository contains a custom ESPHome component for reading data from a
Seplos V2 BMS via Bluetooth Low Energy (BLE).

## Features

- Reads total voltage
- Reads current
- Reads state of charge (SOC)
- Reads cycle count
- Reads individual cell voltages (1–16 cells)
- Works with ESPHome BLE Client
- Compatible with Home Assistant

## Installation

Add this to your ESPHome YAML:

```yaml
external_components:
  - source: github://flykatana001/seplos-esphome@main
    refresh: 0s
