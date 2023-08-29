#pragma once

#include "IAudioLoader.h"

namespace nCine
{
	/// WAVE audio loader
	class AudioLoaderWav : public IAudioLoader
	{
	public:
		explicit AudioLoaderWav(std::unique_ptr<Death::IO::Stream> fileHandle);

		std::unique_ptr<IAudioReader> createReader() override;

	private:
		/// Header for the RIFF WAVE format
		struct WavHeader
		{
			char chunkId[4];
			uint32_t chunkSize;
			char format[4];

			char subchunk1Id[4];
			uint32_t subchunk1Size;
			uint16_t audioFormat;
			uint16_t numChannels;
			uint32_t sampleRate;
			uint32_t byteRate;
			uint16_t blockAlign;
			uint16_t bitsPerSample;

			char subchunk2Id[4];
			uint32_t subchunk2Size;
		};

	public:
		static const unsigned int HeaderSize = sizeof(WavHeader);
	};
}
