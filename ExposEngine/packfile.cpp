#include "stdafx.h"
#include "packfile.h"

namespace expos {

	ALLEGRO_FILE *openMemfile(const PackfileFile& file, const char* mode) {
		return al_open_memfile(file.content, file.size, mode);
	}

	constexpr int PACKFILE_VERSION = 0x01;
	const char *PACKFILE_MAGICENDFILENAME = "!LISTEND._EE";

	Packfile::Packfile() {
		files.push_back({ PACKFILE_MAGICENDFILENAME , nullptr, 0x00, 0x00 });
	}

	Packfile::~Packfile() {
		if (content != nullptr) {
			delete[] content;
		}
	}

	void Packfile::read(const std::string& inputfile) {
		std::ifstream in(inputfile, std::ios::in | std::ios::binary);

		char header[4];
		in.read(header, 4);
		if (header[0] != 'E' || header[1] != 'P' || header[2] != 'F') {

		} else if (header[3] > PACKFILE_VERSION) {

		} else if (header[3] < PACKFILE_VERSION) {

		}

		files.empty();

		while (true) {
			uint8_t filenameLen = in.get();
			std::string filename;
			uint64_t offset, size;
			filename.resize(filenameLen);
			in.read(&filename[0], filenameLen);
			in.read((char*)&offset, 8);
			in.read((char*)&size, 8);

			files.push_back({ filename, nullptr, offset, size });

			if (filename == PACKFILE_MAGICENDFILENAME) break;
		}

		uint64_t len = files.back().offset;
		this->resize( (size_t) len);
		in.read((char*) content, len);
		usedContentSize = (size_t) len;

		updateFilePointers();
	}

	void Packfile::write(const std::string& outputfile) {
		std::ofstream out(outputfile, std::ios::out | std::ios::binary);
		
		out.write("EPF", 3);
		out.put(PACKFILE_VERSION);
		for (PackfileFile f : files) {
			out.put((size_t) f.name.size());
			out.write(f.name.c_str(), f.name.size());
			out.write((char*) &f.offset, 8);
			out.write((char*)&f.size, 8);
		}

		out.write((char*) content, usedContentSize);

		out.close();
	}

	void Packfile::extract(const std::string& foldername) {
		for (size_t i = 0; i < files.size() - 1;i++) {
			std::ofstream out(foldername + files[i].name, std::ios::out | std::ios::binary);
			out.write((char*)content + files[i].offset, files[i].size);
		}
	}

	void Packfile::addFile(const std::string& inputfile, const std::string& pfFilename) {
		std::ifstream in(inputfile, std::ios::in | std::ios::binary);

		if (in) {
			in.seekg(0, std::ios::end);
			size_t fileSize = (size_t) in.tellg();
			size_t fileOffset = (size_t)this->files.back().offset; //last file always size zero
			if (contentSize < usedContentSize + fileSize) {
				this->resize(usedContentSize + fileSize);
			}
			usedContentSize += fileSize;

			in.seekg(0, std::ios::beg);
			PackfileFile file = {pfFilename, (char*)content+fileOffset, fileOffset, fileSize};
			files.insert(files.end() - 1, file);

			in.read(((char*) content + fileOffset), fileSize);

			in.close();

			files.back().offset = fileOffset + fileSize;
		}
	}
	
	void Packfile::resize(size_t size) {
		void *newbuf = new void*[size];

		memcpy(newbuf, content, (size > contentSize ? contentSize : size));
		
		delete[] content;
		content = newbuf;
		contentSize = size;
		updateFilePointers();
	}




}