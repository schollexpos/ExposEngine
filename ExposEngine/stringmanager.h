#pragma once
#include <list>
#include "helper.h"

namespace expos {

	namespace stringmanager {
		struct Language {
			std::string name;
			std::map<ID_ref, std::string> strings;
		};
	}

	class StringManager {
	private:
		std::vector<std::string> languageNames;
		std::vector<stringmanager::Language> languages;
		stringmanager::Language global;
		size_t curLang = 0;

		stringmanager::Language& getCurLang() {
			return languages[curLang];
		}
	public:
		StringManager();

		void setLanguage(size_t curLang) {
			this->curLang = curLang;
		}

		void set(ID_ref id, const std::string& str) { getCurLang().strings[id] = str; }
		void set(const ID& id, const std::string& str) { getCurLang().strings[id._ref()] = str; }
		void setGlobal(ID_ref id, const std::string& str) { global.strings[id] = str; }
		void setGlobal(const ID& id, const std::string& str) { global.strings[id._ref()] = str; }

		const std::string& get(ID_ref id) {
			if(getCurLang().strings.find(id) == getCurLang().strings.end()) {
				return global.strings.at(id);
			} else {
				return getCurLang().strings.at(id);
			}
		}

		const std::string& getGlobal(ID_ref id) {
			return global.strings.at(id);
		}
	};

	extern StringManager *stringManager;

}