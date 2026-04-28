#include "mirage_ventusx.h"
#include "esphome/core/log.h"
#include "esphome/components/remote_base/aeha_protocol.h"

namespace esphome {
namespace mirage_ventusx {

static const char *const TAG = "mirage_ventusx.climate";

void MirageVentusXClimate::transmit_state() {
  uint8_t data[12] = {
      0x64, 0x80, 0x00, 0x24,
      0xC0, 0x00, 0x1C, 0x00,
      0x00, 0x00, 0x01, 0x00};

  uint8_t temp = (uint8_t) this->target_temperature;
  if (temp < 60) temp = 60;
  if (temp > 80) temp = 80;

  // Confirmed encoding from trace analysis
  data[5] = ((temp - 60) * 0x10) >> 1;

  uint8_t sum = 0;
  for (int i = 0; i < 11; i++) sum += data[i];
  data[11] = sum & 0xFF;

  ESP_LOGD(TAG, "TX temp=%d byte5=0x%02X checksum=0x%02X", temp, data[5], data[11]);

  esphome::remote_base::AEHAData aeha;
  aeha.address = 0xC4D3;
  aeha.data.assign(data, data + 12);

  // Unit requires the frame sent twice
  for (int i = 0; i < 2; i++) {
    auto transmit = this->transmitter_->transmit();
    auto *tx_data = transmit.get_data();
    esphome::remote_base::AEHAProtocol().encode(tx_data, aeha);
    transmit.perform();
    if (i == 0) delay(40);
  }
}

bool MirageVentusXClimate::on_receive(remote_base::RemoteReceiveData data) {
  // Ignore the duplicate second frame the unit always sends
  uint32_t now = millis();
  if (now - this->last_receive_ < 200) return false;
  this->last_receive_ = now;

  auto decoded = esphome::remote_base::AEHAProtocol().decode(data);
  if (!decoded) return false;
  if (decoded->address != 0xC4D3) return false;
  if (decoded->data.size() < 12) return false;

  const uint8_t *d = decoded->data.data();

  ESP_LOGD(TAG, "RX AEHA addr=0x%04X data_size=%d", decoded->address, (int) decoded->data.size());

  // Inverse of transmit encoding
  uint8_t raw = d[5];
  uint8_t temp = (raw << 1) / 0x10 + 60;

  ESP_LOGD(TAG, "Decoded temp=%d from byte5=0x%02X", temp, raw);

  this->target_temperature = temp;
  this->mode = climate::CLIMATE_MODE_COOL;

  this->publish_state();
  return true;
}

}  // namespace mirage_ventusx
}  // namespace esphome
