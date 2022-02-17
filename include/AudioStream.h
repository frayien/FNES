#ifndef FNES_AUDIOSTREAM
#define FNES_AUDIOSTREAM

#include <SFML/Audio.hpp>
#include <cstdint>
#include <cmath>

class AudioStream : public sf::SoundStream
{
public:
    AudioStream();
    virtual ~AudioStream();

    virtual bool onGetData(Chunk& data) override;
    virtual void onSeek(sf::Time timeOffset) override;


    inline void push(int16_t sample) 
    {
        buffer[write_buffer_pos] = sample;
        write_buffer_pos++;
        write_buffer_pos %= 4096;
    }

    inline bool needSample() { return (4096 + write_buffer_pos - read_buffer_pos)%4096 < 2048; }

private:
    int16_t sample[512] { 0 };

    int16_t buffer[4096] { 0 };
    uint32_t read_buffer_pos = 0;
    uint32_t write_buffer_pos = 0;
};

#endif // FNES_AUDIOSTREAM
