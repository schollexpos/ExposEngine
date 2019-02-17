
#include "stdafx.h"
#include "animation.h"
#include "contentfile.h"

namespace expos {


	Animation::Animation(ContentFile *cf) : id(cf->getValue("id", "null")), bitmap(cf->getValue("bitmap", "null")) {

	}

	void Animation::draw(size_t frame, Point<int> pos) {

	}

	void AnimationInstance::update() {
		tick++;
		if (tick >= animation->getFrame(frame).ticks) {
			frame++;
			tick = 0;
			if (frame >= animation->getFrameCount()) frame = 0;
		}
	}

	void AnimationInstance::draw(Point<int> pos) {
		animation->draw(frame, pos);
	}

}