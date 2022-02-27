#include "SynchronBlockProcessor.h"

SynchronBlockProcessor::SynchronBlockProcessor()
:m_NrOfChannels(2),m_OutBlockSize(256)
{
    preparetoProcess(m_NrOfChannels,m_OutBlockSize);
}
void SynchronBlockProcessor::preparetoProcess(int channels, int desiredSize)
{
    m_protectBlock.enter();
    m_OutBlockSize = desiredSize;
    m_NrOfChannels = channels;
    m_memory.setSize(m_NrOfChannels,2*m_OutBlockSize);
    m_memory.clear();
    m_block.setSize(m_NrOfChannels,m_OutBlockSize);
    m_block.clear();
    m_OutCounter = 0;
    m_InCounter = 0;
    m_mididata.clear();
    m_pastSamples = 0;
    m_protectBlock.exit();
}
void SynchronBlockProcessor::processBlock(juce::AudioBuffer<float>& data, juce::MidiBuffer& midiMessages)
{
    m_protectBlock.enter();
    int nrofBlockProcessed = 0;
    auto readdatapointers = data.getArrayOfReadPointers();
    auto writedatapointers = data.getArrayOfWritePointers();
    int nrOfInputSamples = data.getNumSamples();
    auto blockreaddatapointers = m_block.getArrayOfReadPointers();
    auto blockwritedatapointers = m_block.getArrayOfWritePointers();
    auto memreaddatapointers = m_memory.getArrayOfReadPointers();
    auto memwritedatapointers = m_memory.getArrayOfWritePointers();

    for (auto kk = 0; kk < nrOfInputSamples; ++kk)
    {
        for (auto cc = 0; cc < m_NrOfChannels; ++cc)
        {
            blockwritedatapointers[cc][m_InCounter] = readdatapointers[cc][kk];
            writedatapointers[cc][kk] = memreaddatapointers[cc][m_OutCounter];
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
                        memwritedatapointers[channel][sample + m_OutBlockSize] = blockreaddatapointers[channel][sample];
                    }
                }
 
            }
            else
            {
                for (auto channel = 0; channel < m_NrOfChannels; ++channel)
                {
                    for (auto sample = 0; sample < m_OutBlockSize; ++sample)
                    {
                        memwritedatapointers[channel][sample] = blockreaddatapointers[channel][sample];
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
    m_protectBlock.exit();
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