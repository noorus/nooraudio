#include "nooraudio.h"

namespace nooraudio {

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

  HRESULT WASAPIDeviceEvents::_PrintDeviceName(LPCWSTR pwstrId)
  {
    HRESULT hr = S_OK;
    IMMDevice *pDevice = NULL;
    IPropertyStore *pProps = NULL;
    PROPVARIANT varString;

    CoInitialize(NULL);
    PropVariantInit(&varString);

    if (_pEnumerator == NULL)
    {
      // Get enumerator for audio endpoint devices.
      hr = CoCreateInstance(__uuidof(MMDeviceEnumerator),
        NULL, CLSCTX_INPROC_SERVER,
        __uuidof(IMMDeviceEnumerator),
        (void**)&_pEnumerator);
    }
    if (hr == S_OK)
    {
      hr = _pEnumerator->GetDevice(pwstrId, &pDevice);
    }
    if (hr == S_OK)
    {
      hr = pDevice->OpenPropertyStore(STGM_READ, &pProps);
    }
    if (hr == S_OK)
    {
      // Get the endpoint device's friendly-name property.
      hr = pProps->GetValue(PKEY_Device_FriendlyName, &varString);
    }
    printf("----------------------\nDevice name: \"%S\"\n"
      "  Endpoint ID string: \"%S\"\n",
      (hr == S_OK) ? varString.pwszVal : L"null device",
      (pwstrId != NULL) ? pwstrId : L"null ID");

    PropVariantClear(&varString);

    SAFE_RELEASE(pProps)
      SAFE_RELEASE(pDevice)
      CoUninitialize();
    return hr;
  }

}