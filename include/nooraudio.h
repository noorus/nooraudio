#pragma once

// Windows Vista or newer
#define NTDDI_VERSION NTDDI_VISTA
#define _WIN32_WINNT _WIN32_WINNT_VISTA
#include <sdkddkver.h>

// Generic headers
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// COM/WASAPI
#include <comdef.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <audiopolicy.h>
#include <functiondiscoverykeys_devpkey.h>

// STL
#include <exception>
#include <vector>
#include <string>
#include <map>

// Boost
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/random.hpp>
#include <boost/tokenizer.hpp>

namespace nooraudio {

# define REFTIMES_PER_SEC       10000000
# define REFTIMES_PER_MILLISEC  10000

# define _STDC STDMETHODCALLTYPE

  const CLSID CLSID_MMDeviceEnumerator  = __uuidof( MMDeviceEnumerator );
  const IID   IID_IMMDeviceEnumerator   = __uuidof( IMMDeviceEnumerator );
  const IID   IID_IMMNotificationClient = __uuidof( IMMNotificationClient );
  const IID   IID_IAudioClient          = __uuidof( IAudioClient );
  const IID   IID_IAudioRenderClient    = __uuidof( IAudioRenderClient );
  const IID   IID_IAudioSessionControl  = __uuidof( IAudioSessionControl );
  const IID   IID_IAudioSessionEvents   = __uuidof( IAudioSessionEvents );

  class WASAPIDeviceEvents: public IMMNotificationClient {
  protected:
    WASAPIAudioPlayer* mPlayer;
    long mRefCount;
    IMMDeviceEnumerator *_pEnumerator;
    HRESULT _PrintDeviceName(LPCWSTR pwstrId);
  public:
    WASAPIDeviceEvents( WASAPIAudioPlayer* player );
    ~WASAPIDeviceEvents();
    ULONG _STDC AddRef();
    ULONG _STDC Release();
    HRESULT _STDC QueryInterface( REFIID riid, VOID** ppvInterface );
    HRESULT _STDC OnDefaultDeviceChanged( EDataFlow flow, ERole role, LPCWSTR pwstrDeviceId );
    HRESULT _STDC OnDeviceAdded( LPCWSTR pwstrDeviceId );
    HRESULT _STDC OnDeviceRemoved( LPCWSTR pwstrDeviceId );
    HRESULT _STDC OnDeviceStateChanged( LPCWSTR pwstrDeviceId, DWORD dwNewState );
    HRESULT _STDC OnPropertyValueChanged( LPCWSTR pwstrDeviceId, const PROPERTYKEY key );
  };

  class WASAPISessionEvents: public IAudioSessionEvents {
  protected:
    WASAPIAudioPlayer* mPlayer;
    long mRefCount;
  public:
    WASAPISessionEvents( WASAPIAudioPlayer* player );
    ~WASAPISessionEvents();
    ULONG _STDC AddRef();
    ULONG _STDC Release();
    HRESULT _STDC QueryInterface( REFIID riid, VOID** ppvInterface );
    HRESULT _STDC OnDisplayNameChanged( LPCWSTR NewDisplayName, LPCGUID EventContext );
    HRESULT _STDC OnIconPathChanged( LPCWSTR NewIconPath, LPCGUID EventContext );
    HRESULT _STDC OnSimpleVolumeChanged( float NewVolume, BOOL NewMute, LPCGUID EventContext );
    HRESULT _STDC OnChannelVolumeChanged( DWORD ChannelCount, float NewChannelVolumeArray[], DWORD ChangedChannel, LPCGUID EventContext );
    HRESULT _STDC OnGroupingParamChanged( LPCGUID NewGroupingParam, LPCGUID EventContext );
    HRESULT _STDC OnStateChanged( AudioSessionState NewState );
    HRESULT _STDC OnSessionDisconnected( AudioSessionDisconnectReason DisconnectReason );
  };

  class WASAPIAudioDevice {
  protected:
    REFERENCE_TIME mRequestDuration;
    IMMDevice* mDevice;
    IPropertyStore* mProps;
    IAudioClient* mAudioClient;
    wstring mName;
    bool mActive;
    WAVEFORMATEX* mWaveFormat;
  public:
    WASAPIAudioDevice( IMMDevice* device );
    IMMDevice* getIMMDevice();
    const wstring& getName();
    IAudioClient* getAudioClient();
    WAVEFORMATEX* getWaveFormatPtr();
    bool isActive();
    void activate();
    void start();
    void initialize();
    void stop();
    void deActivate();
    ~WASAPIAudioDevice();
  };

}