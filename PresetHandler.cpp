/*
  ==============================================================================

    PresetHandler.cpp
    Created: 3 Jun 2020 11:35:05am
    Author:  bitzer
 ==============================================================================
*/

#include "PresetHandler.h"
#include "JadeLookAndFeel.h"
// #include "BinaryData.h"

PresetHandler::PresetHandler()
	:hasCategories(false)
{
	m_categoryList.push_back("Unknown"); // default is None
}
void PresetHandler::addCategory(String newCat)
{
	if (hasCategories == false)
	{
		// m_categoryList.pop_back();
		hasCategories = true;
	}
	m_categoryList.push_back(newCat);
	// Sort and delete all doubles
	std::sort(m_categoryList.begin(), m_categoryList.end());
    m_categoryList.erase(std::unique(m_categoryList.begin(), m_categoryList.end()), m_categoryList.end());
	// erase is quit new, alternative is if ( std::any_of(foo.begin(), foo.end(), [](int i){return i<0;})
	// the last is a lambda function that returns a boolean
	if (m_categoryList.size() >= g_maxNumberOfCategories)
	{
		m_categoryList.pop_back();
	}
}
void PresetHandler::addCategory(StringArray newCat)
{
	for (auto singleCat : newCat)
	{
		addCategory(singleCat);
	}
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

int PresetHandler::addOrChangeCurrentPreset(String name, String category, String bank)
{
	m_vts->state.setProperty("presetname", name, nullptr);
	
	m_vts->state.setProperty("bank", bank, nullptr);

	auto state = m_vts->copyState();
	m_presetList.insert_or_assign(name, state);
	savePreset(name, category, bank);

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
	m_presetList.insert_or_assign(name, newpreset);
	savePreset(newpreset);
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


File PresetHandler::getUserPresetsFolder(bool &wasCreated) 
{
	File rootFolder = File::getSpecialLocation(File::SpecialLocationType::userApplicationDataDirectory);
#ifdef JUCE_MAC

	rootFolder = rootFolder.getChildFile("Audio").getChildFile("Presets");

#endif
	rootFolder = rootFolder.getChildFile(JucePlugin_Manufacturer).getChildFile(JucePlugin_Name);

	auto lala = rootFolder.getFileName();

	wasCreated = !rootFolder.isDirectory();

	//bool wasCreated = !exists;

	Result res = rootFolder.createDirectory(); // creates if not existing
	return rootFolder;
}

File PresetHandler::getFactoryPresetsFolder() 
{
	File rootFolder = File::getSpecialLocation(File::SpecialLocationType::commonApplicationDataDirectory);

#if JUCE_MAC
	rootFolder = rootFolder.getChildFile(�Audio�).getChildFile(�Presets�);
#endif
	rootFolder = rootFolder.getChildFile(JucePlugin_Manufacturer).getChildFile(JucePlugin_Name);
	Result res = rootFolder.createDirectory();
	return rootFolder;
}

int PresetHandler::savePreset(String name, String category, String bank)
{
	bool wasCreated;
	File outfiledir = getUserPresetsFolder(wasCreated);
	File outfileXML = outfiledir.getChildFile(name+".xml");
	FileOutputStream foXML(outfileXML);
	if (foXML.openedOk())
	{
		foXML.setPosition(0);
		foXML.truncate();
	}

	m_vts->state.setProperty("version", JucePlugin_VersionString, nullptr);
	m_vts->state.setProperty("presetname", name, nullptr);
	m_vts->state.setProperty("category", category, nullptr);
	m_vts->state.setProperty("bank", bank, nullptr);

	auto state = m_vts->copyState();
	std::unique_ptr<XmlElement> xml(state.createXml());
	xml->writeTo(foXML);
	return 0;
}
int PresetHandler::savePreset(ValueTree& vt)
{
	bool wasCreated;
	File outfiledir = getUserPresetsFolder(wasCreated);
	String name = vt.getProperty("presetname");
	File outfileXML = outfiledir.getChildFile(name+".xml");
	FileOutputStream foXML(outfileXML);
	if (foXML.openedOk())
	{
		foXML.setPosition(0);
		foXML.truncate();
	}
	std::unique_ptr<XmlElement> xml(vt.createXml());
	xml->writeTo(foXML);
	return 0;
}

ValueTree PresetHandler::loadPreset(String name)
{
	bool wasCreated;
	File infiledir = getUserPresetsFolder(wasCreated);
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
	bool wasCreated;
	File outfiledir = getUserPresetsFolder(wasCreated);
	File outfileXML = outfiledir.getChildFile(name + ".xml");
	outfileXML.deleteFile();
	return 0;
}

int PresetHandler::loadfromFileAllUserPresets()
{
	bool wasCreated;
	File outfiledir = getUserPresetsFolder(wasCreated);
	auto files = outfiledir.findChildFiles(File::findFiles, true, "*.xml");
	if (files.isEmpty() == true)
	{
		addOrChangeCurrentPreset("Init","Unknown");
	}
	for (auto oneFile : files)
	{
		auto vt = loadPreset(oneFile.getFileNameWithoutExtension());
		String cat = vt.getProperty("category");
		repairCategory(vt);
		addPreset(vt);
	}
	return files.size();
}

int PresetHandler::getAllKeys(std::vector<String>& keys, std::vector<String>& presetcats)
{
	for (std::map<String, ValueTree>::iterator it = m_presetList.begin(); it != m_presetList.end(); ++it) 
	{
		keys.push_back(it->first);
		presetcats.push_back(it->second.getProperty("category"));
	}
	return 0;
}
void PresetHandler::DeployFactoryPresets()
{
	bool wasCreated;
	File outfiledir = getUserPresetsFolder(wasCreated);
	if (wasCreated == false)
		return;

	auto namesofbinarydata = BinaryData::namedResourceList;
	auto sizesofbinarydata = BinaryData::namedResourceListSize;
	for (auto kk = 0; kk < sizesofbinarydata;++kk)
	{
		auto binname = namesofbinarydata[kk];
		int sizeData;
		auto data = BinaryData::getNamedResource(binname, sizeData);

		std::unique_ptr<XmlElement> xml(XmlDocument::parse(data));
		ValueTree vt;
		if (sizeData != 0 && xml != nullptr)
		{
			vt = ValueTree::fromXml(*xml);
			// Dies ist vermutlich falsch
			// m_vts->replaceState(vt);
			String name = vt.getProperty("presetname");
			String category = vt.getProperty("category");
			vt.setProperty("bank","Factory",nullptr);
			// addOrChangeCurrentPreset(name, category, "Factory");
			repairCategory(vt);
			addPreset(vt);
			savePreset(vt);
		}
	}
}


PresetComponent::PresetComponent(PresetHandler& ph)
	:m_presetHandler(ph), m_somethingchanged(false)
{
	bool hasCat = m_presetHandler.gethasCategories();
	m_hidecategory = !hasCat;

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


	int id = 1;
	for (auto cat : m_presetHandler.m_categoryList)
		m_categoriesCombo.addItem(cat,id++);

	m_categoriesCombo.setSelectedItemIndex(0, NotificationType::dontSendNotification);
	m_categoriesCombo.onChange = [this]() {categorychanged(); };
	addAndMakeVisible(m_categoriesCombo);


	std::vector<String> presetkeys;
	std::vector<String> presetcats;
	auto mastercombomenu = m_presetCombo.getRootMenu();
	id = 1;
	m_presetHandler.getAllKeys(presetkeys, presetcats);
	for (auto cat : m_presetHandler.m_categoryList)
	{
		PopupMenu submenu;
		for (auto kk = 0u; kk < presetcats.size(); ++kk)
		{
			if (cat == presetcats[kk])
			{
				submenu.addItem(id++,presetkeys[kk]);
			}
		}
		mastercombomenu->addSubMenu(cat,submenu);
	}

	id = 1;
//	for (auto onepreset : presetkeys)
//		m_presetCombo.addItem(onepreset, id++);

	m_presetCombo.onChange = [this]() {itemchanged(); };
	m_presetCombo.setSelectedItemIndex(0, NotificationType::sendNotificationAsync);
	m_presetCombo.isTextEditable();
	m_presetCombo.setEditableText(true);
	m_presetCombo.setColour(ComboBox::ColourIds::backgroundColourId, JadeGray);

	addAndMakeVisible(m_presetCombo);

	//getRootMenu()->addSubMenu(). for submenus in the preset combo


	//itemchanged();
	m_somethingchanged = false;
}

void PresetComponent::paint(Graphics & g)
{
	if (m_hidecategory)
		m_categoriesCombo.setVisible(false);

	g.fillAll(JadeGray);
	if (m_somethingchanged)
		m_saveButton.setColour(TextButton::ColourIds::buttonColourId, JadeRed);
	else
	{
		
		m_saveButton.setColour(TextButton::ColourIds::buttonColourId, JadeGray);
	}
}
#define COMBO_WITH 100
#define ELEMENT_DIST 10
#define BUTTON_WIDTH 40
#define ELEMENT_HEIGHT 20
void PresetComponent::resized()
{
	m_presetCombo.setBounds(getWidth() / 2 - COMBO_WITH / 2, 3, COMBO_WITH, ELEMENT_HEIGHT);

	m_nextButton.setBounds(getWidth() / 2 + COMBO_WITH / 2 + ELEMENT_DIST/2, 3, BUTTON_WIDTH, ELEMENT_HEIGHT);
	m_saveButton.setBounds(getWidth() / 2 + COMBO_WITH / 2 + 2*ELEMENT_DIST + BUTTON_WIDTH, 3, BUTTON_WIDTH, ELEMENT_HEIGHT);
	m_prevButton.setBounds(getWidth() / 2 - COMBO_WITH / 2 - ELEMENT_DIST/2 - BUTTON_WIDTH, 3, BUTTON_WIDTH, ELEMENT_HEIGHT);

	auto r = getBounds();
	auto s = r.removeFromRight(3*COMBO_WITH / 4 + ELEMENT_DIST);
	m_categoriesCombo.setBounds(s.getX(), 3, 3*COMBO_WITH / 4, ELEMENT_HEIGHT);
	if (m_hidecategory)
		m_categoriesCombo.setVisible(false);

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

	m_presetCombo.setSelectedItemIndex(id, NotificationType::sendNotificationAsync);
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

	m_presetCombo.setSelectedItemIndex(id, NotificationType::sendNotificationAsync);
	//itemchanged();
}

void PresetComponent::itemchanged()
{
	repaint();
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
		m_somethingchanged = false;
		repaint();
		// m_categoriesCombo.setSelectedItemIndex(0, false);
		m_presetHandler.addOrChangeCurrentPreset(itemname);

		m_presetCombo.addItem(itemname, m_presetCombo.getNumItems()+1);
		m_presetCombo.setSelectedItemIndex(m_presetCombo.getNumItems()-1);
		return;
	}
	

	ValueTree vt = m_presetHandler.getPreset(itemname);
	String cat = vt.getProperty("category");

	int catid = 0;
	for (auto ca : m_presetHandler.m_categoryList)
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
	{
		m_somethingchanged = true;
		m_oldcatname = catname;
		// Todo Remove preset from submenu

		// ToDo Add preset to the correct submenu


	}
	m_presetHandler.changePresetCategory(itemname, catname);
}

/*
ToDO:


2) Combobox f�r mehr Presets (entweder showPopup() �berladen) oder mit Sub-Menus
getRootMenu()->addSubMenu(). (Mit Banks w�re das einfach (Eine Factory Bank mit 32 festen Presets 
und 7 andere Banks f�r User Presets))

*/