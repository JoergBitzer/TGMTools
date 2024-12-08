#include "WaveOut.h"
#include <memory.h>


WaveOut::WaveOut(const std::string &filename, int sampleRate, int channels, AudioFormat format)
: m_filename(filename), m_sampleRate(sampleRate), m_channels(channels), m_format(format), m_isOpen(false)
{
    setAudioFormat(format);
    m_dataSize = 0;
    updateHeader();
}

void WaveOut::write_file(float *data, int size)
{
    if (m_channels != 1)
    {
        // error
        return;
    }
    m_file.open(m_filename, std::ios::binary | std::ios::out);
    if (m_file.good())
    {
        m_isOpen = true;
        m_dataSize = 0;
        writeHeader();
        for (int cc = 0; cc < size; cc++)
        {
            writeOneValue(data[cc]);
        }
        adjustDataSize(size);
        updateHeader();
        writeDataSize();
        m_file.close();
        m_isOpen = false;
    }
}

void WaveOut::write_file(float **data, int size, int channels)
{
    if (m_channels != channels)
    {
        // error
        return;
    }
    m_file.open(m_filename, std::ios::binary | std::ios::out);
    if (m_file.good())
    {
        m_isOpen = true;
        writeHeader();
        for (int cc = 0; cc < size; cc++)
        {
            for (int ch = 0; ch < channels; ch++)
            {
                writeOneValue(data[ch][cc]);
            }
        }
        adjustDataSize(size);
        updateHeader();
        writeDataSize();
        m_file.close();
        m_isOpen = false;
    }
}

void WaveOut::write_file(const std::vector<float> &data)
{
    if (m_channels != 1)
    {
        // error
        return;
    }
    m_file.open(m_filename, std::ios::binary | std::ios::out);
    if (m_file.good())
    {
        m_isOpen = true;
        m_dataSize = 0;
        writeHeader();

        for (auto &d : data)
        {
            writeOneValue(d);
        }
        adjustDataSize(data.size());
        updateHeader();
        writeDataSize();
        m_file.close();
        m_isOpen = false;
    }
    
}

void WaveOut::write_file(const std::vector<std::vector<float>> &data)
{
    if (m_channels != data.size())
    {
        // error
        return;
    }
    m_file.open(m_filename, std::ios::binary | std::ios::out);
    if (m_file.good())
    {
        m_isOpen = true;
        writeHeader();
        for (auto cc = 0; cc < data.at(0).size(); cc++)
        {
            for (auto ch = 0; ch < data.size(); ch++)
            {
                writeOneValue(data.at(ch).at(cc));
            }
        }
        adjustDataSize(data.at(0).size());
        updateHeader();
        writeDataSize();
        m_file.close();
        m_isOpen = false;
    }
}

void WaveOut::open()
{
    m_file.open(m_filename, std::ios::binary | std::ios::out);
    if (m_file.good())
    {
        m_isOpen = true;
        m_dataSize = 0;
        writeHeader();
    }
}

void WaveOut::write(float *data, int size)
{
    if (m_file.good())
    {
        for (int cc = 0; cc < size; cc++)
        {
            writeOneValue(data[cc]);
        }
        adjustDataSize(size);
        updateHeader();
        writeDataSize();

    }
}

void WaveOut::write(float **data, int size, int channels)
{
    if (m_file.good())
    {
        for (int cc = 0; cc < size; cc++)
        {
            for (int ch = 0; ch < channels; ch++)
            {
                writeOneValue(data[ch][cc]);
            }
        }
        adjustDataSize(size);
        updateHeader();
        writeDataSize();
    }
}

void WaveOut::write(const std::vector<float> &data)
{
    if (m_file.good())
    {
        writeHeader();

        for (auto &d : data)
        {
            writeOneValue(d);
        }
        adjustDataSize(data.size());
        updateHeader();
        writeDataSize();
    }    
}
void WaveOut::write(const std::vector<std::vector<float> > &data)
{
    if (m_file.good())
    {
        writeHeader();
        for (auto cc = 0; cc < data.at(0).size(); cc++)
        {
            for (auto ch = 0; ch < data.size(); ch++)
            {
                writeOneValue(data.at(ch).at(cc));
            }
        }
        adjustDataSize(data.at(0).size());
        updateHeader();
        writeDataSize();
    }
}

