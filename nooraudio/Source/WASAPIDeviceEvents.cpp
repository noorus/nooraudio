#include "nooraudio.h"

namespace nooraudio {

  using std::wstring;

  //-- WASAPIDeviceEvents class -----------------------------------------------

  WASAPIDeviceEvents::WASAPIDeviceEvents( WASAPIAudioPlayer* pPlayer ):
  mPlayer( pPlayer ), mRefCount( 1 )
  {
    //
  }

  WASAPIDeviceEvents::~WASAPIDeviceEvents()
  {
    //
  }

  ULONG WASAPIDeviceEvents::AddRef()
  {
    return InterlockedIncrement( &mRefCount );
  }

  ULONG WASAPIDeviceEvents::Release()
  {
    ULONG ulRef = InterlockedDecrement( &mRefCount );
    if ( !ulRef )
      delete this;
    return ulRef;
  }

  HRESULT WASAPIDeviceEvents::QueryInterface( REFIID riid, VOID** ppvInterface )
  {
    if ( riid == IID_IUnknown ) {
      AddRef();
      *ppvInterface = (IUnknown*)this;
    } else if ( riid == IID_IMMNotificationClient ) {
      AddRef();
      *ppvInterface = (IMMNotificationClient*)this;
    } else {
      *ppvInterface = NULL;
      return E_NOINTERFACE;
    }
    return S_OK;
  }

  HRESULT WASAPIDeviceEvents::OnDefaultDeviceChanged( EDataFlow flow,
  ERole role, LPCWSTR pwstrDeviceId )
  {
    mPlayer->onDefaultDeviceChanged( flow, role, pwstrDeviceId );
    return S_OK;
  }

  HRESULT WASAPIDeviceEvents::OnDeviceAdded( LPCWSTR pwstrDeviceId )
  {
    return S_OK;
  }

  HRESULT WASAPIDeviceEvents::OnDeviceRemoved( LPCWSTR pwstrDeviceId )
  {
    return S_OK;
  }

  HRESULT WASAPIDeviceEvents::OnDeviceStateChanged( LPCWSTR pwstrDeviceId,
  DWORD dwNewState )
  {
    return S_OK;
  }

  HRESULT WASAPIDeviceEvents::OnPropertyValueChanged( LPCWSTR pwstrDeviceId,
  const PROPERTYKEY key )
  {
    return S_OK;
  }

}