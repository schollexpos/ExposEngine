#include "stdafx.h"
#include "stringmanager.h"
#include "contentfile.h"

namespace expos {
	StringManager::StringManager() {
		ContentFile *strings = readContentFile("strings.ecf");
		for(size_t i = 0; i < strings->getArray("languages")->getSize(); i++) {
			languageNames.push_back(strings->getArray("languages")->getValue(i));
		}

		for(const std::string& name : languageNames) {
			stringmanager::Language lang;
			lang.name = name;
			for(auto it = strings->getCF(name)->valueBegin(); it != strings->getCF(name)->valueEnd(); ++it) {
				lang.strings[getID(it->first)._ref()] = it->second;
			}
			languages.push_back(lang);
		}

		global.name = "global";
		for(auto it = strings->getCF(global.name)->valueBegin(); it != strings->getCF(global.name)->valueEnd(); ++it) {
			global.strings[getID(it->first)._ref()] = it->second;
		}
	}

	StringManager *stringManager = nullptr;
}