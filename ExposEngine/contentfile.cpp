#include "stdafx.h"
#include "contentfile.h"

ContentFile *getErrorCF() {
	static ContentFile error("ERROR");
	return &error;
}

ContentFile::ContentFile(ALLEGRO_FILE *file) {
	/*! \brief Lädt ein ContentFile aus einem bereits geöffneten ALLEGRO_FILE*.
	*
	*	In einer Datei können auch mehrere ContentFiles stecken, weswegen die Methode readContentFiles empfohlen wird.
	*/
	logcf << "[INFO] ContentFile::ContentFile" << std::endl;
	std::string data;

	good = true;
	logcf << "[INFO] Lade ContentFile aus " << file << std::endl;
	int endklammerCount = 0;
	do {
		char c = al_fgetc(file);
		if (al_feof(file)) {
			/* Datei nicht vollständig */
			logcf << "[WARN] " << __FILE__ << ": " << __LINE__ << ": Datei nicht vollständig!" << std::endl;
			good = false;
			break;
		}
		/* Darf niemals in data! */
		if (c == '{') {
			endklammerCount++;
		}
		if (c == '}') {
			if (endklammerCount <= 1) {
				break;
			}
			else {
				endklammerCount--;
			}
		}
		data += c;
	} while (true);

	bool insideString = false;
	for (int i = 0; i < data.length(); i++) {
		if (!IS_RIGHT_CHAR(data[i]) && !insideString) {
			if (i == data.size() - 1) {
				data.pop_back();
			}
			else {
				data.erase(i, 1);
				i--;
			}
		}
		else if (data[i] == '"') {
			insideString = !insideString;
			data.erase(i, 1);
			i--;
		}
		else if (data[i] == ';') {
			insideString = false;
		}
	}
	if (data[0] == ';') data = data.substr(1);
#ifdef DEBUG
	logcf << "[INFO] Data: \"" << data << "\"" << std::endl;
#endif

	this->load(data, good);
	this->print();

	logcf << "[INFO] ContentFile::ContentFile finished" << std::endl;
}

ContentFile::ContentFile(const std::string& data, bool isGood) {
	/*! \brief Ruft intern ContentFile::load auf.
	*
	*	IsGood muss true sein, die Variable ist nur dafür da, damit keine Verwechslung mit ContentFile(std::string) stattfinden kann.
	*/
	this->load(data, isGood);
}

ContentFile::ContentFile(const std::string& id) {
	/*!	Erstellt ein leeres ContentFile, bei dem „ID“ den Typ angibt.
	*
	*	good wird automatisch gesetzt.
	*/
	logcf << "[INFO] ContentFile::ContentFile " << id << " (+finished)" << std::endl;
	type = id;
	good = true;
}

ContentFile::~ContentFile() {
	/*! \brief Zerstört intern die anderen Sub-ContentFiles, die das Parentcontentfile hat.
	*
	*/
	for (auto it = contentFiles.begin(); it != contentFiles.end(); it++) {
		delete it->second;
	}
}

static void printTabs(std::fstream& file, int i) {
	for (int zeyen = 0; zeyen != i; zeyen++) { file << "\t"; }
}

void saveContentFileValue(std::fstream& file, std::map<std::string, std::string>::iterator it, int tabs = 0) {
	/*! \brief Wird intern genutzt um die Werte eines ContentFiles in eine Datei zu schreiben.
	*
	*/
	printTabs(file, tabs);
	bool containsS = (it->second.find_first_of(' ') != std::string::npos);

	if (containsS) {
		file << it->first << ": \"" << it->second << "\";" << std::endl;
	}
	else {
		file << it->first << ": " << it->second << ";" << std::endl;
	}

}

void saveContentFileTo(ContentFile *cf, std::fstream& file, int tabs) {
	/*! \brief Speichert ein ganzes ContentFile in eine bereits offene Datei.
	*
	*/
	printTabs(file, tabs);
	file << cf->getType() << " {" << std::endl;

	for (auto it = cf->getVBeginIterator(); it != cf->getVEndIterator(); it++) {
		saveContentFileValue(file, it, tabs + 1);
	}

	for (auto it = cf->getCFBeginIterator(); it != cf->getCFEndIterator(); it++) {
		if (it->second != nullptr) {
			printTabs(file, tabs + 1);
			file << it->first << "[]:" << std::endl;
			saveContentFileTo(it->second, file, tabs + 2);
		}
	}

	printTabs(file, tabs);
	file << "}" << std::endl;
}

void ContentFile::saveToFile(const std::string& filename, bool trunc) {
	/*! \brief Speichert das ContentFile in die Datei filename
	*
	* Öffnet die Datei filename, wenn truncate true ist, wird diese beim Öffnen geleert. Ruft intern saveContentFileTo auf.
	*/
	std::fstream file(filename, trunc ? std::ios::out | std::ios::trunc : std::ios::out);

	saveContentFileTo(this, file, 0);

	file.close();
}

