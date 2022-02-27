/**
 * @file SynchronBlockProcessor.h
 * @author J. Bitzer @ Jade HS, BSD Licence
 * @brief class to rebuffer an JUCE AudioBuffer and MidiMessageQueue of arbitrary length to AudioBuffer of a given length
 * Useful for fft processing or faster parameter updates and modulation
 * Usage: Inherit from this class and override ProcessSynchronBlock
 * @version 2.0
 * @date 2022-02-27
 * 
 * @copyright Copyright (c) 2022
 * 
 */

// class to rebuffer an JUCE AudioBuffer and MidiMessageQueue of arbitrary length to AudioBuffer of a given length
// Useful for fft processing or faster parameter updates and modulation
// Usage: Inherit from this class and override ProcessSynchronBlock
// (c) J. Bitzer @ Jade HS, BSD Licence
//
// Version 2.0 (only JUCE AUdioBUffer, without std::vector)

/* ToDO:
1) rewrite as template class for double
//*/

#pragma once
#include <JuceHeader.h>

class SynchronBlockProcessor
{
public:
    SynchronBlockProcessor();
    /**
     * @brief preparetoprocess sets the desired blocksize for a given numer of channels
     * it can be called at any time (threadsafe), but it will cause audio-glitches (not realtime safe)
     * 
     * @param channels 
     * @param desiredSize 
     */
    void preparetoProcess(int channels, int desiredSize); 
    /**
     * @brief the typical JUCE call just forward the call in Processor
     * 
     * @param data 
     * @param midiMessages 
     */
    void processBlock(juce::AudioBuffer<float>& data, juce::MidiBuffer& midiMessages);
    /**
     * @brief processSynchronBlock is your new processing routine. The block will always be of size desiredSize
     * 
     * @param midiMessages 
     * @return int 
     */
    virtual int processSynchronBlock(juce::AudioBuffer<float>&, juce::MidiBuffer& midiMessages) = 0;
    /**
     * @brief Get the Delay object
     * 
     * @return int this will be DesiredSize
     */
    int getDelay();
private:
    CriticalSection m_protectBlock;
    int m_NrOfChannels;
    int m_OutBlockSize;
    int m_OutCounter;
    int m_InCounter;

    juce::AudioBuffer<float> m_memory;
    juce::AudioBuffer<float> m_block;

    MidiBuffer m_mididata;
    int m_pastSamples;
};
