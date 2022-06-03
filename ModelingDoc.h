// ModelingDoc.h : interface of the CModelingDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MODELINGDOC_H__30453389_3E75_11D7_8611_0060B0EE281E__INCLUDED_)
#define AFX_MODELINGDOC_H__30453389_3E75_11D7_8611_0060B0EE281E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <vector>
#include <iostream>
#include <OCC_3dDoc.h>
#include <..\05_ImportExport\src\ColoredShapes.h>
#include <TopTools_HSequenceOfShape.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <TopoDS_Shape.hxx>


class CModelingDoc : public OCC_3dDoc
{
	
protected: // create from serialization only
	CModelingDoc();
	DECLARE_DYNCREATE(CModelingDoc)

public:
	virtual ~CModelingDoc();

	void InputEvent     (const Standard_Integer  x       ,
	    			     const Standard_Integer  y       ,
                         const Handle(V3d_View)& aView   );
	void Popup          (const Standard_Integer  x       ,
		    			 const Standard_Integer  y       ,
                         const Handle(V3d_View)& aView   );

// Generated message map functions
protected:
	//{{AFX_MSG(CModelingDoc)
	afx_msg void OnMirror();
	afx_msg void OnMirroraxis();
	afx_msg void OnRotate();
	afx_msg void OnScale();
	afx_msg void OnTranslation();
	afx_msg void OnDisplacement();
	afx_msg void OnDeform();
	afx_msg void OnBox();
	afx_msg void OnCylinder();
	afx_msg void OnCone();
	afx_msg void OnSphere();
	afx_msg void OnTorus();
	afx_msg void OnWedge();
	afx_msg void OnPrism();
	afx_msg void OnRevol();
	afx_msg void OnPipe();
	afx_msg void OnThru();
	afx_msg void OnEvolved();
	afx_msg void OnDraft();
	afx_msg void OnCut();
	afx_msg void OnFuse();
	afx_msg void OnSection();
	afx_msg void OnCommon();
	afx_msg void OnPsection();
	afx_msg void OnBlend();
	afx_msg void OnChamf();
	afx_msg void OnEvolvedblend();
	afx_msg void OnPrismLocal();
	afx_msg void OnRevolLocal();
	afx_msg void OnGlueLocal();
	afx_msg void OnDprismLocal();
	afx_msg void OnPipeLocal();
	afx_msg void OnLinearLocal();
	afx_msg void OnSplitLocal();
	afx_msg void OnThickLocal();
	afx_msg void OnOffsetLocal();
	afx_msg void OnVertex();
	afx_msg void OnEdge();
	afx_msg void OnWire();
	afx_msg void OnFace();
	afx_msg void OnShell();
	afx_msg void OnCompound();
	afx_msg void OnGeometrie();
	afx_msg void OnSewing();
	afx_msg void OnExplorer();
	afx_msg void OnBuilder();
	afx_msg void OnValid();
	afx_msg void OnLinear();
	afx_msg void OnVolume();
	afx_msg void OnSurface();
	afx_msg void OnFillwithtang();
	afx_msg void OnButtonFill();
	afx_msg void OnStopStop();

	afx_msg void OnBottle();
	afx_msg void OnStep();
	afx_msg void OnCreateBox();
	afx_msg void OnCreateVolute();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	TopoDS_Shape MakeBottle(const Standard_Real myWidth, const Standard_Real myHeight, const Standard_Real myThickness);

public:
	
	int myState;
	static void ReadSTEP1(const Handle(AIS_InteractiveContext)& anInteractiveContext);
	static Handle(TopTools_HSequenceOfShape) ReadSTEP1(); // not by reference --> the sequence is created here !!
	static IFSelect_ReturnStatus ReadSTEP1(const Standard_CString& aFileName, Handle(TopTools_HSequenceOfShape)& aHSequenceOfShape);

