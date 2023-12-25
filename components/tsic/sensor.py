import esphome.config_validation as cv
import esphome.codegen as cg
from esphome import pins
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    CONF_MODEL,
    CONF_PIN,
    CONF_TEMPERATURE,
    DEVICE_CLASS_TEMPERATURE,
    STATE_CLASS_MEASUREMENT,
    UNIT_CELSIUS,
)
from esphome.cpp_helpers import gpio_pin_expression


tsic_ns = cg.esphome_ns.namespace("tsic")
TSICModel = tsic_ns.enum("TSICModel")
TSIC_MODELS = {
        "TSIC206": TSICModel.TSIC_MODEL_206,
        "TSIC306": TSICModel.TSIC_MODEL_306,
        "TSIC506": TSICModel.TSIC_MODEL_506,
}
TSIC = tsic_ns.class_("TSIC", cg.PollingComponent, sensor.Sensor, cg.Component)


CONFIG_SCHEMA = (
    sensor.sensor_schema(
        unit_of_measurement=UNIT_CELSIUS,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_TEMPERATURE,
        state_class=STATE_CLASS_MEASUREMENT,
    )
    .extend(
        {
            cv.GenerateID(): cv.declare_id(TSIC),
            cv.Required(CONF_PIN): pins.internal_gpio_input_pin_schema,
            cv.Required(CONF_MODEL): cv.enum(TSIC_MODELS, upper=True),
        }
    )
    .extend(cv.polling_component_schema("60s"))
)


async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)

    pin = await gpio_pin_expression(config[CONF_PIN])
    cg.add(var.set_pin(pin))

    cg.add(var.set_model(config[CONF_MODEL]))
