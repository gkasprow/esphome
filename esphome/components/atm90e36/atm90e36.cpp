// Beta version, data retrieval still needs to be verified...
#include "atm90e36.h"
#include "atm90e36_reg.h"
#include "esphome/core/log.h"
#include <cinttypes>

namespace esphome {
namespace atm90e36 {

static const char *const TAG = "atm90e36";
void ATM90E36Component::loop() {
  if (this->get_publish_interval_flag_()) {
    this->set_publish_interval_flag_(false);
    for (uint8_t phase = 0; phase < 3; phase++) {
      if (this->phase_[phase].voltage_sensor_ != nullptr) {
        this->phase_[phase].voltage_ = this->get_phase_voltage_(phase);
      }
    }
    for (uint8_t phase = 0; phase < 3; phase++) {
      if (this->phase_[phase].current_sensor_ != nullptr) {
        this->phase_[phase].current_ = this->get_phase_current_(phase);
      }
    }
    for (uint8_t phase = 0; phase < 3; phase++) {
      if (this->phase_[phase].power_sensor_ != nullptr) {
        this->phase_[phase].active_power_ = this->get_phase_active_power_(phase);
      }
    }
    for (uint8_t phase = 0; phase < 3; phase++) {
      if (this->phase_[phase].power_factor_sensor_ != nullptr) {
        this->phase_[phase].power_factor_ = this->get_phase_power_factor_(phase);
      }
    }
    for (uint8_t phase = 0; phase < 3; phase++) {
      if (this->phase_[phase].reactive_power_sensor_ != nullptr) {
        this->phase_[phase].reactive_power_ = this->get_phase_reactive_power_(phase);
      }
    }
    for (uint8_t phase = 0; phase < 3; phase++) {
      if (this->phase_[phase].apparent_power_sensor_ != nullptr) {
        this->phase_[phase].apparent_power_ = this->get_phase_apparent_power_(phase);
      }
    }
/*
    if (this->total_active_power_ != nullptr) {
      this->total_active_power_->publish_state(this->get_total_active_power_());
    }
        if (this->total_reactive_power_ != nullptr) {
      this->total_reactive_power_->publish_state(this->get_total_reactive_power_()());
    }
        if (this->total_apparent_power_ != nullptr) {
      this->total_apparent_power_->publish_state(this->get_total_apparent_power_());
    }
*/
    for (uint8_t phase = 0; phase < 3; phase++) {
      if (this->phase_[phase].forward_active_energy_sensor_ != nullptr) {
        this->phase_[phase].forward_active_energy_ = this->get_phase_forward_active_energy_(phase);
      }
    }
    for (uint8_t phase = 0; phase < 3; phase++) {
      if (this->phase_[phase].reverse_active_energy_sensor_ != nullptr) {
        this->phase_[phase].reverse_active_energy_ = this->get_phase_reverse_active_energy_(phase);
      }
    }
    for (uint8_t phase = 0; phase < 3; phase++) {
      if (this->phase_[phase].phase_angle_sensor_ != nullptr) {
        this->phase_[phase].phase_angle_ = this->get_phase_angle_(phase);
      }
    }
    for (uint8_t phase = 0; phase < 3; phase++) {
      if (this->phase_[phase].harmonic_active_power_sensor_ != nullptr) {
        this->phase_[phase].harmonic_active_power_ = this->get_phase_harmonic_active_power_(phase);
      }
    }
    for (uint8_t phase = 0; phase < 3; phase++) {
      if (this->phase_[phase].peak_current_sensor_ != nullptr) {
        this->phase_[phase].peak_current_ = this->get_phase_peak_current_(phase);
      }
    }
    for (uint8_t phase = 0; phase < 3; phase++) {
      if (this->phase_[phase].voltage_sensor_ != nullptr) {
        this->phase_[phase].voltage_sensor_->publish_state(this->get_local_phase_voltage_(phase));
      }
    }
    for (uint8_t phase = 0; phase < 3; phase++) {
      if (this->phase_[phase].current_sensor_ != nullptr) {
        this->phase_[phase].current_sensor_->publish_state(this->get_local_phase_current_(phase));
      }
    }
    for (uint8_t phase = 0; phase < 3; phase++) {
      if (this->phase_[phase].power_sensor_ != nullptr) {
        this->phase_[phase].power_sensor_->publish_state(this->get_local_phase_active_power_(phase));
      }
    }
    for (uint8_t phase = 0; phase < 3; phase++) {
      if (this->phase_[phase].power_factor_sensor_ != nullptr) {
        this->phase_[phase].power_factor_sensor_->publish_state(this->get_local_phase_power_factor_(phase));
      }
    }
    for (uint8_t phase = 0; phase < 3; phase++) {
      if (this->phase_[phase].reactive_power_sensor_ != nullptr) {
        this->phase_[phase].reactive_power_sensor_->publish_state(this->get_local_phase_reactive_power_(phase));
      }
    }
    for (uint8_t phase = 0; phase < 3; phase++) {
      if (this->phase_[phase].apparent_power_sensor_ != nullptr) {
        this->phase_[phase].apparent_power_sensor_->publish_state(this->get_local_phase_apparent_power_(phase));
      }
    }
    for (uint8_t phase = 0; phase < 3; phase++) {
      if (this->phase_[phase].forward_active_energy_sensor_ != nullptr) {
        this->phase_[phase].forward_active_energy_sensor_->publish_state(
            this->get_local_phase_forward_active_energy_(phase));
      }
    }
    for (uint8_t phase = 0; phase < 3; phase++) {
      if (this->phase_[phase].reverse_active_energy_sensor_ != nullptr) {
        this->phase_[phase].reverse_active_energy_sensor_->publish_state(
            this->get_local_phase_reverse_active_energy_(phase));
      }
    }
    for (uint8_t phase = 0; phase < 3; phase++) {
      if (this->phase_[phase].phase_angle_sensor_ != nullptr) {
        this->phase_[phase].phase_angle_sensor_->publish_state(this->get_local_phase_angle_(phase));
      }
    }
    for (uint8_t phase = 0; phase < 3; phase++) {
      if (this->phase_[phase].harmonic_active_power_sensor_ != nullptr) {
        this->phase_[phase].harmonic_active_power_sensor_->publish_state(
            this->get_local_phase_harmonic_active_power_(phase));
      }
    }
    for (uint8_t phase = 0; phase < 3; phase++) {
      if (this->phase_[phase].peak_current_sensor_ != nullptr) {
        this->phase_[phase].peak_current_sensor_->publish_state(this->get_local_phase_peak_current_(phase));
      }
    }
    if (this->freq_sensor_ != nullptr) {
      this->freq_sensor_->publish_state(this->get_frequency_());
    }
    if (this->chip_temperature_sensor_ != nullptr) {
      this->chip_temperature_sensor_->publish_state(this->get_chip_temperature_());
    }
  }
}

void ATM90E36Component::update() {
  if (this->read16_(ATM90E36_REGISTER_CONFIGSTART) != 0x8765) { //0x5678) {
    this->status_set_warning();
    ESP_LOGCONFIG(TAG, "Respuesta REGISTER_CONFIGSTART erronea...");
    return;
  }
  this->set_publish_interval_flag_(true);
  this->status_clear_warning();
}

void ATM90E36Component::setup() {
  ESP_LOGCONFIG(TAG, "Configurando componente ATM90E36...");
  this->spi_setup();

  uint16_t mmode0 = 0x87;  // 3P4W 50Hz
  if (line_freq_ == 60) {
    mmode0 |= 1 << 12;  // sets 12th bit to 1, 60Hz
  }

  if (current_phases_ == 2) {
    mmode0 |= 1 << 8;  // sets 8th bit to 1, 3P3W
    mmode0 |= 0 << 1;  // sets 1st bit to 0, phase b is not counted into the all-phase sum energy/power (P/Q/S)
  }
  
  // **************** Inicialización registros ****************
  this->write16_(ATM90E36_REGISTER_SOFTRESET, 0x789A);    // Reg. 0x00 Perform soft reset
  delay(6);                                               // Wait for the minimum 5ms + 1ms
  // SagTh = Vth * 100 * sqrt(2) / (2 * Ugain / 32768)
  this->write16_(ATM90E36_REGISTER_FUNCEN0, 0x0000); // Voltage sag
  this->write16_(ATM90E36_REGISTER_FUNCEN1, 0x0000); // Voltage sag
  this->write16_(ATM90E36_REGISTER_SAGTH, 0x0001);   // Voltage sag threshold
  
  this->write16_(ATM90E36_REGISTER_CONFIGSTART, 0x5678); // Metering calibration startup
  if (this->read16_(ATM90E36_REGISTER_LASTSPIDATA) != 0x5678) {
    ESP_LOGW(TAG, "No se ha podido inicializar ATM90E36 IC, compruebe la configuración SPI");
    this->mark_failed();
    return;
  }
  
  this->write16_(ATM90E36_REGISTER_PLCONSTH, 0x0861);       // PL Constant MSB (default) = 140625000
  this->write16_(ATM90E36_REGISTER_PLCONSTL, 0xC468);       // PL Constant LSB (default)
  this->write16_(ATM90E36_REGISTER_ZXCONFIG, 0xD654);       // ZX2, ZX1, ZX0 pin config
  this->write16_(ATM90E36_REGISTER_MMODE0, mmode0);         // Mode Config (frequency set in main program)
  this->write16_(ATM90E36_REGISTER_MMODE1, pga_gain_);      // PGA Gain Configuration for Current Channels
  this->write16_(ATM90E36_REGISTER_PSTARTTH, 0x1D4C);       // All Active Startup Power Threshold - 0.02A/0.00032 = 7500
  this->write16_(ATM90E36_REGISTER_QSTARTTH, 0x1D4C);       // All Reactive Startup Power Threshold - 50%
  this->write16_(ATM90E36_REGISTER_SSTARTTH, 0x1D4C);       // All Reactive Startup Power Threshold - 50%
  this->write16_(ATM90E36_REGISTER_PPHASETH, 0x02EE);       // Each Phase Active Phase Threshold - 0.002A/0.00032 = 750
  this->write16_(ATM90E36_REGISTER_QPHASETH, 0x02EE);       // Each phase Reactive Phase Threshold - 10%
  this->write16_(ATM90E36_REGISTER_SPHASETH, 0x0000);       // Apparent  Phase Threshold
  this->write16_(ATM90E36_REGISTER_CSO, 0x4741);            // Checksum 0
  // Set metering calibration values (CALIBRATION)
  this->write16_(ATM90E36_REGISTER_HARMSTART, 0x5678); // Inicio de la calibración de medición
  this->write16_(ATM90E36_REGISTER_PQGAINA, 0x0000);       // Line calibration gain
  this->write16_(ATM90E36_REGISTER_PHIA, 0x0000);        // Line calibration angle
  this->write16_(ATM90E36_REGISTER_PQGAINB, 0x0000);       // Line calibration gain
  this->write16_(ATM90E36_REGISTER_PHIB, 0x0000);        // Line calibration angle
  this->write16_(ATM90E36_REGISTER_PQGAINC, 0x0000);       // Line calibration gain
  this->write16_(ATM90E36_REGISTER_PHIC, 0x0000);        // Line calibration angle
  // Configura potencia
  this->write16_(ATM90E36_REGISTER_POFFSETA, 0x0000); // Offset de potencia activa de la línea A
  this->write16_(ATM90E36_REGISTER_QOFFSETA, 0x0000); // Offset de potencia reactiva de la línea A
  this->write16_(ATM90E36_REGISTER_POFFSETB, 0x0000); // Offset de potencia activa de la línea B
  this->write16_(ATM90E36_REGISTER_QOFFSETB, 0x0000); // Offset de potencia reactiva de la línea N
  this->write16_(ATM90E36_REGISTER_POFFSETC, 0x0000); // Offset de potencia activa de la línea C
  this->write16_(ATM90E36_REGISTER_QOFFSETC, 0x0000); // Offset de potencia reactiva de la línea C
  this->write16_(ATM90E36_REGISTER_CS1, 0x0000);       // Checksum 1 ??no se si es correcto el valor
  // Set metering calibration values (HARMONIC)
  this->write16_(ATM90E36_REGISTER_HARMSTART, 0x5678); // Inicio de la calibración de medición
  this->write16_(ATM90E36_REGISTER_POFFSETAF, 0x0000); // Offset de potencia activa fundamental de la línea A
  this->write16_(ATM90E36_REGISTER_POFFSETBF, 0x0000); // Offset de potencia activa fundamental de la línea B
  this->write16_(ATM90E36_REGISTER_POFFSETCF, 0x0000); // Offset de potencia activa fundamental de la línea C
  this->write16_(ATM90E36_REGISTER_PGAINAF, 0x0000);   // Ganancia de potencia activa fundamental de la línea A
  this->write16_(ATM90E36_REGISTER_PGAINBF, 0x0000);   // Ganancia de potencia activa fundamental de la línea B
  this->write16_(ATM90E36_REGISTER_PGAINCF, 0x0000);   // Ganancia de potencia activa fundamental de la línea C
  this->write16_(ATM90E36_REGISTER_CS2, 0x0000);     // Checksum 2 ??no se si es correcto el valor

  //**************** REGISTROS DE CALIBRACIÓN DE MEDICIÓN ****************
  this->write16_(ATM90E36_REGISTER_ADJSTART, 0x5678); // Calibración de medición
  // Configurar los offsets de calibración de voltaje y corriente para la FASE A
  this->phase_[PHASEA].voltage_offset_ = calibrate_voltage_offset_phase(PHASEA);
  this->write16_(ATM90E36_REGISTER_UOFFSETA, this->phase_[PHASEA].voltage_offset_);  // Offset de voltaje de la fase A
  this->phase_[PHASEA].current_offset_ = calibrate_current_offset_phase(PHASEA);
  this->write16_(ATM90E36_REGISTER_IOFFSETA, this->phase_[PHASEA].current_offset_);  // Offset de corriente de la fase A
  // Configurar la ganancia de voltaje y corriente para la FASE A
  this->write16_(ATM90E36_REGISTER_UGAINA, this->phase_[PHASEA].voltage_gain_);  // Ganancia RMS de voltaje de la fase A
  this->write16_(ATM90E36_REGISTER_IGAINA, this->phase_[PHASEA].ct_gain_);       // Ganancia de corriente de la línea A
  // Configurar los offsets de calibración de voltaje y corriente para la FASE B
  this->phase_[PHASEB].voltage_offset_ = calibrate_voltage_offset_phase(PHASEB);
  this->write16_(ATM90E36_REGISTER_UOFFSETB, this->phase_[PHASEB].voltage_offset_);  // Offset de voltaje de la fase B
  this->phase_[PHASEB].current_offset_ = calibrate_current_offset_phase(PHASEB);
  this->write16_(ATM90E36_REGISTER_IOFFSETB, this->phase_[PHASEB].current_offset_);  // Offset de corriente de la fase B
  // Configurar la ganancia de voltaje y corriente para la FASE B
  this->write16_(ATM90E36_REGISTER_UGAINB, this->phase_[PHASEB].voltage_gain_);  // Ganancia RMS de voltaje de la fase B
  this->write16_(ATM90E36_REGISTER_IGAINB, this->phase_[PHASEB].ct_gain_);       // Ganancia de corriente de la línea B
  // Configurar los offsets de calibración de voltaje y corriente para la FASE C
  this->phase_[PHASEC].voltage_offset_ = calibrate_voltage_offset_phase(PHASEC);
  this->write16_(ATM90E36_REGISTER_UOFFSETC, this->phase_[PHASEC].voltage_offset_);  // Offset de voltaje de la fase C
  this->phase_[PHASEC].current_offset_ = calibrate_current_offset_phase(PHASEC);
  this->write16_(ATM90E36_REGISTER_IOFFSETC, this->phase_[PHASEC].current_offset_);  // Offset de corriente de la fase C
  // Configurar la ganancia de voltaje y corriente para la FASE C
  this->write16_(ATM90E36_REGISTER_UGAINC, this->phase_[PHASEC].voltage_gain_);  // Ganancia RMS de voltaje de la fase C
  this->write16_(ATM90E36_REGISTER_IGAINC, this->phase_[PHASEC].ct_gain_);       // Ganancia de corriente de la línea C
  this->write16_(ATM90E36_REGISTER_IGAINN, 0xFD7F); // Ganancia de corriente de la línea D
  this->write16_(ATM90E36_REGISTER_CS3, 0x02F6); // Checksum 3, fin de la configuración
/*
  6886H (Power-up state):
    Es el valor predeterminado tras un reinicio o al encender el dispositivo.
    Este estado desactiva la verificación de checksum, lo que significa que no se generan errores relacionados con la integridad de los datos.
  5678H (Calibration state):
    Este valor activa un modo de calibración.
    También desactiva la verificación de checksum, permitiendo que se escriban y reconfiguren registros relacionados con la calibración.
    Al escribir este valor, se realiza un reinicio de los registros asociados.
  8765H (Operation state):
    Este es el valor que habilita el modo de operación normal.
    En este estado, se habilita la verificación de checksum:
        Si el checksum detecta un error, el sistema genera una señal de advertencia (IRQ/Warn) y detiene las operaciones de medición.
        Esto asegura que los datos críticos no estén corruptos y que el sistema funcione de manera confiable.
*/
  // Finalización configuración
  this->write16_(ATM90E36_REGISTER_CONFIGSTART, 0x8765); // 0x6886 //0x5678 //8765);
  this->write16_(ATM90E36_REGISTER_CALSTART, 0x8765); // 0x6886 //0x5678 //8765);
  this->write16_(ATM90E36_REGISTER_HARMSTART, 0x8765); // 0x6886 //0x5678 //8765);
  this->write16_(ATM90E36_REGISTER_ADJSTART, 0x8765);  // 0x6886 //0x5678 //8765);
}

void ATM90E36Component::dump_config() {
  ESP_LOGCONFIG("", "ATM90E36:");
  LOG_PIN("  CS Pin: ", this->cs_);
  if (this->is_failed()) {
    ESP_LOGE(TAG, "ERROR en comunicación con ATM90E36!");
  }
  LOG_UPDATE_INTERVAL(this);
  LOG_SENSOR("  ", "Voltage A", this->phase_[PHASEA].voltage_sensor_);
  LOG_SENSOR("  ", "Current A", this->phase_[PHASEA].current_sensor_);
  LOG_SENSOR("  ", "Power A", this->phase_[PHASEA].power_sensor_);
  LOG_SENSOR("  ", "Reactive Power A", this->phase_[PHASEA].reactive_power_sensor_);
  LOG_SENSOR("  ", "PF A", this->phase_[PHASEA].power_factor_sensor_);
  LOG_SENSOR("  ", "Active Forward Energy A", this->phase_[PHASEA].forward_active_energy_sensor_);
  LOG_SENSOR("  ", "Active Reverse Energy A", this->phase_[PHASEA].reverse_active_energy_sensor_);
  LOG_SENSOR("  ", "Harmonic Power A", this->phase_[PHASEA].harmonic_active_power_sensor_);
  LOG_SENSOR("  ", "Phase Angle A", this->phase_[PHASEA].phase_angle_sensor_);
  LOG_SENSOR("  ", "Peak Current A", this->phase_[PHASEA].peak_current_sensor_);
  LOG_SENSOR("  ", "Voltage B", this->phase_[PHASEB].voltage_sensor_);
  LOG_SENSOR("  ", "Current B", this->phase_[PHASEB].current_sensor_);
  LOG_SENSOR("  ", "Power B", this->phase_[PHASEB].power_sensor_);
  LOG_SENSOR("  ", "Reactive Power B", this->phase_[PHASEB].reactive_power_sensor_);
  LOG_SENSOR("  ", "PF B", this->phase_[PHASEB].power_factor_sensor_);
  LOG_SENSOR("  ", "Active Forward Energy B", this->phase_[PHASEB].forward_active_energy_sensor_);
  LOG_SENSOR("  ", "Active Reverse Energy B", this->phase_[PHASEB].reverse_active_energy_sensor_);
  LOG_SENSOR("  ", "Harmonic Power A", this->phase_[PHASEB].harmonic_active_power_sensor_);
  LOG_SENSOR("  ", "Phase Angle A", this->phase_[PHASEB].phase_angle_sensor_);
  LOG_SENSOR("  ", "Peak Current A", this->phase_[PHASEB].peak_current_sensor_);
  LOG_SENSOR("  ", "Voltage C", this->phase_[PHASEC].voltage_sensor_);
  LOG_SENSOR("  ", "Current C", this->phase_[PHASEC].current_sensor_);
  LOG_SENSOR("  ", "Power C", this->phase_[PHASEC].power_sensor_);
  LOG_SENSOR("  ", "Reactive Power C", this->phase_[PHASEC].reactive_power_sensor_);
  LOG_SENSOR("  ", "PF C", this->phase_[PHASEC].power_factor_sensor_);
  LOG_SENSOR("  ", "Active Forward Energy C", this->phase_[PHASEC].forward_active_energy_sensor_);
  LOG_SENSOR("  ", "Active Reverse Energy C", this->phase_[PHASEC].reverse_active_energy_sensor_);
  LOG_SENSOR("  ", "Harmonic Power A", this->phase_[PHASEC].harmonic_active_power_sensor_);
  LOG_SENSOR("  ", "Phase Angle A", this->phase_[PHASEC].phase_angle_sensor_);
  LOG_SENSOR("  ", "Peak Current A", this->phase_[PHASEC].peak_current_sensor_);
  LOG_SENSOR("  ", "Frequency", this->freq_sensor_);
  LOG_SENSOR("  ", "Chip Temp", this->chip_temperature_sensor_);
}

float ATM90E36Component::get_setup_priority() const { return setup_priority::IO; }

uint16_t ATM90E36Component::read16_(uint16_t a_register) {
  uint8_t addrh = (1 << 7) | ((a_register >> 8) & 0x03);
  uint8_t addrl = (a_register & 0xFF);
  uint8_t data[2];
  uint16_t output;
  this->enable();
  delay_microseconds_safe(10);
  this->write_byte(addrh);
  this->write_byte(addrl);
  this->read_array(data, 2);
  this->disable();

  output = (uint16_t(data[0] & 0xFF) << 8) | (data[1] & 0xFF);
  ESP_LOGVV(TAG, "read16_ 0x%04" PRIX16 " output 0x%04" PRIX16, a_register, output);
  return output;
}

int ATM90E36Component::read32_(uint16_t addr_h, uint16_t addr_l) {
  const uint16_t val_h = this->read16_(addr_h);
  const uint16_t val_l = this->read16_(addr_l);
  const int32_t val = (val_h << 16) | val_l;

  ESP_LOGVV(TAG,
            "read32_ addr_h 0x%04" PRIX16 " val_h 0x%04" PRIX16 " addr_l 0x%04" PRIX16 " val_l 0x%04" PRIX16
            " = %" PRId32,
            addr_h, val_h, addr_l, val_l, val);

  return val;
}

void ATM90E36Component::write16_(uint16_t a_register, uint16_t val) {
  ESP_LOGVV(TAG, "Escribir registro write16_ 0x%04" PRIX16 " valor 0x%04" PRIX16, a_register, val);
  this->enable();
  this->write_byte16(a_register);
  this->write_byte16(val);
  this->disable();
  if (this->read16_(ATM90E36_REGISTER_LASTSPIDATA) != val)
    ESP_LOGW(TAG, "SPI ERROR escritura registro 0x%04X valor 0x%04X", a_register, val);
}

float ATM90E36Component::get_local_phase_voltage_(uint8_t phase) { return this->phase_[phase].voltage_; }

float ATM90E36Component::get_local_phase_current_(uint8_t phase) { return this->phase_[phase].current_; }

float ATM90E36Component::get_local_phase_active_power_(uint8_t phase) { return this->phase_[phase].active_power_; }

float ATM90E36Component::get_local_phase_reactive_power_(uint8_t phase) { return this->phase_[phase].reactive_power_; }

float ATM90E36Component::get_local_phase_apparent_power_(uint8_t phase) { return this->phase_[phase].apparent_power_; }

float ATM90E36Component::get_local_phase_power_factor_(uint8_t phase) { return this->phase_[phase].power_factor_; }

float ATM90E36Component::get_local_phase_forward_active_energy_(uint8_t phase) {
  return this->phase_[phase].forward_active_energy_;
}

float ATM90E36Component::get_local_phase_reverse_active_energy_(uint8_t phase) {
  return this->phase_[phase].reverse_active_energy_;
}

float ATM90E36Component::get_local_phase_angle_(uint8_t phase) { return this->phase_[phase].phase_angle_; }

float ATM90E36Component::get_local_phase_harmonic_active_power_(uint8_t phase) {
  return this->phase_[phase].harmonic_active_power_;
}

float ATM90E36Component::get_local_phase_peak_current_(uint8_t phase) { return this->phase_[phase].peak_current_; }

float ATM90E36Component::get_phase_voltage_(uint8_t phase) {
  const uint16_t voltage = this->read16_(ATM90E36_REGISTER_URMS + phase);
  if (this->read16_(ATM90E36_REGISTER_LASTSPIDATA) != voltage)
    ESP_LOGW(TAG, "SPI URMS Error de lectura del registro de voltaje.");
  return (float) voltage / 100;
}

float ATM90E36Component::get_phase_voltage_avg_(uint8_t phase) {
  const uint8_t reads = 10;
  uint32_t accumulation = 0;
  uint16_t voltage = 0;
  for (uint8_t i = 0; i < reads; i++) {
    voltage = this->read16_(ATM90E36_REGISTER_URMS + phase);
    if (this->read16_(ATM90E36_REGISTER_LASTSPIDATA) != voltage)
      ESP_LOGW(TAG, "SPI URMS Error de lectura del registro de voltaje..");
    accumulation += voltage;
  }
  voltage = accumulation / reads;
  this->phase_[phase].voltage_ = (float) voltage / 100;
  return this->phase_[phase].voltage_;
}

float ATM90E36Component::get_phase_current_avg_(uint8_t phase) {
  const uint8_t reads = 10;
  uint32_t accumulation = 0;
  uint16_t current = 0;
  for (uint8_t i = 0; i < reads; i++) {
    current = this->read16_(ATM90E36_REGISTER_IRMS + phase);
    if (this->read16_(ATM90E36_REGISTER_LASTSPIDATA) != current)
      ESP_LOGW(TAG, "SPI IRMS Error de lectura del registro de corriente.");
    accumulation += current;
  }
  current = accumulation / reads;
  this->phase_[phase].current_ = (float) current / 1000;
  return this->phase_[phase].current_;
}

float ATM90E36Component::get_phase_current_(uint8_t phase) {
  const uint16_t current = this->read16_(ATM90E36_REGISTER_IRMS + phase);
  if (this->read16_(ATM90E36_REGISTER_LASTSPIDATA) != current)
    ESP_LOGW(TAG, "SPI IRMS Error de lectura del registro de corriente.");
  return (float) current / 1000;
}

// Leer un valor de 16 bits desde el registro de potencia activa para la fase especificada.
float ATM90E36Component::get_phase_active_power_(uint8_t phase) {
  const int16_t val = static_cast<int16_t>(this->read16_(ATM90E36_REGISTER_PMEAN + phase));
  //return static_cast<float>(val) / 1000.0; // Devuelve el valor en kW.
  return static_cast<float>(val); // Devuelve el valor en vatios (W).
}
// Potencia reactiva
float ATM90E36Component::get_phase_reactive_power_(uint8_t phase) {
  uint16_t val = this->read16_(ATM90E36_REGISTER_QMEAN + phase);
  if (val & 0x8000) {
    val= (val & 0x7FFF) * -1;
  }
  return (float)val / 250;
}
// Potencia aparente
float ATM90E36Component::get_phase_apparent_power_(uint8_t phase) {
  uint16_t val = this->read16_(ATM90E36_REGISTER_SMEANA + phase);
  if (val & 0x8000) {
    val= (val & 0x7FFF) * -1;
  }
  //return (float)val / 1000;  // Kva
  return (float)val;
}
/*
// Total potencia activa
float ATM90E36Component::get_total_active_power_() {
  const int16_t apower = this->read16_(ATM90E36_REGISTER_PMEANT); 
  return (float)apower / 250;
}
// Total potencia reactiva
float ATM90E36Component::get_total_reactive_power_() {
  const uint16_t qpower = this->read16_(ATM90E36_REGISTER_QMEANT);
  return (float)qpower / 250;
}
// Total potencia aparente
float ATM90E36Component::get_total_apparent_power_() {
  const uint16_t spower = this->read16_(ATM90E36_REGISTER_SMEANT);
  return (float)spower / 250;
}
*/
float ATM90E36Component::get_phase_power_factor_(uint8_t phase) {
  const int16_t powerfactor = this->read16_(ATM90E36_REGISTER_PFMEAN + phase);
  if (this->read16_(ATM90E36_REGISTER_LASTSPIDATA) != powerfactor)
    ESP_LOGW(TAG, "SPI Error de lectura del factor de potencia..");
  return (float) powerfactor / 1000;
}

float ATM90E36Component::get_phase_forward_active_energy_(uint8_t phase) {
  const uint16_t val = this->read16_(ATM90E36_REGISTER_APENERGY + phase);
  if ((UINT32_MAX - this->phase_[phase].cumulative_forward_active_energy_) > val) {
    this->phase_[phase].cumulative_forward_active_energy_ += val;
  } else {
    this->phase_[phase].cumulative_forward_active_energy_ = val;
  }
  return ((float) this->phase_[phase].cumulative_forward_active_energy_ * 10 / 3200);
}

float ATM90E36Component::get_phase_reverse_active_energy_(uint8_t phase) {
  const uint16_t val = this->read16_(ATM90E36_REGISTER_ANENERGY);
  if (UINT32_MAX - this->phase_[phase].cumulative_reverse_active_energy_ > val) {
    this->phase_[phase].cumulative_reverse_active_energy_ += val;
  } else {
    this->phase_[phase].cumulative_reverse_active_energy_ = val;
  }
  return ((float) this->phase_[phase].cumulative_reverse_active_energy_ * 10 / 3200);
}

float ATM90E36Component::get_phase_harmonic_active_power_(uint8_t phase) {
  int16_t val = this->read16_(ATM90E36_REGISTER_PMEANH + phase); 
  if (val & 0x8000) {
    val= (val & 0x7FFF) * -1;
  }
  return (double)val / 1000;
}

// PHASE ANGLE
float ATM90E36Component::get_phase_angle_(uint8_t phase) {
    int16_t val = static_cast<int16_t>(this->read16_(ATM90E36_REGISTER_PANGLE + phase));
    return (float)val / 10.0f;
}

float ATM90E36Component::get_phase_peak_current_(uint8_t phase) {
  int16_t val = (float) this->read16_(ATM90E36_REGISTER_IPEAK + phase);
  return (float) (val);
}

float ATM90E36Component::get_frequency_() {
  const uint16_t freq = this->read16_(ATM90E36_REGISTER_FREQ);
  return (float) freq / 100;
}

float ATM90E36Component::get_chip_temperature_() {
  const uint16_t ctemp = this->read16_(ATM90E36_REGISTER_TEMP);
  return (float) ctemp;
}

uint16_t ATM90E36Component::calibrate_voltage_offset_phase(uint8_t phase) {
  const uint8_t num_reads = 5;
  uint64_t total_value = 0;
  for (int i = 0; i < num_reads; ++i) {
    const uint32_t measurement_value = read32_(ATM90E36_REGISTER_URMS + phase, ATM90E36_REGISTER_URMSLSB + phase);
    total_value += measurement_value;
  }
  const uint32_t average_value = total_value / num_reads;
  const uint32_t shifted_value = average_value >> 7;
  const uint32_t voltage_offset = ~shifted_value + 1;
  return voltage_offset & 0xFFFF;  // Obtener los 16 bits inferiores
}

uint16_t ATM90E36Component::calibrate_current_offset_phase(uint8_t phase) {
  const uint8_t num_reads = 5;
  uint64_t total_value = 0;
  for (int i = 0; i < num_reads; ++i) {
    const uint32_t measurement_value = read32_(ATM90E36_REGISTER_IRMS + phase, ATM90E36_REGISTER_IRMSLSB + phase);
    total_value += measurement_value;
  }
  const uint32_t average_value = total_value / num_reads;
  const uint32_t current_offset = ~average_value + 1;
  return current_offset & 0xFFFF;  // obtener los 16 bits inferiores
}

}  // namespace atm90e36
}  // namespace esphome
