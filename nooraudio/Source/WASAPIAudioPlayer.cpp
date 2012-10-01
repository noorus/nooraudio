#include "nooraudio.h"

namespace nooraudio {

  using std::wstring;

  //-- WASAPIAudioPlayer class ------------------------------------------------

  WASAPIAudioPlayer::WASAPIAudioPlayer( AudioSource* pSource ):
  mDevice( NULL ), mSource( pSource ), mRenderer( NULL ), mManager( NULL )
  {
    InitializeSRWLock( &mSRWLock );
    mManager = new WASAPIDeviceManager( this );
  }

  void WASAPIAudioPlayer::onDeviceLost()
  {
    wprintf_s( L"onDeviceLost()\r\n" );
  }

  void WASAPIAudioPlayer::onDefaultDeviceChanged( EDataFlow flow, ERole role,
  LPCWSTR devID )
  {
    wprintf_s( L"onDefaultDeviceChanged()\r\n" );
  }

  DWORD WASAPIAudioPlayer::render( long frames )
  {
    BYTE* data;
    DWORD flags = 0;
    mRenderer->getBuffer( frames, &data );
    mSource->renderLooping( frames, data, flags );
    mRenderer->releaseBuffer( frames, flags );
    return flags;
  }

  void WASAPIAudioPlayer::play()
  {
    AcquireSRWLockExclusive( &mSRWLock );
    wprintf_s( L"Player::play()\r\n" );
    flags = 0;
    mDevice = mManager->createDefaultDevice();
    wprintf_s( L"- Audio Device: %s\r\n", mDevice->getName().c_str() );
    mDevice->activate();
    mSource->setFormat( mDevice->getWaveFormatPtr() );
    mDevice->initialize();
    if ( FAILED( mDevice->getAudioClient()->GetBufferSize( &bufferFrameCount ) ) )
      throw std::exception( "Failed to get audio client buffer size" );
    mRenderer = new WASAPIAudioRenderer( mDevice->getAudioClient() );
    mSession = new WASAPIAudioSession( this, mDevice->getAudioClient() );
    flags = render( bufferFrameCount );
    hnsActualDuration = (double)REFTIMES_PER_SEC * bufferFrameCount / mDevice->getWaveFormatPtr()->nSamplesPerSec;
    mDevice->start();
    ReleaseSRWLockExclusive( &mSRWLock );
  }

  void WASAPIAudioPlayer::run()
  {
    AcquireSRWLockExclusive( &mSRWLock );
    wprintf_s( L"Player::run()\r\n" );
    if ( !mDevice )
    {
      Sleep( 250 );
      ReleaseSRWLockExclusive( &mSRWLock );
      return;
    }
    if ( flags != AUDCLNT_BUFFERFLAGS_SILENT )
    {
      Sleep( (DWORD)( hnsActualDuration / REFTIMES_PER_MILLISEC / 2 ) );
      UINT32 numFramesPadding;
      if ( FAILED( mDevice->getAudioClient()->GetCurrentPadding( &numFramesPadding ) ) )
        throw std::exception( "Failed to get current padding" );
      UINT32 numFramesAvailable = bufferFrameCount - numFramesPadding;
      flags = render( numFramesAvailable );
    }
    ReleaseSRWLockExclusive( &mSRWLock );
  }

  void WASAPIAudioPlayer::stop()
  {
    AcquireSRWLockExclusive( &mSRWLock );
    wprintf_s( L"Player::stop()\r\n" );
    if ( mDevice )
      mDevice->stop();
    SAFE_DELETE( mSession );
    SAFE_DELETE( mRenderer );
    if ( mDevice )
      mDevice->deActivate();
    SAFE_DELETE( mDevice );
    ReleaseSRWLockExclusive( &mSRWLock );
  }

  WASAPIAudioPlayer::~WASAPIAudioPlayer()
  {
    SAFE_DELETE( mManager );
  }

}