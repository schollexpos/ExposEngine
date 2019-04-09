#include "stdafx.h"
#include "contentfile.h"

namespace contentfile {

#define NONPRINTABLE " \t\n"

	std::ostream &printTabs(std::ostream& stream, int t) {
		for(int i = 0; i<t; i++) {
			stream << "\t";
		}
		return stream;
	}

	enum CONT {
		CURLYB = 0,
		SQUAREB = 1,
		QUOTE = 2,
		CONTEND
	};

	enum SPLITTYPE {
		ST_ARRAY,
		ST_KEYVAL,
	};


	Object CFather::empty("#ERROR{}");

	static std::vector<std::string> split(const std::string& data, SPLITTYPE st) {
		std::cout << "Split: \"" << data << "\"" << std::endl;
		std::vector<std::string> vec;
		std::string cur = "";



		int open[CONTEND] = {0,0,0};
		bool keyvalPos = false; //false = key, true = val
		bool finished = false;
		for(size_t pos = 0; !finished && pos < data.length(); pos++) {
			bool noAdd = false;
			bool curFinished = false;

			switch(data[pos]) {
				case '{':
					open[CURLYB]++;
					break;
				case '}':
					open[CURLYB]--;
					break;
				case '[':
					open[SQUAREB]++;
					break;
				case ']':
					open[SQUAREB]--;
					break;
				case '\\':
					if(!open[CURLYB] && !open[SQUAREB] && (pos> 0 ? data[pos-1] != '\\' : true)) {
						noAdd = true;
					}
					break;
				case '"':
					if(!open[CURLYB] && !open[SQUAREB] && (pos> 0 ? data[pos-1] != '\\' : true)) {
						open[QUOTE] = !open[QUOTE];
						noAdd = true;
					}
					break;
				case ' ':
				case '\t':
					if(!open[CURLYB] && !open[SQUAREB] && !open[QUOTE]) noAdd = true;
					break;
				case '\n':
					noAdd = true;
					break;
			}


			switch(st) {
				case ST_ARRAY:
					if(!open[CURLYB] && !open[SQUAREB] && !open[QUOTE] && (data[pos] == ',' || pos == data.size()-1)) {
						noAdd = (data[pos] == ',' || data[pos] == '"');
						curFinished = true;
					}
					break;
				case ST_KEYVAL:
					if(!open[CURLYB] && !open[SQUAREB]  && !open[QUOTE]) {
						if(keyvalPos == false && data[pos] == ':') {
							keyvalPos = true;
							noAdd = true;
							curFinished = true;
						} else if(keyvalPos == true && (data[pos] == ';' || pos == data.size()-1)) {
							noAdd = (data[pos] == ';' || data[pos] == '"');
							curFinished = true;
							keyvalPos = false;
						}
					}
					break;
			}

			if(!noAdd) {
				cur += data[pos];
				noAdd = false;
			}

			if(curFinished) {
				vec.push_back(cur);
				cur = "";
				curFinished = false;
			}
		}

		return vec;
	}

	static TYPE identify(const std::string& value) {
		switch(value[0]) {
			case '[':
				return ARRAY;
			case '{':
			case '#':
				return OBJECT;
			case ']':
			case '}':
			case ',':
				return ERROR;
			default:
				return VALUE;
		}
	}

	std::string makeVal(const std::string& data) {
		size_t startPos = data.find_first_not_of(NONPRINTABLE);
		size_t endPos = data.find_last_not_of(NONPRINTABLE);
		if(data[startPos] == '"') {
			return data.substr(1, endPos-startPos+1);
		} else {
			return data;
		}
	}

	Array::Array(const std::string& data) {
		size_t startPos = data.find_first_not_of(NONPRINTABLE);
		size_t endPos = data.find_last_not_of(NONPRINTABLE);
		std::vector<std::string> elements = split(data.substr(startPos+1, endPos-(startPos+1)), ST_ARRAY);

		for(const std::string& str : elements) {
			TYPE t = identify(str);
			switch(t) {
				case ARRAY:
					this->push_back(new Array(str));
					break;
				case OBJECT:
					this->push_back(new Object(str));
					break;
				case VALUE:
					this->push_back(makeVal(str));
					break;
			}
		}
	}

	void Array::saveToFile(std::fstream &file) {
		file << "[";

		for(auto it = objects.begin(); it != objects.end(); it++) {
			it->second->saveToFile(file);
			file << ",";
		}
		for(auto it = arrays.begin(); it != arrays.end(); it++) {
			it->second->saveToFile(file);
			file << ";";
		}
		for(auto it = values.begin(); it != values.end(); it++) {
			file << "\"" << it->second << "\",";
		}

		file << "]";
	}

	Object::Object(const std::string& data, Object *parent) : parent(parent) {
		size_t startpos = data.find_first_not_of(NONPRINTABLE);
		size_t endpos = data.find_last_not_of(NONPRINTABLE);
		if(data[startpos] == '#') {
			size_t oldsp = startpos;
			startpos = data.find_first_of('{');
			type = data.substr(oldsp+1, startpos-(oldsp+1));

			std::cout << oldsp << "-" << startpos << " -> \"" << type << "\"" << std::endl;
		} else if(data[startpos] == '{') {
			type = "ANONYMOUS";
		}

		std::vector<std::string> vec = split(data.substr(startpos+1, endpos-(startpos+1)), ST_KEYVAL);

		for(int i = 0; i < vec.size(); i++) {
			std::cout << "vec[" << i << "] = \"" << vec[i] << "\"" << std::endl;
		}

		for(size_t i = 0; i+1 < vec.size();i+=2) {
			TYPE t = identify(vec[i+1]);

			switch(t) {
				case ARRAY:
					this->setArray(vec[i], new Array(vec[i+1]));
					break;
				case OBJECT:
					this->setObject(vec[i], new Object(vec[i+1], this));
					break;
				case VALUE:
					this->setValue(vec[i], makeVal(vec[i+1]));
					break;
			}
		}
	}

	const std::string& Object::getValRecursive(const std::string& key) {
		if(values.find(key) == values.end()) {
			if(this->parent) {
				return parent->getValRecursive(key);
			} else {
				throw std::out_of_range("Object::getValRecursive");
			}
		} else {
			return values.at(key);
		}
	}

	const std::string& Object::getValRecursive(const std::string& key, const std::string& notvalue) {
		if(values.find(key) == values.end()) {
			if(this->parent) {
				return parent->getValRecursive(key);
			} else {
				return notvalue;
			}
		} else {
			return values.at(key);
		}
	}

	void Object::saveToFile(std::fstream &file) {
		if(type != "ANONYMOUS") {
			file << "#" << type;
		}
		file << "{" << std::endl;

		for(auto it = objects.begin(); it != objects.end(); it++) {
			file << it->first << ":";
			it->second->saveToFile(file);
			file << ";" << std::endl;
		}
		for(auto it = arrays.begin(); it != arrays.end(); it++) {
			file << it->first << ":";
			it->second->saveToFile(file);
			file << ";" << std::endl;
		}
		for(auto it = values.begin(); it != values.end(); it++) {
			file << it->first << ":\"" << it->second << "\";" << std::endl;
		}

		file << "}";
	}
}

ContentFile *readContentFile(const std::string& filename) {
	std::fstream file(filename, std::ios::in);

	file.seekg(0, file.end);
	auto length = file.tellg();
	file.seekg(0, file.beg);

	std::string data;
	data.resize(length);

	file.read(&data[0], length);

	return new ContentFile(data);
}