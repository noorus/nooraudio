#pragma once

#include "nooraudio.h"
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

namespace Utilities {

  inline std::wstring textStringToWString( const std::string& sIn )
  {
    DWORD dwLength = MultiByteToWideChar( CP_UTF8, NULL, sIn.c_str(), -1, NULL, NULL );
    if ( dwLength == 0 )
      throw new std::exception( "MultiByteToWideChar failed" );
    std::vector<WCHAR> vConversion( dwLength );
    DWORD dwConverted = MultiByteToWideChar( CP_UTF8, NULL,
      sIn.c_str(), -1, &vConversion[0], dwLength );
    if ( dwConverted != dwLength )
      throw new std::exception( "MultiByteToWideChar failed" );
    return std::wstring( &vConversion[0] );
  }

}

namespace nooraudio {

  typedef std::map<std::wstring,std::wstring> StringMap;
  typedef std::vector<std::wstring> StringVector;
  typedef boost::char_separator<wchar_t> CharSeparator;
  typedef boost::tokenizer<boost::char_separator<wchar_t>> CharTokenizer;

  typedef std::pair<std::wstring,std::wstring> StringPair;

  class VorbisAudioSource: public AudioSource {
  protected:
    OggVorbis_File mVorbisFile;
    vorbis_info* mInfo;
    int mCurrentSection;
    float mTotalTime;
    float mCurrentTime;
    StringMap mTagMap;
  public:
    VorbisAudioSource( const std::wstring& fileName, SampleSize sampleSize );
    ~VorbisAudioSource();
    long getChannels();
    long getSampleRate();
    long getBitRate();
    virtual float getTotalTime();
    virtual float getCurrentTime();
    const StringMap& getTagMap();
    void setFormat( WAVEFORMATEX* waveFormat );
    void render( long frames, BYTE* data, DWORD& flags );
    void renderLooping( long frames, BYTE* data, DWORD& flags );
  };

}