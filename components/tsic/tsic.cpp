#include "tsic.h"


namespace esphome {
namespace tsic {


static const char *const TAG = "tsic";


void TSIC::dump_config() {
    ESP_LOGCONFIG(TAG,  "TSIC:");
    LOG_PIN("  Pin: ", this->pin_)
    ESP_LOGCONFIG(TAG,  "  Model: %i", this->model_);
    LOG_UPDATE_INTERVAL(this);
}


float TSIC::get_setup_priority() const {
    return esphome::setup_priority::DATA;
}


void TSIC::setup() {
    ESP_LOGCONFIG(TAG, "Setting up TSIC...");

    this->buffer_idx_ = 0;
    this->buffer_read_ = 0;
    this->last_edge_us_ = micros();
    this->pin_->setup();
    this->pin_->attach_interrupt(TSIC::edge_interrupt_, this, gpio::INTERRUPT_RISING_EDGE);
}


void TSIC::update() {
    // post an error if there was no signal for more than 100ms
    if (micros() - this->last_edge_us_ > 100000) {
        this->status_set_warning();
        ESP_LOGE(TAG, "Not connected");
        return;
    }

    // there's no new data there yet
    if (!this->buffer_read_) {
        return;
    }

    // make a local copy of the data
    uint16_t data = this->buffer_read_;
    this->buffer_read_ = 0;

    if (!this->check_data_(data)) {
        this->status_set_warning();
        ESP_LOGE(TAG, "Failed to read data");
        return;
    }

    this->status_clear_warning();
    publish_state(this->get_temperature(data));
}


float TSIC::get_temperature(uint16_t data) {
    // strip parity and stop bits
    data >>= 1;
    data = (data >> 2 & 0x700) | (data & 0xff);

    // from datasheet: T = Sig / (adc_max) * (t_high - t_low) + t_low
    // reshuffled a bit to hopefully reduce floating point operations
    float temp = -273.4;
    switch (this->model_) {
        case TSIC_MODEL_206:
        case TSIC_MODEL_306:
            temp = (data * 200 - (50 * 2047)) / 2047.;
            break;
        case TSIC_MODEL_316:
            temp = (data * 200 - (50 * 16383)) / 16383.;
            break;
        case TSIC_MODEL_506:
            temp = (data * 70 - (10 * 2047)) / 2047.;
            break;
        case TSIC_MODEL_516:
        case TSIC_MODEL_716:
            temp = (data * 70 - (10 * 16383)) / 16383.;
    }
    return temp;
}


bool TSIC::check_data_(uint16_t data) {
    // check first parity
    for (uint8_t i=0; i<=8; ++i) {
        data = (data & 1) ^ (data >> 1);
    }
    if (data & 1) {
        return false;
    }

    // check second parity
    for (uint8_t i=0; i<=7; ++i) {
        data = (data & 1) ^ (data >> 1);
    }
    if (data & 1) {
        return false;
    }

    return true;
}


void IRAM_ATTR TSIC::edge_interrupt_(TSIC *sensor) {
    uint32_t now = micros();
    uint32_t delta = now - sensor->last_edge_us_;
    sensor->last_edge_us_ = now;

    // skip the strobe bit at the beginning of a new packet
    if (delta > 500) {
        sensor->buffer_idx_ = 0;
        sensor->buffer_write_ = 0;
        return;
    }

    sensor->buffer_idx_++;

    // First bit is always zero and can be used instead of the strobe bit to
    // recover a good approximation to the strobe time.
    if (sensor->buffer_idx_ == 1)  {
        sensor->strobe_time_ = delta / 2;
        return;
    }

    uint8_t thr = sensor->strobe_time_;

    // account for stop-bit of previous word
    if (sensor->buffer_idx_ == 11) {
        thr += sensor->strobe_time_ / 2;
    }
    // offset if last bit was long (i.e. 1)
    else if (sensor->buffer_write_ & 1) {
        thr += sensor->strobe_time_;
    }

    // shove a new bit in
    sensor->buffer_write_ <<= 1;
    if (delta < thr) {
        sensor->buffer_write_ |= 1;
    }

    // copy double buffer
    if (sensor->buffer_idx_ == 19) {
        sensor->buffer_read_ = sensor->buffer_write_;
    }
}


} // namespace tsic
} // namespace esphome
