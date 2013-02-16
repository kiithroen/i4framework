//**************************************************************************/
// Copyright (c) 1998-2007 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
// DESCRIPTION: Appwizard generated plugin
// AUTHOR: 
//***************************************************************************/

#include "I4MaxExporter.h"
#include <vector>
#include <algorithm>
using namespace std;

const float POS_KEY_EPSILON = 0.001f;
const float ROT_KEY_EPSILON = 0.000001f;
const float WEIGHT_EPSILON = 0.000001f;

#define I4MaxExporter_CLASS_ID	Class_ID(0x41fc8424, 0x2dafc6eb)

#define TicksToMilliSec(ticks) (((float)(ticks)/(float)TIME_TICKSPERSEC) * 1000.f)
#define MilliSecToTicks(msec)  (((float)msec / 1000.f) * TIME_TICKSPERSEC)

struct WeightInfo
{
	int boneIdx;
	float weight;

	bool operator < (const WeightInfo& other) const
	{
		return weight > other.weight;	// 웨이트가 높은 순으로
	}
};

bool IsMesh(INode *pNode)
{
	if(pNode == NULL)
		return false;

	ObjectState os = pNode->EvalWorldState(0); 

	if (os.obj == NULL)
		return false;

	if(os.obj->SuperClassID() == GEOMOBJECT_CLASS_ID)
		return true;

	return false;
}

bool IsBone(INode *pNode)
{
	if(pNode == NULL)
		return false;

	ObjectState os = pNode->EvalWorldState(0);
	if (os.obj == NULL) return false;

	if(os.obj->ClassID() == BONE_OBJ_CLASSID)
		return true;

	if(os.obj->ClassID() == Class_ID(DUMMY_CLASS_ID, 0))
		return false;

	Control *cont = pNode->GetTMController();
	if(cont->ClassID() == BIPSLAVE_CONTROL_CLASS_ID ||		//others biped parts
		cont->ClassID() == BIPBODY_CONTROL_CLASS_ID	||		//biped root "Bip01"
		cont->ClassID() == FOOTPRINT_CLASS_ID)
	{
		return true;
	}

	return false;
}

int RecursiveGetBoneIndex(INode *pNode, INode *pNodeTest, int &boneCount)
{
	if(IsBone(pNode))
	{
		if(pNode == pNodeTest)
			return boneCount;

		boneCount++;
	}

	for(int i = 0; i < pNode->NumberOfChildren(); i++)
	{
		int boneIdx = RecursiveGetBoneIndex(pNode->GetChildNode(i), pNodeTest, boneCount);
		if(boneIdx >= 0)
			return boneIdx;
	}

	return -1;
}

int GetBoneIndex(INode *pRoot, INode *pNode)
{
	if(!IsBone(pNode))
		return -1;

	int boneCount = 0;
	return RecursiveGetBoneIndex(pRoot, pNode, boneCount);
}

Point3 MaxToDX(const Point3& p)
{
	return Point3(p.x, p.z, p.y);
}

Matrix3 MaxToDX(const Matrix3& m)
{
	Matrix3 mat;

	mat.SetRow(0, MaxToDX(m.GetRow(0)));
	mat.SetRow(1, MaxToDX(m.GetRow(2)));
	mat.SetRow(2, MaxToDX(m.GetRow(1)));
	mat.SetRow(3, MaxToDX(m.GetRow(3)));

	return mat;
}

Modifier* FindPhysiqueModifier(INode *pNode)
{
	Object* ObjectPtr = pNode->GetObjectRef();

	if (!ObjectPtr) return NULL;

	while (ObjectPtr->SuperClassID() == GEN_DERIVOB_CLASS_ID && ObjectPtr)
	{
		IDerivedObject *DerivedObjectPtr = (IDerivedObject *)(ObjectPtr);

		int ModStackIndex = 0;
		while (ModStackIndex < DerivedObjectPtr->NumModifiers())
		{
			Modifier* ModifierPtr = DerivedObjectPtr->GetModifier(ModStackIndex);

			if (ModifierPtr->ClassID() == Class_ID(PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B))
			{
				return ModifierPtr;
			}

			ModStackIndex++;
		}
		ObjectPtr = DerivedObjectPtr->GetObjRef();
	}

	return NULL;
}

