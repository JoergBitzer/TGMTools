/*
  ==============================================================================

    PresetHandler.cpp
    Created: 3 Jun 2020 11:35:05am
    Author:  bitzer

	// Version 1.0.1 18.06.20 JB: color in save button changed to jadeGrey and JadeRed
	// new dependency from JadeLookAndFeel
  ==============================================================================
*/

#include "PresetHandler.h"
#include "JadeLookAndFeel.h"
PresetHandler::PresetHandler()
	: Categories({"Unknown", "Lead", "Brass", "Template", "Bass",
	"Keys", "Organ" , "Pad", "Drums_Perc", "SpecialEffect", "Strings" })
{
}

int PresetHandler::setAudioValueTreeState(AudioProcessorValueTreeState* vts)
{
	m_vts = vts;
	return 0;
}

int PresetHandler::addPreset(ValueTree& newpreset)
{
	String name = newpreset.getProperty("presetname");
	m_presetList[name] = newpreset;
	return 0;
}

int PresetHandler::addOrChangeCurrentPreset(String name, String category)
{
	m_vts->state.setProperty("presetname", name, nullptr);
	m_vts->state.setProperty("category", category, nullptr);

	auto state = m_vts->copyState();
	m_presetList.insert_or_assign(name, state);
	savePreset(name, category);

	return 0;
}

ValueTree PresetHandler::getPreset(String name)
{
	int isKey = m_presetList.count(name);
	if (isKey == 1)
	{
		ValueTree vt = m_presetList.at(name);
		return vt;
	}
	else
	{
		ValueTree vt;
		return vt;
	}
}

int PresetHandler::changePreset(ValueTree& newpreset)
{
	String name = newpreset.getProperty("presetname");
	String cat = newpreset.getProperty("category");
	m_presetList.insert_or_assign(name, newpreset);
	savePreset(name, cat);
	return 0;
}

int PresetHandler::changePresetCategory(String name, String category)
{
	int isKey = m_presetList.count(name);
	if (isKey == true)
		m_presetList[name].setProperty("category", category, nullptr);
	return 0;
}

int PresetHandler::deletePreset(ValueTree & newpreset)
{
	String name = newpreset.getProperty("presetname");
	m_presetList.erase(name);
	deletePresetFile(name);
	return 0;
}


File PresetHandler::getUserPresetsFolder() 
{
	File rootFolder = File::getSpecialLocation(File::SpecialLocationType::userApplicationDataDirectory);
#ifdef JUCE_MAC

	rootFolder = rootFolder.getChildFile("Audio").getChildFile("Presets");

#endif
	rootFolder = rootFolder.getChildFile(JucePlugin_Manufacturer).getChildFile(JucePlugin_Name);
	Result res = rootFolder.createDirectory(); // creates if not existing
	return rootFolder;
}

File PresetHandler::getFactoryPresetsFolder() 
{
	File rootFolder = File::getSpecialLocation(File::SpecialLocationType::commonApplicationDataDirectory);

#if JUCE_MAC
	rootFolder = rootFolder.getChildFile(“Audio”).getChildFile(“Presets”);
#endif
	rootFolder = rootFolder.getChildFile(JucePlugin_Manufacturer).getChildFile(JucePlugin_Name);
	Result res = rootFolder.createDirectory();
	return rootFolder;
}

int PresetHandler::savePreset(String name, String category)
{
	File outfiledir = getUserPresetsFolder();
	File outfileXML = outfiledir.getChildFile(name+".xml");
	FileOutputStream foXML(outfileXML);
	if (foXML.openedOk())
	{
		foXML.setPosition(0);
		foXML.truncate();
	}

	m_vts->state.setProperty("presetname", name, nullptr);
	m_vts->state.setProperty("category", category, nullptr);

	auto state = m_vts->copyState();
	std::unique_ptr<XmlElement> xml(state.createXml());
	xml->writeTo(foXML);
	return 0;
}

ValueTree PresetHandler::loadPreset(String name)
{
	File infiledir = getUserPresetsFolder();
	File infileXML = infiledir.getChildFile(name + ".xml");

	//File settingsFile2("c:\\AudioDev\\init.xml");
	//XmlDocument settingsDocument(settingsFile2);
	std::unique_ptr<XmlElement> xml(XmlDocument::parse(infileXML));
	ValueTree vt;
	if (infileXML.existsAsFile() && xml != nullptr)
	{
		vt = ValueTree::fromXml(*xml);
		
	}
	return vt;
}

