#include "ModulationMatrix.h"

ModulationMatrix::ModulationMatrix()
{
    m_modulationList.clear();
    IDcounter = 0;
}
void ModulationMatrix::updateModulation()
{
    m_protect.enter();
    std::list<MatrixEntry>::iterator it = m_modulationList.begin();
    if (m_modulationList.size()> 1)
        it++; //points to the next element in the list
    
    float modval = 0.f;
    for (auto matrixentry : m_modulationList)
    {
        float value = matrixentry.getModulation();
        value*=matrixentry.depth;
        value+=matrixentry.bias;
        if (it != m_modulationList.end())
        {
            if (matrixentry.targetID == it->targetID )
            {
                modval += value;
            }
            else
            {
                matrixentry.setModulation(value + modval);
                modval = 0.f;
            }
            it++;            
        }
        else
        {
            matrixentry.setModulation(value + modval);
        }
        
    }
    m_protect.exit();
}
int ModulationMatrix::addListEntry(MatrixEntry newentry)
{
    m_protect.enter();
    if (newentry.getModulation != nullptr && newentry.setModulation != nullptr)
    {
        newentry.ID = IDcounter++;
        std::list<MatrixEntry>::iterator it = m_modulationList.begin();
        for (auto kk = 0; kk < m_modulationList.size(); kk++)
        {
            if (it->targetID == newentry.targetID)
            {
                m_modulationList.insert(it,newentry);
                break;  
            }
            it++;
        }
        if (it == m_modulationList.end())
            m_modulationList.push_back(newentry);

        m_protect.exit();
        return newentry.ID;
    }
    m_protect.exit();
    return -1;
}
int ModulationMatrix::removeListEntry(int ID)
{
    m_protect.enter();
    for (std::list<MatrixEntry>::iterator it = m_modulationList.begin(); it != m_modulationList.end();)
    {
        if (it->ID == ID)
            it = m_modulationList.erase(it);
        
        it++;
    }
    m_protect.exit();
}