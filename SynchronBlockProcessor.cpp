#include "SynchronBlockProcessor.h"

SynchronBlockProcessor::SynchronBlockProcessor()
:m_NrOfChannels(2),m_OutBlockSize(256),m_maxInputSize(256)
{
    preparetoProcess(m_NrOfChannels,m_maxInputSize);
    
}
void SynchronBlockProcessor::preparetoProcess(int channels, int maxinputlen)
{
    m_NrOfChannels = channels;
    m_maxInputSize = maxinputlen;
    m_mem.resize(m_NrOfChannels);
    m_block.resize(m_NrOfChannels);
    for (auto kk = 0; kk < m_NrOfChannels; ++kk)
    {
        m_mem.at(kk).resize(2*m_OutBlockSize);
        m_block.at(kk).resize(m_OutBlockSize);
        std::fill(m_mem.at(kk).begin(),m_mem.at(kk).end(),0.f);
        std::fill(m_block.at(kk).begin(),m_block.at(kk).end(),0.f);
    }
    m_OutCounter = 0;
    m_InCounter = 0;
    m_mididata.clear();
    m_pastSamples = 0;
}
void SynchronBlockProcessor::processBlock(juce::AudioBuffer<float>& data, juce::MidiBuffer& midiMessages)
{
    int nrofBlockProcessed = 0;
    auto readdatapointers = data.getArrayOfReadPointers();
    auto writedatapointers = data.getArrayOfWritePointers();
    int nrOfInputSamples = data.getNumSamples();

    for (auto kk = 0; kk < nrOfInputSamples; ++kk)
    {
        for (auto cc = 0; cc < m_NrOfChannels; ++cc)
        {
            m_block[cc][m_InCounter] = readdatapointers[cc][kk];
            writedatapointers[cc][kk] = m_mem[cc][m_OutCounter];
        }
        m_InCounter++;
        if (m_InCounter == m_OutBlockSize)
        {
            m_InCounter = 0;
            if (kk < m_OutBlockSize)
                m_mididata.addEvents(midiMessages,0, kk ,m_pastSamples);
            else
            {
                m_mididata.addEvents(midiMessages,kk-m_OutBlockSize,m_OutBlockSize,-(kk-m_OutBlockSize));
            }

            nrofBlockProcessed++;
            processSynchronBlock(m_block, m_mididata);
            m_mididata.clear();
            m_pastSamples = 0;

                // copy block into mem
            if (m_OutCounter < m_OutBlockSize)
            {
                for (auto channel = 0; channel < m_NrOfChannels; ++channel)
                {
                    for (auto sample = 0; sample < m_OutBlockSize; ++sample)
                    {
                        m_mem[channel][sample + m_OutBlockSize] = m_block[channel][sample];
                    }
                }
 
            }
            else
            {
                for (auto channel = 0; channel < m_NrOfChannels; ++channel)
                {
                    for (auto sample = 0; sample < m_OutBlockSize; ++sample)
                    {
                        m_mem[channel][sample] = m_block[channel][sample];
                    }
                }
            }
            
        }
        m_OutCounter++;
        if (m_OutCounter == 2*m_OutBlockSize)
            m_OutCounter = 0;
    }
    if (nrofBlockProcessed>0)
    {
        int lastMidiSamples = nrOfInputSamples- m_InCounter;
        m_mididata.addEvents(midiMessages,lastMidiSamples, m_InCounter,-lastMidiSamples);
        m_pastSamples += m_InCounter;

    }
    else
    {
        m_mididata.addEvents(midiMessages,0,nrOfInputSamples,m_pastSamples);
        m_pastSamples += nrOfInputSamples;
    }
    
}
int SynchronBlockProcessor::processBlock(std::vector <std::vector<float>>& data, juce::MidiBuffer& midiMessages)
{
    int nrofBlockProcessed = 0;
    int nrOfInputSamples = data[0].size();
    for (auto kk = 0; kk < nrOfInputSamples; ++kk)
    {
        for (auto cc = 0; cc < m_NrOfChannels; ++cc)
        {
            m_block[cc][m_InCounter] = data[cc][kk];
            data[cc][kk] = m_mem[cc][m_OutCounter];
        }
        m_InCounter++;
        if (m_InCounter == m_OutBlockSize)
        {
            m_InCounter = 0;
            if (kk < m_OutBlockSize)
                m_mididata.addEvents(midiMessages,0, kk ,m_pastSamples);
            else
            {
                m_mididata.addEvents(midiMessages,kk-m_OutBlockSize,m_OutBlockSize,-(kk-m_OutBlockSize));
            }
            nrofBlockProcessed++;
            processSynchronBlock(m_block,m_mididata);            
            m_mididata.clear();
            m_pastSamples = 0;
                // copy block into mem
            if (m_OutCounter < m_OutBlockSize)
            {
                for (auto channel = 0; channel < m_NrOfChannels; ++channel)
                {
                    for (auto sample = 0; sample < m_OutBlockSize; ++sample)
                    {
                        m_mem[channel][sample + m_OutBlockSize] = m_block[channel][sample];
                    }
                }
 
            }
            else
            {
                for (auto channel = 0; channel < m_NrOfChannels; ++channel)
                {
                    for (auto sample = 0; sample < m_OutBlockSize; ++sample)
                    {
                        m_mem[channel][sample] = m_block[channel][sample];
                    }
                }
            }
            
        }
        m_OutCounter++;
        if (m_OutCounter == 2*m_OutBlockSize)
            m_OutCounter = 0;
    }
    if (nrofBlockProcessed>0)
    {
        int lastMidiSamples = nrOfInputSamples- m_InCounter;
        m_mididata.addEvents(midiMessages,lastMidiSamples, m_InCounter,-lastMidiSamples);
        m_pastSamples += m_InCounter;

    }
    else
    {
        m_mididata.addEvents(midiMessages,0,nrOfInputSamples,m_pastSamples);
        m_pastSamples += nrOfInputSamples;
    }
    
}

int SynchronBlockProcessor::setDesiredBlockSizeSamples(int desiredSize)
{
    m_OutBlockSize = desiredSize;
    preparetoProcess(m_NrOfChannels,m_maxInputSize);

}
int SynchronBlockProcessor::setDesiredBlockSizeMiliSeconds(float desiredSize_ms, float fs)
{
    int desiredSize = int(desiredSize_ms*0.001*fs + 0.5);
    setDesiredBlockSizeSamples(desiredSize);
}

int SynchronBlockProcessor::getDelay()
{
    return m_OutBlockSize;
}
/* // Midi Debugcode
    auto a = midiMessages.getNumEvents();
    auto b = midiMessages.getFirstEventTime();
    auto c = midiMessages.getLastEventTime();    

    if (a > 0)
    {    
        DBG(String("Anz= ") + String(a) + String("Start= ") + String(b) + String("End= ") + String(c) );
    }


            auto a = m_mididata.getNumEvents();
            auto b = m_mididata.getFirstEventTime();
            auto c = m_mididata.getLastEventTime();    

            if (a > 0)
            {    
                DBG(String("BlockFkt Anz= ") + String(a) + String("Start= ") + String(b) + String("End= ") + String(c) );
                DBG(String("InCounter= " + String(m_InCounter) + "kk = " + String(kk) ));
            }
//*/