Modifier* FindSkinModifier(INode *pNode)
{
	// Get object from node. Abort if no object.
	Object* ObjectPtr = pNode->GetObjectRef();


	if (!ObjectPtr) return NULL;

	// Is derived object ?
	while (ObjectPtr->SuperClassID() == GEN_DERIVOB_CLASS_ID && ObjectPtr)
	{
		// Yes -> Cast.
		IDerivedObject *DerivedObjectPtr = (IDerivedObject *)(ObjectPtr);

		// Iterate over all entries of the modifier stack.
		int ModStackIndex = 0;
		while (ModStackIndex < DerivedObjectPtr->NumModifiers())
		{
			// Get current modifier.
			Modifier* ModifierPtr = DerivedObjectPtr->GetModifier(ModStackIndex);

			// Is this Skin ?
			if (ModifierPtr->ClassID() == SKIN_CLASSID)
			{
				// Yes -> Exit.
				return ModifierPtr;
			}

			// Next modifier stack entry.
			ModStackIndex++;
		}
		ObjectPtr = DerivedObjectPtr->GetObjRef();
	}

	// Not found.
	return NULL;
}

// dummy view used internally by WriteMesh
class NullView : public View {
public:
	Point2 ViewToScreen(Point3 p)
	{ 
		return Point2(p.x,p.y); 
	}

	NullView() {
		worldToView.IdentityMatrix();
		screenW=800.0f; screenH = 600.0f;
	}

};

static NullView nullView;

class I4MaxExporter : public SceneExport {
	public:
		
		static HWND hParams;
		
		int				ExtCount();					// Number of extensions supported
		const TCHAR *	Ext(int n);					// Extension #n (i.e. "3DS")
		const TCHAR *	LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
		const TCHAR *	ShortDesc();				// Short ASCII description (i.e. "3D Studio")
		const TCHAR *	AuthorName();				// ASCII Author name
		const TCHAR *	CopyrightMessage();			// ASCII Copyright message
		const TCHAR *	OtherMessage1();			// Other message #1
		const TCHAR *	OtherMessage2();			// Other message #2
		unsigned int	Version();					// Version number * 100 (i.e. v3.01 = 301)
		void			ShowAbout(HWND hWnd);		// Show DLL's "About..." box

		BOOL SupportsOptions(int ext, DWORD options);
		int				DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);

		BOOL IsExportBone()						{ return exportBone; }
		void SetExportBone(BOOL isBone)			{ exportBone = isBone; }

		BOOL IsExportMesh()						{ return exportMesh; }
		void SetExportMesh(BOOL isMesh)			{ exportMesh = isMesh; }

		BOOL IsExportAnimation()				{ return exportAnimation; }
		void SetExportAnimation(BOOL isAni)		{ exportAnimation = isAni; }

		int GetSamplingStep()					{ return samplingStep; }
		void SetSamplingStep(int step)			{ samplingStep = step; }

		int GetSamplingStart()					{ return samplingStart; }
		void SetSamplingStart(int frame)		{ samplingStart = frame; }

		int GetSamplingEnd()					{ return samplingEnd; }
		void SetSamplingEnd(int frame)			{ samplingEnd = frame; }

		Interface* GetMaxInterface()			{ return maxInteface; }

		void ExportRecursive(INode* node);

		void WriteNodeStart(FILE* fp, INode* node);
		void WriteNodeEnd(FILE* fp);
		void WriteNodeInfo(FILE* fp, INode* node, const Matrix3& localTM, const Matrix3& worldTM);

		void WriteMaterial(FILE* fp, Mtl* mtl);

		void WriteTexture(FILE* fp, Texmap* tex, int subNo);
		void WriteMeshVertex(FILE* fp, Mesh& mesh, const Matrix3& objectTM, const Matrix3& worldInvTM);
		void WriteMeshNormal(FILE* fp, Mesh& mesh);
		void WriteMeshIndex(FILE* fp, Mesh& mesh);
		void WriteMeshTexUVIndex(FILE* fp, Mesh& mesh);
		void WriteMeshSkin(FILE* fp, INode* node, Modifier* mod);

		//Constructor/Destructor
		I4MaxExporter();
		~I4MaxExporter();		
	private:
		Interface*	maxInteface;
		FILE*		boneFile;
		FILE*		meshFile;
		FILE*		aniFile;

		BOOL		exportBone;
		BOOL		exportMesh;
		BOOL		exportAnimation;

		int			samplingStep;
		int			samplingStart;
		int			samplingEnd;
};



class I4MaxExporterClassDesc : public ClassDesc2 
{
public:
	virtual int IsPublic() 							{ return TRUE; }
	virtual void* Create(BOOL /*loading = FALSE*/) 	{ return new I4MaxExporter(); }
	virtual const TCHAR *	ClassName() 			{ return GetString(IDS_CLASS_NAME); }
	virtual SClass_ID SuperClassID() 				{ return SCENE_EXPORT_CLASS_ID; }
	virtual Class_ID ClassID() 						{ return I4MaxExporter_CLASS_ID; }
	virtual const TCHAR* Category() 				{ return GetString(IDS_CATEGORY); }

