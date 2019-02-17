#pragma once
#include "stdafx.h"
#include "helper.h"

namespace expos {

	struct AnimationImage {
		Point<int> positionInBitmap;
	};

	struct AnimationFrame {
		size_t image, ticks;
	};

	class Animation;

	class AnimationInstance {
	private:
		Animation * animation;
		size_t frame, tick;
	public:
		AnimationInstance(Animation *animation) {
			this->animation = animation;
		}

		void update();
		void draw(Point<int>);

		size_t getFrame() {
			return frame;
		}

		size_t getTick() {
			return tick;
		}
	};

	class Animation {
	private:
		ID id, bitmap;
		std::vector<AnimationImage> images;
		std::vector<AnimationFrame> frames;
	public:
		Animation(ContentFile *cf);


		void draw(size_t, Point<int>);


		AnimationInstance createInstance() {
			AnimationInstance ai(this);
			return ai;
		}

		ID getBitmap() { return bitmap; }

		size_t getImageCount() { return images.size(); }
		size_t getFrameCount() { return frames.size(); }
		const AnimationImage& getImage(size_t i) {
			return images.at(i);
		}
		const AnimationFrame& getFrame(size_t i) {
			return frames.at(i);
		}
	};

}