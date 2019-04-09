#pragma once
#include <queue>
#include "messagebus.h"
#include "packfile.h"

namespace expos {

	class FileManager : public MessageReciever {
	private:
		std::vector<Packfile> packfiles;
		std::queue<Message> jobs;
	public:
		FileManager(MessageBus *mb) : MessageReciever(mb, RECIEVER_FILEMAN) {

		}

		void update();

		//returns true if the event was consumed and shouldn't be propagated further
		bool recieve(const Message&);

		//returns true if the message was answered (this can be either success or failure)
		virtual bool answerRequest(const Message&, Answer*) { return false; }

	};

}