	virtual const TCHAR* InternalName() 			{ return _T("I4MaxExporter"); }	// returns fixed parsable name (scripter-visible name)
	virtual HINSTANCE HInstance() 					{ return hInstance; }					// returns owning module handle
	

};


ClassDesc2* GetI4MaxExporterDesc() { 
	static I4MaxExporterClassDesc I4MaxExporterDesc;
	return &I4MaxExporterDesc; 
}





INT_PTR CALLBACK I4MaxExporterOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
	static I4MaxExporter *exp = NULL;
	ISpinnerControl  *spin;
	int startFrame;
	int endFrame;

	switch(message) {
		case WM_INITDIALOG:
			exp = (I4MaxExporter *)lParam;
			startFrame = exp->GetMaxInterface()->GetAnimRange().Start()/GetTicksPerFrame();
			endFrame = exp->GetMaxInterface()->GetAnimRange().End()/GetTicksPerFrame();

			CenterWindow(hWnd,GetParent(hWnd));
			CheckDlgButton(hWnd, IDC_CHECK_EXPORT_BONE,			exp->IsExportBone());
			CheckDlgButton(hWnd, IDC_CHECK_EXPORT_MESH,			exp->IsExportMesh()); 
			CheckDlgButton(hWnd, IDC_CHECK_EXPORT_ANIMATION,	exp->IsExportAnimation());

			spin = GetISpinner(GetDlgItem(hWnd, IDC_SPIN_SAMPLING_STEP)); 
			spin->LinkToEdit(GetDlgItem(hWnd,IDC_EDIT_SAMPLING_STEP), EDITTYPE_INT ); 
			spin->SetLimits(1, 100, TRUE); 
			spin->SetScale(1.0f);
			spin->SetValue(exp->GetSamplingStep(), FALSE);
			ReleaseISpinner(spin);


			spin = GetISpinner(GetDlgItem(hWnd, IDC_SPIN_SAMPLING_START)); 
			spin->LinkToEdit(GetDlgItem(hWnd,IDC_EDIT_SAMPLING_START), EDITTYPE_INT ); 
			spin->SetLimits(startFrame, endFrame, TRUE); 
			spin->SetScale(1.0f);
			spin->SetValue(startFrame, FALSE);
			ReleaseISpinner(spin);


			spin = GetISpinner(GetDlgItem(hWnd, IDC_SPIN_SAMPLING_END)); 
			spin->LinkToEdit(GetDlgItem(hWnd,IDC_EDIT_SAMPLING_END), EDITTYPE_INT ); 
			spin->SetLimits(startFrame, endFrame, TRUE); 
			spin->SetScale(1.0f);
			spin->SetValue(endFrame, FALSE);
			ReleaseISpinner(spin);
			return TRUE;
		case CC_SPINNER_CHANGE:
			spin = (ISpinnerControl*)lParam; 
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDOK:
				exp->SetExportBone(IsDlgButtonChecked(hWnd, IDC_CHECK_EXPORT_BONE)); 
				exp->SetExportMesh(IsDlgButtonChecked(hWnd, IDC_CHECK_EXPORT_MESH)); 
				exp->SetExportAnimation(IsDlgButtonChecked(hWnd, IDC_CHECK_EXPORT_ANIMATION)); 

				spin = GetISpinner(GetDlgItem(hWnd, IDC_SPIN_SAMPLING_STEP)); 
				exp->SetSamplingStep(spin->GetIVal());
				ReleaseISpinner(spin);

				spin = GetISpinner(GetDlgItem(hWnd, IDC_SPIN_SAMPLING_START)); 
				exp->SetSamplingStart(spin->GetIVal());
				ReleaseISpinner(spin);

				spin = GetISpinner(GetDlgItem(hWnd, IDC_SPIN_SAMPLING_END)); 
				exp->SetSamplingEnd(spin->GetIVal());
				ReleaseISpinner(spin);

				EndDialog(hWnd, TRUE);
				break;
			case IDCANCEL:
				EndDialog(hWnd, FALSE);
				break;
			}
			break;
		case WM_CLOSE:
			EndDialog(hWnd, 0);
			return 1;
	}
	return 0;
}


//--- I4MaxExporter -------------------------------------------------------
I4MaxExporter::I4MaxExporter()
: maxInteface(NULL)
, boneFile(NULL)
, meshFile(NULL)
, aniFile(NULL)
, exportBone(TRUE)
, exportMesh(TRUE)
, exportAnimation(TRUE)
, samplingStep(2)
{

}