	std::vector<TopoDS_Shape> BladeShapes;
	std::vector<TopoDS_Shape> FinalHubShapes;
	std::vector<TopoDS_Shape> FinalBladeShapes;
	std::vector<TopoDS_Edge> psEdgeArray;
	std::vector<TopoDS_Edge> ssEdgeArray;
	std::vector<TopoDS_Edge> leEdgeArray;
	std::vector<TopoDS_Edge> teEdgeArray;
	std::vector<TopoDS_Edge> horEdgeArray;
	std::vector<TopoDS_Edge> horEdgeArray2;
	std::vector<gp_Pnt> gp_Pnt_list;
	std::vector<TopoDS_Wire> wireList;
	std::vector<TopoDS_Edge> CreateIsoCurves(TopoDS_Face face, int numSections, int uIso);
	TopoDS_Shape CreateFaceFromWires(std::vector<TopoDS_Edge> edgeArray);
	TopoDS_Edge CreateHubline(TopoDS_Face ps, TopoDS_Face ss);
	TopoDS_Edge ExtendEdge(TopoDS_Edge edgeToExtend, Standard_Boolean extendAfter, bool extendStart);
	Handle(Geom_BSplineCurve) MkBSplinecurve();
	std::vector<gp_Pnt> GetPointsFromEdge(TopoDS_Edge edge, int n);

	TopoDS_Shape GetTranslation(TopoDS_Shape shape, gp_Vec vec);
	TopoDS_Shape CreateHubFace(TopoDS_Edge extendEdge, TopoDS_Edge transformEdge);
	TopoDS_Shape Rotating(Standard_Real arcWidth, TopoDS_Shape shape);
	TopoDS_Shape Revolving(TopoDS_Shape myShape);
	TopoDS_Face TopHubFace(TopoDS_Shape shape);
	TopoDS_Edge BladeBaseEdges(TopoDS_Face le, TopoDS_Face te);
	std::vector<TopoDS_Shape> CModelingDoc::FinalShapes(TopoDS_Shape shape, TopoDS_Face face, std::vector<TopoDS_Wire> wireList);
	TopoDS_Shape makeVolumeShape(std::vector<TopoDS_Shape> faceList, double tolerance);
	
	TopoDS_Wire ConnectBladeBaseEdges();
	bool CheckForReverseWire(TopoDS_Face face, TopoDS_Wire wire);
	TopoDS_Face CreateHoledFaceModified(TopoDS_Face inputFace, std::vector<TopoDS_Wire> &holeWireList);
};

class GenerateVolute : public OCC_3dDoc
{
public:
	std::vector<TopoDS_Wire> CreateSection(double width, std::vector<double> array);
	std::vector<TopoDS_Wire> CreateSectionWithoutBase(double width, std::vector<double> area);
	std::vector<TopoDS_Wire> CreateBaseSections(double width, std::vector<double> area);

	std::vector<double> AreaCalculation(double startArea, double endArea);
	TopoDS_Wire RotateWire(TopoDS_Wire wire, Standard_Real angle);
	std::vector<TopoDS_Wire> RotateCrossSections(std::vector<TopoDS_Wire> wireVec, std::vector<double> angleVec);
	TopoDS_Shape CreateCompoundShape(std::vector<TopoDS_Wire> list);
	TopoDS_Shape CreateShellThruSect(std::vector<TopoDS_Wire> wireArray, Standard_Real startAngle, Standard_Real endAngle, std::vector<double> angleVec);
	std::vector<TopoDS_Shape> CreateShellList(std::vector<TopoDS_Wire> wireVec, std::vector<double> angleVec);
	TopoDS_Shape SewingScrollShells(std::vector<TopoDS_Shape> shellList, double tolerance);
	std::vector<double> CreateAngleVector();
	gp_Pnt GetMiddlePoint();
	TopoDS_Wire CreateSquareToGetExitPipe(gp_Pnt centerPnt, double area);
	TopoDS_Wire CreateCircleToGetExitPipe(gp_Pnt centrePnt, double area);
	TopoDS_Wire GetTranslationOfWire(TopoDS_Shape shape, gp_Vec vec);
	TopoDS_Wire GetFilletedWireForTrans(TopoDS_Shape shape);
	TopoDS_Shape ThruSectExitPipe(TopoDS_Wire wire1, TopoDS_Wire wire2, Standard_Boolean isSolid);
	TopoDS_Shape SewedExitPipe(TopoDS_Shape shape1, TopoDS_Shape shape2, double tolerance);
	
	
	TopoDS_Shape CreateSolidThruSect(std::vector<TopoDS_Wire> wireVector, Standard_Real startAngle, Standard_Real endAngle, std::vector<double> angleVec);
	std::vector<TopoDS_Shape> CreateSolidList(std::vector<TopoDS_Wire> wireVec, std::vector<double> angleVec);
	TopoDS_Shape GetSimpleBooleanResult(TopoDS_Shape shape, TopoDS_Solid solid);
	TopoDS_Shape GetSimpleBooleanResultForFilletedShapes(TopoDS_Shape shape, TopoDS_Shape solid);
	TopoDS_Shape GetBooleanResultForSolids(TopoDS_Shape shape, TopoDS_Shape solid);
	TopoDS_Shape GetBooleanFuseForSolids(TopoDS_Shape shape, TopoDS_Shape solid);
	std::vector<TopoDS_Shape> CreateInputPlanes(std::vector<TopoDS_Wire> wireVec, std::vector<double> angleVec);
	std::vector<TopoDS_Face> CreateInputPlaneFaces(std::vector<TopoDS_Shape> baseScroll);