void ContentFile::load(const std::string& data, bool isGood) {
	/*! \brief Erstellt ein ContentFile aus dem data-String.
	*
	*	IsGood muss true sein, da ansonsten der Ladevorgang abgebrochen wird.
	*/
	good = isGood;

	if (good) {
		bool foundType = false;
		for (int i = 0; i != data.size() && good; i++) {
			if (!foundType) {
				if (data[i] != '{') {
					type += data[i];
				}
				else {
					foundType = true;
				}
				continue;
			}

			std::string name;
			bool rekursiv = false;
			while (true && i != data.size()) {
				if (data[i] == ':') {
					i++;
					break;
				}
				else if (data[i] == '[' && data[i + 1] == ']' && data[i + 2] == ':') {
					i += 3;
					rekursiv = true;
					break;
				}
				else {
					name += data[i];
				}
				i++;
			}
			if (i == data.size()) {
				logcf << "[WARN] " << __FILE__ << ": " << __LINE__ << ": Datei nicht gut" << std::endl;
				good = false;
				break;
			}

			if (!rekursiv) {
				std::string value;
				while (true && i != data.size()) {
					if (data[i] == ';') {
						break;
					}
					else {
						value += data[i];
					}
					i++;
				}
				if (i == data.size()) {
					logcf << "[WARN] " << __FILE__ << ": " << __LINE__ << ": Datei nicht gut" << std::endl;
					good = false;
					break;
				}
				values[name] = value;
			}
			else {
				std::string rekData;
				int rekDataKC = 0;
				for (int un = 0; i != data.size(); i++) { //SIC!
														  /* Darf niemals in data! */
					if (data[i] == '{') {
						rekDataKC++;
					}
					if (data[i] == '}') {
						if (rekDataKC <= 1) {
							break;
						}
						else {
							rekDataKC--;
						}
					}
					rekData += data[i];
				}
				contentFiles[name] = new ContentFile(rekData, true);
				contentFiles[name]->parent = this;
				if (data[i + 1] == ';') {
					i++;
				}
			}
		}
	}
}

void ContentFile::print() {
	/* \brief Gibt das ContentFile in logstream aus.
	*
	*	Subcontentfiles werden ebenfalls mit ausgegeben.
	*/
	logcf << "[INFO] CF.type = " << type << std::endl;
	for (auto it = values.begin(); it != values.end(); it++) {
		logcf << "[INFO] value[" << it->first << "] = \"" << it->second << "\"" << std::endl;
	}

	for (auto it = contentFiles.begin(); it != contentFiles.end(); it++) {
		logcf << "[INFO] Contentfile[" << it->first << "]: " << std::endl;
		it->second->print();
		logcf << "\t[INFO] Rec. finished!" << std::endl;
	}
	logcf << "[INFO] ContentFile::print finished!" << std::endl;
}

std::vector<ContentFile*> readContentFiles(const std::string& filename, int maxCount) {
	/*! \brief Liest ContentFiles aus der Datei filename und speichert sie in einem Vektor.
	*
	*	Lädt maxCount ContentFiles aus der Datei mit dem Namen filename. Hierbei wird sichergestellt, das in dem zurückgegebenen Vector nur vollständige ContentFiles landen, nicht kompatible werden beim Start aussortiert. Die Elemente im Vector müssen später selbstständig zerstört werden. Wenn die Datei nicht geöffnet werden konnte, wird ein std::string als Exception geworfen.
	*	== Achtung == Es gibt keine Garantie, dass maxCount Elemente im Vector sind, es wird nur garantiert, dass es weniger sind.
	*/
	logcf << "[INFO] readContentFiles(" << filename << ")" << std::endl;
	std::vector<ContentFile*> files;
	ALLEGRO_FILE *file = al_fopen(filename.c_str(), "r");
	if (!file) {
		logcf << "[ERR.] " << __FILE__ << ": " << __LINE__ << ": Konnte Datei nicht öffnen!" << std::endl;
		throw std::string("Can't open file");
		return files;
	}

	for (int i = 0; !al_feof(file) && i != maxCount; i++) {//ISSO // -1 muss ja als unendlich gelten!
		ContentFile *daCF = new ContentFile(file);
		if (daCF->isGood()) {
			files.push_back(daCF);
		}
	}

	al_fclose(file);
	logcf << "[INFO] readContentFiles finished" << std::endl;
	return files;
}

void compressCFComparison(ContentFile *cf, ContentFile *comp) {
	/* Komprimiert ein Contentfile indem es es mit comp vergleicht und alle identischen
	Werte löscht */
	auto it = cf->getVBeginIterator();
	while (it != cf->getVEndIterator()) {
		if (comp->valueExists(it->first) && it->second == comp->getValue(it->first)) {
			it = cf->deleteValue(it);
		}
		else {
			it++;
		}
	}

	for (auto tit = cf->getCFBeginIterator(); tit != cf->getCFEndIterator(); tit++) {
		if (comp->contentFileExists(tit->first)) compressCFComparison(tit->second, comp->getCF(tit->first));
	}
}
