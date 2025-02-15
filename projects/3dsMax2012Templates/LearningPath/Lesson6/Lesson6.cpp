/**********************************************************************
 *<
	FILE: Lesson6.cpp

	DESCRIPTION:	This plugin shows how to:
					
					Create a simple procedural object for 3ds Max.

					This Visual Studio project accompanies an article
					in the 3ds Max SDK Help file that walks a developer, 
					who is new to the 3ds Max SDK, through the process of
					writing a basic procedural object.

	OVERVIEW:		The Lesson6 object is a simple procedural object that can be
					created in the 3ds Max viewport. It is located in the create
					command panel under the category "Max SDK tutorials". 

					A Lesson6 looks like a collapsed oblique box with two attached
					'boxes' on each side.
					
					A Lesson6 has three parameters:

					Size: A float value that governs it's overall size.

					Left: A float value that determines the extension of the box
						  on the left side of the Lesson6

				    Right: A float value that determines the extension of the box
						  on the right side of the Lesson6.

				    This project was created using the Visual Studio Application
					wizard for 3ds Max. Hence many TODO comments are still in the
					body of the plugin.

					The most important functions that were implemented were:

					int Lesson6CreateCallBack::proc(... ...)
					void Lesson6::BuildMesh(TimeValue t)

					The parameter block implemented was:

					static ParamBlockDesc2 Lesson6_param_blk(... ...)
					
	CREATED BY:		Chris Johnson
		
	HISTORY:		Code started in April 2005

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#include "Lesson6.h"

#define Lesson6_CLASS_ID	Class_ID(0x986df9b4, 0x792ce6d7)

#define PBLOCK_REF	0

class Lesson6 : public SimpleObject2 {
	public:
		// Parameter block handled by parent

		//Class vars
		static IObjParam *ip;			//Access to the interface
		// From BaseObject
		CreateMouseCallBack* GetCreateMouseCallBack();
		
		// From Object
		BOOL HasUVW();
		void SetGenUVW(BOOL sw);
		void GetCollapseTypes(Tab<Class_ID> &clist,Tab<TSTR*> &nlist);
		int IntersectRay(TimeValue t, Ray& ray, float& at, Point3& norm);

		// From Animatable
		void BeginEditParams( IObjParam  *ip, ULONG flags,Animatable *prev);
		void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);

		// From SimpleObject
		void BuildMesh(TimeValue t);
		BOOL OKtoDisplay(TimeValue t);
		void InvalidateUI();
		
		//From Animatable
		Class_ID ClassID() {return Lesson6_CLASS_ID;}		
		SClass_ID SuperClassID() { return GEOMOBJECT_CLASS_ID; }
		void GetClassName(TSTR& s) {s = GetString(IDS_CLASS_NAME);}

		RefTargetHandle Clone( RemapDir &remap );

		//This returns a name that will be used to automatically name the object
		//when it gets created
		TCHAR *GetObjectName() { return GetString(IDS_CLASS_NAME); }
		int	NumParamBlocks() { return 1; }					// return number of ParamBlocks in this instance
		IParamBlock2* GetParamBlock(int i) { return pblock2; } // return i'th ParamBlock
		IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock2->ID() == id) ? pblock2 : NULL; } // return id'd ParamBlock

		void DeleteThis() { delete this; }		
		
		//Constructor/Destructor
		Lesson6();
		~Lesson6();
};



class Lesson6ClassDesc : public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return new Lesson6(); }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return GEOMOBJECT_CLASS_ID; }
	Class_ID		ClassID() { return Lesson6_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("Lesson6"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }			// returns owning module handle
	

};

static Lesson6ClassDesc Lesson6Desc;
ClassDesc2* GetLesson6Desc() { return &Lesson6Desc; }


//TODO: Add enumerations for each parameter block
enum { Lesson6_params };


//TODO: Add enumerations for various parameters
enum { 
	Lesson6_size,
	Lesson6_left,
	Lesson6_right
};


static ParamBlockDesc2 Lesson6_param_blk ( 
	//Required arguments ----------------------------
	Lesson6_params, _T("params"),  0, &Lesson6Desc,
	//flags
	P_AUTO_CONSTRUCT + P_AUTO_UI,

	//Dependent arguments ---------------------------
	//required because P_AUTO_CONSTRUCT was flagged
	//This declares the number of rollouts
	PBLOCK_REF,
	//required because P_AUTO_UI was flagged. 
	//This is the Rollout description
	IDD_PANEL, IDS_PARAMS, 0, 0, NULL,

	//Parameter Specifications ----------------------
	// For each control create a parameter:
	Lesson6_size, 		_T("Size"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_SPIN, 
		//Zero or more optional tags
		p_default, 		1.0f, 
		p_range, 		0.0f,1000.0f, 
		p_ui, 			TYPE_SPINNER,		EDITTYPE_FLOAT, IDC_SIZE_EDIT,	IDC_SIZE_SPIN, 0.50f, 
		end,
	end
	);


IObjParam *Lesson6::ip			= NULL;

//--- Lesson6 -------------------------------------------------------

Lesson6::Lesson6()
{
	Lesson6Desc.MakeAutoParamBlocks(this);

}

Lesson6::~Lesson6()
{
}

void 
Lesson6::BeginEditParams(IObjParam *ip,ULONG flags,Animatable *prev)
{
	this->ip = ip;

	SimpleObject2::BeginEditParams(ip,flags,prev);
	Lesson6Desc.BeginEditParams(ip, this, flags, prev);
}

void 
Lesson6::EndEditParams( IObjParam *ip, ULONG flags, Animatable *next )
{
	//TODO: Save plugin parameter values into class variables, if they are not hosted in ParamBlocks. 
	
	SimpleObject2::EndEditParams(ip,flags,next);
	Lesson6Desc.EndEditParams(ip, this, flags, next);

	this->ip = NULL;
}

//From Object
BOOL 
Lesson6::HasUVW() 
{ 
	//TODO: Return whether the object has UVW coordinates or not
	return TRUE; 
}

void 
Lesson6::SetGenUVW(BOOL sw) 
{  
	if (sw==HasUVW()) return;
	//TODO: Set the plugin's internal value to sw				
}

//Class for interactive creation of the object using the mouse
class Lesson6CreateCallBack : public CreateMouseCallBack {
	IPoint2 sp0;	//First point in screen coordinates
	Lesson6 *ob;	//Pointer to the object 
	Point3 p0;		//First point in world coordinates
	Point3 p1;		//We added this point. Second point in world coordinates.
public:	
	int proc( ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat);
	void SetObj(Lesson6 *obj) {ob = obj;}
};

int 
Lesson6CreateCallBack::proc(ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat )
{
	
	if (msg==MOUSE_POINT||msg==MOUSE_MOVE) 
	{
		switch(point) 
		{
			case 0: // only happens with MOUSE_POINT msg
				ob->suspendSnap = TRUE;
				p0 = vpt->SnapPoint(m,m,NULL,SNAP_IN_PLANE);
				mat.SetTrans(p0);

				//Set a default overall size in the parameter block
				ob->pblock2->SetValue(Lesson6_size, ob->ip->GetTime(), 0.0f);
				break;
			case 1:
			{
				ob->suspendSnap = TRUE; 
				p1 = vpt->SnapPoint(m,m,NULL,SNAP_IN_PLANE);

				float speedFactor = 24.0f;
				float theSize = (Length(p1 - p0) / speedFactor);

				//Set the overall size in parameter block
				ob->pblock2->SetValue(Lesson6_size, ob->ip->GetTime(), theSize);

				//Invalidate and display the mesh in the viewport
				Lesson6_param_blk.InvalidateUI();
				break;
			}
			case 2:
			{
				return CREATE_STOP;
			}
		}
	} 
	else {
		if (msg == MOUSE_ABORT) return CREATE_ABORT;
	}
	return TRUE;
}

static Lesson6CreateCallBack Lesson6CreateCB;

//From BaseObject
CreateMouseCallBack* Lesson6::GetCreateMouseCallBack() 
{
	Lesson6CreateCB.SetObj(this);
	return(&Lesson6CreateCB);
}

//From SimpleObject
void 
Lesson6::BuildMesh(TimeValue t)
{
	// SimpleObject has a ivalid member that stores the validity of the object.
	// We need to update the validity here. The mesh is supposed to be always valid.
	// If values controlling the mesh are animated they will updated the validity interval
	// when GetValue is called.
	ivalid = FOREVER;
	float size;//, sizeLeft, sizeRight;
	pblock2->GetValue(Lesson6_size, t, size, ivalid );

	// Update mesh using these parameters with the edited content of Lesson6mesh.h
	mesh.setNumVerts(4);
	mesh.setNumFaces(3);
	mesh.setVert(0,size*Point3(0.0,0.0,0.0));
	mesh.setVert(1,size*Point3(10.0,0.0,0.0));
	mesh.setVert(2,size*Point3(0.0,10.0,0.0));
	mesh.setVert(3,size*Point3(0.0,0.0,10.0));

	mesh.faces[0].setVerts(0, 1, 2);
	mesh.faces[0].setEdgeVisFlags(1,1,0);
	mesh.faces[0].setSmGroup(2);
	mesh.faces[1].setVerts(3, 1, 0);
	mesh.faces[1].setEdgeVisFlags(1,1,0);
	mesh.faces[1].setSmGroup(2);
	mesh.faces[2].setVerts(0, 2, 3);
	mesh.faces[2].setEdgeVisFlags(1,1,0);
	mesh.faces[2].setSmGroup(4);
	
	mesh.InvalidateGeomCache();
}

BOOL 
Lesson6::OKtoDisplay(TimeValue t) 
{
	BOOL displayOk = TRUE;
	float size = pblock2->GetFloat(Lesson6_size, t);
	if ( size <= 0.0f ) {
		// Do not display when size is not valid.
		displayOk = FALSE;
	}
	return displayOk;
}

void 
Lesson6::InvalidateUI() 
{
	// Hey! Update the param blocks
	pblock2->GetDesc()->InvalidateUI();
}

// From Object
int 
Lesson6::IntersectRay(
		TimeValue t, Ray& ray, float& at, Point3& norm)
{
	//TODO: Return TRUE after you implement this method
	return FALSE;
}

void 
Lesson6::GetCollapseTypes(Tab<Class_ID> &clist,Tab<TSTR*> &nlist)
{
    Object::GetCollapseTypes(clist, nlist);
	//TODO: Append any any other collapse type the plugin supports
}

// From ReferenceTarget
RefTargetHandle Lesson6::Clone(RemapDir& remap) 
{
	Lesson6* newob = new Lesson6();	
	// Make a copy all the data and also clone all the references
	newob->ReplaceReference(0,remap.CloneRef(pblock2));
	newob->ivalid.SetEmpty();
	BaseClone(this, newob, remap);
	return(newob);
}
