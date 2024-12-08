// class to write wave files
// the implementation is intended to be simple and easy to use and not optimized for speed
// differences to other implementations are:
// support for vector<float>, vector<vector<float> > and float* as well as float** data
// ToDo:- 
// speed up by using block oriented writing (converting to byte fields first, write a block second) 
// Perhaps add JUCE AudioBuffer support


#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <cmath>
class WaveOut
{
public:
    enum class AudioFormat : uint16_t {
        INT16 = 1,
        INT24 = 2,
        INT32 = 3,
        FLOAT32 = 4,
        FLOAT64 = 5,
    };

    struct WavHeader {
        char riffchunk_id[4];
        uint32_t chunk_size;
        char waveformat[4];
        char fmtchunk_id[4];
        uint32_t fmtchunk_size;
        uint16_t audio_format;
        uint16_t num_channels;
        uint32_t sample_rate;
        uint32_t byte_rate;
        uint16_t block_align;
        uint16_t bits_per_sample;
        char datachunk_id[4];
        uint32_t datachunk_size;
    };

    WaveOut(const std::string &filename, int sampleRate = 44100, int channels = 1, AudioFormat format = AudioFormat::INT16);
    ~WaveOut(){};

    void write_file(float *data, int size);
    void write_file(float **data, int size, int channels);
    void write_file(const std::vector<float> &data);
    void write_file(const std::vector<std::vector<float> > &data);
    inline void writeOneValue(float fvalue)
    {
        // saturate for integer formats 
        if (m_format == AudioFormat::INT16 || m_format == AudioFormat::INT24 || m_format == AudioFormat::INT32)
        {
            if (fvalue >= 0.9999998) // this is just an approximation (1- Q)
                fvalue = 0.9999998;
            if (fvalue < -1.0)
                fvalue = -1.0;
        }
        if (m_format == AudioFormat::INT16)
        {
            int16_t val = static_cast<int16_t>(fvalue * 32767);
            m_file.write(reinterpret_cast<char*>(&val), sizeof(val));
        }
        else if (m_format == AudioFormat::INT24)
        {
            int32_t val = static_cast<int32_t>(fvalue * 8388607);
            char g[3];
            g[0] = val  & 0x000000ff;
            g[1] = (val & 0x0000ff00) >> 8;
            g[2] = (val & 0x00ff0000) >> 16;
            m_file.write(g, 3);
        }

        else if (m_format == AudioFormat::INT32)
        {
            int32_t val = static_cast<int32_t>(fvalue * 2147483647);
            m_file.write(reinterpret_cast<char*>(&val), sizeof(val));
        }
        else if (m_format == AudioFormat::FLOAT32)
        {
            m_file.write(reinterpret_cast<char*>(&fvalue), sizeof(fvalue));
        }
        else if (m_format == AudioFormat::FLOAT64)
        {
            double dd = fvalue;
            m_file.write(reinterpret_cast<char*>(&dd), sizeof(dd));
        }
    }
    inline void adjustDataSize(int size)
    {
        m_dataSize += size * m_channels * m_bitsPerSample / 8;
    }
    void open();
    void write(float *data, int size);
    void write(float **data, int size, int channels);
    void write(const std::vector<float> &data);
    void write(const std::vector<std::vector<float> > &data);

    void close();
    //bool isOpen();
    int getSampleRate(){return m_sampleRate;};
    int getBitsPerSample(){return m_bitsPerSample;};
    int getChannels(){return m_channels;};

    void setSampleRate(int sampleRate){if (!m_isOpen) m_sampleRate = sampleRate;};
    void setChannels(int channels){if (!m_isOpen) m_channels = channels;};
    void setFileName(const std::string &filename){if (!m_isOpen) m_filename = filename;};
    void setAudioFormat(AudioFormat format);
private:
    std::ofstream m_file;
    std::string m_filename;
    int m_sampleRate;
    int m_bitsPerSample;
    int m_channels;
    int m_dataSize;
    AudioFormat m_format;
    bool m_isOpen;
    void writeHeader();
    void writeDataSize();
    WavHeader m_header;
    void updateHeader();
    int16_t m_officialFileFormat;
    float m_Qstep;
};

void normalizeAudio(std::vector<float> &data, float max_val_dB = 0.0);
void normalizeAudio(std::vector<std::vector<float> > &data, float TargetMaxVal_dB);