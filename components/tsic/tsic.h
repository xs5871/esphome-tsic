#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/sensor/sensor.h"

#include "ZACwire.h"

namespace esphome {
namespace tsic {

enum TSICModel {
    TSIC_MODEL_206 = 206,
    TSIC_MODEL_306 = 306,
    TSIC_MODEL_506 = 506,
};

class TSIC : public PollingComponent, public sensor::Sensor {
public:
    void dump_config() override;
    float get_setup_priority() const override;
    void setup() override;
    void set_pin(InternalGPIOPin *pin) { pin_ = pin; }
    void set_model(TSICModel model) { model_ = model; }
    void update() override;

protected:
    ZACwire* sensor_;
    InternalGPIOPin *pin_;
    int16_t model_;
};

} // namespace tsic
} // namespace esphome
