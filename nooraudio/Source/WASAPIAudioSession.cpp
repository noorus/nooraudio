#include "nooraudio.h"

namespace nooraudio {

  using std::wstring;

  //-- WASAPIAudioSession -----------------------------------------------------

  WASAPIAudioSession::WASAPIAudioSession( WASAPIAudioPlayer* pPlayer,
  IAudioClient* pClient ): mPlayer( pPlayer ), mSessionControl( NULL ),
  mEvents( NULL )
  {
    if ( FAILED( pClient->GetService( IID_IAudioSessionControl, (void**)&mSessionControl ) ) )
      throw std::exception( "Failed to get audio render service" );
    mEvents = new WASAPISessionEvents( mPlayer );
    mSessionControl->RegisterAudioSessionNotification( mEvents );
  }

  void WASAPIAudioSession::setIcon( const wstring& sPath )
  {
    mSessionControl->SetIconPath( sPath.c_str(), NULL );
  }

  void WASAPIAudioSession::setName( const wstring& sName )
  {
    mSessionControl->SetDisplayName( sName.c_str(), NULL );
  }

  WASAPIAudioSession::~WASAPIAudioSession()
  {
    if ( mEvents )
      mSessionControl->UnregisterAudioSessionNotification( mEvents );
    SAFE_DELETE( mEvents );
    SAFE_RELEASE( mSessionControl );
  }

}