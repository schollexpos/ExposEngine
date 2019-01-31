#include "stdafx.h"
#include "messagebus.h"

namespace expos {
	MessageBus::MessageBus() : messages(50) {

	}

	MessageBus::~MessageBus() {

	}

	Message *MessageBus::createMessage() {
		return messages.get();
	}

}