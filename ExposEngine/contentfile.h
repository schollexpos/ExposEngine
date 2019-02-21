#pragma once

#ifndef EXPOS_CONTENTFILE_H
#define EXPOS_CONTENTFILE_H
#include <allegro5/allegro5.h>
#include <string>
#include <map>

#define logcf std::cout


#define IS_RIGHT_CHAR(c) (c > 32)

/*! \brief ContentFiles speichern den meisten Spielcontent.
*
*	In ContentFiles werden die meisten Spieldateien gespeichert. In einer Datei auf der Festplatte können
*	jedoch auch mehrere ContentFiles enthalten sein (z.b. charakters.txt enthält mehrere CHARAKTER-ContentFiles.
*	ContentFiles haben einen Ähnlichen Aufbau wie JSON-Dateien:
*	#STRUCTNAME {
*		name:value:
*		name:value:
*		name: #STRUCTNAME {
*			name: value;
*		};
*		name: ["Object", "Bla2", "Bla"]; 
*	}
*	Wie man sieht, können ContentFiles auch "SubcontentFiles" enthalten.
*   Die Variablen:
* 	std::string type - Der Typ des ContentFiles. Wird vom Autor des ContentFiles gesetzt muss also nicht zwangsläufig
*   dem tatsächlichen Inhalt entsprechen
*	std::map <std::string,std::string> values - Die Felder des ContentFiles
*	std::map <std::string,ContentFile*> contentFiles - Die SubcontentFiles
* 	bool good - gibt an ob das ContentFile vollständig und korrekt geladen wurde.
*/

class ContentFile;

ContentFile *getErrorCF();

class ContentFile {
private:
	ContentFile *parent = nullptr;
	std::string type = "";
	std::map <std::string, std::string> values;
	std::map <std::string, ContentFile*> contentFiles;
	bool good = false;


	bool _getValRec(const std::string& var, std::string *ret) {
		auto it = values.find(var);
		if (it != values.end()) {
			*ret = it->second;
			return true;
		}
		else if (parent == nullptr) {
			return false;
		}
		else {
			return parent->_getValRec(var, ret);
		}
	}

public:
	ContentFile(ALLEGRO_FILE*);

	ContentFile(const std::string&, bool);
	ContentFile(const std::string&);

	~ContentFile();

	ContentFile *getMeACopyOfYou() const {
		/*! \brief erstellt eine Kopie des ContentFiles.
		*
		*	Die Subcontentfiles werden auch kopiert. Die Kopie kann also problemlos selbst zerstört werden und muss dies auch.
		*/
		ContentFile *cf = new ContentFile(type);

		cf->parent = this->parent;

		for (auto it = values.begin(); it != values.end(); it++) {
			cf->setValue(it->first, it->second);
		}

		for (auto it = contentFiles.begin(); it != contentFiles.end(); it++) {
			cf->setCF(it->first, it->second->getMeACopyOfYou());
		}
		return cf;
	}

	void saveToFile(const std::string&, bool);
	void saveToFile(ALLEGRO_FILE*);

	void load(const std::string&, bool);

	void print();

	std::string getValue(const std::string& var) const {
		/*! \brief Gibt den Wert des Feldes var zurück.
		*
		*	Unbekannte Elemente produzieren eine Exception
		*/
		return values.at(var);
	}

	std::string getValue(const std::string& var, const std::string& notvalue) const {
		/*! \brief Gibt den Wert des Feldes var zurück.
		*
		*	Wenn var nicht existiert wird notvalue zurückgegeben.
		*/
		auto it = values.find(var);
		if (it != values.end()) {
			return it->second;
		}
		else {
			throw(std::out_of_range("ContentFile::getValue"));
		}
	}

	std::string getValueRec(const std::string& var) {
		/*! \brief Gibt den Wert des Feldes var zurück, bei nicht-existenz fragt es seinen parent etc.
		*
		* Es wird geprüft, ob der Wert in diesem CF existiert, und wenn ja zurückgegeben.
		* Wenn er nicht existiert, wird rekursiv die getValueRecursive-Funktion des Eltern-ContentFiles
		* aufgerufen, bis dieses null ist. In diesem Fall wird std::out_of_range geworfenn */
		std::string str;
		if (this->_getValRec(var, &str)) return str;
		throw(std::out_of_range("ContentFile::getValueRecursive: Tried to access non-existent field"));
	}

	std::string getValueRec(const std::string& var, const std::string& notvalue) {
		std::string str;
		if (this->_getValRec(var, &str)) return str;
		return notvalue;
	}

	

