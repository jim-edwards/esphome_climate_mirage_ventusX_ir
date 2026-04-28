#pragma once

#include "esphome/components/climate_ir/climate_ir.h"

namespace esphome {
namespace mirage_ventusx {

const float VENTUSX_TEMP_MIN = 60.0f;
const float VENTUSX_TEMP_MAX = 80.0f;

class MirageVentusXClimate : public climate_ir::ClimateIR {
 public:
  MirageVentusXClimate()
      : climate_ir::ClimateIR(VENTUSX_TEMP_MIN, VENTUSX_TEMP_MAX, 1.0f, true, false, {}, {}) {}

 protected:
  void transmit_state() override;
  bool on_receive(remote_base::RemoteReceiveData data) override;

  uint32_t last_receive_{0};
};

}  // namespace mirage_ventusx
}  // namespace esphome