I4MaxExporter::~I4MaxExporter() 
{

}

int I4MaxExporter::ExtCount()
{
//	#pragma message(TODO("Returns the number of file name extensions supported by the plug-in."))
	return 1;
}

const TCHAR *I4MaxExporter::Ext(int n)
{		
//	#pragma message(TODO("Return the 'i-th' file name extension (i.e. \"3DS\")."))
	return _T("xml");
}

const TCHAR *I4MaxExporter::LongDesc()
{
//	#pragma message(TODO("Return long ASCII description (i.e. \"Targa 2.0 Image File\")"))
	return _T("I4 mesh/bone/ani xml File");
}
	
const TCHAR *I4MaxExporter::ShortDesc() 
{			
//	#pragma message(TODO("Return short ASCII description (i.e. \"Targa\")"))
	return _T("I4 xml");
}

const TCHAR *I4MaxExporter::AuthorName()
{			
//	#pragma message(TODO("Return ASCII Author name"))
	return _T("");
}

const TCHAR *I4MaxExporter::CopyrightMessage() 
{	
//	#pragma message(TODO("Return ASCII Copyright message"))
	return _T("");
}

const TCHAR *I4MaxExporter::OtherMessage1() 
{		
	//TODO: Return Other message #1 if any
	return _T("");
}

const TCHAR *I4MaxExporter::OtherMessage2() 
{		
	//TODO: Return other message #2 in any
	return _T("");
}

unsigned int I4MaxExporter::Version()
{				
//	#pragma message(TODO("Return Version number * 100 (i.e. v3.01 = 301)"))
	return 100;
}

void I4MaxExporter::ShowAbout(HWND hWnd)
{			
	// Optional
}

BOOL I4MaxExporter::SupportsOptions(int ext, DWORD options)
{
//	#pragma message(TODO("Decide which options to support.  Simply return true for each option supported by each Extension the exporter supports."))
	return TRUE;
}


int	I4MaxExporter::DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options)
{
	maxInteface = i;

	if(!suppressPrompts)
	{
		if (!DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_PANEL), GetActiveWindow(), I4MaxExporterOptionsDlgProc, (LPARAM)this))
			return FALSE;
	}

	CStr fullpath = name;
	CStr path;
	CStr file;
	CStr ext;

	SplitFilename(fullpath, &path, &file, &ext);

	if (IsExportBone())
	{
		CStr boneFileName = path + "\\" + file + ".bone.xml";
		boneFile = _tfopen(boneFileName.data(), "w");
		if (boneFile == NULL)
		{
			MessageBox(NULL, "Couldn't open file", NULL, MB_OK);
			return FALSE;
		}

		fprintf(boneFile, "<bone>\n");
		fprintf(boneFile, "\t<version>1.0.0</version>\n");
	}


	if (IsExportMesh())
	{
		CStr meshFileName = path + "\\" + file + ".mesh.xml";
		meshFile = _tfopen(meshFileName.data(), "w");
		if (meshFile == NULL)
		{
			MessageBox(NULL, "Couldn't open file", NULL, MB_OK);
			return FALSE;
		}

		fprintf(meshFile, "<mesh>\n");
		fprintf(meshFile, "\t<version>1.0.0</version>\n");
	}

	if (IsExportAnimation())
	{
		CStr aniFileName = path + "\\" + file + ".ani.xml";
		aniFile = _tfopen(aniFileName.data(), "w");
		if (aniFile == NULL)
		{
			MessageBox(NULL, "Couldn't open file", NULL, MB_OK);
			return FALSE;
		}

		fprintf(aniFile, "<ani>\n");
		fprintf(aniFile, "\t<version>1.0.0</version>\n");

		Interval range = maxInteface->GetAnimRange();
		TimeValue startTick = range.Start();
		TimeValue endTick = range.End();

		int startFrame = startTick/GetTicksPerFrame();
		int endFrame = endTick/GetTicksPerFrame();

		fprintf(aniFile, "\t<startFrame>%d</startFrame>\n", startFrame);
		fprintf(aniFile, "\t<endFrame>%d</endFrame>\n", endFrame - startFrame);
	}

	ExportRecursive(maxInteface->GetRootNode());

	if (IsExportBone())
	{
		fprintf(boneFile, "</bone>\n");
		fclose(boneFile);
	}

	if (IsExportMesh())
	{
		fprintf(meshFile, "</mesh>\n");
		fclose(meshFile);
	}

	if (IsExportAnimation())
	{
		fprintf(aniFile, "</ani>\n");

		fclose(aniFile);
	}

	return TRUE;
}