int PresetHandler::loadPresetAndActivate(String name)
{
	ValueTree vt = loadPreset(name);
	m_vts->replaceState(vt);
	return 0;
}

int PresetHandler::deletePresetFile(String name)
{
	File outfiledir = getUserPresetsFolder();
	File outfileXML = outfiledir.getChildFile(name + ".xml");
	outfileXML.deleteFile();
	return 0;
}

int PresetHandler::loadAllUserPresets()
{
	File outfiledir = getUserPresetsFolder();
	auto files = outfiledir.findChildFiles(File::findFiles, true, "*.xml");
	if (files.isEmpty() == true)
	{
		addOrChangeCurrentPreset("Init","Unknown");
	}
	for (auto kk : files)
	{
		auto vt = loadPreset(kk.getFileNameWithoutExtension());
		addPreset(vt);
	}
	return files.size();
}

int PresetHandler::getAllKeys(std::vector<String>& keys)
{
	for (std::map<String, ValueTree>::iterator it = m_presetList.begin(); it != m_presetList.end(); ++it) 
	{
		keys.push_back(it->first);
	}
	return 0;
}

//auto files = File{ "/Users/name/Programming/pluging/presets" }.findChildFiles(File::findFiles, true, "empty.xml");
		//String cat = vt.getProperty("category");

PresetComponent::PresetComponent(PresetHandler& ph)
	:m_presetHandler(ph), m_somethingchanged(false), m_hidecategory(false)
{
	m_nextButton.setButtonText("Next");
	m_nextButton.setColour(TextButton::ColourIds::buttonColourId,JadeGray);
	m_nextButton.onClick = [this]() {nextButtonClick(); };
	addAndMakeVisible(m_nextButton);

	m_prevButton.setButtonText("Prev");
	m_prevButton.onClick = [this]() {prevButtonClick(); };
	m_prevButton.setColour(TextButton::ColourIds::buttonColourId, JadeGray);
	addAndMakeVisible(m_prevButton);

	m_saveButton.setButtonText("Save");
	m_saveButton.setColour(TextButton::ColourIds::buttonColourId, JadeGray);

	m_saveButton.onClick = [this]() {savePreset(); };
	addAndMakeVisible(m_saveButton);

	std::vector<String> keys;
	m_presetHandler.getAllKeys(keys);

	int id = 0;
	for (auto cat : keys)
		m_presetCombo.addItem(cat, ++id);

	m_presetCombo.onChange = [this]() {itemchanged(); };
	m_presetCombo.setSelectedItemIndex(0, false);
	m_presetCombo.isTextEditable();
	m_presetCombo.setEditableText(true);
	m_presetCombo.setColour(ComboBox::ColourIds::backgroundColourId, JadeGray);

	addAndMakeVisible(m_presetCombo);

	id = 1;
	for (auto cat : ph.Categories)
		m_categoriesCombo.addItem(cat,id++);

	m_categoriesCombo.setSelectedItemIndex(0, false);
	m_categoriesCombo.onChange = [this]() {categorychanged(); };
	addAndMakeVisible(m_categoriesCombo);

	itemchanged();
	m_somethingchanged = false;
}

void PresetComponent::paint(Graphics & g)
{
	g.fillAll(JadeGray);
	if (m_somethingchanged)
		m_saveButton.setColour(TextButton::ColourIds::buttonColourId, JadeRed);
	else
	{
		
		m_saveButton.setColour(TextButton::ColourIds::buttonColourId, JadeGray);
	}
}
#define COMBO_WITH 150
#define ELEMENT_DIST 10
#define BUTTON_WIDTH 40
#define ELEMENT_HEIGHT 20
void PresetComponent::resized()
{
	m_presetCombo.setBounds(getWidth() / 2 - COMBO_WITH / 2, 3, COMBO_WITH, ELEMENT_HEIGHT);
	if (m_hidecategory)
		m_categoriesCombo.setVisible(false);

	m_categoriesCombo.setBounds(getWidth() / 2 + COMBO_WITH / 2 + 2*BUTTON_WIDTH + 3*ELEMENT_DIST, 3, COMBO_WITH/2, ELEMENT_HEIGHT);
	if (m_hidecategory)
		m_categoriesCombo.setVisible(false);

	m_nextButton.setBounds(getWidth() / 2 + COMBO_WITH / 2 + ELEMENT_DIST/2, 3, BUTTON_WIDTH, ELEMENT_HEIGHT);
	m_saveButton.setBounds(getWidth() / 2 + COMBO_WITH / 2 + 2*ELEMENT_DIST + BUTTON_WIDTH, 3, BUTTON_WIDTH, ELEMENT_HEIGHT);
	m_prevButton.setBounds(getWidth() / 2 - COMBO_WITH / 2 - ELEMENT_DIST/2 - BUTTON_WIDTH, 3, BUTTON_WIDTH, ELEMENT_HEIGHT);

	auto r = getBounds();
	auto s = r.removeFromRight(3*COMBO_WITH / 4 + ELEMENT_DIST);
	m_categoriesCombo.setBounds(s.getX(), 3, 3*COMBO_WITH / 4, ELEMENT_HEIGHT);

}

