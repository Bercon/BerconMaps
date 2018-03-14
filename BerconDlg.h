#include "3dsmaxsdk_preinclude.h"
#include "Max.h"
#include "resource.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"
#include "curvectrl.h"	

class BerconDlg: public ParamDlg {
public:
	ParamDlg* pd;
	Class_ID id;
	int ref;

	BerconDlg(ParamDlg* pd, Class_ID id, int ref): pd (pd), id (id), ref (ref) {}

	Class_ID ClassID() { return pd->ClassID(); }
	void  SetThing (ReferenceTarget *m) { 
		ReferenceTarget* old = pd->GetThing();
		if (old->ClassID() == id && m->ClassID() == id && old != m) {		
			((ICurveCtl*)old->GetReference(ref))->SetActive(FALSE);			
			((ICurveCtl*)old->GetReference(ref))->EnableDraw(FALSE);						
		}		
		pd->SetThing(m);	
	}
	ReferenceTarget *  GetThing () { return pd->GetThing(); }
	void  SetTime (TimeValue t) { pd->SetTime(t); }
	void  ReloadDialog () { pd->ReloadDialog(); }
	void  DeleteThis () { pd->DeleteThis(); delete this; }
	void  ActivateDlg (BOOL onOff) { pd->ActivateDlg(onOff); }
	int  FindSubTexFromHWND (HWND hwnd) { return pd->FindSubTexFromHWND(hwnd); }
	int  FindSubMtlFromHWND (HWND hwnd) { return pd->FindSubMtlFromHWND(hwnd); }
	INT_PTR Execute (int cmd, ULONG_PTR arg1=0, ULONG_PTR arg2=0, ULONG_PTR arg3=0) { return pd->Execute(cmd, arg1, arg2, arg3); }
};