void I4MaxExporter::ExportRecursive(INode* node)
{
	if (node == NULL)
		return;

	if (IsBone(node) || IsMesh(node))
	{
		Matrix3 objectTM = node->GetObjectTM(0);
		Matrix3 objectInvTM = Inverse(objectTM);
		Matrix3 worldTM = node->GetNodeTM(0);
		Matrix3 worldInvTM = Inverse(worldTM);
		Matrix3 parentInvTM = Inverse(node->GetParentTM(0));
		Matrix3 localTM = worldTM*parentInvTM;
		Matrix3 localInvTM = Inverse(localTM);

		if (IsBone(node))
		{
			if (IsExportBone())
			{
				WriteNodeStart(boneFile, node);
				WriteNodeInfo(boneFile, node, localTM, worldTM);
				WriteNodeEnd(boneFile);
			}			
		}
		else if (IsMesh(node))
		{
			if (IsExportMesh())
			{
				WriteNodeStart(meshFile, node);
				WriteNodeInfo(meshFile, node, localTM, worldTM);
				

				fprintf(meshFile, "\t\t<material>\n");
				WriteMaterial(meshFile, node->GetMtl());
				fprintf(meshFile, "\t\t</material>\n");

				ObjectState os = node->EvalWorldState(0);
				int needDelete;
				Mesh& mesh = *(((GeomObject*)os.obj)->GetRenderMesh(0, node, nullView, needDelete));

				WriteMeshVertex(meshFile, mesh, objectTM, worldInvTM);
				WriteMeshNormal(meshFile, mesh);
				WriteMeshIndex(meshFile, mesh);
				WriteMeshTexUVIndex(meshFile, mesh);

				Modifier* phyMod = FindPhysiqueModifier(node);
				if (phyMod)
				{
					// 지원안함.
				}

				Modifier* skinMod = FindSkinModifier(node);
				if (skinMod)
				{
					WriteMeshSkin(meshFile, node, skinMod);
				}

				WriteNodeEnd(meshFile);
			}

		}

		if (IsExportAnimation())
		{
			// 에니메이션 샘플링..

			// TODO : 키값만 저장하는 부분은 추후에 필요할때 지원한다.
			// TODO : 스케일은 추후에 필요할때 지원한다.

			Interval range = maxInteface->GetAnimRange();

			bool isPosKey = false;
			bool isRotKey = false;
			bool isScaleKey = false;

			int deltaTick = GetTicksPerFrame()*samplingStep;
			TimeValue startTick = GetTicksPerFrame()*samplingStart;
			TimeValue endTick = GetTicksPerFrame()*samplingEnd;

			Point3 initPos;
			Quat initRot;
			Point3 initScale;
			Matrix3 initTM = node->GetNodeTM(0)*Inverse(node->GetParentTM(0));
			DecomposeMatrix(initTM, initPos, initRot, initScale);

			Matrix3 tm;
			Point3 pos;
			Quat rot;
			Point3 scale;

			for (TimeValue t = startTick; t < endTick + deltaTick; t += deltaTick)
			{
				if (t > endTick)
				{
					t = endTick;
				}

				tm = node->GetNodeTM(t)*Inverse(node->GetParentTM(t));
				DecomposeMatrix(tm, pos, rot, scale);
				if (!isPosKey)
				{
					if (!pos.Equals(initPos, POS_KEY_EPSILON))
					{
						isPosKey = true;
					}
				}

				if (!isRotKey)
				{
					if (!rot.Equals(initRot, ROT_KEY_EPSILON))
					{
						isRotKey = true;
					}
				}

				if (isPosKey && isRotKey)		// 키가 모두 존재하면 끝..
					break;
			}

			if (isPosKey || isRotKey)
			{
				WriteNodeStart(aniFile, node);
				if (isPosKey)
				{
					vector<int>		vecKeyFrame;
					vector<Point3>	vecKeyPos;				

					int size = (endTick - startTick)/deltaTick + 1;
					vecKeyFrame.reserve(size);
					vecKeyPos.reserve(size);				

					Point3 prevPos = initPos;
					int frame = 0;
					bool prevEquals = false;
					for (TimeValue t = startTick; t < endTick + deltaTick; t += deltaTick, frame += samplingStep)
					{
						if (t > endTick)
						{
							t = endTick;
						}

						if (frame > samplingEnd - samplingStart)
						{
							frame = samplingEnd - samplingStart;
						}

						Matrix3 m = node->GetParentTM(t);
						tm = node->GetNodeTM(t)*Inverse(node->GetParentTM(t));
						pos = tm.GetTrans();
						if (vecKeyPos.size() != 0)
						{
							if (pos.Equals(prevPos, POS_KEY_EPSILON))
							{
								prevEquals = true;
								continue;
							}

							if (prevEquals)	// 이전 키값이 같은 상태가 유지되었으면
							{
								prevEquals = false;

								// 이전 키값을 한번 더 넣어줘서 보간에 의한 의도되지 않은 에니메이션이 이루어지는 것을 막는다.
								vecKeyFrame.push_back(frame - samplingStep);
								vecKeyPos.push_back(prevPos);
							}
						}

						prevPos = pos;

						vecKeyFrame.push_back(frame);
						vecKeyPos.push_back(pos);
					}

					size_t numKeyPos = vecKeyPos.size();
					fprintf(aniFile, "\t\t<posKey count=\"%d\">\n", numKeyPos);
					for (size_t i = 0; i < numKeyPos; ++i)
					{
						Point3 p = MaxToDX(vecKeyPos[i]);
						fprintf(aniFile, "\t\t\t<a frame=\"%d\">%g %g %g</a>\n", vecKeyFrame[i], p.x, p.y, p.z);
					}

					fprintf(aniFile, "\t\t</posKey>\n");
				}


				if (isRotKey)
				{
					vector<int>		vecKeyFrame;
					vector<Quat>	vecKeyRot;				

					int size = (endTick - startTick)/deltaTick + 1;
					vecKeyFrame.reserve(size);
					vecKeyRot.reserve(size);				

					Quat prevRot = initRot;
					Quat accumRot;
					int frame = 0;
					bool prevIdentity = false;
					for (TimeValue t = startTick; t < endTick + deltaTick; t += deltaTick, frame += samplingStep)
					{
						if (t > endTick)
						{
							t = endTick;
						}

						if (frame > samplingEnd - samplingStart)
						{
							frame = samplingEnd - samplingStart;
						}

						tm = node->GetNodeTM(t)*Inverse(node->GetParentTM(t));

						Quat quat(tm);

						Quat deltaQuat = quat/prevRot;

						if (vecKeyRot.size() != 0)
						{
							if (deltaQuat.IsIdentity())
							{
								prevIdentity = true;
								continue;
							}

							if (prevIdentity)	// 이전 키값이 같은 상태가 유지되었으면
							{
								prevIdentity = false;

								// 이전 키값을 한번 더 넣어줘서 보간에 의한 의도되지 않은 에니메이션이 이루어지는 것을 막는다.
								vecKeyFrame.push_back(frame - samplingStep);
								vecKeyRot.push_back(accumRot);
							}
						}

						accumRot = accumRot*deltaQuat;
						prevRot = quat;

						vecKeyFrame.push_back(frame);
						vecKeyRot.push_back(accumRot);
					}

					size_t numKeyRot = vecKeyRot.size();
					fprintf(aniFile, "\t\t<rotKey count=\"%d\">\n", numKeyRot);
					for (size_t i = 0; i < numKeyRot; ++i)
					{
						fprintf(aniFile, "\t\t\t<a frame=\"%d\">%g %g %g %g</a>\n",  vecKeyFrame[i], vecKeyRot[i].x, vecKeyRot[i].z, vecKeyRot[i].y, vecKeyRot[i].w);
					}

					fprintf(aniFile, "\t\t</rotKey>\n");
				}

				WriteNodeEnd(aniFile);
			}
		}
	}

	for (int i=0; i<node->NumberOfChildren(); ++i)
	{
		ExportRecursive(node->GetChildNode(i));
	}
}



