# TSIC Temperature Sensors for ESPHome

ESP-TSIC is implemens an external component for integrating some TSIC
temperature sensors into ESPHome.

---


## Usage

Resources for usage are:

* [ESPHome documentation](https://esphome.io/components/external_components.html),
* the `example_tsic.yaml`, or
* the following yaml snippet:

```yaml
# Example configuration entry
esphome:
  name: tsic-test
  platform: ESP8266
  board: d1_mini

external_components:
  # - source: components
  - source: github://xs5871/esphome-tsic

sensor:
  - platform: tsic
    name: "TSIC306 Temperature Sensor"
    pin: D5
    model: TSIC306
    update_interval: 60s
```


## Configuration variables:

* **pin** (**Required**, [Pin](https://esphome.io/guides/configuration-types#config-pin)):
  The pin where the ZACwire bus is connected.
* **model** (**Required**, int): Specify the TSIC model, can be one of ``TSIC206``,
  `TSIC306`, `TSIC506`.
* **update_interval** (*Optional*, [Time](https://esphome.io/guides/configuration-types#config-time)):
  The interval to check the sensor. Defaults to ``60s``.
* All other options from [Sensor](https://esphome.io/components/sensor/#config-sensor).


## Notes

TSIC sensors push readings at a fixed interval, which is independend of the
components update interval.

If the component's update interval is longer than the sensor's push interval,
only the most recent sensor reading is published, effectively downsampling the
sensor readings.

If the update interval is shorter than the sensor interval, then update requests
are skipped until a new sensor reading is ready: all readings are published, new
values are only published once.
