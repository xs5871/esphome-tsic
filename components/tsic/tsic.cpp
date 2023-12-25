#include "tsic.h"

namespace esphome {
namespace tsic {

static const char *const TAG = "tsic";

void TSIC::dump_config() {
    ESP_LOGCONFIG(TAG,  "TSIC:");
    LOG_PIN("  Pin: ", this->pin_)
    ESP_LOGCONFIG(TAG,  "  Model: %i", this->model_);
    LOG_UPDATE_INTERVAL(this);
    // LOG_SENSOR("  ", "Temperature", this->sensor_->getTemp());
}

float TSIC::get_setup_priority() const {
    return esphome::setup_priority::DATA;
}

void TSIC::setup() {
    ESP_LOGCONFIG(TAG, "Setting up TSIC...");
    sensor_ = new ZACwire(this->pin_->get_pin(), this->model_);
    if (this->sensor_->begin() != true) {
        this->mark_failed();
    }
}

void TSIC::update() {
    float value = this->sensor_->getTemp();

    if (value == 221) {
        this->status_set_warning();
        ESP_LOGE(TAG, "not connected");
    } else if (value == 222) {
        this->status_set_warning();
        ESP_LOGE(TAG, "failed to read data");
    } else {
        this->status_clear_warning();
        publish_state(value);
    }
}

} // namespace tsic
} // namespace esphome