void I4MaxExporter::WriteNodeStart(FILE* fp, INode* node)
{
	fprintf(fp, "\t<node name=\"%s\" ", node->GetName());

	if (node->GetParentNode()->IsRootNode())
	{
		fprintf(fp, "parent_name=\"%s\">\n", "undefined");
	}
	else
	{
		fprintf(fp, "parent_name=\"%s\">\n", node->GetParentNode()->GetName());
	}
}


void I4MaxExporter::WriteNodeEnd(FILE* fp)
{
	fprintf(fp, "\t</node>\n");
}

void I4MaxExporter::WriteNodeInfo(FILE* fp, INode* node, const Matrix3& localTM, const Matrix3& worldTM)
{	
	// node info - localTM
	Point3 p;
	Matrix3 localTM_DX = MaxToDX(localTM);
	fprintf(fp, "\t\t<localTM>\n");
	for (int i = 0; i < 4; ++i)
	{
		p = localTM_DX.GetRow(i);
		fprintf(fp, "\t\t\t<a>%g %g %g</a>\n", p.x, p.y, p.z);
	}
	fprintf(fp, "\t\t</localTM>\n");

	// node info - worldTM
	Matrix3 worldTM_DX = MaxToDX(worldTM);
	fprintf(fp, "\t\t<worldTM>\n");
	for (int i = 0; i < 4; ++i)
	{
		p = worldTM_DX.GetRow(i);
		fprintf(fp, "\t\t\t<a>%g %g %g</a>\n", p.x, p.y, p.z);
	}
	fprintf(fp, "\t\t</worldTM>\n");
}


