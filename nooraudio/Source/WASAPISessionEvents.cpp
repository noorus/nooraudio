#include "nooraudio.h"

namespace nooraudio {

  //-- WASAPISessionEvents class ----------------------------------------------

  WASAPISessionEvents::WASAPISessionEvents( WASAPIAudioPlayer* pPlayer ):
  mPlayer( pPlayer ), mRefCount( 1 )
  {
    //
  }

  WASAPISessionEvents::~WASAPISessionEvents()
  {
    //
  }

  ULONG WASAPISessionEvents::AddRef()
  {
    return InterlockedIncrement( &mRefCount );
  }

  ULONG WASAPISessionEvents::Release()
  {
    ULONG ulRef = InterlockedDecrement( &mRefCount );
    if ( !ulRef )
      delete this;
    return ulRef;
  }

  HRESULT WASAPISessionEvents::QueryInterface( REFIID riid, VOID** ppvInterface )
  {
    if ( riid == IID_IUnknown ) {
      AddRef();
      *ppvInterface = (IUnknown*)this;
    } else if ( riid == IID_IAudioSessionEvents ) {
      AddRef();
      *ppvInterface = (IAudioSessionEvents*)this;
    } else {
      *ppvInterface = NULL;
      return E_NOINTERFACE;
    }
    return S_OK;
  }

  HRESULT WASAPISessionEvents::OnDisplayNameChanged( LPCWSTR NewDisplayName,
  LPCGUID EventContext )
  {
    wprintf_s( L"WASAPISessionEvent: New display name %s\r\n", NewDisplayName );
    return S_OK;
  }

  HRESULT WASAPISessionEvents::OnIconPathChanged( LPCWSTR NewIconPath,
  LPCGUID EventContext )
  {
    wprintf_s( L"WASAPISessionEvent: New icon path %s\r\n", NewIconPath );
    return S_OK;
  }

  HRESULT WASAPISessionEvents::OnSimpleVolumeChanged( float NewVolume,
  BOOL NewMute, LPCGUID EventContext )
  {
    wprintf_s( L"WASAPISessionEvent: New volume %.2f (%s)\r\n", NewVolume, NewMute ? L"Muted" : L"Unmuted" );
    return S_OK;
  }

  HRESULT WASAPISessionEvents::OnChannelVolumeChanged( DWORD ChannelCount,
  float NewChannelVolumeArray[], DWORD ChangedChannel, LPCGUID EventContext )
  {
    return S_OK;
  }

  HRESULT WASAPISessionEvents::OnGroupingParamChanged(
  LPCGUID NewGroupingParam, LPCGUID EventContext )
  {
    return S_OK;
  }

  HRESULT WASAPISessionEvents::OnStateChanged( AudioSessionState NewState )
  {
    return S_OK;
  }

  HRESULT WASAPISessionEvents::OnSessionDisconnected(
  AudioSessionDisconnectReason DisconnectReason )
  {
    wprintf_s( L"WASAPISessionEvent: Session disconnected!\r\n" );
    mPlayer->onDeviceLost();
    return S_OK;
  }

}