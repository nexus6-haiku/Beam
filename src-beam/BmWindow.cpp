/*
	BmWindow.cpp
		$Id$
*/

#include <File.h>

#include "BmBasics.h"
#include "BmLogHandler.h"
#include "BmMsgTypes.h"
#include "BmResources.h"
#include "BmUtil.h"
#include "BmWindow.h"

const char* const BmWindow::MSG_FRAME = 	"bm:frm";

/*------------------------------------------------------------------------------*\
	BmWindow()
		-	constructor, creates window, reading frame and position from
			state-file, if that exists.
\*------------------------------------------------------------------------------*/
BmWindow::BmWindow( const char* statefileName, BRect frame, const char* title,
						  window_look look, window_feel feel, uint32 flags)
	:	MWindow( frame, title, look, feel, flags)
	,	mStatefileName( statefileName)
	,	mLifeHasBegun( false)
{ 
}

/*------------------------------------------------------------------------------*\
	~BmWindow()
		-	standard d'tor
\*------------------------------------------------------------------------------*/
BmWindow::~BmWindow() {
}

/*------------------------------------------------------------------------------*\
	()
		-	
\*------------------------------------------------------------------------------*/
status_t BmWindow::ArchiveState( BMessage* archive) const {
	BRect frame = Frame();
BM_LOG2( BM_LogUtil, BmString("Window ") << Name() << " stored with left:" << frame.left << " top: " << frame.top << "\nwidth:" << frame.Width() << " height: " << frame.Height());
	status_t ret = archive->AddRect( MSG_FRAME, frame);
	return ret;
}

/*------------------------------------------------------------------------------*\
	()
		-	
\*------------------------------------------------------------------------------*/
status_t BmWindow::UnarchiveState( BMessage* archive) {
	BRect frame;
	status_t ret = archive->FindRect( MSG_FRAME, &frame);
	if (ret == B_OK) {
BM_LOG2( BM_LogUtil, BmString("Window ") << Name() << " opened at left:" << frame.left << " top: " << frame.top << "\nwidth:" << frame.Width() << " height: " << frame.Height());
		MoveTo( frame.LeftTop());
		ResizeTo( frame.Width(), frame.Height());
	}
	return ret;
}

/*------------------------------------------------------------------------------*\
	ReadStateInfo()
		-	reads frame and position from	state-file, if that exists.
\*------------------------------------------------------------------------------*/
bool BmWindow::ReadStateInfo() {
	status_t err;
	BFile winFile;

	// try to open state-cache-file...
	if ((err = winFile.SetTo( TheResources->StateInfoFolder(), mStatefileName.String(), B_READ_ONLY)) == B_OK) {
		// ...ok, archive file found, we fetch our dimensions from it:
		try {
			BMessage archive;
			(err = archive.Unflatten( &winFile)) == B_OK
													|| BM_THROW_RUNTIME( BmString("Could not fetch window archive from file\n\t<") << mStatefileName << ">\n\n Result: " << strerror(err));
			UnarchiveState( &archive);
		} catch (exception &e) {
			BM_SHOWERR( e.what());
			return false;
		}
	}
	return true;
}

/*------------------------------------------------------------------------------*\
	WriteStateInfo()
		-	stores Window-state inside StateCache-folder:
\*------------------------------------------------------------------------------*/
bool BmWindow::WriteStateInfo() {
	BMessage archive;
	BFile cacheFile;
	status_t err;

	try {
		this->ArchiveState( &archive) == B_OK
													|| BM_THROW_RUNTIME("Unable to archive Window-object");
		(err = cacheFile.SetTo( TheResources->StateInfoFolder(), mStatefileName.String(), 
										B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE)) == B_OK
													|| BM_THROW_RUNTIME( BmString("Could not create cache file\n\t<") << mStatefileName << ">\n\n Result: " << strerror(err));
		(err = archive.Flatten( &cacheFile)) == B_OK
													|| BM_THROW_RUNTIME( BmString("Could not store state-cache into file\n\t<") << mStatefileName << ">\n\n Result: " << strerror(err));
	} catch( exception &e) {
		BM_SHOWERR( e.what());
		return false;
	}
	return true;
}

/*------------------------------------------------------------------------------*\
	Show()
		-	we begin life of window, if not done yet:
\*------------------------------------------------------------------------------*/
void BmWindow::Show() {
	if (!mLifeHasBegun) {
		BeginLife();
		mLifeHasBegun = true;
	}
	inherited::Show();
}

/*------------------------------------------------------------------------------*\
	Quit()
		-	we write the state-info before we quit
\*------------------------------------------------------------------------------*/
void BmWindow::Quit() {
	WriteStateInfo();
	inherited::Quit();
}

/*------------------------------------------------------------------------------*\
	Quit()
		-	we write the state-info before we quit
\*------------------------------------------------------------------------------*/
void BmWindow::MessageReceived( BMessage* msg) {
	switch( msg->what) {
		case B_COPY:
		case B_CUT: 
		case B_PASTE: 
		case B_UNDO: 
		case B_REDO: 
		case B_SELECT_ALL: {
			bool seenThis;
			if (msg->FindBool( "seenThis", &seenThis) != B_OK) {
				msg->AddBool( "seenThis", true);
				BView* focusView = CurrentFocus();
				if (focusView)
					PostMessage( msg, focusView);
			}
			break;
		}
		default: {
			inherited::MessageReceived( msg);
		}
	}
}
