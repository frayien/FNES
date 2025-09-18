#pragma once

#include "AudioStream.hpp"
#include "IBus.hpp"

#include <cstdint>

class Apu2A03
{
public:
    static float fsin(float t)
    {
        float j = t * 0.15915;
        j = j - (int) j;
        return 20.785 * j * (j - 0.5) * (j - 1.0f);
    }

public:
    struct envelope
    {
    public:
        bool start_flag = false;
        bool loop_flag = false;
        bool constant_volume_flag = false;
        uint8_t divider_reload = 0x0;

    private:
        uint8_t divider = 0x0;
        uint8_t decay_level_counter = 0x00;

        void clockDecayLevel()
        {
            if(decay_level_counter != 0)
            {
                decay_level_counter--;
            }
            else
            {
                if(loop_flag)
                {
                    decay_level_counter = 15;
                }
            }
        }

        void clockDivider()
        {
            if(divider == 0)
            {
                divider = divider_reload;
                clockDecayLevel();
            }
            else
            {
                divider--;
            }
        }

    public:
        // clocked every quarter frame
        void clock()
        {
            if(!start_flag)
            {
                clockDivider();
            }
            else
            {
                start_flag = false;
                decay_level_counter = 15;
                divider = divider_reload;
            }
        }

        void output(float& out)
        {
            if(constant_volume_flag)
            {
                out = ((float) divider_reload)/15.0;
            }
            else
            {
                out = ((float) decay_level_counter)/15.0;
            }
        }
    };

    struct pulse
    {
        bool halt_flag = false;
        uint8_t length_counter = 0x000;
        uint8_t length_counter_reload = 0x000;

        bool enabled = false;

        union
        {
            uint16_t reload = 0x0000;
            struct
            {
                uint8_t reload_lo : 8;
                uint8_t reload_hi : 8;
            };
        };

        uint16_t timer = 0x0000;

        float frequency = 0;
        float dutycycle = 0;
        const float pi = 3.1415926535f;
        const float harmonics = 20;

        // clock every half frame
        void clock()
        {
            if(!(length_counter == 0 || halt_flag))
            {
                length_counter--;
            }
        }

        void update()
        {
            frequency = 1789773.0 / (16.0 * (float)(timer + 1));
        }

        void sample(float &out, float time)
        {
            if(!enabled || length_counter == 0)
            {
                out = 0.0;
            }
            else
            {
                float a = 0;
                float b = 0;
                float p = dutycycle * 2.0 * pi;

                for(float n = 1; n<harmonics; ++n)
                {
                    float c = n * frequency * 2.0 * pi * time;
                    a += -fsin(c) / n;
                    b += -fsin(c - p * n) / n;
                }

                out *= (2.0 / pi) * (a -b);
            }

        }
    };

    struct sweep
    {
    public:
        bool reload_flag = false;
        bool enabled = false;
        bool negate = false;
        uint8_t divider_period = 0x00;
        uint8_t shift = 0x00;

        bool force_mute = false;

    private:
        uint8_t divider_counter = 0x00;

    public:
        // clock every half frame
        void clock(pulse& target, bool is_pulse_1)
        {
            if(divider_counter == 0 && enabled && !force_mute && shift != 0)
            {
                uint16_t barrel = target.timer;
                for(int i = 0; i < shift; ++i) barrel >>= 1;
                if(negate)
                {
                    target.timer = target.timer - barrel - (is_pulse_1);
                }
                else
                {
                    target.timer = target.timer + barrel;
                }
            }
            if(divider_counter == 0 || reload_flag)
            {
                divider_counter = divider_period;
                reload_flag = false;
            }
            else
            {
                divider_counter--;
            }

            if(target.timer > 0x7FF || target.timer < 8 || (target.timer > 0x400 && shift == 0 && !negate && enabled))
            {
                force_mute = true;
            }
            else
            {
                force_mute = false;
            }
        }

        void output(float& out)
        {
            if(force_mute)
            {
                out = 0.0;
            }
        }
    };

    struct triangle
    {
        bool length_counter_halt = false;
        uint8_t linear_counter_reload = 0x000;
        uint8_t length_counter_reload = 0x000;

        union
        {
            uint16_t reload = 0x0000;
            struct
            {
                uint8_t reload_lo : 8;
                uint8_t reload_hi : 8;
            };
        };

        uint16_t timer = 0x0000;

        bool enabled = false;
        bool linear_counter_reload_flag = false;


        uint8_t linear_counter = 0x000;
        uint8_t length_counter = 0x000;


        float frequency = 0;
        const float pi = 3.1415926535f;
        const float harmonics = 10;

        void update()
        {
            frequency = 1789773.0 / (32.0 * (float)(timer + 1));
        }

        // clock quarter frame
        void clockQuarter()
        {
            // linear counter clock
            if(linear_counter_reload_flag)
            {
                linear_counter = linear_counter_reload;
            }
            else if(linear_counter != 0)
            {
                linear_counter--;
            }

            if(!length_counter_halt)
            {
                linear_counter_reload_flag = false;
            }
        }

        void clockHalf()
        {
            if(!(length_counter == 0 || length_counter_halt))
            {
                length_counter--;
            }
        }

