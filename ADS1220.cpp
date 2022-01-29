#include "ADS1220.h"


void ADS1220::begin()
{
	SPI.begin();
	pinMode(m_rdy_pin, INPUT);
	digitalWrite(m_cs_pin, HIGH);
	pinMode(m_cs_pin, OUTPUT);
}

bool ADS1220::reset()
{
	wr_cmd(ads_reset);
	delay(1);
	// Verify that all registers have default values
	bool ok = true;
	for (int i = 0; i < 4; ++i) {
		m_reg[i] = rd_reg(i);
		if (m_reg[i] != 0)
			ok = false;
	}
	return ok;
}

bool ADS1220::probe()
{
	uint8_t const reg0 = m_reg[0] ^ 1; // Invert PGA bypass bit
	wr_reg(0, reg0);
	bool ok = (reg0 == rd_reg(0));
	wr_reg(0, m_reg[0]);
	return ok && (m_reg[0] == rd_reg(0));
}

void ADS1220::wr_cmd(uint8_t cmd)
{
	tx_begin();
	SPI.transfer(cmd);
	tx_end();
}

void ADS1220::wr_reg(uint8_t reg, uint8_t val)
{
	uint8_t cmd[] = {(uint8_t)(ads_wreg | (reg << 2)), val};
	tx_begin();
	SPI.transfer(cmd, 2);
	tx_end();
}

uint8_t ADS1220::rd_reg(uint8_t reg)
{
	uint8_t cmd[] = {(uint8_t)(ads_rreg | (reg << 2)), 0};
	tx_begin();
	SPI.transfer(cmd, 2);
	tx_end();
	return cmd[1];
}

void ADS1220::tx_begin()
{
	digitalWrite(m_cs_pin, LOW);
	SPI.beginTransaction(m_spi_settings);
}

void ADS1220::tx_end()
{
	SPI.endTransaction();
	digitalWrite(m_cs_pin, HIGH);
}

bool ADS1220::wait_data(unsigned usec_tout)
{
	if (!digitalRead(m_rdy_pin))
		return true;

	uint32_t const start = micros();
	do {
		if (!digitalRead(m_rdy_pin))
			return true;
	} while ((micros() - start) <= usec_tout);

	return false;
}

int32_t ADS1220::rd_data()
{
	uint8_t buff[3] = {0};
	tx_begin();
	SPI.transfer(buff, 3);
	tx_end();
	int32_t data = ((int32_t)buff[0] << 16) | ((int32_t)buff[1] << 8) | buff[2];
	return data & (1ULL << 23) ? data - (1ULL << 24) : data;
}

void ADS1220::set_input_mode(ads1220_inp_mode_t mode, ads1220_gain_t gain, bool no_pga)
{
	m_reg[0] = (mode << 4) | (gain << 1) | (no_pga ? 1 : 0);
	wr_reg(0, m_reg[0]);
}

void ADS1220::set_conv_mode(ads1220_drate_t drate, ads1220_drate_mode_t mode, bool cont, bool BCS_on)
{
	m_reg[1] = (drate << 5) | (mode << 3) | (cont ? 1 << 2 : 0) | (BCS_on ? 1 : 0);
	// No support for temperature sensor mode
	wr_reg(1, m_reg[1]);
}

void ADS1220::set_idac_mode(ads1220_idac_curr_t curr, ads1220_idac_mode_t mode1, ads1220_idac_mode_t mode2)
{
	m_reg[2] &= ~7;
	m_reg[2] |= curr;
	wr_reg(2, m_reg[2]);
	m_reg[3] &= 3;
	m_reg[3] |= mode1 << 5;
	m_reg[3] |= mode2 << 2;
	wr_reg(3, m_reg[3]);
}