void I4MaxExporter::WriteMaterial(FILE* fp, Mtl* mtl)
{
	if(mtl == NULL)
		return;

	if(mtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0))
	{
		// 표준 재질로 변환함
		StdMat* stdMtl = static_cast<StdMat*>(mtl);

		Color ambient = stdMtl->GetAmbient(0);
		fprintf(fp,"\t\t\t<ambient>%g %g %g</ambient>\n", ambient.r, ambient.g, ambient.b);

		Color diffuse = stdMtl->GetDiffuse(0);
		fprintf(fp,"\t\t\t<diffuse>%g %g %g</diffuse>\n", diffuse.r, diffuse.g, diffuse.b);

		Color specular = stdMtl->GetSpecular(0);
		fprintf(fp,"\t\t\t<specular>%g %g %g</specular>\n", specular.r, specular.g, specular.b);

		float emissive = stdMtl->GetSelfIllum(0);
		fprintf(fp,"\t\t\t<emissive>%g</emissive>\n", emissive);

		Shader* shader = static_cast<StdMat2*>(stdMtl)->GetShader();

		float glossiness = shader->GetGlossiness(0);
		fprintf(fp,"\t\t\t<glossiness>%g</glossiness>\n", glossiness);

		float power = shader->GetSpecularLevel(0);
		fprintf(fp,"\t\t\t<power>%g</power>\n", power*100);

		if (stdMtl->GetTwoSided())
		{
			fprintf(fp,"\t\t\t<twoside>true</twoside>\n");
		}
		else
		{
			fprintf(fp,"\t\t\t<twoside>false</twoside>\n");
		}

		for (int i = 0; i < mtl->NumSubTexmaps(); ++i)
		{
			Texmap* tex = mtl->GetSubTexmap(i);
			if (tex != NULL) 
			{
				// If it is a standard material we can see if the map is enabled.
				if (mtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0)) {
					if (!((StdMat*)mtl)->MapEnabled(i))
						continue;
				}

				WriteTexture(fp, tex, i);
			}
		}
	}
	else
	{
		// 표준 마테리얼이 아닌 재질은 지원하지 않는다.
	}
}


void I4MaxExporter::WriteTexture(FILE* fp, Texmap* tex, int subNo)
{
	if (tex == NULL)
		return;

	// 비트맵 텍스처일 경우만 처리
	if(tex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0))
	{
		CStr fullpath = static_cast<BitmapTex*>(tex)->GetMapName();
		CStr path;
		CStr name;
		CStr ext;

		CStr classname;

		SplitFilename(fullpath, &path, &name, &ext);

		CStr file = name + ext;

		switch (subNo)
		{
			case ID_AM:
				break;
			case ID_DI:
				fprintf(fp,"\t\t\t<diffuseMap>%s</diffuseMap>\n", file.data());
				break;
			case ID_SP:
				break;
			case ID_SH:				
				fprintf(fp,"\t\t\t<specularMap>%s</specularMap>\n", file.data());
				break;
			case ID_SS:
				break;
			case ID_SI:
				break;
			case ID_OP:
				break;
			case ID_FI:
				break;
			case ID_BU:
				fprintf(fp,"\t\t\t<normalMap>%s</normalMap>\n", file.data());
				break;
			case ID_RL:
				break;
			case ID_RR:
				break;
		}
	}

	for (int j=0; j<tex->NumSubTexmaps(); j++)
	{
		Texmap* subTex = tex->GetSubTexmap(j);
		WriteTexture(fp, subTex, j);
	}
}

void I4MaxExporter::WriteMeshVertex(FILE* fp, Mesh& mesh, const Matrix3& objectTM, const Matrix3& worldInvTM)
{
	// mesh vertex
	int numVerts = mesh.getNumVerts();
	fprintf(fp, "\t\t<vertex count=\"%d\">\n", numVerts);
	for (int i = 0; i < numVerts; ++i)
	{
		// 맥스에서 피봇은 로컬좌표계의 원점이 아니라 회전의 중심축일 뿐이다.
		// 그래서 로컬 버텍스를 직접 구하면 피봇을 무시한 잘못된 로컬 버텍스를 구하게 된다.
		// 따라서 world vertex를 구해서 inverse tm을 곱하는 방법을 사용해야한다.
		Point3 p = MaxToDX(mesh.getVert(i)*objectTM*worldInvTM);
		fprintf(fp, "\t\t\t<a>%g %g %g</a>\n", p.x, p.y, p.z);
	}
	fprintf(fp, "\t\t</vertex>\n");
}

