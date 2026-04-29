#pragma once

#include "esphome/components/climate_ir/climate_ir.h"

namespace esphome {
namespace mirage_ventusx {

const float VENTUSX_TEMP_MIN = 61.0f;
const float VENTUSX_TEMP_MAX = 88.0f;

class MirageVentusXClimate : public climate_ir::ClimateIR {
 public:
  MirageVentusXClimate()
      : climate_ir::ClimateIR(VENTUSX_TEMP_MIN, VENTUSX_TEMP_MAX, 1.0f, true, true,
                              {climate::CLIMATE_FAN_AUTO, climate::CLIMATE_FAN_LOW,
                               climate::CLIMATE_FAN_MEDIUM, climate::CLIMATE_FAN_HIGH},
                              {climate::CLIMATE_SWING_OFF, climate::CLIMATE_SWING_VERTICAL,
                               climate::CLIMATE_SWING_HORIZONTAL, climate::CLIMATE_SWING_BOTH}) {}

 protected:
   uint8_t calc_checksum(const uint8_t *data, uint8 len);
   void transmit_state() override;
   bool on_receive(remote_base::RemoteReceiveData data) override;

   uint32_t last_receive_{0};
};

}  // namespace mirage_ventusx
}  // namespace esphome
