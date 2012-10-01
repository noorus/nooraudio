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

# ifndef SAFE_DELETE
#   define SAFE_DELETE(p) {if(p){delete p;(p)=NULL;}}
# endif

# ifndef SAFE_RELEASE
#   define SAFE_RELEASE(p) {if(p){p->Release();(p)=NULL;}}
# endif

  const CLSID CLSID_MMDeviceEnumerator  = __uuidof( MMDeviceEnumerator );
  const IID   IID_IMMDeviceEnumerator   = __uuidof( IMMDeviceEnumerator );
  const IID   IID_IMMNotificationClient = __uuidof( IMMNotificationClient );
  const IID   IID_IAudioClient          = __uuidof( IAudioClient );
  const IID   IID_IAudioRenderClient    = __uuidof( IAudioRenderClient );
  const IID   IID_IAudioSessionControl  = __uuidof( IAudioSessionControl );
  const IID   IID_IAudioSessionEvents   = __uuidof( IAudioSessionEvents );

  class WASAPIDeviceEvents;
  class WASAPISessionEvents;
  class WASAPIAudioDevice;
  class WASAPIAudioPlayer;
  class WASAPIDeviceManager;
  class WASAPIAudioSession;
  class WASAPIAudioRenderer;
  class AudioSource;

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
    std::wstring mName;
    bool mActive;
    WAVEFORMATEX* mWaveFormat;
  public:
    WASAPIAudioDevice( IMMDevice* device );
    IMMDevice* getIMMDevice();
    const std::wstring& getName();
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

  class WASAPIDeviceManager {
  protected:
    IMMDeviceEnumerator* mEnumerator;
    WASAPIDeviceEvents* mEvents;
    WASAPIAudioPlayer* mPlayer;
  public:
    WASAPIDeviceManager( WASAPIAudioPlayer* pPlayer );
    WASAPIAudioDevice* createDefaultDevice();
    ~WASAPIDeviceManager();
  };

  class WASAPIAudioRenderer {
  protected:
    IAudioRenderClient* mRenderClient;
  public:
    WASAPIAudioRenderer( IAudioClient* pClient );
    void getBuffer( unsigned int frames, BYTE** data );
    void releaseBuffer( unsigned int framesWritten, DWORD flags );
    ~WASAPIAudioRenderer();
  };

  class WASAPIAudioSession {
  protected:
    WASAPIAudioPlayer* mPlayer;
    IAudioSessionControl* mSessionControl;
    WASAPISessionEvents* mEvents;
  public:
    WASAPIAudioSession( WASAPIAudioPlayer* pPlayer, IAudioClient* pClient );
    void setIcon( const std::wstring& sPath );
    void setName( const std::wstring& sName );
    ~WASAPIAudioSession();
  };

  class WASAPIAudioPlayer {
  protected:
    IMMDevice* mRawDevice;
    WASAPIAudioDevice* mDevice;
    AudioSource* mSource;
    WASAPIAudioRenderer* mRenderer;
    WASAPIAudioSession* mSession;
    WASAPIDeviceManager* mManager;
    UINT32 bufferFrameCount;
    REFERENCE_TIME hnsActualDuration;
    DWORD flags;
    SRWLOCK mSRWLock;
  public:
    WASAPIAudioPlayer( AudioSource* pSource );
    DWORD render( long frames );
    void onDeviceLost();
    void onDefaultDeviceChanged( EDataFlow flow, ERole role, LPCWSTR devID );
    void doStop();
    void play();
    void run();
    void stop();
    ~WASAPIAudioPlayer();
  };

  enum SampleSize {
    SampleSize_8bit,
    SampleSize_16bit
  };

  class AudioSource {
  protected:
    FILE* mFile;
    WAVEFORMATEX* mWaveFormat;
    SampleSize mSampleSize;
    long mSampleBits;
    long mSampleBytes;
    void resolveSampleSize( SampleSize sampleSize );
  public:
    AudioSource( const std::wstring& fileName, SampleSize sampleSize );
    virtual ~AudioSource();
    virtual long getChannels() = 0;
    virtual long getSampleRate() = 0;
    virtual long getBitRate() = 0;
    virtual float getTotalTime() = 0;
    virtual float getCurrentTime() = 0;
    virtual void setFormat( WAVEFORMATEX* waveFormat ) = 0;
    virtual void render( long frames, BYTE* data, DWORD& flags ) = 0;
    virtual void renderLooping( long frames, BYTE* data, DWORD& flags ) = 0;
  };

}