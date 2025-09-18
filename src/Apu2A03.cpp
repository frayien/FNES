#include "Apu2A03.hpp"

Apu2A03::Apu2A03()
{
    audio_stream.play();
}

Apu2A03::~Apu2A03()
{
}

void Apu2A03::cpuWrite(uint16_t addr, uint8_t data)
{
    switch (addr)
    {
    case 0x4000: // Pulse 1, duty & control & volume
        switch ((data & 0xC0) >> 6)
        {
        case 0x00: pulse_1.dutycycle = 0.125; break;
        case 0x01: pulse_1.dutycycle = 0.250; break;
        case 0x02: pulse_1.dutycycle = 0.500; break;
        case 0x03: pulse_1.dutycycle = 0.750; break;
        }
        pulse_1.halt_flag = (data & 0x20) > 0;

        pulse_1_envelope.loop_flag = (data & 0x20) > 0;
        pulse_1_envelope.constant_volume_flag = (data & 0x10) > 0;
        pulse_1_envelope.divider_reload = data & 0x0F;
        break;
    case 0x4001: // Pulse 1, sweep
        pulse_1_sweep.enabled = (data & 0x80) > 0;
        pulse_1_sweep.divider_period  = (data & 0x70) >> 4;
        pulse_1_sweep.negate  = (data & 0x08) > 0;
        pulse_1_sweep.shift   = (data & 0x07);
        pulse_1_sweep.reload_flag = true;
        break;
    case 0x4002: // Pulse 1, timer lo
        pulse_1.reload_lo = data;
        break;
    case 0x4003: // Pulse 1, timer hi & length counter
        pulse_1.reload_hi = data & 0x07;
        pulse_1.timer = pulse_1.reload;
        if(pulse_1.enabled)
        {
            pulse_1.length_counter_reload = length_counter_values[(data & 0xF8) >> 3];
            pulse_1.length_counter = pulse_1.length_counter_reload;
        }
        pulse_1_envelope.start_flag = true;
        break;
    case 0x4004: // Pulse 2, duty & control & volume
        switch ((data & 0xC0) >> 6)
        {
        case 0x00: pulse_2.dutycycle = 0.125; break;
        case 0x01: pulse_2.dutycycle = 0.250; break;
        case 0x02: pulse_2.dutycycle = 0.500; break;
        case 0x03: pulse_2.dutycycle = 0.750; break;
        }
        pulse_2.halt_flag = (data & 0x20) > 0;

        pulse_2_envelope.loop_flag = (data & 0x20) > 0;
        pulse_2_envelope.constant_volume_flag = (data & 0x10) > 0;
        pulse_2_envelope.divider_reload = data & 0x0F;
        break;
    case 0x4005: // Pulse 2, sweep
        pulse_2_sweep.enabled = (data & 0x80) > 0;
        pulse_2_sweep.divider_period  = (data & 0x70) >> 4;
        pulse_2_sweep.negate  = (data & 0x08) > 0;
        pulse_2_sweep.shift   = (data & 0x07);
        pulse_2_sweep.reload_flag = true;
        break;
    case 0x4006: // Pulse 2, timer lo
        pulse_2.reload_lo = data;
        break;
    case 0x4007: // Pulse 2, timer hi & length counter
        pulse_2.reload_hi = data & 0x07;
        pulse_2.timer = pulse_2.reload;
        if(pulse_2.enabled)
        {
            pulse_2.length_counter_reload = length_counter_values[(data & 0xF8) >> 3];
            pulse_2.length_counter = pulse_2.length_counter_reload;
        }
        pulse_2_envelope.start_flag = true;
        break;

    case 0x4008: // Triangle
        triangle.length_counter_halt = (data & 0x80) > 0;
        break;
    case 0x400A: // Triangle
        triangle.reload_lo = data;
        break;
    case 0x400B: // Triangle
        triangle.reload_hi = data & 0x07;
        triangle.timer = triangle.reload;
        if(triangle.enabled)
        {
            triangle.length_counter_reload = length_counter_values[(data & 0xF8) >> 3];
            triangle.length_counter = triangle.length_counter_reload;
        }
        triangle.linear_counter_reload_flag = true;
        break;

    case 0x400C: // Noise
        noise.length_counter_halt = (data & 0x80) > 0;

        noise_envelope.loop_flag = (data & 0x20) > 0;
        noise_envelope.constant_volume_flag = (data & 0x10) > 0;
        noise_envelope.divider_reload = data & 0x0F;
        break;
    case 0x400E: // Noise
        noise.mode_flag = (data & 0x80) > 0;
        noise.timer_period = ntsc_timer_period[data & 0x0F];
        break;
    case 0x400F: // Noise
        if(noise.enabled)
        {
            noise.length_counter_reload = length_counter_values[(data & 0xF8) >> 3];
            noise.length_counter = noise.length_counter_reload;
        }
        noise_envelope.start_flag = true;
        break;

    case 0x4010: // DMC
        dmc.irq_enabled_flag = (data & 0x80) > 0;
        dmc.loop_flag = (data & 0x40) > 0;
        dmc.rate = ntsc_rate[data & 0x0F];
        break;
    case 0x4011: // DMC
        dmc.output_level = data & 0x7F;
        break;
    case 0x4012: // DMC
        dmc.sample_addr = 0xC000 | (data << 6);
        break;
    case 0x4013: // DMC
        dmc.sample_length = (data << 4) | 0x0001;
        break;

    case 0x4015: // All, Channel enable and length counter status
        pulse_1.enabled = data & 0x01;
        if(!pulse_1.enabled) pulse_1.length_counter = 0;

        pulse_2.enabled = (data & 0x02) > 0;
        if(!pulse_2.enabled) pulse_2.length_counter = 0;

        triangle.enabled = (data & 0x04) > 0;
        if(!triangle.enabled) triangle.length_counter = 0;

        noise.enabled = (data & 0x08) > 0;
        if(!noise.enabled) noise.length_counter = 0;

        dmc.enabled = (data & 0x10) > 0;
        if(!dmc.enabled) dmc.sample_byte_remaining = 0;
        dmc.irq = false;

        break;

    case 0x4017: // All, frame counter
        frame_counter_mode = (data & 0x80) > 0;
        interrupt_inhibit_flag = (data & 0x40) > 0;
        if(frame_counter_mode) // 5-step mode
        {
            quarter_frame_clock = true;
            half_frame_clock = true;
        }
        if(interrupt_inhibit_flag)
        {
            irq = false;
        }
        clear_frame_clock_counter_delayed = 1;
        break;
    }
}
uint8_t Apu2A03::cpuRead(uint16_t addr)
{
    uint8_t data = 0x00;

    if(addr == 0x4015) // Read status
    {
        if(pulse_1.length_counter  != 0) data |= 0x01;
        if(pulse_2.length_counter  != 0) data |= 0x02;
        if(triangle.length_counter != 0) data |= 0x04;
        if(noise.length_counter    != 0) data |= 0x08;
        if(dmc.sample_byte_remaining != 0) data |= 0x10;
        if(dmc.irq) data |= 0x40;
        if(irq    ) data |= 0x80;

        clear_irq_delayed = 1;
    }

    return data;
}
void Apu2A03::clock()
{
    // Clock APU twice as fast to handler half APU cycle timings
    if(clock_counter % 3 == 0)
    {
        // Even & Odd (Get & Put) cycles
        cycle_is_even = !cycle_is_even;

        // Get
        if(cycle_is_even)
        {
            if((clear_irq_delayed & 0x80) == 0)
            {
                --clear_irq_delayed;
                if((clear_irq_delayed & 0x80) != 0)
                {
                    irq = false;
                }
            }

            if((clear_frame_clock_counter_delayed & 0x80) == 0)
            {
                --clear_frame_clock_counter_delayed;
                if((clear_frame_clock_counter_delayed & 0x80) != 0)
                {
                    frame_clock_counter = 0;
                }
            }
        }

        // Tick
        frame_clock_counter++;

        // Timings are multiplied by two (half cycles)
        switch(frame_clock_counter)
        {
        case 3728*2+1: // Put (Odd)
            quarter_frame_clock = true;
            break;
        case 7456*2+1: // Put
            quarter_frame_clock = true;
            half_frame_clock = true;
            break;
        case 11185*2+1: // Put
            quarter_frame_clock = true;
            break;
        case 14914*2: // Get
            if(!frame_counter_mode) // 4-step mode
            {
                if(!interrupt_inhibit_flag) irq = true;
            }
            break;
        case 14914*2+1: // Put
            if(!frame_counter_mode) // 4-step mode
            {
                quarter_frame_clock = true;
                half_frame_clock = true;
                if(!interrupt_inhibit_flag) irq = true;
            }
            break;
        case 14915*2: // Get
            if(!frame_counter_mode) // 4-step mode
            {
                frame_clock_counter = 0;
                if(!interrupt_inhibit_flag) irq = true;
            }
            break;
        case 18640*2+1: // Put
            // 5-step mode
            quarter_frame_clock = true;
            half_frame_clock = true;
            break;
        case 18641*2: // Get
            // 5-step mode
            frame_clock_counter = 0;
            break;
        default: break;
        }

        if(quarter_frame_clock)
        {
            pulse_1_envelope.clock();
            pulse_2_envelope.clock();
            noise_envelope.clock();

            triangle.clockQuarter();
        }

        if(half_frame_clock)
        {
            pulse_1_sweep.clock(pulse_1, true);
            pulse_2_sweep.clock(pulse_2, false);

            pulse_1.clock();
            pulse_2.clock();

            triangle.clockHalf();
            noise.clockHalf();
        }

        pulse_1.update();
        pulse_2.update();
        triangle.update();

        noise.update();
        dmc.update(this);

    }

    quarter_frame_clock = false;
    half_frame_clock = false;
    clock_counter++;

    if(audio_stream.needSample())
    {
        audio_stream.push(getSample());
    }
}

void Apu2A03::reset()
{

}

int16_t Apu2A03::getSample()
{
    float time =  ((float) offset++) / 44100.0f;

    float output_pulse_1 = 1.0;
    pulse_1_envelope.output(output_pulse_1);
    pulse_1_sweep.output(output_pulse_1);
    pulse_1.sample(output_pulse_1, time);

    float output_pulse_2 = 1.0;
    pulse_2_envelope.output(output_pulse_2);
    pulse_2_sweep.output(output_pulse_2);
    pulse_2.sample(output_pulse_2, time);

    float output_pulse  = 0.00752 * (output_pulse_1 + output_pulse_2);

    float output_triangle = 1.0f;
    triangle.sample(output_triangle, time);

    float output_noise = 1.0f;
    noise_envelope.output(output_noise);
    noise.sample(output_noise);

    float output_dmc = 1.0f;
    dmc.sample(output_dmc);

    float tnd_out = (0.00851 * output_triangle) + (0.00494 * output_noise) + (0.00335 * output_dmc);

    return (int) 32767.0f * (output_pulse + tnd_out);
}