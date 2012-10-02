#pragma once

#include "nooraudio.h"
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

typedef std::map<std::wstring,std::wstring> StringMap;
typedef std::pair<std::wstring,std::wstring> StringPair;

class VorbisAudioSource: public nooraudio::AudioSource {
protected:
  OggVorbis_File mVorbisFile;
  vorbis_info* mInfo;
  int mCurrentSection;
  double mTotalTime;
  double mCurrentTime;
  StringMap mTagMap;
  bool mLooping;
public:
  VorbisAudioSource( const std::wstring& fileName, nooraudio::SampleSize sampleSize );
  ~VorbisAudioSource();
  long getChannels();
  long getSampleRate();
  long getBitRate();
  double getTotalTime();
  double getCurrentTime();
  const StringMap& getTagMap();
  void setLooping( bool looping );
  void setFormat( WAVEFORMATEX* waveFormat );
  void render( long frames, BYTE* data, DWORD& flags );
};