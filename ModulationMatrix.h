#pragma once

#include <vector>
#include <map>
#include <list>
#include <functional>
#include <JuceHeader.h>

/* ToDO:

 3) Wo muss die Matrix sitzen. Im Manager und in der Engine.
 4) Wie GUI Einbindung: Liste der Quellen und Senken über IDs aufbauen und in Comboboxen ablegen
 5) Wie die Matrix in die Presets kriegen (alles über Parameter, EInfach 32 Slots fest vorgeben mit den Parametern)


//*/
class ModulationMatrix
{
public:
    class MatrixEntry
    {
    public:
        MatrixEntry()
        :getModulation(nullptr),setModulation(nullptr),bias(0.f),depth(0.f)
        {};
        int ID = 0;
        std::function<float(void)> getModulation;
        std::function<void(float)> setModulation;
        float bias;
        float depth;
        std::string targetID; //necessary for sorting to find equal target entries
        std::string sourceID; // necessary for presets
        // future: Transform function
        bool isActive = true;
        bool isMultiplicative = false;
    };

    ModulationMatrix();
    void updateModulation();
    int addListEntry(MatrixEntry newentry);
    int removeListEntry(int ID);

private:
    CriticalSection m_protect;
    int IDcounter;
    std::list<MatrixEntry> m_modulationList;

};
