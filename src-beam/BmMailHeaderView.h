/*
	BmMailHeaderView.h
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


#ifndef _BmMailHeaderView_h
#define _BmMailHeaderView_h

#include <vector>

#include <MessageFilter.h>
#include <View.h>

#include "BmMailHeader.h"
#include "BmRefManager.h"

/*------------------------------------------------------------------------------*\
	types of messages handled by a BmMailHeaderView:
\*------------------------------------------------------------------------------*/
enum {
	BM_HEADERVIEW_SMALL			= 'bmfa',
	BM_HEADERVIEW_LARGE			= 'bmfb',
	BM_HEADERVIEW_FULL			= 'bmfc',
	BM_HEADERVIEW_SWITCH_RESENT= 'bmfd',
	BM_HEADERVIEW_COPY_HEADER	= 'bmfe'
};

class BStringView;
class BTextView;
/*------------------------------------------------------------------------------*\
	BmMailHeaderFieldView
		-	
\*------------------------------------------------------------------------------*/
class BmMailHeaderFieldView : public BView {
	typedef BView inherited;
	
	class BmMsgFilter : public BMessageFilter {
	public:
		BmMsgFilter( BHandler* destHandler, uint32 cmd)
			: 	BMessageFilter( B_ANY_DELIVERY, B_ANY_SOURCE, cmd) 
			,	mDestHandler( destHandler)
		{
		}
		filter_result Filter( BMessage* msg, BHandler** handler);
	private:
		BHandler* mDestHandler;
	};

public:
	// c'tors and d'tor:
	BmMailHeaderFieldView( BmString fieldName, BmString value, BFont* font, 
								  float fixedWidth, float titleWidth=-1);
	~BmMailHeaderFieldView();

	// native methods:
	void SetTitleWidth( float newWidth, float fixedWidth);
	float GetTitleWidth();
	void ShowMenu( BPoint point);

	// overrides of BView base:
	void MouseDown(BPoint point);

private:
	BStringView* mTitleView;
	BTextView* mContentView;

	// Hide copy-constructor and assignment:
	BmMailHeaderFieldView( const BmMailHeaderFieldView&);
	BmMailHeaderFieldView operator=( const BmMailHeaderFieldView&);
};



/*------------------------------------------------------------------------------*\
	BmMailHeaderView
		-	
\*------------------------------------------------------------------------------*/
class BmMailHeaderView : public BView {
	typedef BView inherited;
	friend BmMailHeaderFieldView;

	static const int SMALL_HEADERS = 0;
	static const int LARGE_HEADERS = 1;
	static const int FULL_HEADERS = 2;

	// archival-fieldnames:
	static const char* const MSG_VERSION;
	static const char* const MSG_MODE;
	static const char* const MSG_REDIRECT_MODE;
	static const char* const MSG_FONTNAME;
	static const char* const MSG_FONTSIZE;

public:
	// c'tors and d'tor:
	BmMailHeaderView( BmMailHeader* header);
	~BmMailHeaderView();

	// native methods:
	void ShowHeader( BmMailHeader* header, bool invalidate=true);
	status_t Archive( BMessage* archive, bool deep=true) const;
	status_t Unarchive( BMessage* archive, bool deep=true);
	float AddFieldViews();
	void RemoveFieldViews();
	inline float FixedWidth() 				{ return 5000; }

	// overrides of BView base:
	void MessageReceived( BMessage* msg);

private:
	BmRef<BmMailHeader> mMailHeader;
	int16 mDisplayMode;
							// 0=small, 2=large, anyother=medium
	BFont mFont;		
							// font to be used for header-fields
	bool mShowRedirectFields;
							// true=>show redirect-fields false=>show original fields
	float mMaxTitleWidth;
	vector<BmMailHeaderFieldView*> mFieldViews;

	// Hide copy-constructor and assignment:
	BmMailHeaderView( const BmMailHeaderView&);
	BmMailHeaderView operator=( const BmMailHeaderView&);
};

#endif