	ContentFile *getCF(const std::string& id) const {
		/*! \brief Liefert das Subcontentfile id zurück.
		*
		*	Wenn dieses nicht existiert, wird das Parentcontentfile zurückgegeben. Beispiel:
		*		ContentFile *parent = new ContentFile(„TEST“);
		*		parent->setValue(parent->getCF(„child“)->getValue(„testvar“, „error“));
		*	parent->getCF(„child“) liefert hier wieder parent, da das Subcontentfile nicht existiert.
		*/
		return contentFiles.at(id);
	}

	ContentFile *getCFSafe(const std::string& id) const {
		/*! \brief Liefert das Subcontentfile id zurück. Niemals nullptr.
		*
		*	Wenn das Subcontentfile nicht existiert, wird ein Error-ContentFile zurückgegeben.
		*/
		if (contentFiles.find(id) == contentFiles.end()) {
			return getErrorCF();
		}
		else {
			return contentFiles.at(id);
		}
	}

	void setValue(const std::string& var, const std::string& value) {
		/*! \brief Setzt var auf value
		*
		*	Wenn das Element nicht erstellt werden kann, tritt undokumentiertes Verhalten auf.
		*/
		values[var] = value;
	}

	template<typename T>
	void setValue(const std::string& var, T value) {
		values[var] = std::to_string(value);
	}

	void setValue(const std::string& var, const char *value) {
		values[var] = value;
	}

	void setCF(const std::string& var, ContentFile *cf) {
		/*! \brief Setzt das Subcontentfile var auf cf
		*
		*	Wenn das Element nicht erstellt werden kann, tritt undokumentiertes Verhalten auf.
		*/
		if (!cf) {
			std::cerr << "[WARN] ContentFile::setCF: " << var << " = nullptr" << std::endl;
		}
		contentFiles[var] = cf;
		cf->parent = this;
	}

	void deleteValue(const std::string& str) {
		values.erase(str);
	}

	std::map<std::string, std::string>::iterator deleteValue(std::map<std::string, std::string>::iterator it) {
		return values.erase(it);
	}

	void deleteCF(const std::string& str) {
		contentFiles.erase(str);
	}

	const std::string& getType() const {
		/*! \brief Gibt den Typ des ContentFiles zurück
		*
		*	Dieser dient nur als Hilfe. Siehe "type"
		*/
		return type;
	}

	bool isGood() const {
		/*! \brief Gibt zurück, ob das ContentFile erfolgreich erstellt wurde.
		*
		*	Bei der Erstellung von ContentFiles aus  Dateien/dem data-String kann es passieren, dass die Datei nicht konform ist.
		*	Dabei wird keine Exception geliefert, sondern nur diese Variable geändert.
		*	ReadContentFiles achtet jedoch vor dem befüllen des Vectors automatisch, ob das ContentFile kompatibel ist.
		*/
		return good;
	}

	bool valueExists(const std::string& key) {
		return values.find(key) != values.end();
	}

	bool contentFileExists(const std::string& key) {
		return contentFiles.find(key) != contentFiles.end();
	}

	std::map<std::string, std::string>::iterator getVBeginIterator() {
		/*! \brief Gibt den Begin-Iterator der Variablen-Map zurück **/
		return values.begin();
	}

	std::map<std::string, std::string>::iterator getVEndIterator() {
		/*! \brief Gibt den End-Iterator der Variablen-Map zurück **/
		return values.end();
	}

	std::map<std::string, ContentFile*>::iterator getCFBeginIterator() {
		/*! \brief Gibt den Begin-Iterator der ContentFile-Map zurück */
		return contentFiles.begin();
	}

	std::map<std::string, ContentFile*>::iterator getCFEndIterator() {
		/* \brief Gibt den End-Iterator der ContentFile-Map zurück */
		return contentFiles.end();
	}

	size_t getValueCount() {
		return values.size();
	}
};

std::vector<ContentFile*> readContentFiles(const std::string& filename, int maxCount = -1);
void saveContentFileTo(ContentFile *cf, std::fstream& file, int tabs = 0);

inline ContentFile *findCF(const std::vector<ContentFile*> vec, const std::string& name, const std::string& value) {
	for (size_t i = 0; i < vec.size(); i++) {
		if (vec[i]->getValue(name, (value == "null" ? "X" : "null")) == value) return vec[i];
	}
	return nullptr;
}

void compressCFComparison(ContentFile *cf, ContentFile *comp);

#endif
