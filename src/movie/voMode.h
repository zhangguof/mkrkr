/****************************************************************************/
/*! @file
@brief ビデオ再生モード

-----------------------------------------------------------------------------
	Copyright (C) 2004 T.Imoto
-----------------------------------------------------------------------------
@author		T.Imoto
@date		2004/09/19
@note
			2004/09/19	T.Imoto		
*****************************************************************************/

#ifndef __VOMODE_H__
#define __VOMODE_H__

/*[*/
//---------------------------------------------------------------------------
// tTVPVideoOverlayMode
//---------------------------------------------------------------------------
enum tTVPVideoOverlayMode {
	vomOverlay,		// Overlay
	vomLayer,		// Draw Layer
	vomMixer,		// VMR
	vomMFEVR,		// Media Foundation with EVR
};

//evcode
#define EC_COMPLETE (0x1000)
#define EC_UPDATE (EC_COMPLETE + 1)

// enum  VSSTATE
// {
// 	vsStopped,
// 	vsPlaying,
// 	vsPaused,
// 	vsReady,
// 	vsEnded,
// };





/*]*/

#endif	// __VOMODE_H__
