#pragma once

#include <Arduino.h>
#include <SPI.h>

#define ADS_SPI_SETTINGS 8000000, MSBFIRST, SPI_MODE1

typedef enum {
	ads_down  = 2,
	ads_reset = 6,
	ads_start = 8,
	ads_rdata = 0x10,
	ads_rreg  = 0x20,
	ads_wreg  = 0x40,
} ads1220_cmd_t;

typedef enum {
	ads_idac_off = 0,
	ads_idac_10uA,
	ads_idac_50uA,
	ads_idac_100uA,
	ads_idac_250uA,
	ads_idac_500uA,
	ads_idac_1000uA,
	ads_idac_1500uA
} ads1220_idac_curr_t;

typedef enum {
	ads_idac_disable = 0,
	ads_idac_IN0,
	ads_idac_IN1,
	ads_idac_IN2,
	ads_idac_IN3,
	ads_idac_REFP0,
	ads_idac_REFN0
} ads1220_idac_mode_t;

typedef enum {
	ads_inp_01 = 0, // AINP = AIN0, AINN = AIN1
	ads_inp_02,     // AINP = AIN0, AINN = AIN2
	ads_inp_03,     // AINP = AIN0, AINN = AIN3
	// See datasheet for other modes
} ads1220_inp_mode_t;

typedef enum {
	// Gains 1, 2, and 4 can be used without the PGA. In this case, gain is obtained by
	// a switched-capacitor structure.
	ads_gain1 = 0, // (default)
	ads_gain2,
	ads_gain4,
	ads_gain8,
	ads_gain16,
	ads_gain32,
	ads_gain64,
	ads_gain128
} ads1220_gain_t;

typedef enum {
	ads_20sps = 0,
	ads_45sps,
	ads_90sps,
	ads_175sps,
	ads_330sps,
	ads_600sps,
	ads_1000sps
} ads1220_drate_t;

typedef enum {
	ads_dr_normal = 0, // Normal mode (256-kHz modulator clock, default)
	ads_dr_duty_cycle, // Duty-cycle mode /4 rate
	ads_dr_turbo       // Turbo mode 2x rate
} ads1220_drate_mode_t;

class ADS1220 {
public:
	ADS1220(uint8_t cs_pin, uint8_t rdy_pin)
		: m_cs_pin(cs_pin), m_rdy_pin(rdy_pin)
		, m_spi_settings(ADS_SPI_SETTINGS)
		{}

	void begin();
	bool reset();
	bool probe();
	void start() { wr_cmd(ads_start); }

	void set_input_mode(ads1220_inp_mode_t mode, ads1220_gain_t gain, bool no_pga);
	void set_conv_mode(ads1220_drate_t drate, ads1220_drate_mode_t mode, bool cont, bool BCS_on = false);
	void set_idac_mode(ads1220_idac_curr_t curr, ads1220_idac_mode_t mode1, ads1220_idac_mode_t mode2);

	bool wait_data(unsigned usec_tout);
	int32_t rd_data();

protected:
	void tx_begin();
	void tx_end();

	void    wr_cmd(uint8_t cmd);
	void    wr_reg(uint8_t reg, uint8_t val);
	uint8_t rd_reg(uint8_t reg);

	uint8_t     m_cs_pin;
	uint8_t     m_rdy_pin;
	SPISettings m_spi_settings;
	
	uint8_t m_reg[4];
};
