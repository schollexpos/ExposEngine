#include "stdafx.h"
#include "filemanager.h"
#include "stringmanager.h"

namespace expos {

	bool FileManager::recieve(const Message& message) {
		std::cout << "REC!" << std::endl;
		switch(message.type) {
			case MESSAGE_FILEMANAGER_LOADPF:
				std::cout << "MESSAGE LOAD PF " << std::endl;
				jobs.push(message);
				break;
			default:
				break;
		}
		return false;
	}

	void FileManager::update() {
		while(!jobs.empty()) {
			switch(jobs.front().type) {
				case MESSAGE_FILEMANAGER_LOADPF:
				packfiles.push_back(Packfile());
				packfiles.back().read(stringManager->get(jobs.front().data.filemanager.filename));
				break;
			}

			jobs.pop();
		}
	}


}