void WaveOut::close()
{
    if (m_file.good())
    {
        updateHeader();
        writeDataSize();
        m_file.close();
        m_isOpen = false;
    }
}

void WaveOut::setAudioFormat(AudioFormat format)
{
    if (!m_isOpen)
    {
        m_format = format;
        if (m_format == AudioFormat::INT16 || m_format == AudioFormat::INT24 || m_format == AudioFormat::INT32)
        {
            m_officialFileFormat = 1; // PCM
            if (m_format == AudioFormat::INT16)
            {
                m_bitsPerSample = 16;
            }
            else if (m_format == AudioFormat::INT24)
            {
                m_bitsPerSample = 24;
            }
            else if (m_format == AudioFormat::INT32)
            {
                m_bitsPerSample = 32;
            }
            m_Qstep = 1.0 / (1 << (m_bitsPerSample - 1));
        }
        else if (m_format == AudioFormat::FLOAT32 || m_format == AudioFormat::FLOAT64)
        {
            m_officialFileFormat = 3; // IEEE float
            if (m_format == AudioFormat::FLOAT32)
            {
                m_bitsPerSample = 32;
            }
            else if (m_format == AudioFormat::FLOAT64)
            {
                m_bitsPerSample = 64;
            }
        }
        updateHeader();
    }
}

void WaveOut::writeHeader()
{
    int currentPos = m_file.tellp();
    m_file.seekp(0);
    m_file.write(reinterpret_cast<char*>(&m_header), sizeof(m_header));
    if (currentPos != 0)
        m_file.seekp(currentPos);
}

void WaveOut::writeDataSize()
{
    int currentPos = m_file.tellp();
    m_file.seekp(4);
    m_file.write(reinterpret_cast<char*>(&m_header.chunk_size), sizeof(m_header.chunk_size));
    m_file.seekp(40);
    m_file.write(reinterpret_cast<char*>(&m_header.datachunk_size), sizeof(m_header.datachunk_size));
    m_file.seekp(currentPos);
}

void WaveOut::updateHeader()
{
    memcpy(m_header.riffchunk_id, "RIFF", 4);
    m_header.chunk_size = m_dataSize + 36; // is 36 correct?
    memcpy(m_header.waveformat, "WAVE", 4);
    memcpy(m_header.fmtchunk_id, "fmt ", 4);
    m_header.fmtchunk_size = 16;
    m_header.audio_format = m_officialFileFormat;
    m_header.num_channels = m_channels;
    m_header.sample_rate = m_sampleRate;
    m_header.bits_per_sample = m_bitsPerSample;
    m_header.byte_rate = m_sampleRate * m_channels * m_bitsPerSample / 8;
    m_header.block_align = m_channels * m_bitsPerSample / 8;
    memcpy(m_header.datachunk_id, "data", 4);
    m_header.datachunk_size = m_dataSize;
 
}

void normalizeAudio(std::vector<float> &data, float TargetMaxVal_dB)
{
    float max_val = 0.0;
    for (auto &d : data)
    {
        float abs_d = std::abs(d);
        if (abs_d > max_val)
            max_val = abs_d;
    }
    float scale = 1.0;
    if (max_val > 0.0)
    {
        scale = 1.0 / max_val;
        float max_val_dB_lin = std::pow(10, TargetMaxVal_dB / 20.0);
        scale *= max_val_dB_lin;
    
        for (auto &d : data)
        {
            d *= scale;
        }
    }
}
void normalizeAudio(std::vector<std::vector<float> > &data, float TargetMaxVal_dB)
{
    float max_val = 0.0;
    for (auto &d : data)
    {
        for (auto &dd : d)
        {
            float abs_d = std::abs(dd);
            if (abs_d > max_val)
                max_val = abs_d;
        }
    }
    float scale = 1.0;
    if (max_val > 0.0)
    {
        scale = 1.0 / max_val;
        float max_val_dB_lin = std::pow(10, TargetMaxVal_dB / 20.0);
        scale *= max_val_dB_lin;
    
        for (auto &d : data)
        {
            for (auto &dd : d)
            {
                dd *= scale;
            }
        }
    }
}