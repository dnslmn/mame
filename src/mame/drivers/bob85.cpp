// license:BSD-3-Clause
// copyright-holders:Miodrag Milanovic
/***************************************************************************

        BOB85 driver by Miodrag Milanovic

        2008-05-24 Preliminary driver.
        2009-05-12 Skeleton driver.
        2013-06-02 Working driver.

Pasting:
        0-F : as is
        NEXT : ^
        SMEM : -
        GO : X

Test Paste:
        -0600^11^22^33^44^55^66^77^88^99^--0600^
        Now press up-arrow to confirm the data has been entered.


****************************************************************************/

#include "emu.h"
#include "cpu/i8085/i8085.h"
#include "imagedev/cassette.h"
#include "bob85.lh"


class bob85_state : public driver_device
{
public:
	bob85_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_cass(*this, "cassette")
		, m_line0(*this, "LINE0")
		, m_line1(*this, "LINE1")
		, m_line2(*this, "LINE2")
		, m_digits(*this, "digit%u", 0U)
		{ }

	void bob85(machine_config &config);
	void bob85_io(address_map &map);
	void bob85_mem(address_map &map);
	DECLARE_READ8_MEMBER(bob85_keyboard_r);
	DECLARE_WRITE8_MEMBER(bob85_7seg_w);
	DECLARE_WRITE_LINE_MEMBER(sod_w);
	DECLARE_READ_LINE_MEMBER(sid_r);

private:
	uint8_t m_prev_key;
	uint8_t m_count_key;
	virtual void machine_reset() override;
	virtual void machine_start() override { m_digits.resolve(); }
	required_device<i8085a_cpu_device> m_maincpu;
	required_device<cassette_image_device> m_cass;
	required_ioport m_line0;
	required_ioport m_line1;
	required_ioport m_line2;
	output_finder<6> m_digits;
};



READ8_MEMBER(bob85_state::bob85_keyboard_r)
{
	uint8_t retVal = 0;
	uint8_t line0 = m_line0->read();
	uint8_t line1 = m_line1->read();
	uint8_t line2 = m_line2->read();

	if (line0)
	{
		switch(line0)
		{
			case 0x01 : retVal = 0x80; break;
			case 0x02 : retVal = 0x81; break;
			case 0x04 : retVal = 0x82; break;
			case 0x08 : retVal = 0x83; break;
			case 0x10 : retVal = 0x84; break;
			case 0x20 : retVal = 0x85; break;
			case 0x40 : retVal = 0x86; break;
			case 0x80 : retVal = 0x87; break;
			default : break;
		}
	}

	if (line1)
	{
		switch(line1)
		{
			case 0x01 : retVal = 0x88; break;
			case 0x02 : retVal = 0x89; break;
			case 0x04 : retVal = 0x8A; break;
			case 0x08 : retVal = 0x8B; break;
			case 0x10 : retVal = 0x8C; break;
			case 0x20 : retVal = 0x8D; break;
			case 0x40 : retVal = 0x8E; break;
			case 0x80 : retVal = 0x8F; break;
			default : break;
		}
	}

	if (line2)
	{
		switch(line2)
		{
			case 0x01 : retVal |= 0x90; break;
			case 0x02 : retVal |= 0xA0; break;
			case 0x04 : retVal |= 0xB0; break;
			case 0x08 : retVal |= 0xC0; break;
			case 0x10 : retVal |= 0xD0; break;
			case 0x20 : retVal |= 0xE0; break;
			default : break;
		}
	}

	if (retVal != m_prev_key)
	{
		m_prev_key = retVal;
		m_count_key = 0;
		return retVal;
	}
	else
	{
		if (m_count_key <1)
		{
			m_count_key++;
			return retVal;
		}
		else
			return 0;
	}
}

WRITE8_MEMBER(bob85_state::bob85_7seg_w)
{
	m_digits[offset] = bitswap<8>( data,3,2,1,0,7,6,5,4 );
}

void bob85_state::bob85_mem(address_map &map)
{
	map.unmap_value_high();
	map(0x0000, 0x02ff).rom();
	map(0x0600, 0x09ff).ram();
}

void bob85_state::bob85_io(address_map &map)
{
	map.unmap_value_high();
	map(0x0a, 0x0a).r(FUNC(bob85_state::bob85_keyboard_r));
	map(0x0a, 0x0f).w(FUNC(bob85_state::bob85_7seg_w));
}

/* Input ports */
static INPUT_PORTS_START( bob85 )
	PORT_START("LINE0")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_0) PORT_CHAR('0')
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_1) PORT_CHAR('1')
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_2) PORT_CHAR('2')
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_3) PORT_CHAR('3')
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_4) PORT_CHAR('4')
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_5) PORT_CHAR('5')
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_6) PORT_CHAR('6')
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_7) PORT_CHAR('7')

	PORT_START("LINE1")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_8) PORT_CHAR('8')
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_9) PORT_CHAR('9')
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_A) PORT_CHAR('A')
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_B) PORT_CHAR('B')
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_C) PORT_CHAR('C')
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_D) PORT_CHAR('D')
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_E) PORT_CHAR('E')
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_F) PORT_CHAR('F')

	PORT_START("LINE2")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("GO") PORT_CODE(KEYCODE_X) PORT_CHAR('X')
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("SMEM") PORT_CODE(KEYCODE_MINUS) PORT_CHAR('-')
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("REC") PORT_CODE(KEYCODE_R) PORT_CHAR('R')
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("VEK1") PORT_CODE(KEYCODE_Q) PORT_CHAR('Q')
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("VEK2") PORT_CODE(KEYCODE_W) PORT_CHAR('W')
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("NEXT") PORT_CODE(KEYCODE_UP) PORT_CHAR('^')
	PORT_BIT(0xC0, IP_ACTIVE_HIGH, IPT_UNUSED)
INPUT_PORTS_END

void bob85_state::machine_reset()
{
}

WRITE_LINE_MEMBER( bob85_state::sod_w )
{
	m_cass->output(state ? +1.0 : -1.0);
}

READ_LINE_MEMBER( bob85_state::sid_r )
{
	return m_cass->input() > 0.0;
}

void bob85_state::bob85(machine_config &config)
{
	/* basic machine hardware */
	I8085A(config, m_maincpu, XTAL(5'000'000));
	m_maincpu->set_addrmap(AS_PROGRAM, &bob85_state::bob85_mem);
	m_maincpu->set_addrmap(AS_IO, &bob85_state::bob85_io);
	m_maincpu->in_sid_func().set(FUNC(bob85_state::sid_r));
	m_maincpu->out_sod_func().set(FUNC(bob85_state::sod_w));

	/* video hardware */
	config.set_default_layout(layout_bob85);

	// devices
	CASSETTE(config, m_cass).set_default_state((cassette_state)(CASSETTE_STOPPED | CASSETTE_MOTOR_ENABLED | CASSETTE_SPEAKER_MUTED));
}

/* ROM definition */
ROM_START( bob85 )
	ROM_REGION( 0x600, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "bob85.rom", 0x0000, 0x0300, BAD_DUMP CRC(adde33a8) SHA1(00f26dd0c52005e7705e6cc9cb11a20e572682c6) ) // should be 6 separate 74S287's (256x4)
ROM_END

/* Driver */

//    YEAR  NAME   PARENT  COMPAT  MACHINE  INPUT  CLASS        INIT        COMPANY             FULLNAME  FLAGS
COMP( 1984, bob85, 0,      0,      bob85,   bob85, bob85_state, empty_init, "Josef Kratochvil", "BOB-85", MACHINE_NO_SOUND_HW)