void PresetComponent::setNoCategory()
{
	m_hidecategory = true;
}

void PresetComponent::nextButtonClick()
{
	int id = m_presetCombo.getSelectedItemIndex();
	id++;
	if (id == m_presetCombo.getNumItems())
		id = 0;

	m_presetCombo.setSelectedItemIndex(id, false);
	//itemchanged();
}
void PresetComponent::savePreset()
{
	int catid = m_categoriesCombo.getSelectedItemIndex();
	String catname = m_categoriesCombo.getItemText(catid);

	int id = m_presetCombo.getSelectedItemIndex();
	String itemname;
	if (id == -1)
		itemname = m_presetCombo.getText();
	else
		itemname = m_presetCombo.getItemText(id);


	m_presetHandler.savePreset(itemname, catname);
	m_somethingchanged = false;
	repaint();
}
void PresetComponent::prevButtonClick()
{
	int id = m_presetCombo.getSelectedItemIndex();

	id--;
	if (id == -1)
		id = m_presetCombo.getNumItems()-1;
	if (id == -2)
		id = m_presetCombo.getNumItems() - 2;

	m_presetCombo.setSelectedItemIndex(id, false);
	//itemchanged();
}

void PresetComponent::itemchanged()
{
	int id = m_presetCombo.getSelectedItemIndex();
	String itemname;
	if (id == -1)
		itemname = m_presetCombo.getText();
	else
		itemname = m_presetCombo.getItemText(id);

	if (itemname == "")
		return;

	bool isKey =m_presetHandler.isAlreadyAPreset(itemname);
	if (isKey == false)
	{
		m_somethingchanged = true;
		repaint();
		m_categoriesCombo.setSelectedItemIndex(0, false);
		m_presetHandler.addOrChangeCurrentPreset(itemname);

		m_presetCombo.addItem(itemname, m_presetCombo.getNumItems()+1);
		//m_presetCombo.setSelectedItemIndex(m_presetCombo.getNumItems()-1);
		return;
	}
	

	ValueTree vt = m_presetHandler.getPreset(itemname);
	String cat = vt.getProperty("category");

	int catid = 0;
	for (auto ca : m_presetHandler.Categories)
	{
		if (ca == cat)
			break;
		catid++;
	}
	m_categoriesCombo.setSelectedItemIndex(catid);
	m_oldcatname = m_categoriesCombo.getItemText(catid);
	m_presetHandler.loadPresetAndActivate(itemname);
	m_somethingchanged = false;
}

void PresetComponent::categorychanged()
{
	
	repaint();
	int id = m_presetCombo.getSelectedItemIndex();
	String itemname;
	if (id == -1)
		itemname = m_presetCombo.getText();
	else
		itemname = m_presetCombo.getItemText(id);

	int catid = m_categoriesCombo.getSelectedItemIndex();
	String catname = m_categoriesCombo.getItemText(catid);
	
	if (catname != m_oldcatname)
		m_somethingchanged = true;

	m_presetHandler.changePresetCategory(itemname, catname);
}

/*
ToDO:

1) Factory Presets einbauen (vermutlich am Besten ueber RAM, also im Produjucer einpflegen)
2) Combobox für mehr Presets (entweder showPopup() überladen) oder mit Sub-Menus
getRootMenu()->addSubMenu(). (Mit Banks wäre das einfach (Eine Factory Bank mit 32 festen Presets 
und 7 andere Banks für User Presets))

*/