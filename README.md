# TSIC Temperature Sensors for ESPHome

ESP-TSIC is implemens an external component for integrating some TSIC
temperature sensors into ESPHome.

---

**Note**: The TSIC Sensor Component is based on
[libuni/ZACwire](https://github.com/lebuni/ZACwire-Library), which has to be
included explicitly.
Please note the [caveats](#caveats) section.


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
  libraries:
    - lebuni/ZACwire for TSIC@^2.0.0

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


## Caveats

This component is just a minimal effort wrapper around the ZACwire communication
protocol.
That also means that this component is somewhat inefficient, because it can't
control the necessary interrupt handling, or publish a new sensor reading as
soon as it is complete.
The TSIC component resorts to polling the ZACwire lib, never knowing if a
reading gives a new or a previous value.
The `update_interval` should always be chosen to be longer than the what the
sensor hardware provides.

Things like sleep mode or suspending the component are not tested and may result
in unexpected behavior.