	TopoDS_Shape CreateBasePlane();
	
	TopoDS_Shape ReverseShapeIfInsideOut(TopoDS_Solid& solid);
	TopoDS_Shape SewBaseScrollWithExitPipe(std::vector<TopoDS_Shape> scrollShells, std::vector<TopoDS_Face> baseScroll, std::vector<TopoDS_Shape> exitPipe, std::vector<TopoDS_Face> exitPlane, double tolerance);
	TopoDS_Shape SewVoluteFaces(std::vector<TopoDS_Face> sewScroll, std::vector<TopoDS_Face> baseScroll, std::vector<TopoDS_Face> exitPipe, std::vector<TopoDS_Face> exitPlane, double tolerance);
	TopoDS_Shape SewSolidVolute(std::vector<TopoDS_Shape> sewScroll, std::vector<TopoDS_Shape> exitPipe, double tolerance);
	TopoDS_Solid mkSolidOfSewedVoluteWithExitPipe(TopoDS_Shape voluteShape);
	std::vector<TopoDS_Face> GetFacesFromShapes(std::vector<TopoDS_Shape> shapeVec);
	std::vector<TopoDS_Shape> CreateShellVectorAfterBooleanCut();
	std::vector<TopoDS_Shape> CreateShellVectorWithBoolCut(TopoDS_Shape smallBoolCutShape, std::vector<TopoDS_Shape> scrollShapeVec);
	std::vector<TopoDS_Shape> CreateSolidVectorWithBoolCut(TopoDS_Shape smallBoolCutShape);
	std::vector<TopoDS_Shape> CreateSolidScrollVector();
	TopoDS_Wire Apply2dFilletsForWireWithoutBase(TopoDS_Wire wire, double radius);
	//TopoDS_Wire Apply2dFilletsForWireWithBase(TopoDS_Wire wire, double radius);
	std::vector<TopoDS_Wire> Create2dFilletedWirevec(std::vector<TopoDS_Edge> edgeVec);
	void getVerticesFromEdges();
	void getVerticesFromWires(TopoDS_Wire wire);

