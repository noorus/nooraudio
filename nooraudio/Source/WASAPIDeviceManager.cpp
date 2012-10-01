#include "nooraudio.h"

namespace nooraudio {

  using std::wstring;

  WASAPIDeviceManager::WASAPIDeviceManager( WASAPIAudioPlayer* pPlayer ):
  mEnumerator( NULL ), mEvents( NULL ), mPlayer( pPlayer )
  {
    if ( FAILED( CoCreateInstance( CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL,
      IID_IMMDeviceEnumerator, (void**)&mEnumerator ) ) )
      throw std::exception( "Failed to instance multimedia device enumerator" );
    mEvents = new WASAPIDeviceEvents( mPlayer );
    mEnumerator->RegisterEndpointNotificationCallback( mEvents );
  }

  WASAPIAudioDevice* WASAPIDeviceManager::createDefaultDevice()
  {
    IMMDevice* pRawDevice;
    if ( FAILED( mEnumerator->GetDefaultAudioEndpoint( eRender, eMultimedia, &pRawDevice ) ) )
      throw std::exception( "Failed to get default audio endpoint" );
    return new WASAPIAudioDevice( pRawDevice );
  }

  WASAPIDeviceManager::~WASAPIDeviceManager()
  {
    if ( mEvents ) {
      mEnumerator->UnregisterEndpointNotificationCallback( mEvents );
      delete mEvents;
    }
    SAFE_RELEASE( mEnumerator );
  }

}