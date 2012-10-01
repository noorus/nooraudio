#include "nooraudio.h"
#include "VorbisAudioSource.h"

namespace nooraudio {

  inline bool splitTag( const std::wstring& str, StringPair& pair )
  {
    size_t sep = str.find_first_of( L"=" );
    if ( sep == std::string::npos )
      return false;
    std::wstring tag = str.substr( 0, sep );
    std::wstring val = str.substr( sep+1 );
    pair = make_pair( tag, val );
    return true;
  }

  //-- VorbisAudioSource class ------------------------------------------------

  VorbisAudioSource::VorbisAudioSource( const std::wstring& fileName,
  SampleSize sampleSize ):
  AudioSource( fileName, sampleSize ), mCurrentSection( 0 ),
  mTotalTime( 0.0f ), mCurrentTime( 0.0f )
  {
    if ( ov_open( mFile, &mVorbisFile, NULL, 0 ) < 0 )
      throw std::exception( "Input file not a valid Ogg bitstream" );
    mInfo = ov_info( &mVorbisFile, -1 );
    mTotalTime = ov_time_total( &mVorbisFile, -1 );
    //wprintf_s( L"- Encoder: %S\r\n", ov_comment( &mVorbisFile, -1 )->vendor );
    char **ptr = ov_comment( &mVorbisFile, -1 )->user_comments;
    while ( *ptr ) {
      std::wstring str = Utilities::textStringToWString( *ptr );
      StringPair pair;
      if ( splitTag( str, pair ) )
        mTagMap.insert( pair );
      ptr++;
    }
  }

  VorbisAudioSource::~VorbisAudioSource()
  {
    ov_clear( &mVorbisFile );
  }

  long VorbisAudioSource::getChannels() {
    return mInfo->channels;
  }

  long VorbisAudioSource::getSampleRate() {
    return mInfo->rate;
  }

  long VorbisAudioSource::getBitRate() {
    return mInfo->bitrate_nominal ? mInfo->bitrate_nominal : 0;
  }

  float VorbisAudioSource::getTotalTime()
  {
    return mTotalTime;
  }

  float VorbisAudioSource::getCurrentTime()
  {
    return mCurrentTime;
  }

  const StringMap& VorbisAudioSource::getTagMap()
  {
    return mTagMap;
  }

  void VorbisAudioSource::setFormat( WAVEFORMATEX* waveFormat )
  {
    memset( waveFormat, NULL, sizeof ( WAVEFORMATEX ) );
    waveFormat->wFormatTag = WAVE_FORMAT_PCM;
    waveFormat->wBitsPerSample = mSampleBits;
    waveFormat->nChannels = mInfo->channels;
    waveFormat->nSamplesPerSec = mInfo->rate;
    waveFormat->nBlockAlign = ( waveFormat->nChannels * mSampleBytes );
    waveFormat->nAvgBytesPerSec = waveFormat->nSamplesPerSec * waveFormat->nBlockAlign;
    waveFormat->cbSize = 0;
    mWaveFormat = waveFormat;
  }

  void VorbisAudioSource::render( long frames, BYTE* data, DWORD& flags )
  {
    int bytes = frames * mWaveFormat->nChannels * mSampleBytes;
    // int written = 0;
    bool reading = true;
    flags = 0;
    while ( reading )
    {
      long ret = ov_read( &mVorbisFile, (char*)data, bytes, 0, mSampleBytes, 1, &mCurrentSection );
      if ( ret < 0 ) {
        throw std::exception( "ov_read failed" );
      } else if ( ret == 0 ) {
        // weird: i don't think we should set this flag unless it's the FIRST
        // write that failed, but this way it works
        flags = AUDCLNT_BUFFERFLAGS_SILENT;
        return;
      }
      bytes -= ret;
      // written += ret;
      data = (BYTE*)((DWORD)data + ret);
      if ( bytes < 1 || ret < 1 )
        reading = false;
    }
    mCurrentTime = ov_time_tell( &mVorbisFile );
  }

  void VorbisAudioSource::renderLooping( long frames, BYTE* data, DWORD& flags )
  {
    int bytes = frames * mWaveFormat->nChannels * mSampleBytes;
    bool reading = true;
    flags = 0;
    while ( reading )
    {
      long ret = ov_read( &mVorbisFile, (char*)data, bytes, 0, mSampleBytes, 1, &mCurrentSection );
      if ( ret < 0 ) {
        throw std::exception( "ov_read failed" );
      } else if ( ret == 0 ) {
        if ( ov_raw_seek_lap( &mVorbisFile, 0 ) != 0 )
          throw std::exception( "ov_raw_seek_lap failed" );
        continue;
      }
      bytes -= ret;
      data = (BYTE*)((DWORD)data + ret);
      if ( bytes < 1 || ret < 1 )
        reading = false;
    }
    mCurrentTime = ov_time_tell( &mVorbisFile );
  }

}