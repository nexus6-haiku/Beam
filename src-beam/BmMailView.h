/*
	BmMailView.h
		$Id$
*/
/*************************************************************************/
/*                                                                       */
/*  Beam - BEware Another Mailer                                         */
/*                                                                       */
/*  http://www.hirschkaefer.de/beam                                      */
/*                                                                       */
/*  Copyright (C) 2002 Oliver Tappe <beam@hirschkaefer.de>               */
/*                                                                       */
/*  This program is free software; you can redistribute it and/or        */
/*  modify it under the terms of the GNU General Public License          */
/*  as published by the Free Software Foundation; either version 2       */
/*  of the License, or (at your option) any later version.               */
/*                                                                       */
/*  This program is distributed in the hope that it will be useful,      */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU    */
/*  General Public License for more details.                             */
/*                                                                       */
/*  You should have received a copy of the GNU General Public            */
/*  License along with this program; if not, write to the                */
/*  Free Software Foundation, Inc., 59 Temple Place - Suite 330,         */
/*  Boston, MA  02111-1307, USA.                                         */
/*                                                                       */
/*************************************************************************/


#ifndef _BmMailView_h
#define _BmMailView_h

#include <layout.h>

#include "BetterScrollView.h"
#include "Colors.h"
#include "WrappingTextView.h"

#include "BmController.h"
#include "BmMail.h"
#include "BmMemIO.h"

class BMessageRunner;
class BmBodyPart;
class BmBodyPartView;
class BmMailRef;
class BmMailViewContainer;
class BmMailHeaderView;
class BmMailRefView;
class BmRulerView;

/*------------------------------------------------------------------------------*\
	types of messages sent via the observe/notify system:
\*------------------------------------------------------------------------------*/
#define BM_NTFY_MAIL_VIEW	'bmbc'
						// sent from BmMailView to observers whenever selection changes

#define BM_MAILVIEW_SHOWRAW						'bmMa'
#define BM_MAILVIEW_SHOWCOOKED					'bmMb'
#define BM_MAILVIEW_SHOWINLINES_SEPARATELY	'bmMc'
#define BM_MAILVIEW_SHOWINLINES_CONCATENATED	'bmMd'
#define BM_MAILVIEW_SELECT_ENCODING				'bmMe'
#define BM_MAILVIEW_COPY_URL						'bmMf'

/*------------------------------------------------------------------------------*\
	BmMailView
		-	
\*------------------------------------------------------------------------------*/
class BmMailView : public WrappingTextView, public BmJobController {
	typedef WrappingTextView inherited;
	typedef BmJobController inheritedController;
	struct BmTextRunInfo {
		BmTextRunInfo( rgb_color c=Black, bool url=false) { 
			color = c; isURL = url;
		}
		rgb_color color;
		bool isURL;
	};
	typedef map<int32,BmTextRunInfo> BmTextRunMap;
	typedef BmTextRunMap::const_iterator BmTextRunIter;
	
	// archival-fieldnames:
	static const char* const MSG_VERSION;
	static const char* const MSG_RAW;
	static const char* const MSG_FONTNAME;
	static const char* const MSG_FONTSIZE;
	//
	static const char* const MSG_MAIL;
	static const char* const MSG_ENCODING;

	static const int16 nArchiveVersion;

public:
	static const char* const MSG_HAS_MAIL;

	// creator-func, c'tors and d'tor:
	static BmMailView* CreateInstance(  minimax minmax, BRect frame, bool outbound);
	BmMailView( minimax minmax, BRect frame, bool outbound);
	~BmMailView();

	// native methods:
	void ShowMail( BmMailRef* ref, bool async=true);
	void ShowMail( BmMail* mail, bool async=true);
	void DisplayBodyPart( BmStringOBuf& displayBuf, BmBodyPart* bodyPart);
	status_t Archive( BMessage* archive, bool deep=true) const;
	status_t Unarchive( BMessage* archive, bool deep=true);
	bool WriteStateInfo();
	void GetWrappedText( BmString& out, bool hardWrapIfNeeded=true);
	void SetSignatureByName( const BmString sigName);
	void UpdateFont( const BFont& font);
	bool IsOverURL( BPoint point);
	BmString GetTextForTextrun( BmTextRunIter run);

	// overrides of BTextView base:
	bool AcceptsDrop( const BMessage* msg);
	void AttachedToWindow();
	bool CanEndLine(int32 offset);
	void FrameResized( float newWidth, float newHeight);
	void InsertText(const char *text, int32 length, int32 offset,
						 const text_run_array *runs);
	void KeyDown(const char *bytes, int32 numBytes);
	void MakeFocus(bool focused);
	void MessageReceived( BMessage* msg);
	void MouseDown( BPoint point);
	void MouseUp( BPoint point);
	void MouseMoved( BPoint point, uint32 transit, const BMessage *message);
	
	// overrides of BmController base:
	BHandler* GetControllerHandler()		{ return this; }
	void JobIsDone( bool completed);
	void DetachModel();

	// getters:
	inline BmMailViewContainer* ContainerView() const	{ return mScrollView; }
	inline BmRef<BmMail> CurrMail()				{ return mCurrMail; }
	inline bool ShowRaw()							{ return mShowRaw; }
	inline bool ShowInlinesSeparately()			{ return mShowInlinesSeparately; }
	inline BmBodyPartView* BodyPartView()		{ return mBodyPartView; }
	inline BmMailHeaderView* HeaderView()		{ return mHeaderView; }

	// setters:
	inline void TeamUpWith( BmMailRefView* v)	{ mPartnerMailRefView = v; }
	inline void ShowRaw( bool b) 					{ mShowRaw = b; }
	inline void ShowInlinesSeparately( bool b)	{ mShowInlinesSeparately = b; }

private:
	void ShowMenu( BPoint point);
	BmTextRunIter TextRunInfoAt( int32 pos) const;

	// will not be archived:
	bool mOutbound;
	BmRef< BmMail> mCurrMail;
	BmMailViewContainer* mScrollView;
	BmMailHeaderView* mHeaderView;
	BmBodyPartView* mBodyPartView;
	BmRulerView* mRulerView;
	BmMailRefView* mPartnerMailRefView;
	BmTextRunMap mTextRunMap;
	BmTextRunIter mClickedTextRun;
	BMessageRunner* mReadRunner;
	bool mShowingUrlCursor;
	
	// will be archived:
	BmString mFontName;
	int16 mFontSize;
	BFont mFont;
	bool mShowRaw;
	bool mShowInlinesSeparately;

	// Hide copy-constructor and assignment:
	BmMailView( const BmMailView&);
	BmMailView operator=( const BmMailView&);
};

class BmBusyView;
/*------------------------------------------------------------------------------*\
	BmMailViewContainer
		-	
\*------------------------------------------------------------------------------*/
class BmMailViewContainer : public MView, public BetterScrollView {
	typedef BetterScrollView inherited;

public:
	BmMailViewContainer( minimax minmax, BmMailView* target, uint32 resizingMode, 
								uint32 flags);
	~BmMailViewContainer();

	// native methods:
	void SetBusy();
	void UnsetBusy();
	void PulseBusyView();
	void RedrawScrollbars();

	// overrides of BView base:
	void FrameResized(float new_width, float new_height);
	void Draw( BRect bounds);

	// additions for liblayout:
	virtual minimax layoutprefs();
	virtual BRect layout(BRect);

private:
	BmBusyView* mBusyView;

	// Hide copy-constructor and assignment:
	BmMailViewContainer( const BmMailViewContainer&);
	BmMailViewContainer operator=( const BmMailViewContainer&);
};

#endif
