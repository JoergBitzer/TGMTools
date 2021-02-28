#pragma once

#include <vector>
#include <JuceHeader.h>

class SynchronBlockProcessor
{
public:
    SynchronBlockProcessor();
    void preparetoProcess(int channels, int maxinputlen);
    int processBlock(std::vector <std::vector<float>>& data, juce::MidiBuffer& midiMessages);
    void processBlock(juce::AudioBuffer<float>& data, juce::MidiBuffer& midiMessages);

    int setDesiredBlockSizeSamples(int desiredSize);
    int setDesiredBlockSizeMiliSeconds(float desiredSize_ms, float fs);
    virtual int processSynchronBlock(std::vector <std::vector<float>>&, juce::MidiBuffer& midiMessages) = 0;
    int getDelay();
private:
    int m_NrOfChannels;
    int m_OutBlockSize;
    int m_maxInputSize;
    int m_OutCounter;
    int m_InCounter;

    std::vector <std::vector<float>> m_mem;
    std::vector <std::vector<float>> m_block;
    MidiBuffer m_mididata;
    int m_pastSamples;



};
