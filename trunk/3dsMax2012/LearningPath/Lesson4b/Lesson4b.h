#pragma once

//**************************************************************************/
// Copyright (c) 2011 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
// DESCRIPTION: Includes for Plugins
// AUTHOR: 
//***************************************************************************/

#include "3dsmaxsdk_preinclude.h"
#include "Max.h"
#include "resource.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"
//SIMPLE TYPE



extern TCHAR *GetString(int id);

extern HINSTANCE hInstance;

class SampleKFCtrl : public Control
{
private:

	Control* mpXCtrl;
	Control* mpYCtrl;

	enum MyRefs {
		kXCtrlRef,
		kYCtrlRef,
		kNumRefs
	};

public:

	SampleKFCtrl();
	~SampleKFCtrl() { DeleteAllRefs(); };

	SClass_ID SuperClassID() { return CTRL_POSITION_CLASS_ID; }

	int NumSubs();
	Animatable* SubAnim(int n);
	int NumRefs() { return kNumRefs; }
	void SetReference(int i, ReferenceTarget* pTarget);
	ReferenceTarget* GetReference(int i);
	RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message);

	void Copy(Control* pFrom);

	void GetValue(TimeValue t, void *ptr, Interval &valid, GetSetMethod method);
	void SetValue(TimeValue t, void *ptr, int commit, GetSetMethod method);
};