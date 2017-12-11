#pragma once

#include <string>

#include <SFML/Audio/Music.hpp>

namespace rat {
	class Music {

	private:

		float _timeLeft;
		float _postTime;
		float _baseVolume;
		bool _isEnding = false;
		sf::Music _base;

	public:

		bool init(const std::string& fileName, float postTime, float volume);
		void update(float deltaTime);

		bool isEnding();
		bool finish(float deltaTime);
		void start(float deltaTime, float preTime);

		void play();
		void pause();
		void stop();

		float getVolume() const;
		void setVolume(float volume);

		sf::SoundSource::Status getStatus() const;

		float getPostTime() const;
		float getDuration() const;

	private:

		void reset();

		bool loadMusic(const std::string& fileName);

		std::string getPath(const std::string& fileName) const;

	};
}