/*
  ==============================================================================

    PresetHandler.h
    Created: 3 Jun 2020 11:35:05am
    Author:  bitzer

	// Version 1.0.1 18.06.20 JB: color in save button changed to jadeGrey and JadeRed
								  new dependency from JadeLookAndFeel
	// Version 1.1.1 11.02.20 JB: Added Handling of Factory presets, 
								  changed how to create categories (more flexibel)
								  automation of category combobox, if no categories are set
	// Version 1.1.2 14.02.20 JB: improved category handling (categories can be set and 
								  save (no unknown categories-> set to Unknown))

 
  ==============================================================================
*/
/*
	Ideas: delete a preset by a special menu entry on right clock button on a preset entry (savety measure)

*/

#pragma once

#include <JuceHeader.h>
#include <list>
#include <vector>

const int g_maxNumberOfCategories = 20;

class PresetHandler
{
public:
//	const std::vector<std::string> Categories;


	PresetHandler();
	int setAudioValueTreeState(AudioProcessorValueTreeState* vts);
	int addPreset(ValueTree& newpreset);
	int addOrChangeCurrentPreset(String name, String category = "Unknown", String bank = "User");
	ValueTree getPreset(String name);
	int changePreset(ValueTree& newpreset);
	int changePresetCategory(String name, String category);

	int deletePreset(ValueTree& newpreset);
	File getUserPresetsFolder(bool & wasCreated);
	File getFactoryPresetsFolder();
	int savePreset(String name, String category = "Unknown");
	int savePreset(ValueTree& vt);
	ValueTree loadPreset(String name);
	int loadPresetAndActivate(String name);
	int deletePresetFile(String name);
	int loadfromFileAllUserPresets();
	int getNrOfPresets() { return m_presetList.size(); };
	int getAllKeys(std::vector<String>& keys);
	int changeCategoryOfCurrentTreeState(String category)
	{
		m_vts->state.setProperty("category", category, nullptr);
	}
	bool isAlreadyAPreset(String name)
	{
		return m_presetList.count(name);
	}
	bool isAValidCategory(String category)
	{
		return std::any_of(m_categoryList.begin(), m_categoryList.end(), [category](String st){return st == category;});
	}

// new methods for categories
	void addCategory(String newCat);
	void addCategory(StringArray newCat);
	bool gethasCategories(){return hasCategories;};
	std::vector<String> m_categoryList;

// Factory Presets 
	void DeployFactoryPresets();

private:
	AudioProcessorValueTreeState* m_vts;
	std::map <String, ValueTree> m_presetList;

	bool hasCategories;

	void repairCategory(ValueTree& vt)
	{
		String category = vt.getProperty("category");
		bool isCat = isAValidCategory(category);
		if (isCat)
		{
			vt.setProperty("category", category, nullptr);
		}
		else
		{
			vt.setProperty("category", "Unknown", nullptr);
		}

	}

};

class PresetComponent : public Component
{
public:
	PresetComponent(PresetHandler&);
	void paint(Graphics& g) override;
	void resized() override;
	void setSomethingChanged() {
		m_somethingchanged = true; repaint();
	};
	void setNoCategory();

private:
	ComboBox m_presetCombo;
	PopupMenu m_popupCat[g_maxNumberOfCategories];
	TextButton m_nextButton;
	TextButton m_prevButton;
	TextButton m_saveButton;
	ComboBox m_categoriesCombo;

	PresetHandler& m_presetHandler;

	void nextButtonClick();
	void prevButtonClick();
	void itemchanged();
	void categorychanged();
	void savePreset();

	String m_oldcatname;
	bool m_somethingchanged;
	bool m_hidecategory;
};