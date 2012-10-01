#include "nooraudio.h"

namespace nooraudio {

  //-- WASAPIAudioDevice class ------------------------------------------------

  WASAPIAudioDevice::WASAPIAudioDevice( IMMDevice* device ):
  mDevice( device ), mProps( NULL ), mAudioClient( NULL ), mActive( false ),
  mWaveFormat( NULL ), mRequestDuration( REFTIMES_PER_SEC )
  {
    if ( FAILED( mDevice->OpenPropertyStore( STGM_READ, &mProps ) ) )
      throw std::exception( "Cannot open audio device property store" );
    PROPVARIANT pv;
    PropVariantInit( &pv );
    mProps->GetValue( PKEY_Device_FriendlyName, &pv );
    if ( pv.vt != VT_LPWSTR )
      throw std::exception( "Audio device friendly name not a string" );
    mName = pv.pwszVal;
    PropVariantClear( &pv );
  }

  IMMDevice* WASAPIAudioDevice::getIMMDevice()
  {
    return mDevice;
  }

  const wstring& WASAPIAudioDevice::getName()
  {
    return mName;
  }

  IAudioClient* WASAPIAudioDevice::getAudioClient()
  {
    return mAudioClient;
  }

  WAVEFORMATEX* WASAPIAudioDevice::getWaveFormatPtr()
  {
    return mWaveFormat;
  }

  bool WASAPIAudioDevice::isActive()
  {
    return mActive;
  }

  void WASAPIAudioDevice::activate()
  {
    if ( mActive )
      return;
    if ( FAILED( mDevice->Activate( IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&mAudioClient ) ) )
      throw std::exception( "Failed to activate audio client" );
    if ( FAILED( mAudioClient->GetMixFormat( &mWaveFormat ) ) )
      throw std::exception( "Failed to get audio mix format" );
  }

  void WASAPIAudioDevice::start()
  {
    if ( FAILED( mAudioClient->Start() ) )
      throw std::exception( "Failed to start audioclient" );
  }

  void WASAPIAudioDevice::initialize()
  {
    HRESULT hr = mAudioClient->Initialize( AUDCLNT_SHAREMODE_SHARED, 0, mRequestDuration, 0, mWaveFormat, NULL );
    if ( FAILED( hr ) ) {
      switch ( hr ) {
        case AUDCLNT_E_UNSUPPORTED_FORMAT:
          throw std::exception( "Audio client rejected requested wave format" );
        break;
        default:
          throw std::exception( "Failed to initialize audio client" );
        break;
      }
    }
  }

  void WASAPIAudioDevice::stop()
  {
    mAudioClient->Stop();
  }

  void WASAPIAudioDevice::deActivate()
  {
    if ( mWaveFormat ) {
      CoTaskMemFree( mWaveFormat );
      mWaveFormat = NULL;
    }
    SAFE_RELEASE( mAudioClient );
  }

  WASAPIAudioDevice::~WASAPIAudioDevice()
  {
    deActivate();
    SAFE_RELEASE( mProps );
    SAFE_RELEASE( mDevice );
  }

}