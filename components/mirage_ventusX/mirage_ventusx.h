#pragma once

#include "esphome/components/climate/climate.h"
#include "esphome/components/remote_base/remote_base.h"

namespace esphome
{
  namespace mirage_ventusx
  {

    class MirageVentusXClimate : public climate::Climate, public Component
    {
    public:
      void setup() override;
      void loop() override;

      void on_receive(remote_base::RemoteReceiveData data);

    protected:
      void control(const climate::ClimateCall &call) override;
      void transmit_state_();

      uint8_t current_temp_{72};
      climate::ClimateMode mode_{climate::CLIMATE_MODE_COOL};

      uint32_t last_receive_{0};
    };

  } // namespace ventus_mirage
} // namespace esphome