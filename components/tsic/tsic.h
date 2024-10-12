#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/sensor/sensor.h"


namespace esphome {
namespace tsic {

enum TSICModel {
    TSIC_MODEL_206 = 206,
    TSIC_MODEL_306 = 306,
    TSIC_MODEL_316 = 316,
    TSIC_MODEL_506 = 506,
    TSIC_MODEL_516 = 516,
    TSIC_MODEL_716 = 716,
};

class TSIC : public PollingComponent, public sensor::Sensor {
public:
    void dump_config() override;
    float get_setup_priority() const override;
    void setup() override;
    void set_pin(InternalGPIOPin *pin) { pin_ = pin; }
    void set_model(TSICModel model) { model_ = model; }
    void update() override;

    bool check_data_(uint16_t data);
    float get_temperature(uint16_t data);

protected:
    InternalGPIOPin *pin_;
    int16_t model_;
    volatile uint8_t buffer_position_;
    volatile uint16_t buffer_read_;
    volatile uint16_t buffer_write_;
    volatile uint32_t last_edge_us_;
    volatile uint16_t strobe_time_;

    static void IRAM_ATTR edge_intr(TSIC *sensor);
};

} // namespace tsic
} // namespace esphome