        void sample(float& out, float time)
        {
            if(linear_counter != 0 && length_counter != 0 && enabled)
            {
                float a = 0;
                float one = 1.0;

                for(float n = 0; n<harmonics; ++n)
                {
                    const float tn = 2.0*n+1.0;
                    float c = tn * frequency * 2.0 * pi * time;
                    a += one * fsin(c) / tn / tn;
                    one *= -1.0;
                }

                out = (2.0 / pi) * a;
            }
            else
            {
                out = 0.0;
            }
        }
    };

    struct noise
    {
        bool length_counter_halt = false;
        bool mode_flag = false;

        uint16_t timer_period = 0x0000;

        uint8_t length_counter_reload = 0x000;

        uint8_t length_counter = 0x000;
        bool enabled = false;

        uint16_t shift_register = 1;

        // half frame
        void clockHalf()
        {
            if(!(length_counter == 0 || length_counter_halt))
            {
                length_counter--;
            }
        }

        void update()
        {
            uint16_t feedback = shift_register & 0x01;
            if(mode_flag)
            {
                feedback ^= (shift_register & 0x40) >> 6;
            }
            else
            {
                feedback ^= (shift_register & 0x02) >> 1;
            }

            shift_register >>= 1;

            feedback &= 0x01;
            feedback <<= 14;

            shift_register |= feedback;

        }

        void sample(float& out)
        {
            if((shift_register & 0x01) || length_counter == 0)
            {
                out = 0.0f;
            }
        }
    };

    struct dmc
    {
        bool irq_enabled_flag = false;
        bool loop_flag = false;
        uint16_t rate = 0x0000;
        uint16_t sample_addr = 0x0000;
        uint16_t sample_length = 0x0000;

        bool enabled = false;

        uint8_t output_level = 0x00;

        bool irq = false;

private:
        uint8_t sample_buffer = 0x00;
        uint8_t sample_buffer_size = 0;

        uint16_t sample_addr_counter = 0x0000;
public:
        uint16_t sample_byte_remaining = 0x0000;
private:

        uint16_t rate_couter = 0x0000;

        bool silence = false;

public:
        void update(Apu2A03* apu)
        {
            // output cycle end
            if(sample_buffer_size == 0)
            {
                sample_buffer_size = 8;
                if(sample_byte_remaining > 0)
                {
                    silence = false;
                    sample_buffer = apu->bus->cpuRead(0x8000 | (sample_addr_counter & 0x7FFF));
                    sample_addr_counter++;
                    sample_byte_remaining--;
                }
                else
                {
                    silence = true;
                }

                // Becomes 0
                if(sample_byte_remaining == 0 && enabled)
                {
                    if(loop_flag)
                    {
                        // restart
                        sample_addr_counter = sample_addr;
                        sample_byte_remaining = sample_length;
                    }
                    else
                    {
                        if(irq_enabled_flag)
                        {
                            irq = true;
                        }
                    }
                }
            }

            // clock
            if(enabled)
            {
                if(rate_couter == 0)
                {
                    rate_couter = rate/2;
                    if(!silence)
                    {
                        if(sample_buffer & 0x01)
                        {
                            if(output_level <= 125) output_level += 2;
                        }
                        else
                        {
                            if(output_level >= 2) output_level -= 2;
                        }
                    }

                    sample_buffer >>= 1;
                    sample_buffer_size--;
                }
                else
                {
                    rate_couter--;
                }
            }
        }

        void sample(float& out)
        {
            if(silence)
            {
                out = 0.0f;
            }
            else
            {
                out = ((float) (output_level & 0x7F)) / 127.0f;
            }
        }
    };



public:
    Apu2A03();
    virtual ~Apu2A03();

    inline void connectBus(IBus *_bus) { bus = _bus; }

    void cpuWrite(uint16_t addr, uint8_t data);
    uint8_t cpuRead(uint16_t addr);
    void clock();
    void reset();

    int16_t getSample();
    int16_t getPulse(float time, float freq, float);

    bool irq = false;
    bool cycle_is_even = false;

private:
    IBus* bus = nullptr;

    uint32_t frame_clock_counter = 0;
    uint32_t clock_counter = 0;

    bool quarter_frame_clock = false;
    bool half_frame_clock = false;

    bool frame_counter_mode = 0;
    bool interrupt_inhibit_flag = 0;

    uint8_t clear_irq_delayed = 0xFF;
    uint8_t clear_frame_clock_counter_delayed = 0xFF;

public:
    envelope pulse_1_envelope;
    sweep pulse_1_sweep;
    pulse pulse_1;

    envelope pulse_2_envelope;
    sweep pulse_2_sweep;
    pulse pulse_2;

    triangle triangle;

    envelope noise_envelope;
    noise noise;

    dmc dmc;

private:
    uint32_t offset = 0;
    AudioStream audio_stream;


    uint8_t length_counter_values[0x20]
    {
        10,254, 20,  2, 40,  4, 80,  6, 160,  8, 60, 10, 14, 12, 26, 14,
        12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30
    };

    uint16_t ntsc_timer_period[0x10]
    {
        4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068
    };

    uint16_t ntsc_rate[0x10]
    {
        428, 380, 340, 320, 286, 254, 226, 214, 190, 160, 142, 128, 106,  84,  72,  54
    };
};