	std::vector<TopoDS_Wire> createFilletedCrossSections(std::vector<TopoDS_Wire> wireVec);
	TopoDS_Shape ApplyFilletScrollShapes(TopoDS_Shape scrollShape, double radius);
	TopoDS_Shape ApplyFilletSolidPipe(TopoDS_Shape solidPipe, double radius);
	TopoDS_Shape ApplyFilletExitPipe(TopoDS_Shape exitPipe, double radius);
	TopoDS_Shape ApplyFilletSolidScroll(TopoDS_Shape exitPipe, double radius);
	std::vector<TopoDS_Shape> mkFilletToScrollShapes(std::vector<TopoDS_Shape> scrollShapeVec);
	std::vector<TopoDS_Shape> mkFilletToExitPipe(std::vector<TopoDS_Shape> exitPipeVec);
	TopoDS_Shape mkFilletToSolidPipe(std::vector<TopoDS_Shape> solidPipeVec);
	std::vector<TopoDS_Shape> mkFilletToSolidScroll(std::vector<TopoDS_Shape> solid);
	TopoDS_Face CreateSquareToFormFilletPipe(std::vector<TopoDS_Shape> exitPipe);
	void GetVerticesToCreateCurve(gp_Pnt pointOne, gp_Pnt pointTwo, gp_Pnt pointThree);
	TopoDS_Wire CreateBsplineWithThreePoints(gp_Pnt p1, gp_Pnt p2, gp_Pnt p3);
	TopoDS_Wire CreateTrnslWire(TopoDS_Wire curvedWire, TopoDS_Wire wire1);
	TopoDS_Shape CreatePipeShellFromTrnsWire(TopoDS_Wire curvedWire, TopoDS_Wire wire1, TopoDS_Wire wire2);
	TopoDS_Shape CreatePipeSoildFromTrnsWire(TopoDS_Wire curvedWire, TopoDS_Wire wire1, TopoDS_Wire wire2);
	TopoDS_Shape SewVoluteWithCurvedPipe(std::vector<TopoDS_Shape> sewScroll, std::vector<TopoDS_Shape> exitPipe, double tolerance);
	gp_Vec CreateVector(TopoDS_Edge edge);
	gp_Pnt getCommonPoint(std::vector<gp_Pnt> pointVec1, std::vector<gp_Pnt> pointVec2);
	std::vector<TopoDS_Edge> create2dFilletsForSquare(double radius);
	TopoDS_Wire Create2dWire(std::vector<TopoDS_Edge> edgeVec);
	TopoDS_Wire GetLargest2dFilletWire();
	TopoDS_Wire MakeSpineCurve();


	std::vector<TopoDS_Shape> CreateVoluteWithoutFillets();
	TopoDS_Shape CreateFilletedStraightPipe();
	TopoDS_Shape CreateStraightExitPipeWithCircularExit();
	TopoDS_Shape CreateStraightPipe();
	TopoDS_Shape CreateCurvedPipeWithCircularExit();
	TopoDS_Shape CreateCurvedPipeWithRectExit();
	TopoDS_Face CreateRectangularExit();
	TopoDS_Face CreateFilletedRectangularExit();
	TopoDS_Face CreateCircularExit();
	std::vector<TopoDS_Shape> TotalVolumeWithoutInOut(/*std::vector<TopoDS_Shape> &thruSectPipe*/);
	std::vector<TopoDS_Shape> CreateVoluteWith2dFillets();
	//std::vector<TopoDS_Shape> CreateVoluteWith2dFilletsWithBase();
	TopoDS_Shape CreateStraightExitPipeWith2dFilletedWire();
	std::vector<TopoDS_Wire> Create2dFilletedWireWithBase(std::vector<TopoDS_Wire> filleted2dWireVec);
	std::vector<TopoDS_Wire> CreateNewCrossSection(double L1, double L2, double L3, double L4, double L5, std::vector<double> area);

	std::vector<TopoDS_Shape> CreateNewCrossSection();
	TopoDS_Wire CreateTransitionStartSection(double L1, double L2, double L3, double area);
	void getVerticesFromEdges_New(TopoDS_Wire wire);
	TopoDS_Wire Apply2dFilletsForNewCrossSection(/*std::vector<TopoDS_Edge> newEdgeVec, */TopoDS_Wire wire, double radius);
	std::vector<TopoDS_Wire> createFilletedCrossSections_newShape(std::vector<TopoDS_Wire> wireVec);
	std::vector<TopoDS_Shape> mkFilletToNewScrollShapes(std::vector<TopoDS_Shape> scrollShapeVec);
	TopoDS_Shape ApplyFilletNewScrollShapes(TopoDS_Shape scrollShape, double radius);
	TopoDS_Shape CreateThruSect(TopoDS_Wire transitionStartWire, TopoDS_Solid smallScrollSolid, TopoDS_Shape smallScrollShell);
	std::vector<TopoDS_Face> RemoveTopFaceOfScroll(TopoDS_Shape scrollShape);
	TopoDS_Wire CreateRectangleForAirExit(gp_Pnt centrePnt, double area);

