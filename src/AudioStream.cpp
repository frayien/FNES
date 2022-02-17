#include "AudioStream.h"

AudioStream::AudioStream()
{
    initialize(1, 44100);
   /* for(int i = 0; i<512; ++i)
    {
        float time = ((float) i) / 44100.0f;
        sample[i] = (int) 32767.0f * std::sin(time * 440.0f * 2.0f * 3.14159f);
    }*/
}

AudioStream::~AudioStream()
{
}

bool AudioStream::onGetData(Chunk& data)
{
    for(int i = 0; i<512; ++i)
    {
        sample[i] = buffer[read_buffer_pos%4096];
        read_buffer_pos++;
    }
    read_buffer_pos %= 4096;

    data.samples = sample;
    data.sampleCount = 512;
    return true;
}
void AudioStream::onSeek(sf::Time timeOffset)
{

}