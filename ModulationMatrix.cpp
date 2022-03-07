#include "ModulationMatrix.h"

ModulationMatrix::ModulationMatrix()
{
    m_modulationList.clear();
    IDcounter = 0;
}
void ModulationMatrix::updateModulation()
{
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

}
int ModulationMatrix::addListEntry(MatrixEntry newentry)
{
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

        return newentry.ID;
    }
    return -1;
}
