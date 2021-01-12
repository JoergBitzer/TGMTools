/*
  ==============================================================================

    PresetHandler.h
    Created: 3 Jun 2020 11:35:05am
    Author:  bitzer

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <list>
#include <vector>


class PresetHandler
{
public:
	const std::vector<std::string> Categories;


	PresetHandler();
	int setAudioValueTreeState(AudioProcessorValueTreeState* vts);
	int addPreset(ValueTree& newpreset);
	int addOrChangeCurrentPreset(String name, String category = "Unknown");
	ValueTree getPreset(String name);
	int changePreset(ValueTree& newpreset);
	int changePresetCategory(String name, String category);

	int deletePreset(ValueTree& newpreset);
	File getUserPresetsFolder();
	File getFactoryPresetsFolder();
	int savePreset(String name, String category = "Unknown");
	ValueTree loadPreset(String name);
	int loadPresetAndActivate(String name);
	int deletePresetFile(String name);
	int loadAllUserPresets();
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
private:
	AudioProcessorValueTreeState* m_vts;
	std::map <String, ValueTree> m_presetList;
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