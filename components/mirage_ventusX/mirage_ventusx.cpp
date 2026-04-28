#include "mirage_ventusx.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

namespace esphome
{
  namespace mirage_ventusx
  {

    static const char *TAG = "mirage_ventusx.climate";

    void MirageVentusXClimate::setup()
    {
      ESP_LOGI(TAG, "Ventus Mirage climate initialized");
    }

    void MirageVentusXClimate::loop()
    {
      // nothing needed
    }

    void MirageVentusXClimate::control(const climate::ClimateCall &call)
    {
      if (call.get_target_temperature().has_value())
        this->current_temp_ = (uint8_t)*call.get_target_temperature();

      if (call.get_mode().has_value())
        this->mode_ = *call.get_mode();

      this->transmit_state_();
      this->publish_state();
    }

    void VentusMirageVentusXClimatelimate::transmit_state_()
    {
      uint8_t data[12] = {
          0x64, 0x80, 0x00, 0x24,
          0xC0, 0x00, 0x1C, 0x00,
          0x00, 0x00, 0x01, 0x00};

      uint8_t temp = this->current_temp_;
      if (temp < 60)
        temp = 60;
      if (temp > 80)
        temp = 80;

      // ✅ TEMP ENCODING (confirmed)
      data[5] = ((temp - 60) * 0x10) >> 1;

      // TODO: mode mapping later if needed

      // ✅ CHECKSUM
      uint8_t sum = 0;
      for (int i = 0; i < 11; i++)
        sum += data[i];
      data[11] = sum & 0xFF;

      ESP_LOGD(TAG, "TX temp=%d byte5=0x%02X checksum=0x%02X",
               temp, data[5], data[11]);

      auto *tx = App.get_component<remote_base::RemoteTransmitterComponent>();

      if (tx == nullptr)
      {
        ESP_LOGE(TAG, "Remote transmitter not found!");
        return;
      }

      // 🔁 send twice (required)
      for (int i = 0; i < 2; i++)
      {
        auto call = tx->transmit();

        remote_base::AEHAData aeha;
        aeha.address = 0xC4D3;
        aeha.data.assign(data, data + 12);

        call.set_aeha(aeha);
        call.perform();

        delay(40);
      }
    }

    void MirageVentusXClimate::on_receive(remote_base::RemoteReceiveData data)
    {
      // debounce (ignore duplicate second frame)
      uint32_t now = millis();
      if (now - this->last_receive_ < 200)
        return;
      this->last_receive_ = now;

      remote_base::AEHAData decoded;
      if (!remote_base::decode_aeha(data, &decoded))
        return;

      if (decoded.address != 0xC4D3)
        return;

      if (decoded.data.size() < 12)
        return;

      const uint8_t *d = decoded.data.data();

      ESP_LOGD(TAG, "RX AEHA addr=0x%04X data size=%d", decoded.address, decoded.data.size());

      // ✅ TEMP DECODE (inverse of encode)
      uint8_t raw = d[5];
      uint8_t temp = (raw << 1) / 0x10 + 60;

      ESP_LOGD(TAG, "Decoded temp=%d from 0x%02X", temp, raw);

      this->current_temperature = temp;
      this->target_temperature = temp;

      // Basic mode assumption (can refine later)
      this->mode = climate::CLIMATE_MODE_COOL;

      this->publish_state();
    }

  } // namespace ventus_mirage
} // namespace esphome