	double my_width;

	std::vector<TopoDS_Edge> my_newCrossSectionEdgeVec;
	std::vector<gp_Pnt> newShape_pointVec1;
	std::vector<gp_Pnt> newShape_pointVec2;
	std::vector<gp_Pnt> newShape_pointVec3;
	std::vector<gp_Pnt> newShape_pointVec4;
	std::vector<gp_Pnt> newShape_pointVec5;

	int my_numOfSections;

	TopoDS_Wire filletedWire;
	//TopoDS_Wire wireWith2dFillets;
	TopoDS_Wire my_rectWire2dFillets;
	TopoDS_Wire my_circleWire;
	//std::vector<double> angleVector;

	std::vector<gp_Pnt> pointVec1;
	std::vector<gp_Pnt> pointVec2;
	std::vector<gp_Pnt> pointVec3;
	std::vector<gp_Pnt> pointVec4;

	TopoDS_Shape my_exitPipeBooleanCut;
	TopoDS_Shape my_SmallShellBooleanCut;
	TopoDS_Shape my_ThruSectPipeSolidToBe;

	std::vector<TopoDS_Edge> edgesOfSquare;
	std::vector<TopoDS_Shape> solidVector2dFillet;
	std::vector<TopoDS_Face> my_inputPlaneFaceList;
	std::vector<TopoDS_Face> my_filletedExitPlaneVec;
	std::vector<TopoDS_Face> my_curvedCircularExitVec;
	std::vector<TopoDS_Face> my_curvedRectangularExitWithFillets;
	std::vector<TopoDS_Face> my_curvedRectangularExit;

	std::vector<TopoDS_Wire> rotated2dFilletWireWithoutBase;
	std::vector<TopoDS_Shape> scrollVectorWithoutBoolCut;
	std::vector<TopoDS_Wire> my_sectionWireList;
	std::vector<TopoDS_Wire> my_RotatedWireWithoutBaseWire;
	std::vector<TopoDS_Wire> my_filletedSqaureWireVec;
	std::vector<TopoDS_Wire> my_squareEdgeVec;
	std::vector<TopoDS_Wire> filleted2dWireVecWithBase;
	std::vector<TopoDS_Wire> BaseRotatedWire;
	std::vector<TopoDS_Wire> my_BaseWireList;

	std::vector<TopoDS_Shape> my_scrollVectorWithoutBoolCut;
	std::vector<TopoDS_Shape> my_shellVector;
	std::vector<TopoDS_Shape> my_solidVector;
	std::vector<TopoDS_Shape> my_exitPlanes;
	//std::vector<TopoDS_Shape> my_boolFuseVec;
	std::vector<TopoDS_Shape> my_inputPlaneList;
	std::vector<TopoDS_Shape> my_vectorExitPipeBooleanCut;
	std::vector<TopoDS_Shape> my_thruSectPipeVec;
	std::vector<TopoDS_Shape> my_filletedThruSectPipeVec;
	std::vector<TopoDS_Shape> my_filletedScrollShape;
	std::vector<TopoDS_Shape> my_shellVectorWithBoolCut;
	std::vector<TopoDS_Shape> my_filletedExitPipesVec;
	std::vector<TopoDS_Shape> my_filletSolidScrolls;
	std::vector<TopoDS_Shape> my_pipeVecWithBoolCut;
	std::vector<TopoDS_Shape> my_solidExitPipeVecBoolCut;
	std::vector<TopoDS_Shape> my_solidPipeVec;
	std::vector<TopoDS_Shape> my_solidCurvedPipe;
	std::vector<TopoDS_Shape> my_filleted2dScrollShellShapesCurvedPipe;
	std::vector<TopoDS_Shape> my_filleted2dScrollShapesStraightPipe;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODELINGDOC_H__30453389_3E75_11D7_8611_0060B0EE281E__INCLUDED_)
