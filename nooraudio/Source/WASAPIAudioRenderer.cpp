#include "nooraudio.h"

namespace nooraudio {

  using std::wstring;

  //-- WASAPIAudioRenderer class ----------------------------------------------

  WASAPIAudioRenderer::WASAPIAudioRenderer( IAudioClient* pClient ): mRenderClient( NULL )
  {
    if ( FAILED( pClient->GetService( IID_IAudioRenderClient, (void**)&mRenderClient ) ) )
      throw std::exception( "Failed to get audio render service" );
  }

  void WASAPIAudioRenderer::getBuffer( unsigned int frames, BYTE** data )
  {
    if ( FAILED( mRenderClient->GetBuffer( frames, data ) ) )
      throw std::exception( "Failed to get audio render buffer" );
  }

  void WASAPIAudioRenderer::releaseBuffer( unsigned int framesWritten, DWORD flags )
  {
    if ( FAILED( mRenderClient->ReleaseBuffer( framesWritten, flags ) ) )
      throw std::exception( "Failed to release buffer" );
  }

  WASAPIAudioRenderer::~WASAPIAudioRenderer()
  {
    SAFE_RELEASE( mRenderClient );
  }

}