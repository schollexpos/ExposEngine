#include "stdafx.h"
#include "packfile.h"
#include "helper.h"

namespace expos {

	ALLEGRO_FILE *openMemfile(const PackfileFile& file, const char* mode) {
		return al_open_memfile(file.content, file.size, mode);
	}

	constexpr int PACKFILE_VERSION = 0x01;
	const char *PACKFILE_MAGICENDFILENAME = "!LISTEND._EE";

	Packfile::Packfile() {
		files.push_back({ {"", PACKFILE_MAGICENDFILENAME} , nullptr, 0x00, 0x00 });
	}

	Packfile::~Packfile() {
		if (content != nullptr) {
			delete[] content;
		}
	}

	Filename Packfile::getFilename(const std::string& fullFilename) {
		ALLEGRO_PATH *path = al_create_path(fullFilename.c_str());

		Filename filename;

		filename.filename = al_get_path_filename(path);
		for (int i = 0; i < al_get_path_num_components(path); i++) 
			filename.path += std::string(al_get_path_component(path, i)) + "/";

		al_destroy_path(path);

		std::cout << "Filename: " << fullFilename << " -> \"" << filename.path << "\" \"" << filename.filename << "\"" << std::endl;
		return filename;
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
			std::string fullFilename;
			uint64_t offset, size;
			fullFilename.resize(filenameLen);
			in.read(&fullFilename[0], filenameLen);
			in.read((char*)&offset, 8);
			in.read((char*)&size, 8);

			Filename filename = getFilename(fullFilename);
			files.push_back({filename, nullptr, offset, size });


			if (filename.filename == PACKFILE_MAGICENDFILENAME) break;
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
			out.put((size_t) f.filename.getFull().length());
			out.write(f.filename.getFull().c_str(), f.filename.getFull().length());
			out.write((char*) &f.offset, sizeof(f.offset));
			out.write((char*) &f.size, sizeof(f.size));
		}

		out.write((char*) content, usedContentSize);

		out.close();
	}

	void Packfile::extract(const std::string& foldername) {
		for (size_t i = 0; i < files.size() - 1;i++) {
			al_make_directory((foldername + files[i].filename.path).c_str());

			std::ofstream out(foldername + files[i].filename.path + files[i].filename.filename, std::ios::out | std::ios::binary);
			out.write((char*)content + files[i].offset, files[i].size);
		}
	}

	void Packfile::addFile(const std::string& inputfile, const std::string& pfFilename) {
		std::cout << "Packfile::addFile(" << inputfile << ", " << pfFilename << ")" << std::endl;
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

			Filename filename = getFilename(pfFilename);
			PackfileFile file = {filename, (char*)content+fileOffset, fileOffset, fileSize};
			files.insert(files.end() - 1, file);

			in.read(((char*) content + fileOffset), fileSize);

			in.close();

			files.back().offset = fileOffset + fileSize;
		}
	}

	void Packfile::addFolder(const std::string& inputfolder, const std::string& pfFoldername) {
		ALLEGRO_FS_ENTRY *folder = al_create_fs_entry(inputfolder.c_str());

		if (al_fs_entry_exists(folder) && al_open_directory(folder)) {
			ALLEGRO_FS_ENTRY *file = nullptr;
			while (file = al_read_directory(folder)) {

				Filename fn = this->getFilename(al_get_fs_entry_name(file));
				fn.path = pfFoldername;

				this->addFile(al_get_fs_entry_name(file), fn.getFull());

				al_destroy_fs_entry(file);
			}

			al_close_directory(folder);
		}

		al_destroy_fs_entry(folder);
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