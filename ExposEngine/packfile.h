#pragma once
#include "stdafx.h"
#include <allegro5/allegro_memfile.h>

namespace expos {

	struct PackfileFile {
		std::string name;
		void *content;
		uint64_t offset, size;
	};

	ALLEGRO_FILE *openMemfile(const PackfileFile& file, const char* mode);

	/* \brief A Packfile represents a collection of different logical files, saved as one physical
		file on disk
	*
	*	File format (little endian):
	*	EPF{VERSION NUMBER}{INDEX}{FILES}
	*	VERSION NUMBER:
	*		always 0x01
	*	INDEX:
	*		{FILENAMESIZE}{FILENAME}{OFFSET}{SIZE}
	*		FILENAMESIZE:
	*			Single Byte to indicate length of the filename
	*		FILENAME:
	*			String
	*			Can contain forward slashes ('/') for grouping files in folders
	*		OFFSET:
	*			64-Bit Integer (offset is counted from the beginning of the actual files
	*		SIZE
	*			64-Bit Integer
	*		The INDEX is ended by a file called !LISTEND._EE (size must be zero!)
	*	FILES
	*/
	class Packfile {
	private:
		std::vector<PackfileFile> files;
		void *content = nullptr;
		size_t contentSize = 0, usedContentSize = 0;

		void updateFilePointers() {
			for (PackfileFile f : files) {
				f.content = (char*)content + f.offset;
			}
		}
	public:
		Packfile();
		~Packfile();

		void read(const std::string&);
		void write(const std::string&);

		void extract(const std::string&);

		void addFile(const std::string&, const std::string&);

		void resize(size_t);

		void shrink() {
			this->resize(usedContentSize);
		}

		size_t getFileCount() {
			return files.size();
		}

		const PackfileFile& getFile(size_t n) {
			return files[n];
		}

		size_t getBufferSize() {
			return contentSize;
		}

	};

}