void I4MaxExporter::WriteMeshNormal(FILE* fp, Mesh& mesh)
{
	// mesh normals
	Point3 n;
	mesh.buildNormals();
	int numNorms = mesh.getNumVerts();
	fprintf(fp, "\t\t<normal count=\"%d\">\n", numNorms);
	for (int i = 0; i < numNorms; ++i)
	{ 
		n = MaxToDX(Normalize(mesh.getNormal(i)));
		fprintf(fp, "\t\t\t<a>%g %g %g</a>\n", n.x, n.y, n.z);
	}
	fprintf(fp, "\t\t</normal>\n");
}

void I4MaxExporter::WriteMeshIndex(FILE* fp, Mesh& mesh)
{
	// mesh index
	int numIndex = mesh.getNumFaces();
	fprintf(fp, "\t\t<index count=\"%d\">\n", numIndex);
	for (int i = 0; i < numIndex; ++i)
	{
		fprintf(fp, "\t\t\t<a>%d %d %d</a>\n", mesh.faces[i].v[0],  mesh.faces[i].v[2], mesh.faces[i].v[1]);
	}
	fprintf(fp, "\t\t</index>\n");
}

void I4MaxExporter::WriteMeshTexUVIndex(FILE* fp, Mesh& mesh)
{
	int numTVerts = mesh.getNumTVerts();
	if (numTVerts > 0)
	{
		// mesh texUV
		fprintf(fp, "\t\t<texUV count=\"%d\">\n", numTVerts);
		for (int i = 0; i < numTVerts; ++i)
		{
			fprintf(fp, "\t\t\t<a>%g %g</a>\n", mesh.tVerts[i].x, 1.0f - mesh.tVerts[i].y);
		}
		fprintf(fp, "\t\t</texUV>\n");

		// mesh texIndex
		int numTIndex = mesh.getNumFaces();
		fprintf(fp, "\t\t<texIndex count=\"%d\">\n", numTIndex);
		for (int i = 0; i < numTIndex; ++i)
		{
			fprintf(fp, "\t\t\t<a>%d %d %d</a>\n", mesh.tvFace[i].t[0],  mesh.tvFace[i].t[2], mesh.tvFace[i].t[1]);
		}
		fprintf(fp, "\t\t</texIndex>\n");
	}
}

void I4MaxExporter::WriteMeshSkin(FILE* fp, INode* node, Modifier* mod)
{
	ISkin *skin = (ISkin *)mod->GetInterface(I_SKIN);

	ISkinContextData *skin_data = (ISkinContextData *)skin->GetContextInterface(node);

	Object *obj = node->EvalWorldState(maxInteface->GetTime()).obj;

	TriObject *tri = (TriObject*)obj->ConvertToType(maxInteface->GetTime(),Class_ID(TRIOBJ_CLASS_ID,0));
	Mesh* mesh = &tri->GetMesh();

	int numVerts = mesh->numVerts ;

	fprintf(fp, "\t\t<weight count=\"%d\">\n", numVerts);

	for (int i = 0; i<numVerts; i++)
	{
		fprintf(fp, "\t\t\t<vertex>\n"); 
		float weight = 0.0f;
		int boneIdx = -1;

		vector<WeightInfo> vecWeight;
		int numWeights = skin_data->GetNumAssignedBones(i);
		for(int j = 0 ; j < numWeights ; j ++)
		{ 
			INode * pBone = skin->GetBone(skin_data->GetAssignedBone(i, j));			
			weight = skin_data->GetBoneWeight(i, j);
			if(weight < WEIGHT_EPSILON) continue ;

			boneIdx = GetBoneIndex(maxInteface->GetRootNode(), pBone);
			WeightInfo w;
			w.boneIdx = boneIdx;
			w.weight = weight;
			vecWeight.push_back(w);

		}

		std::sort(vecWeight.begin(), vecWeight.end());
		for (int j = 0; j < vecWeight.size(); ++j)
		{
			fprintf(fp, "\t\t\t\t<a>%d %g</a>\n", vecWeight[j].boneIdx, vecWeight[j].weight);
		}
		fprintf(fp, "\t\t\t</vertex>\n");
	}

	fprintf(fp, "\t\t</weight>\n");
	mod->ReleaseInterface(I_SKIN, skin);
}

