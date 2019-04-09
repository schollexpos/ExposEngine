#pragma once
#include <map>
#include <vector>
#include <string>

namespace contentfile {
	enum TYPE {
		VALUE,
		OBJECT,
		ARRAY,
		ERROR
	};

	class Object;
	class Array;

	class CFather {
	protected:
		static Object empty;
	public:
	};

	std::ostream &printTabs(std::ostream&, int);

	template<typename T>
	class Container : public CFather {
	protected:
		
		std::map<T, Object*> objects;
		std::map<T, Array*> arrays;
		std::map<T, std::string> values;
	public:
		~Container() {
			for(std::pair<T, Object*> it : objects) {
				delete it.second;
			}

			for(std::pair<T, Array*> it : arrays) {
				delete it.second;
			}
		}

		Object *getObject(T key) { return objects.at(key); }
		Object *getCF(T key) { return getObject(key); }
		Object *getCFSafe(T key) { if(!isObject(key)) return &empty; return this->getCF(key); }
		Array *getArray(T key) { return arrays.at(key); }
		const std::string& getValue(T key) { return values.at(key); }

		const std::string& getValue(T key, const std::string& notvalue) {
			if(values.find(key) == values.end()) {
				return notvalue;
			} else {
				return values.at(key);
			}
		}

		void setObject(T key, Object *object) { objects[key] = object; }
		void setArray(T key, Array *arr) { arrays[key] = arr; }
		void setValue(T key, const std::string& value) { values[key] = value; }

		bool isObject(T key) { return objects.find(key) != objects.end(); }
		bool isArray(T key) { return arrays.find(key) != arrays.end(); }
		bool isValue(T key) { return values.find(key) != values.end(); }


		typename std::map< T,Object*>::iterator cfBegin() { return objects.begin(); }
		typename std::map<T, Object*>::iterator cfEnd() { return objects.end(); }
		typename std::map< T, Array*>::iterator arrayBegin() { return arrays.begin(); }
		typename std::map< T, Array*>::iterator arrayEnd() { return arrays.end(); }
		typename std::map< T, std::string>::iterator valueBegin() { return values.begin(); }
		typename std::map< T, std::string>::iterator valueEnd() { return values.end(); }

		void print(std::ostream& out, int tabs) {
			if(tabs==0) std::cout << "=== ContentFile-Container: ===" << std::endl;
			if(objects.size() > 0) {
				printTabs(out, tabs) << "ContentFiles: " << std::endl;
				for(std::pair<T, Object*> p : objects) {
					printTabs(out, tabs+1) << p.first << ": " << std::endl;
					p.second->print(out, tabs+2);
				}
			}
			if(arrays.size() > 0) {
				printTabs(out, tabs) << "Array: " << std::endl;
				for(std::pair<T, Array*> p : arrays) {
					 printTabs(out, tabs+1) << p.first << ": " << std::endl;
					p.second->print(out, tabs+2);
				}
			}
			if(values.size() > 0) {
				printTabs(out, tabs) << "Values: " << std::endl;
				for(std::pair<T, std::string> p : values) {
					printTabs(out, tabs+1) << p.first << ": " << p.second << std::endl;
				}
			}
		}

		void print(std::ostream& out) {
			print(out, 0);
		}

	};

	class Object : public Container<std::string> {
	private:
		Object *parent;
		std::string type;
	public:
		Object(const std::string& data, Object *parent = nullptr);

		const std::string& getValRecursive(const std::string& key);
		const std::string& getValRecursive(const std::string& key, const std::string& notvalue);

		Object *copy() {
			Object *o = new Object(*this);
			return o;
		}

		void saveToFile(std::fstream&);
	};

	class Array : public Container<size_t> {
	private:
		size_t mysize = 0;
	public:
		Array(const std::string& data);

		void push_back(Object *o) { objects[mysize++] = o; }
		void push_back(Array *a) { arrays[mysize++] = a; }
		void push_back(const std::string& str) { values[mysize++] = str; }

		void saveToFile(std::fstream &file);

		size_t getSize() { return mysize; }
	};
}

typedef contentfile::Object ContentFile;

ContentFile *readContentFile(const std::string& filename);