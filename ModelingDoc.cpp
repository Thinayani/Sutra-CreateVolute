// ModelingDoc.cpp : implementation of the CMot'delingDoc class
//

#include <iostream>
#include <fstream>
#include <vector>
#include "stdafx.h"
#include "ModelingDoc.h"

#include "ModelingApp.h"
#include "ResultDialog.h"
#include "State.h"

#include "GenerateBox1.h"
#include "ISession_Direction.h"
#include "..\res\resource.h"

#include <Adaptor3d_HCurveOnSurface.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <AIS_ColoredShape.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <OCC_MainFrame.h>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <TopExp_Explorer.hxx>
#include <Geom_Plane.hxx>
#include <BRepTools.hxx>

#include <Geom_TrimmedCurve.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <GC_MakeSegment.hxx>
#include <Geom2d_Ellipse.hxx>

#include <STEPControl_Reader.hxx>
#include <XSControl_WorkSession.hxx>
#include <XSControl_TransferReader.hxx>
#include <Transfer_TransientProcess.hxx>
#include <AIS_InteractiveContext.hxx>
#include <GCPnts_UniformAbscissa.hxx>
#include <GeomAPI_Interpolate.hxx>
#include <ShapeFix_Wire.hxx>
#include <ShapeFix_Face.hxx>
#include <ShapeAnalysis_Shell.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <ShapeFix_Solid.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepOffsetAPI_MakePipeShell.hxx>
#include <ShapeFix_Shape.hxx>
#include <ChFi2d_FilletAPI.hxx>

using namespace std;

static Handle(AIS_Shape) AIS1;
static TopoDS_Face THE_F1, THE_F2;
static TopoDS_Edge THE_E1, THE_E2;

/////////////////////////////////////////////////////////////////////////////
// CModelingDoc

IMPLEMENT_DYNCREATE(CModelingDoc, CDocument)

BEGIN_MESSAGE_MAP(CModelingDoc, OCC_3dBaseDoc)
	//{{AFX_MSG_MAP(CModelingDoc)
	ON_COMMAND(ID_MIRROR, OnMirror)
	ON_COMMAND(ID_MIRRORAXIS, OnMirroraxis)
	ON_COMMAND(ID_ROTATE, OnRotate)
	ON_COMMAND(ID_SCALE, OnScale)
	ON_COMMAND(ID_TRANSLATION, OnTranslation)
	ON_COMMAND(ID_DISPLACEMENT, OnDisplacement)
	ON_COMMAND(ID_DEFORM, OnDeform)
	ON_COMMAND(ID_BOX, OnBox)
	ON_COMMAND(ID_Cylinder, OnCylinder)
	ON_COMMAND(ID_CONE, OnCone)
	ON_COMMAND(ID_SPHERE, OnSphere)
	ON_COMMAND(ID_TORUS, OnTorus)
	ON_COMMAND(ID_WEDGE, OnWedge)
	ON_COMMAND(ID_PRISM, OnPrism)
	ON_COMMAND(ID_REVOL, OnRevol)
	ON_COMMAND(ID_PIPE, OnPipe)
	ON_COMMAND(ID_THRU, OnThru)
	ON_COMMAND(ID_EVOLVED, OnEvolved)
	ON_COMMAND(ID_DRAFT, OnDraft)
	ON_COMMAND(ID_CUT, OnCut)
	ON_COMMAND(ID_FUSE, OnFuse)
	ON_COMMAND(ID_SECTION, OnSection)
	ON_COMMAND(ID_COMMON, OnCommon)
	ON_COMMAND(ID_PSECTION, OnPsection)
	ON_COMMAND(ID_BLEND, OnBlend)
	ON_COMMAND(ID_CHAMF, OnChamf)
	ON_COMMAND(ID_EVOLVEDBLEND, OnEvolvedblend)
	ON_COMMAND(ID_PRISM_LOCAL, OnPrismLocal)
	ON_COMMAND(ID_REVOL_LOCAL, OnRevolLocal)
	ON_COMMAND(ID_GLUE_LOCAL, OnGlueLocal)
 	ON_COMMAND(ID_DPRISM_LOCAL, OnDprismLocal)
 	ON_COMMAND(ID_Pipe_LOCAL, OnPipeLocal)
	ON_COMMAND(ID_LINEAR_LOCAL, OnLinearLocal)
	ON_COMMAND(ID_SPLIT_LOCAL, OnSplitLocal)
	ON_COMMAND(ID_THICK_LOCAL, OnThickLocal)
	ON_COMMAND(ID_OFFSET_LOCAL, OnOffsetLocal)
	ON_COMMAND(ID_VERTEX, OnVertex)
	ON_COMMAND(ID_EDGE, OnEdge)
	ON_COMMAND(ID_WIRE, OnWire)
	ON_COMMAND(ID_FACE, OnFace)
	ON_COMMAND(ID_SHELL, OnShell)
	ON_COMMAND(ID_COMPOUND, OnCompound)
	ON_COMMAND(ID_GEOMETRIE, OnGeometrie)
	ON_COMMAND(ID_SEWING, OnSewing)
	ON_COMMAND(ID_EXPLORER, OnExplorer)
	ON_COMMAND(ID_BUILDER, OnBuilder)
	ON_COMMAND(ID_VALID, OnValid)
	ON_COMMAND(ID_LINEAR, OnLinear)
	ON_COMMAND(ID_VOLUME, OnVolume)
	ON_COMMAND(ID_SURFACE, OnSurface)
	ON_COMMAND(ID_BUTTON_FILL, OnButtonFill)
	ON_COMMAND(ID_STOP_STOP, OnStopStop)
	ON_COMMAND(ID_FILLWITHTANG, OnFillwithtang)
	ON_COMMAND(ID_BOTTLE, OnBottle)
	ON_COMMAND(ID_BUTTON_STEP, OnStep)
	ON_COMMAND(ID_BUTTON_BOX, OnCreateBox)
	ON_COMMAND(ID_BUTTON_VOLUTE, OnCreateVolute)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelingDoc construction/destruction

CModelingDoc::CModelingDoc()
{
	myAISContext->SetDisplayMode(AIS_Shaded,Standard_False);
}

CModelingDoc::~CModelingDoc()
{
}
 
void CModelingDoc::OnMirror() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}
	TopoDS_Shape S = BRepPrimAPI_MakeWedge (60.,100.,80.,20.).Shape(); 
	Handle(AIS_Shape) ais1 = new AIS_Shape(S);
	myAISContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False);
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);
	myAISContext->Display(ais1,Standard_False);
	gp_Trsf theTransformation;
	gp_Pnt PntCenterOfTheTransformation(110,60,60);
	Handle(AIS_Point) aispnt = new AIS_Point(new Geom_CartesianPoint(PntCenterOfTheTransformation));
	myAISContext->Display(aispnt,Standard_False);
	theTransformation.SetMirror(PntCenterOfTheTransformation);
	BRepBuilderAPI_Transform myBRepTransformation(S,theTransformation);
	TopoDS_Shape S2 = myBRepTransformation.Shape();
	Handle(AIS_Shape) ais2 = new AIS_Shape(S2);
	myAISContext->SetColor(ais2,Quantity_NOC_BLUE1,Standard_False); 
	myAISContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais2,Standard_False);
	Fit();
    TCollection_AsciiString Message ("\
\n\
TopoDS_Shape S = BRepBuilderAPI_MakeWedge(60.,100.,80.,20.); \n\
gp_Trsf theTransformation; \n\
gp_Pnt PntCenterOfTheTransformation(110,60,60); \n\
theTransformation.SetMirror(PntCenterOfTheTransformation);\n\
BRepBuilderAPI_Transform myBRepTransformation(S,theTransformation);\n\
TopoDS_Shape TransformedShape = myBRepTransformation.Shape();	\n");
	PocessTextInDialog("Transform a Shape with Mirror and One point.", Message);
	
}

// Calculate areas in a linear decrement
std::vector<double> GenerateVolute::AreaCalculation(double startArea, double endArea)
{
	std::vector<double> areaVector;

	double area = startArea - endArea;
	double sections = area / my_numOfSections;

	for (double i = 1; i <= my_numOfSections + 1; i++) // changed
	{
		double areaValue = sections * i;
		double parts = startArea - areaValue;
		areaVector.push_back(parts);
	}

	return areaVector;
}

// Creating cross-sections with the linear decrement of areas with input faces
std::vector<TopoDS_Wire> GenerateVolute::CreateSection(double width, std::vector<double> area)
{
	gp_Pnt P1(150, 0, 0);
	gp_Pnt P2(150, 0, 50);

	std::vector<TopoDS_Wire> my_WithBaseWireList;
	TopoDS_Wire Wire1;
	std::vector<TopoDS_Wire> TotalWires_vec;

	for (int i = 0; i < area.size(); i++)
	{
		double height = area[i] / width; 
		gp_Pnt P3(150 + height, 0, 0);
		gp_Pnt P4(150 + height, 0, 50);
		TopoDS_Edge Edge1 = BRepBuilderAPI_MakeEdge(P1, P2);
		TopoDS_Edge Edge2 = BRepBuilderAPI_MakeEdge(P3, P4);
		TopoDS_Edge Edge3 = BRepBuilderAPI_MakeEdge(P1, P3);
		TopoDS_Edge Edge4 = BRepBuilderAPI_MakeEdge(P2, P4);
		Wire1 = BRepBuilderAPI_MakeWire(Edge1, Edge3, Edge2, Edge4);
		TotalWires_vec.push_back(Wire1);
		BRepTools::Write(Wire1, "wireWithBase1.brep");
	}

	// Creating wires for the obtained cross-sections
	TopoDS_Wire FullWire;
	BRepLib_MakeWire wireMaker;
	
	for (int i = 0; i < TotalWires_vec.size(); i++)
	{
		wireMaker.Add(TotalWires_vec[i]);
		my_WithBaseWireList.push_back(TotalWires_vec[i]);//globel variable why?
	}
	FullWire = wireMaker.Wire();	

	return my_WithBaseWireList;
}

std::vector<TopoDS_Wire> GenerateVolute::CreateNewCrossSection(double L1, double L2, double L3, double L4, double L5, std::vector<double> area)
{

	TopoDS_Wire NewCrossSectionWire;
	std::vector<TopoDS_Wire> TotalWires_vec;
	std::vector<TopoDS_Wire> NewCrossSectionWireList;

	/*std::vector<TopoDS_Edge> edge_vec;*/

	gp_Pnt P1(150, 0, 0);
	gp_Pnt P2(150 - L2, 0, 0);
	gp_Pnt P3(150 - L2, 0, 0 - L1);
	gp_Pnt P4(150, 0, L3);
	gp_Pnt P5(150 - L4, 0, L3);
	gp_Pnt P6(150 - L4, 0, L3 + L5);

	double width = L1 + L3 + L5;

	for (int i = 0; i < area.size(); i++)
	{
		double height = area[i] / width;

		gp_Pnt P7(150 + height, 0, 0 - L1);
		gp_Pnt P8(150 + height, 0, L3 + L5);

		TopoDS_Edge Edge1 = BRepBuilderAPI_MakeEdge(P1, P2);
		BRepTools::Write(Edge1, "Edge1.brep");
		TopoDS_Edge Edge2 = BRepBuilderAPI_MakeEdge(P2, P3);
		BRepTools::Write(Edge2, "Edge2.brep");
		TopoDS_Edge Edge3 = BRepBuilderAPI_MakeEdge(P3, P7);
		BRepTools::Write(Edge3, "Edge3.brep");
		TopoDS_Edge Edge4 = BRepBuilderAPI_MakeEdge(P7, P8);
		BRepTools::Write(Edge4, "Edge4.brep");
		TopoDS_Edge Edge5 = BRepBuilderAPI_MakeEdge(P8, P6);
		BRepTools::Write(Edge5, "Edge5.brep");
		TopoDS_Edge Edge6 = BRepBuilderAPI_MakeEdge(P6, P5);
		BRepTools::Write(Edge6, "Edge6.brep");
		TopoDS_Edge Edge7 = BRepBuilderAPI_MakeEdge(P5, P4);
		BRepTools::Write(Edge7, "Edge7.brep");
		TopoDS_Edge Edge8 = BRepBuilderAPI_MakeEdge(P4, P1);
		BRepTools::Write(Edge8, "Edge8.brep");

		my_newCrossSectionEdgeVec.clear();
		my_newCrossSectionEdgeVec.push_back(Edge1);
		my_newCrossSectionEdgeVec.push_back(Edge2);
		my_newCrossSectionEdgeVec.push_back(Edge3);
		my_newCrossSectionEdgeVec.push_back(Edge4);
		my_newCrossSectionEdgeVec.push_back(Edge5);
		my_newCrossSectionEdgeVec.push_back(Edge6);
		my_newCrossSectionEdgeVec.push_back(Edge7);
		my_newCrossSectionEdgeVec.push_back(Edge8);

		BRepBuilderAPI_MakeWire makeWholeWire;

		for (int i = 0; i < my_newCrossSectionEdgeVec.size(); i++)
		{
			makeWholeWire.Add(my_newCrossSectionEdgeVec[i]);

		}
		makeWholeWire.Build();

		TopoDS_Wire wholeWire = makeWholeWire.Wire();
		BRepTools::Write(wholeWire, "wholeWire.brep");
		NewCrossSectionWireList.push_back(wholeWire);
	}
	
	return NewCrossSectionWireList;
}

// Creating cross-sections with the linear decrement of areas without input faces
std::vector<TopoDS_Wire> GenerateVolute::CreateSectionWithoutBase(double width, std::vector<double> area)
{
	gp_Pnt P1(150, 0, 0);
	gp_Pnt P2(150, 0, 50);
	std::vector<TopoDS_Wire> my_WithoutBaseWireList;
	TopoDS_Wire Wire2;
	std::vector<TopoDS_Wire> WithoutBaseWire_vec;
	
	for (int i = 0; i < area.size(); i++)
	{
		double height = area[i] / width;
		gp_Pnt P3(150 + height, 0, 0);
		gp_Pnt P4(150 + height, 0, 50);
	
		TopoDS_Edge Edge2 = BRepBuilderAPI_MakeEdge(P3, P4);
		TopoDS_Edge Edge3 = BRepBuilderAPI_MakeEdge(P1, P3);
		TopoDS_Edge Edge4 = BRepBuilderAPI_MakeEdge(P2, P4);
		Wire2 = BRepBuilderAPI_MakeWire(Edge3, Edge2, Edge4);
		WithoutBaseWire_vec.push_back(Wire2);
	}

	TopoDS_Wire WithoutBaseWire;
	BRepLib_MakeWire wireMaker;

	for (int i = 0; i < WithoutBaseWire_vec.size(); i++)
	{
		wireMaker.Add(WithoutBaseWire_vec[i]);
		my_WithoutBaseWireList.push_back(WithoutBaseWire_vec[i]);
	}
	WithoutBaseWire = wireMaker.Wire();
	// newWire = add2d_Fillets(wire, redius);
	return my_WithoutBaseWireList;
}

// Creating cross-sections with the linear decrement of areas with only input faces
std::vector<TopoDS_Wire> GenerateVolute::CreateBaseSections(double width, std::vector<double> area)
{
	gp_Pnt P1(150, 0, 0);
	gp_Pnt P2(150, 0, 50);

	TopoDS_Wire Wire3;
	std::vector<TopoDS_Wire> BaseWire_vec;

	for (int i = 0; i < area.size(); i++)
	{
		TopoDS_Edge Edge1 = BRepBuilderAPI_MakeEdge(P1, P2);
		Wire3 = BRepBuilderAPI_MakeWire(Edge1);
		BaseWire_vec.push_back(Wire3);
		//BRepTools::Write(Wire3, "testWired.brep");
	}

	TopoDS_Wire BaseWire;
	BRepLib_MakeWire wireMaker;

	for (int i = 0; i < BaseWire_vec.size(); i++)
	{
		wireMaker.Add(BaseWire_vec[i]);
		my_BaseWireList.push_back(BaseWire_vec[i]);
	}
	BaseWire = wireMaker.Wire();

	return my_BaseWireList;
}

// Rotating a wire according to the given angle
TopoDS_Wire GenerateVolute::RotateWire(TopoDS_Wire wire, Standard_Real angle)
{
	gp_Trsf theTransformation;
	gp_Ax1 axe = gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(0., 0., 1.));
	TopoDS_Shape rotatedShape;
	
	theTransformation.SetRotation(axe, angle * M_PI / 180);
	BRepBuilderAPI_Transform myBRepTransformation(wire, theTransformation);
	rotatedShape = myBRepTransformation.Shape();

	TopoDS_Wire newWire = TopoDS::Wire(rotatedShape);
	BRepTools::Write(rotatedShape, "pre_rotShape.brep");

	return newWire;
}

// Creating an angle list according to the rotation of the cross-sections
std::vector<double> GenerateVolute::CreateAngleVector()
{
	Standard_Real angleInDegrees = 360 / my_numOfSections;
	Standard_Real linearAngles;
	std::vector<double> my_angleList;
	for (int i = 0; i <= my_numOfSections; i++)
	{
		linearAngles = angleInDegrees * i;
		my_angleList.push_back(linearAngles);
	}

	return my_angleList;
}

// Rotating the cross-sections
std::vector<TopoDS_Wire> GenerateVolute::RotateCrossSections(std::vector<TopoDS_Wire> wireVec, std::vector<double> angleVec)
{
	TopoDS_Wire my_rotShape;
	std::vector<TopoDS_Wire> rotatedWireList;

	for (int i = 0; i < wireVec.size(); i++)
	{
		my_rotShape = RotateWire(wireVec[i], angleVec[i]);
		BRepTools::Write(my_rotShape, "rotShape.brep");
		rotatedWireList.push_back(my_rotShape);
	}
	
	return rotatedWireList;
}

// Getting the compound shape with the rotated wires
TopoDS_Shape GenerateVolute::CreateCompoundShape(std::vector<TopoDS_Wire> list)
{
	TopoDS_Compound compoundShape;
	BRep_Builder builder_com;
	builder_com.MakeCompound(compoundShape);

	for (int i = 0; i < list.size(); i++)
	{
		builder_com.Add(compoundShape, list[i]);
	}

	return compoundShape;
}

// Creating the shell for the rotated wires with an angle gap
TopoDS_Shape GenerateVolute::CreateShellThruSect(std::vector<TopoDS_Wire> wireVector, Standard_Real startAngle, Standard_Real endAngle, std::vector<double> angleVec)
{
	BRepOffsetAPI_ThruSections BTS1(Standard_False, Standard_False, 1.0e-10);
	BTS1.SetMaxDegree(2);

	for (int i = 0; i <= my_numOfSections; i++)
	{
		if (angleVec[i] >= startAngle && angleVec[i - 1] < endAngle)
		{
			TopoDS_Wire aWire = BRepBuilderAPI_MakeWire(wireVector[i]);
			BTS1.AddWire(aWire);
		}
	}
	BTS1.Build();
	TopoDS_Shape shellShape = BTS1.Shape();

	return shellShape;
}

// Adding the created shells to a vector
std::vector<TopoDS_Shape> GenerateVolute::CreateShellList(std::vector<TopoDS_Wire> wireVec, std::vector<double> angleVec)
{
	my_shellVector.clear();
	// Calling the CreateShell function 4 times 
	TopoDS_Shape shell = CreateShellThruSect(wireVec, 0, 60, angleVec);
	BRepTools::Write(shell, "shell.brep");
	TopoDS_Shape shell2 = CreateShellThruSect(wireVec, 60, 210, angleVec);
	BRepTools::Write(shell2, "shell2.brep");
	TopoDS_Shape shell3 = CreateShellThruSect(wireVec, 210, 300, angleVec);
	BRepTools::Write(shell3, "shell3.brep");
	TopoDS_Shape shell4 = CreateShellThruSect(wireVec, 300, 360, angleVec);
	BRepTools::Write(shell4, "shell4.brep");

	my_shellVector.push_back(shell);
	my_shellVector.push_back(shell2);
	my_shellVector.push_back(shell3);
	my_shellVector.push_back(shell4);

	return my_shellVector;
}

// Sewing the created scroll shells to form one shape
TopoDS_Shape GenerateVolute::SewingScrollShells(std::vector<TopoDS_Shape> shellList, double tolerance)
{
	BRepBuilderAPI_Sewing sewer;
	sewer.SetTolerance(tolerance);

	for (int i = 0; i < shellList.size(); i++)
	{
		sewer.Add(shellList[i]);
	}
	sewer.Perform();

	//extrat the shell
	TopoDS_Shell sewedShell;
	for (TopExp_Explorer Expl(sewer.SewedShape(), TopAbs_SHELL); Expl.More(); Expl.Next())
	{
		sewedShell = TopoDS::Shell(Expl.Current());
	}

	//return sewedShell;
	ShapeAnalysis_Shell SAS;
	SAS.LoadShells(sewedShell);
	if (SAS.HasFreeEdges()) //can't make a closed volume
		return sewedShell;

	TopoDS_Shape sewedShape;
	BRepBuilderAPI_MakeSolid mkSolid(sewedShell);
	if (mkSolid.IsDone())
	{
		TopoDS_Solid sewedSolid = mkSolid.Solid();
		ShapeFix_Solid fixSolid(sewedSolid);
		fixSolid.Perform();
		sewedShape = fixSolid.Solid();
		//return sewedShape;
	}
	return sewedShape;
}

// Get the middle point of a square
gp_Pnt GenerateVolute::GetMiddlePoint()
{
	BRepBuilderAPI_MakeFace mkFace(my_sectionWireList[0], true);
	TopoDS_Face planeFace = mkFace.Face();
	GProp_GProps SProps;
	gp_Pnt centre_pnt(150, 0, 25);
	BRepGProp::SurfaceProperties(planeFace, SProps);
	centre_pnt = SProps.CentreOfMass();
	TopoDS_Vertex V = BRepBuilderAPI_MakeVertex(centre_pnt);
	BRepTools::Write(V, "centrePnt.brep");

	return centre_pnt;
}

// Create a square for the exit plane
TopoDS_Wire GenerateVolute::CreateSquareToGetExitPipe(gp_Pnt centrePnt, double area)
{
	//edgesOfSquare.clear();

	TopoDS_Wire Wire1;
	std::vector<TopoDS_Wire> wires;

	double height = sqrt(area);
	gp_Pnt p1(centrePnt.X() - height, 0, centrePnt.Z() - height);
	gp_Pnt p2(centrePnt.X() - height, 0, centrePnt.Z() + height);
	gp_Pnt p3(centrePnt.X() + height, 0, centrePnt.Z() - height);
	gp_Pnt p4(centrePnt.X() + height, 0, centrePnt.Z() + height);

	TopoDS_Edge Edge1 = BRepBuilderAPI_MakeEdge(p1, p2);
	TopoDS_Edge Edge2 = BRepBuilderAPI_MakeEdge(p1, p3);
	TopoDS_Edge Edge3 = BRepBuilderAPI_MakeEdge(p3, p4);
	TopoDS_Edge Edge4 = BRepBuilderAPI_MakeEdge(p2, p4);

	Wire1 = BRepBuilderAPI_MakeWire(Edge1, Edge2, Edge3, Edge4);
	BRepTools::Write(Wire1, "Wire1.brep");
	wires.push_back(Wire1);

	edgesOfSquare.push_back(Edge1);
	BRepTools::Write(Edge1, "Edge_1.brep");
	edgesOfSquare.push_back(Edge2);
	BRepTools::Write(Edge2, "Edge_2.brep");
	edgesOfSquare.push_back(Edge3);
	BRepTools::Write(Edge3, "Edge_3.brep");
	edgesOfSquare.push_back(Edge4);
	BRepTools::Write(Edge4, "Edge_4.brep");

	// Creating wires 
	TopoDS_Wire wire;
	BRepLib_MakeWire wireMaker;

	for (int i = 0; i < wires.size(); i++)
	{
		wireMaker.Add(wires[i]);
	}
	wire = wireMaker.Wire();
	BRepTools::Write(wire, "wire_test.brep");

	return wire;
}

// Getting the largest 2D Filleted wire 
TopoDS_Wire GenerateVolute::GetLargest2dFilletWire()
{
	std::vector<TopoDS_Wire> wires;
	TopoDS_Wire wire;
	BRepLib_MakeWire wireMaker;
	
	wires.push_back(rotated2dFilletWireWithoutBase[0]);
	BRepTools::Write(rotated2dFilletWireWithoutBase[0], "rotated2dFilletWireWithoutBase[0].brep");
	wires.push_back(BaseRotatedWire[0]);
	BRepTools::Write(BaseRotatedWire[0], "BaseRotatedWire[0].brep");

	for (int i = 0; i < wires.size(); i++)
	{
		wireMaker.Add(wires[i]);
	}
	wire = wireMaker.Wire();

	return wire;
}

// Getting the two vertices at the two ends of an edge
void GenerateVolute::getVerticesFromEdges()
{
	std::vector<TopoDS_Vertex> vertexVec1;
	std::vector<TopoDS_Vertex> vertexVec2;
	std::vector<TopoDS_Vertex> vertexVec3;
	std::vector<TopoDS_Vertex> vertexVec4;

	TopoDS_Vertex vertex;

	for (int i = 0; i < edgesOfSquare.size(); i++)
	{
		for (TopExp_Explorer explr(edgesOfSquare[i], TopAbs_VERTEX); explr.More(); explr.Next())
		{
			BRepTools::Write(edgesOfSquare[0], "sqEdge1.brep");
			BRepTools::Write(edgesOfSquare[1], "sqEdge2.brep");
			BRepTools::Write(edgesOfSquare[2], "sqEdge3.brep");
			BRepTools::Write(edgesOfSquare[3], "sqEdge4.brep");

			vertex = TopoDS::Vertex(explr.Current());
			gp_Pnt point = BRep_Tool::Pnt(vertex);

			if (i == 0)
			{
				vertexVec1.push_back(vertex);
				BRepTools::Write(vertex, "vertex1.brep");
				pointVec1.push_back(point);
			}
			if (i == 1)
			{
				vertexVec2.push_back(vertex);
				BRepTools::Write(vertex, "vertex2.brep");
				pointVec2.push_back(point);
			}
			if (i == 2)
			{
				vertexVec3.push_back(vertex);
				BRepTools::Write(vertex, "vertex3.brep");
				pointVec3.push_back(point);
			}
			if (i == 3)
			{
				vertexVec4.push_back(vertex);
				BRepTools::Write(vertex, "vertex4.brep");
				pointVec4.push_back(point);
			}
		}
	}
}

// Getting the common point of two vertices
gp_Pnt GenerateVolute::getCommonPoint(std::vector<gp_Pnt> pointVec1, std::vector<gp_Pnt> pointVec2)
{
	gp_Pnt commonPoint;
	for (int j = 0; j < pointVec1.size(); j++)
	{
		for (int k = 0; k < pointVec2.size(); k++)
		{
			if (pointVec1[j].Distance(pointVec2[k]) < 1.0e-6)
			{
				commonPoint = pointVec1[j];
				BRepTools::Write(BRepBuilderAPI_MakeVertex(commonPoint), "commonvertex.brep");
			}
		}
	}
	return commonPoint;
}

// Applying 2D Fillets for the rectangle
std::vector<TopoDS_Edge> GenerateVolute::create2dFilletsForSquare(double radius)
{
	gp_Pnt commonPoint1 = getCommonPoint(pointVec1, pointVec2);
	gp_Pnt commonPoint2 = getCommonPoint(pointVec2, pointVec3);
	gp_Pnt commonPoint3 = getCommonPoint(pointVec3, pointVec4);
	gp_Pnt commonPoint4 = getCommonPoint(pointVec4, pointVec1);

	TopoDS_Edge tempEdge0 = edgesOfSquare[0];
	TopoDS_Edge tempEdge1 = edgesOfSquare[1];

	gp_Ax2 axis1 = gp_Ax2(commonPoint1, gp_Dir(gp_Vec(0, 1, 0)));
	gp_Pln plane1(axis1);
	ChFi2d_FilletAPI mkFillet1(edgesOfSquare[0], edgesOfSquare[1], plane1);
	mkFillet1.Init(edgesOfSquare[0], edgesOfSquare[1], plane1);
	mkFillet1.Perform(radius);
	TopoDS_Edge curvedEdge1 = mkFillet1.Result(commonPoint1, edgesOfSquare[0], edgesOfSquare[1], -1);
	BRepTools::Write(curvedEdge1, "sqCurvedEdge1.brep");
	BRepTools::Write(edgesOfSquare[0], "edgesOfSquare[0].brep");
	BRepTools::Write(edgesOfSquare[1], "edgesOfSquare[1].brep");

	//// fillet 1
	ChFi2d_FilletAPI mkFillet2_tempEdge1(tempEdge1, tempEdge0, plane1);
	mkFillet2_tempEdge1.Perform(radius);
	mkFillet2_tempEdge1.Result(commonPoint1, tempEdge1, tempEdge0, -1);
	BRepTools::Write(tempEdge0, "tempEdge0.brep");
	BRepTools::Write(tempEdge1, "tempEdge1.brep");
	BRepTools::Write(edgesOfSquare[1], "edgesOfSquare[1].brep");

	edgesOfSquare[1] = tempEdge1;
	TopoDS_Edge tempEdge2 = edgesOfSquare[2];
	
	///////fillet 2
	gp_Ax2 axis2 = gp_Ax2(commonPoint2, gp_Dir(gp_Vec(0, 1, 0)));
	gp_Pln plane2(axis2);
	ChFi2d_FilletAPI mkFillet2(edgesOfSquare[1], edgesOfSquare[2], plane2);
	mkFillet2.Init(edgesOfSquare[1], edgesOfSquare[2], plane2);
	mkFillet2.Perform(radius);
	TopoDS_Edge curvedEdge2 = mkFillet2.Result(commonPoint2, edgesOfSquare[1], edgesOfSquare[2], -1);
	BRepTools::Write(curvedEdge2, "sqCurvedEdge2.brep");
	BRepTools::Write(edgesOfSquare[1], "edgesOfSquare[1].brep");
	BRepTools::Write(edgesOfSquare[2], "edgesOfSquare[2].brep");

	//fillet 3
	TopoDS_Edge tempEdge3 = edgesOfSquare[3];
	tempEdge2 = edgesOfSquare[2];
	BRepTools::Write(tempEdge2, "tempEdge2.brep");

	gp_Ax2 axis3 = gp_Ax2(commonPoint3, gp_Dir(gp_Vec(0, 1, 0)));
	gp_Pln plane3(axis3);
	ChFi2d_FilletAPI mkFillet3(edgesOfSquare[2], edgesOfSquare[3], plane3);
	mkFillet3.Init(edgesOfSquare[2], edgesOfSquare[3], plane3);
	mkFillet3.Perform(radius);
	TopoDS_Edge curvedEdge3 = mkFillet3.Result(commonPoint3, edgesOfSquare[2], edgesOfSquare[3], -1);
	BRepTools::Write(curvedEdge3, "sqCurvedEdge3.brep");
	BRepTools::Write(edgesOfSquare[3], "edgesOfSquare[3].brep");
	BRepTools::Write(edgesOfSquare[2], "edgesOfSquare[2].brep");

	ChFi2d_FilletAPI mkFillet2_tempEdge3(tempEdge3, tempEdge2, plane3);
	mkFillet2_tempEdge3.Perform(radius);
	mkFillet2_tempEdge3.Result(commonPoint3, tempEdge3, tempEdge2, -1);
	BRepTools::Write(tempEdge2, "tempEdge2.brep");
	BRepTools::Write(tempEdge3, "tempEdge3.brep");

	edgesOfSquare[3] = tempEdge3;

	// fillet 4
	gp_Ax2 axis4 = gp_Ax2(commonPoint4, gp_Dir(gp_Vec(0, 1, 0)));
	gp_Pln plane4(axis4);
	ChFi2d_FilletAPI mkFillet4(edgesOfSquare[3], edgesOfSquare[0], plane4);
	mkFillet4.Init(edgesOfSquare[3], edgesOfSquare[0], plane4);
	mkFillet4.Perform(radius);
	TopoDS_Edge curvedEdge4 = mkFillet4.Result(commonPoint4, edgesOfSquare[3], edgesOfSquare[0], -1);
	BRepTools::Write(edgesOfSquare[0], "edgesOfSquare[0].brep");
	BRepTools::Write(edgesOfSquare[3], "edgesOfSquare[3].brep");
	BRepTools::Write(curvedEdge4, "sqCurvedEdge4.brep");

	BRepTools::Write(edgesOfSquare[0], "edgeOfsq1.brep");
	BRepTools::Write(edgesOfSquare[1], "edgeOfsq2.brep");
	BRepTools::Write(edgesOfSquare[2], "edgeOfsq3.brep");
	BRepTools::Write(edgesOfSquare[3], "edgeOfsq4.brep");

	std::vector<TopoDS_Edge> edgeVec;
	edgeVec.push_back(edgesOfSquare[0]);
	edgeVec.push_back(curvedEdge1);
	edgeVec.push_back(edgesOfSquare[1]);
	edgeVec.push_back(curvedEdge2);
	edgeVec.push_back(edgesOfSquare[2]);
	edgeVec.push_back(curvedEdge3);
	edgeVec.push_back(edgesOfSquare[3]);
	edgeVec.push_back(curvedEdge4);

	return edgeVec;
}

// Create the 2D Filleted wire
TopoDS_Wire GenerateVolute::Create2dWire(std::vector<TopoDS_Edge> edgeVec)
{
	std::vector<TopoDS_Wire> wires;
	TopoDS_Wire wire;
	BRepLib_MakeWire wireMaker;

	for (int i = 0; i < edgeVec.size(); i++)
	{
		TopoDS_Wire Wire = BRepBuilderAPI_MakeWire(edgeVec[i]);
		wires.push_back(Wire);
	}

	for (int i = 0; i < wires.size(); i++)
	{
		wireMaker.Add(wires[i]);
	}
	wire = wireMaker.Wire();

	return wire;
}

// get vertices for new cross section
void GenerateVolute::getVerticesFromEdges_New(TopoDS_Wire wire)
{
	std::vector<TopoDS_Vertex> vertexVec1;
	std::vector<TopoDS_Vertex> vertexVec2;
	std::vector<TopoDS_Vertex> vertexVec3;
	std::vector<TopoDS_Vertex> vertexVec4;
	std::vector<TopoDS_Vertex> vertexVec5;
	std::vector<TopoDS_Edge> edgeVec;
	TopoDS_Vertex vertex;


	for (TopExp_Explorer explr(wire, TopAbs_EDGE); explr.More(); explr.Next())
	{
		TopoDS_Edge edgeFromWire = TopoDS::Edge(explr.Current());
		edgeVec.push_back(edgeFromWire);
		BRepTools::Write(edgeFromWire, "edgeFromWire.brep");

	}

	for (int i = 0; i < edgeVec.size(); i++)
	{
		for (TopExp_Explorer explr(edgeVec[i], TopAbs_VERTEX); explr.More(); explr.Next())
		{
			BRepTools::Write(edgeVec[1], "newEdge1.brep");
			BRepTools::Write(edgeVec[2], "newEdge2.brep");
			BRepTools::Write(edgeVec[3], "newEdge3.brep");
			BRepTools::Write(edgeVec[4], "newEdge4.brep");
			BRepTools::Write(edgeVec[5], "newEdge7.brep");

			vertex = TopoDS::Vertex(explr.Current());
			gp_Pnt point = BRep_Tool::Pnt(vertex);

			if (i == 1)
			{
				vertexVec1.push_back(vertex);
				BRepTools::Write(vertex, "vertex1.brep");
				newShape_pointVec1.push_back(point);
			}
			if (i == 2)
			{
				vertexVec2.push_back(vertex);
				BRepTools::Write(vertex, "vertex2.brep");
				newShape_pointVec2.push_back(point);
			}
			if (i == 3)
			{
				vertexVec3.push_back(vertex);
				BRepTools::Write(vertex, "vertex3.brep");
				newShape_pointVec3.push_back(point);
			}
			if (i == 4)
			{
				vertexVec4.push_back(vertex);
				BRepTools::Write(vertex, "vertex4.brep");
				newShape_pointVec4.push_back(point);
			}
			if (i == 5)
			{
				vertexVec5.push_back(vertex);
				BRepTools::Write(vertex, "vertex7.brep");
				newShape_pointVec5.push_back(point);
			}
		}
	}
}

// Applying 2D Fillets fir the new cross section
TopoDS_Wire GenerateVolute::Apply2dFilletsForNewCrossSection(/*std::vector<TopoDS_Edge> newEdgeVec*/ TopoDS_Wire wire, double radius)
{
	gp_Pnt commonPoint1 = getCommonPoint(newShape_pointVec1, newShape_pointVec2);
	gp_Pnt commonPoint2 = getCommonPoint(newShape_pointVec2, newShape_pointVec3);
	gp_Pnt commonPoint3 = getCommonPoint(newShape_pointVec3, newShape_pointVec4);
	gp_Pnt commonPoint4 = getCommonPoint(newShape_pointVec4, newShape_pointVec5);

	std::vector<TopoDS_Edge> newEdgeVec;

	for (TopExp_Explorer explr(wire, TopAbs_EDGE); explr.More(); explr.Next())
	{
		TopoDS_Edge edgeFromWire = TopoDS::Edge(explr.Current());
		newEdgeVec.push_back(edgeFromWire);
	}

	TopoDS_Edge tempEdge1 = newEdgeVec[1];
	TopoDS_Edge tempEdge2 = newEdgeVec[2];

	gp_Ax2 axis1 = gp_Ax2(commonPoint1, gp_Dir(gp_Vec(0, 1, 0)));
	gp_Pln plane1(axis1);
	ChFi2d_FilletAPI mkFillet1(newEdgeVec[1], newEdgeVec[2], plane1);
	mkFillet1.Init(newEdgeVec[1], newEdgeVec[2], plane1);
	mkFillet1.Perform(radius);
	TopoDS_Edge curvedEdge1 = mkFillet1.Result(commonPoint1, newEdgeVec[1], newEdgeVec[2], -1);
	BRepTools::Write(curvedEdge1, "CurvedEdge1.brep");
	BRepTools::Write(newEdgeVec[1], "newEdgeVec[1].brep");
	BRepTools::Write(newEdgeVec[2], "newEdgeVec[2].brep");

	//// fillet 1
	ChFi2d_FilletAPI mkFillet2_tempEdge1(tempEdge2, tempEdge1, plane1);
	mkFillet2_tempEdge1.Perform(radius);
	mkFillet2_tempEdge1.Result(commonPoint1, tempEdge2, tempEdge1, -1);
	BRepTools::Write(tempEdge1, "tempEdge0.brep");
	BRepTools::Write(tempEdge2, "tempEdge1.brep");
	BRepTools::Write(newEdgeVec[2], "newEdgeVec[2].brep");

	newEdgeVec[2] = tempEdge2;
	TopoDS_Edge tempEdge3 = newEdgeVec[3];

	///////fillet 2
	gp_Ax2 axis2 = gp_Ax2(commonPoint2, gp_Dir(gp_Vec(0, 1, 0)));
	gp_Pln plane2(axis2);
	ChFi2d_FilletAPI mkFillet2(newEdgeVec[2], newEdgeVec[3], plane2);
	mkFillet2.Init(newEdgeVec[2], newEdgeVec[3], plane2);
	mkFillet2.Perform(radius);
	TopoDS_Edge curvedEdge2 = mkFillet2.Result(commonPoint2, newEdgeVec[2], newEdgeVec[3], -1);
	BRepTools::Write(curvedEdge2, "CurvedEdge2.brep");
	BRepTools::Write(newEdgeVec[2], "newEdgeVec[2].brep");
	BRepTools::Write(newEdgeVec[3], "newEdgeVec[3].brep");

	//fillet 3
	TopoDS_Edge tempEdge4 = newEdgeVec[4];
	tempEdge3 = newEdgeVec[3];
	BRepTools::Write(tempEdge3, "tempEdge2.brep");

	gp_Ax2 axis3 = gp_Ax2(commonPoint3, gp_Dir(gp_Vec(0, 1, 0)));
	gp_Pln plane3(axis3);
	ChFi2d_FilletAPI mkFillet3(newEdgeVec[3], newEdgeVec[4], plane3);
	mkFillet3.Init(newEdgeVec[3], newEdgeVec[4], plane3);
	mkFillet3.Perform(radius);
	TopoDS_Edge curvedEdge3 = mkFillet3.Result(commonPoint3, newEdgeVec[3], newEdgeVec[4], -1);
	BRepTools::Write(curvedEdge3, "CurvedEdge3.brep");
	BRepTools::Write(newEdgeVec[4], "newEdgeVec[4].brep");
	BRepTools::Write(newEdgeVec[3], "newEdgeVec[3].brep");

	ChFi2d_FilletAPI mkFillet2_tempEdge3(tempEdge4, tempEdge3, plane3);
	mkFillet2_tempEdge3.Perform(radius);
	mkFillet2_tempEdge3.Result(commonPoint3, tempEdge4, tempEdge3, -1);
	BRepTools::Write(tempEdge3, "tempEdge3.brep");
	BRepTools::Write(tempEdge4, "tempEdge4.brep");

	newEdgeVec[4] = tempEdge4;

	// fillet 4
	gp_Ax2 axis4 = gp_Ax2(commonPoint4, gp_Dir(gp_Vec(0, 1, 0)));
	gp_Pln plane4(axis4);
	ChFi2d_FilletAPI mkFillet4(newEdgeVec[5], newEdgeVec[4], plane4);
	mkFillet4.Init(newEdgeVec[5], newEdgeVec[4], plane4);
	mkFillet4.Perform(radius);
	TopoDS_Edge curvedEdge4 = mkFillet4.Result(commonPoint4, newEdgeVec[5], newEdgeVec[4], -1);
	BRepTools::Write(newEdgeVec[4], "newEdgeVec[4].brep");
	BRepTools::Write(newEdgeVec[5], "newEdgeVec[5].brep");
	BRepTools::Write(curvedEdge4, "CurvedEdge4.brep");

	BRepTools::Write(newEdgeVec[1], "edgeOfnewShape1.brep");
	BRepTools::Write(newEdgeVec[2], "edgeOfnewShape2.brep");
	BRepTools::Write(newEdgeVec[3], "edgeOfnewShape3.brep");
	BRepTools::Write(newEdgeVec[4], "edgeOfnewShape4.brep");
	BRepTools::Write(newEdgeVec[5], "edgeOfnewShape5.brep");

	BRepBuilderAPI_MakeWire makeWholeWire;
	makeWholeWire.Add(newEdgeVec[1]);
	makeWholeWire.Add(curvedEdge1);
	makeWholeWire.Add(newEdgeVec[2]);
	makeWholeWire.Add(curvedEdge2);
	makeWholeWire.Add(newEdgeVec[3]);
	makeWholeWire.Add(curvedEdge3);
	makeWholeWire.Add(newEdgeVec[4]);
	makeWholeWire.Add(curvedEdge4);
	makeWholeWire.Add(newEdgeVec[5]);
	makeWholeWire.Add(newEdgeVec[6]);
	makeWholeWire.Add(newEdgeVec[7]);
	makeWholeWire.Add(newEdgeVec[0]);
	
	makeWholeWire.Build();

	TopoDS_Wire wholeWire = makeWholeWire.Wire();
	BRepTools::Write(wholeWire, "wholeWire.brep");

	return wholeWire;
	
}

// create wire vector applying 2D fillets for the new cross sections
std::vector<TopoDS_Wire> GenerateVolute::createFilletedCrossSections_newShape(std::vector<TopoDS_Wire> wireVec)
{
	TopoDS_Wire filleted2dwire;
	std::vector<TopoDS_Wire> wholeWireVec;
	
	for (int i = 0; i < wireVec.size(); i++)
	{
		getVerticesFromEdges_New(wireVec[i]);
		filleted2dwire = Apply2dFilletsForNewCrossSection(wireVec[i], 6);
		BRepTools::Write(filleted2dwire, "filleted2dwire.brep");
		wholeWireVec.push_back(filleted2dwire);
			
	}

	return wholeWireVec;
}

// Create circular exit wire
TopoDS_Wire GenerateVolute::CreateCircleToGetExitPipe(gp_Pnt centrePnt, double area)
{
	std::vector<TopoDS_Wire> wires;

	double radius = sqrt(area / M_PI);
	gp_Circ c1 = gp_Circ(gp_Ax2(centrePnt, gp_Dir(0., 1., 0.)), radius);

	TopoDS_Edge circle = BRepBuilderAPI_MakeEdge(c1);
	TopoDS_Wire Wire1 = BRepBuilderAPI_MakeWire(circle);
	wires.push_back(Wire1);

	// Creating wires 
	TopoDS_Wire wire;
	BRepLib_MakeWire wireMaker;

	for (int i = 0; i < wires.size(); i++)
	{
		wireMaker.Add(wires[i]);
	}
	wire = wireMaker.Wire();

	return wire;
}

// Creating square from 3D filleted exit pipe
TopoDS_Face GenerateVolute::CreateSquareToFormFilletPipe(std::vector<TopoDS_Shape> exitPipe)
{
	std::vector<TopoDS_Wire> wires;

	int counter = 0;

	for (int i = 0; i < exitPipe.size(); i++) {
		for (TopExp_Explorer ex(exitPipe[i], TopAbs_EDGE); ex.More(); ex.Next())
		{
			TopoDS_Edge anEdge = TopoDS::Edge(ex.Current());
			BRepTools::Write(anEdge, "squareFilletEdges.brep");

			counter++;
			if (counter == 11 | counter == 4 | counter == 8 | counter == 16 | counter == 24 | counter == 20)
			{
				TopoDS_Wire Wire = BRepBuilderAPI_MakeWire(anEdge);
				wires.push_back(Wire);
			}
		}
	}

	// Creating wire
	TopoDS_Wire Wire1;
	BRepLib_MakeWire wireMaker;

	for (int i = 0; i < wires.size(); i++)
	{
		wireMaker.Add(wires[i]);
		my_squareEdgeVec.push_back(wires[i]);
	}

	Wire1 = wireMaker.Wire();
	my_filletedSqaureWireVec.push_back(Wire1);
	BRepTools::Write(Wire1, "Wire1.brep");

	TopoDS_Face newface = BRepBuilderAPI_MakeFace(Wire1);
	//my_filletedExitPlaneVec.push_back(newface);
	BRepTools::Write(newface, "newface.brep");

	return newface;
}

// Get translation of the square
TopoDS_Wire GenerateVolute::GetTranslationOfWire(TopoDS_Shape shape, gp_Vec vec)
{
	gp_Trsf translation1;
	translation1.SetTranslation(vec);

	BRepBuilderAPI_Transform frontTrans(shape, translation1, true);

	TopoDS_Shape trnsShape = frontTrans.Shape();
	TopoDS_Wire wire = TopoDS::Wire(trnsShape);

	return wire;
}

// Get the 3D filleted wire for translated square
TopoDS_Wire GenerateVolute::GetFilletedWireForTrans(TopoDS_Shape shape)
{
	TopoDS_Wire Wire;
	std::vector<TopoDS_Edge> edgeVec;
	std::vector<TopoDS_Wire> TotalWires_vec;

	int counter = 0;

	for (TopExp_Explorer ex(shape, TopAbs_EDGE); ex.More(); ex.Next())
	{
		TopoDS_Edge anEdge = TopoDS::Edge(ex.Current());
	
		counter++;

		if (counter == 1 | counter == 6 | counter == 8 | counter == 10 | counter == 9 | counter == 7)
		{
			BRepTools::Write(anEdge, "anEdge.brep");
			Wire = BRepBuilderAPI_MakeWire(anEdge);
			TotalWires_vec.push_back(Wire);
		}

	}
	
	TopoDS_Wire FullWire;
	BRepLib_MakeWire wireMaker;

	for (int i = 0; i < TotalWires_vec.size(); i++)
	{
		wireMaker.Add(TotalWires_vec[i]);
	}
	FullWire = wireMaker.Wire();

	return FullWire;
}

// Get thruSection of square to create the exit pipe
TopoDS_Shape GenerateVolute::ThruSectExitPipe(TopoDS_Wire wire1, TopoDS_Wire wire2, Standard_Boolean isSolid)
{
	BRepOffsetAPI_ThruSections BTS1(isSolid, Standard_False, 1.0e-10);
	BTS1.SetMaxDegree(2);

	std::vector<TopoDS_Wire> wires;
	
	wires.push_back(wire1);
	BRepTools::Write(wire1, "wire1.brep");
	wires.push_back(wire2);
	BRepTools::Write(wire2, "wire2.brep");

	for (int i = 0; i < wires.size(); i++)
	{
		TopoDS_Wire aWire = BRepBuilderAPI_MakeWire(wires[i]);
		BTS1.AddWire(aWire);
	}

	BTS1.Build();
	TopoDS_Shape shape = BTS1.Shape();
	
	return shape;
}

// Get the sewed exit pipe
TopoDS_Shape GenerateVolute::SewedExitPipe(TopoDS_Shape shape1, TopoDS_Shape shape2, double tolerance)
{
	BRepBuilderAPI_Sewing sewer;
	sewer.SetTolerance(tolerance);
	
	sewer.Add(shape1);
	sewer.Add(shape2);
	
	sewer.Perform();

	//extrat the shell
	TopoDS_Shell sewedShell;
	for (TopExp_Explorer Expl(sewer.SewedShape(), TopAbs_SHELL); Expl.More(); Expl.Next())
	{
		sewedShell = TopoDS::Shell(Expl.Current());
	}

	//return sewedShell;
	ShapeAnalysis_Shell SAS;
	SAS.LoadShells(sewedShell);
	if (SAS.HasFreeEdges()) //can't make a closed volume
		return sewedShell;

	BRepBuilderAPI_MakeSolid mkSolid(sewedShell);
	if (mkSolid.IsDone())
	{
		TopoDS_Solid sewedSolid = mkSolid.Solid();
		ShapeFix_Solid fixSolid(sewedSolid);
		fixSolid.Perform();
		TopoDS_Shape sewedShape = fixSolid.Solid();
		return sewedShape;
	}

	return sewedShell;
}

// Creating the solid scrolls for the rotated wires with an angle gap
TopoDS_Shape GenerateVolute::CreateSolidThruSect(std::vector<TopoDS_Wire> wireVector, Standard_Real startAngle, Standard_Real endAngle, std::vector<double> angleVec)
{
	BRepOffsetAPI_ThruSections BTS1(Standard_True, Standard_False, 1.0e-10);
	BTS1.SetMaxDegree(2);

	for (int i = 0; i <= my_numOfSections; i++)
	{
		if (angleVec[i] >= startAngle && angleVec[i - 1] < endAngle)
		{
			TopoDS_Wire aWire = BRepBuilderAPI_MakeWire(wireVector[i]);
			BTS1.AddWire(aWire);
		}
	}
	BTS1.Build();
	TopoDS_Shape soildShape = BTS1.Shape();

	return soildShape;
}

// Creating the list of solid scrolls
std::vector<TopoDS_Shape> GenerateVolute::CreateSolidList(std::vector<TopoDS_Wire> wireVec, std::vector<double> angleVec)
{
	// Calling the CreateShell function 4 times 
	my_solidVector.clear();
	TopoDS_Shape solid = CreateSolidThruSect(wireVec, 0, 60, angleVec);
	BRepTools::Write(solid, "solid.brep");
	TopoDS_Shape solid2 = CreateSolidThruSect(wireVec, 60, 210, angleVec);
	BRepTools::Write(solid2, "solid2.brep");
	TopoDS_Shape solid3 = CreateSolidThruSect(wireVec, 210, 300, angleVec);
	BRepTools::Write(solid3, "solid3.brep");
	TopoDS_Shape solid4 = CreateSolidThruSect(wireVec, 300, 360, angleVec);
	BRepTools::Write(solid4, "solid4.brep");

	my_solidVector.push_back(solid);
	my_solidVector.push_back(solid2);
	my_solidVector.push_back(solid3);
	my_solidVector.push_back(solid4);

	return my_solidVector;
}

// boolean cut for a shell without fillets
TopoDS_Shape GenerateVolute::GetSimpleBooleanResult(TopoDS_Shape shape, TopoDS_Solid solid)
{
	TopoDS_Shape result;
	BRepAlgoAPI_Cut booleanCut(shape, solid);
	booleanCut.Build();
	if (booleanCut.IsDone())
	{
		result = booleanCut.Shape();
	}

	return result;
}

// boolean cut for a shell with fillets
TopoDS_Shape GenerateVolute::GetSimpleBooleanResultForFilletedShapes(TopoDS_Shape shape, TopoDS_Shape solid)
{
	TopoDS_Shape result;
	BRepAlgoAPI_Cut booleanCut(shape, solid);
	booleanCut.Build();
	if (booleanCut.IsDone())
	{
		result = booleanCut.Shape();
	}

	return result;
}

// boolean cut using two solids
TopoDS_Shape GenerateVolute::GetBooleanResultForSolids(TopoDS_Shape shape, TopoDS_Shape solid)
{
	TopoDS_Shape result;
	BRepAlgoAPI_Cut booleanCut(shape, solid);
	booleanCut.Build();
	if (booleanCut.IsDone())
	{
		result = booleanCut.Shape();
	}

	return result;
}

// boolean fuse using two two solids
TopoDS_Shape GenerateVolute::GetBooleanFuseForSolids(TopoDS_Shape shape, TopoDS_Shape solid)
{
	TopoDS_Shape result;
	std::vector<TopoDS_Shape> boolFuseVecPipe;
	BRepAlgoAPI_Fuse booleanFuse(shape, solid);
	booleanFuse.Build();
	if (booleanFuse.IsDone())
	{
		result = booleanFuse.Shape();
	}
	boolFuseVecPipe.push_back(result);

	return result;
}

// create the list of input plane list
std::vector<TopoDS_Shape> GenerateVolute::CreateInputPlanes(std::vector<TopoDS_Wire> wireVec, std::vector<double> angleVec)
{
	my_inputPlaneList.clear();
	// Calling the CreateShell function 4 times 
	TopoDS_Shape basePlane = CreateShellThruSect(wireVec, 0, 60, angleVec);
	BRepTools::Write(basePlane, "basePlane.brep");
	TopoDS_Shape basePlane1 = CreateShellThruSect(wireVec, 60, 210, angleVec);
	BRepTools::Write(basePlane1, "basePlane1.brep");
	TopoDS_Shape basePlane2 = CreateShellThruSect(wireVec, 210, 300, angleVec);
	BRepTools::Write(basePlane2, "basePlane2.brep");
	TopoDS_Shape basePlane3 = CreateShellThruSect(wireVec, 300, 360, angleVec);
	BRepTools::Write(basePlane3, "basePlane3.brep");

	my_inputPlaneList.push_back(basePlane);
	my_inputPlaneList.push_back(basePlane1);
	my_inputPlaneList.push_back(basePlane2);
	my_inputPlaneList.push_back(basePlane3);

	return my_inputPlaneList;
}

// Create input plane faces from input plane shapes
std::vector<TopoDS_Face> GenerateVolute::CreateInputPlaneFaces(std::vector<TopoDS_Shape> baseScroll)
{
	TopoDS_Face baseFace;

	for (int i = 0; i < baseScroll.size(); i++)
	{
		for (TopExp_Explorer explr(baseScroll[i], TopAbs_FACE); explr.More(); explr.Next())
		{
			baseFace = TopoDS::Face(explr.Current());
			my_inputPlaneFaceList.push_back(baseFace);
			BRepTools::Write(baseFace, "baseFace.brep");
		}
	}

	return my_inputPlaneFaceList;
}

// sewing scroll shapes, exit pipe and input planes
TopoDS_Shape GenerateVolute::SewBaseScrollWithExitPipe(std::vector<TopoDS_Shape> scrollShells, std::vector<TopoDS_Face> inputPlaneList, std::vector<TopoDS_Shape> exitPipe, std::vector<TopoDS_Face> exitPlane, double tolerance)
{
	BRepBuilderAPI_Sewing sewer;
	sewer.SetTolerance(tolerance);

	//sroll vector loop sewer without last one
	for (int i = 0; i < scrollShells.size(); i++)
	{
		sewer.Add(scrollShells[i]);
		BRepTools::Write(scrollShells[i], "scrollShells_test.brep");
	}
	
	//inputplanelist 
	for (int i = 0; i < inputPlaneList.size(); i++)
	{
		sewer.Add(inputPlaneList[i]);
		BRepTools::Write(inputPlaneList[i], "inputPlane_test.brep");
	}
	
	//boolean shapes 
	for (int i = 0; i < exitPipe.size(); i++)
	{
		sewer.Add(exitPipe[i]);
	}

	//exit plane list
	for (int i = 0; i < exitPlane.size(); i++)
	{
		sewer.Add(exitPlane[i]);
	}
	
	sewer.Perform();

	//extrat the shell
	TopoDS_Shape sewedShape = sewer.SewedShape();
	BRepTools::Write(sewedShape, "sewedShape.brep");

	//sewer voluteWithoutInputExitPlane
	BRepBuilderAPI_Sewing sewerVoluteWithoutInputExitPlane;
	sewerVoluteWithoutInputExitPlane.SetTolerance(tolerance);
	//sroll vector loop sewer without last one
	for (int i = 0; i < scrollShells.size(); i++)
	{
		sewerVoluteWithoutInputExitPlane.Add(scrollShells[i]);
	}

	//inputplanelist 
	for (int i = 0; i < inputPlaneList.size(); i++)
	{
		sewerVoluteWithoutInputExitPlane.Add(inputPlaneList[i]);
	}

	//boolean shapes 
	for (int i = 0; i < exitPipe.size(); i++)
	{
		sewerVoluteWithoutInputExitPlane.Add(exitPipe[i]);
	}

	sewerVoluteWithoutInputExitPlane.Perform();

	//extrat the shell
	TopoDS_Shape sewedShapeWithoutExitPlane = sewerVoluteWithoutInputExitPlane.SewedShape();
	BRepTools::Write(sewedShapeWithoutExitPlane, "sewedShapeWithoutExitPlane.brep");
	TopoDS_Shell sewedShellWithoutExitPlane;

	for (TopExp_Explorer Expl(sewerVoluteWithoutInputExitPlane.SewedShape(), TopAbs_SHELL); Expl.More(); Expl.Next())
	{
		sewedShellWithoutExitPlane = TopoDS::Shell(Expl.Current());

	}

	BRepTools::Write(sewedShellWithoutExitPlane, "sewedShellWithoutExitPlane.brep");
	
	return sewedShape;
}

// sewing all the 'faces' of the volute
TopoDS_Shape GenerateVolute::SewVoluteFaces(std::vector<TopoDS_Face> sewScroll, std::vector<TopoDS_Face> baseScroll, std::vector<TopoDS_Face> exitPipe, std::vector<TopoDS_Face> exitPlane, double tolerance)
{
	BRepBuilderAPI_Sewing sewer;
	sewer.SetTolerance(tolerance);

	//sroll vector loop sewer without last one
	for (int i = 0; i < sewScroll.size(); i++)
	{
		sewer.Add(sewScroll[i]);
	}

	//inputplanelist 
	for (int i = 0; i < baseScroll.size(); i++)
	{
		sewer.Add(baseScroll[i]);
	}

	//boolean shapes 
	for (int i = 0; i < exitPipe.size(); i++)
	{
		sewer.Add(exitPipe[i]);
	}

	//exit plane list
	for (int i = 0; i < exitPlane.size(); i++)
	{
		sewer.Add(exitPlane[i]);
	}

	sewer.Perform();
	
	//extrat the shell
	TopoDS_Shape sewedShape = sewer.SewedShape();

	//TopoDS_Shape shape1 = sewer.SewedShape();
	BRepTools::Write(sewedShape, "sewedShape_faces.brep");

	return sewedShape;
}

// sewing the solid scrolls and solid exit pipe
TopoDS_Shape GenerateVolute::SewSolidVolute(std::vector<TopoDS_Shape> sewScroll, std::vector<TopoDS_Shape> exitPipe, double tolerance)
{
	BRepBuilderAPI_Sewing sewer;
	sewer.SetTolerance(tolerance);

	//sroll vector loop sewer without last one
	for (int i = 0; i < sewScroll.size(); i++)
	{
		sewer.Add(sewScroll[i]);
	}

	//boolean shapes 
	for (int i = 0; i < exitPipe.size(); i++)
	{
		sewer.Add(exitPipe[i]);
	}

	sewer.Perform();

	//extrat the shell
	TopoDS_Shape sewedShape = sewer.SewedShape();

	//TopoDS_Shape shape1 = sewer.SewedShape();
	BRepTools::Write(sewedShape, "sewedShape_solid.brep");

	return sewedShape;
}

// iterating the shapes to get faces
std::vector<TopoDS_Face> GenerateVolute::GetFacesFromShapes(std::vector<TopoDS_Shape> shapeVec)
{
	TopoDS_Face face;
	std::vector<TopoDS_Face> faceList;
	faceList.clear();
	for (int i = 0; i < shapeVec.size(); i++)
	{
		for (TopExp_Explorer explr(shapeVec[i], TopAbs_FACE); explr.More(); explr.Next())
		{
			face = TopoDS::Face(explr.Current());
			BRepTools::Write(face, "iteratedFace.brep");
			faceList.push_back(face);
			
		}
	}

	return faceList;
}

// Creating a shell Vector with the boolean cut of small scroll
std::vector<TopoDS_Shape> GenerateVolute::CreateShellVectorAfterBooleanCut()
{
	std::vector<TopoDS_Shape> shellVectorWithBooleanCut;
	shellVectorWithBooleanCut.clear();

	// adding exitPipeBooleanCut to a separate vector
	my_vectorExitPipeBooleanCut.push_back(my_exitPipeBooleanCut);

	shellVectorWithBooleanCut.push_back(my_exitPipeBooleanCut);
	shellVectorWithBooleanCut.push_back(my_SmallShellBooleanCut);
	shellVectorWithBooleanCut.push_back(my_shellVector[2]);
	BRepTools::Write(my_shellVector[2], "testShell2.brep");
	shellVectorWithBooleanCut.push_back(my_shellVector[1]);
	BRepTools::Write(my_shellVector[1], "tetsShell3.brep");
	shellVectorWithBooleanCut.push_back(my_shellVector[0]);
	BRepTools::Write(my_shellVector[0], "testShell4.brep");

	return shellVectorWithBooleanCut;
}

// Creating a solid scroll vector with the boolean cut
std::vector<TopoDS_Shape> GenerateVolute::CreateSolidVectorWithBoolCut(TopoDS_Shape smallBoolCutShape)
{
	std::vector<TopoDS_Shape> solidVectorAfterBooleanCut;

	solidVectorAfterBooleanCut.push_back(smallBoolCutShape);
	BRepTools::Write(smallBoolCutShape, "smallBoolCutSolid.brep");
	solidVectorAfterBooleanCut.push_back(my_filletSolidScrolls[2]);
	BRepTools::Write(my_filletSolidScrolls[2], "solid_test3.brep");
	solidVectorAfterBooleanCut.push_back(my_filletSolidScrolls[1]);
	BRepTools::Write(my_filletSolidScrolls[1], "solid_test2.brep");
	solidVectorAfterBooleanCut.push_back(my_filletSolidScrolls[0]);
	BRepTools::Write(my_filletSolidScrolls[0], "solid_test1.brep");

	return solidVectorAfterBooleanCut;
}

// Creating a solid scroll vector without the boolean fuse
std::vector<TopoDS_Shape> GenerateVolute::CreateSolidScrollVector()
{
	std::vector<TopoDS_Shape> solidVectorAfterBooleanFuse;
	solidVectorAfterBooleanFuse.clear();
	solidVectorAfterBooleanFuse.push_back(my_filletSolidScrolls[2]);
	BRepTools::Write(my_filletSolidScrolls[2], "fuseSolid.brep");
	solidVectorAfterBooleanFuse.push_back(my_filletSolidScrolls[1]);
	BRepTools::Write(my_filletSolidScrolls[1], "fuseSolid1.brep");
	solidVectorAfterBooleanFuse.push_back(my_filletSolidScrolls[0]);
	BRepTools::Write(my_filletSolidScrolls[0], "fuseSolid2.brep");

	return solidVectorAfterBooleanFuse;
}

// Creating a shell vector with the boolean cut
std::vector<TopoDS_Shape> GenerateVolute::CreateShellVectorWithBoolCut(TopoDS_Shape smallBoolCutShape, std::vector<TopoDS_Shape> scrollShapeVec)
{
	std::vector<TopoDS_Shape> shellVec;
	shellVec.clear();
	shellVec.push_back(smallBoolCutShape);
	BRepTools::Write(smallBoolCutShape, "smallBoolCutShape.brep");
	shellVec.push_back(scrollShapeVec[2]);
	BRepTools::Write(scrollShapeVec[2], "scrollShape3.brep");
	shellVec.push_back(scrollShapeVec[1]);
	BRepTools::Write(scrollShapeVec[1], "scrollShape2.brep");
	shellVec.push_back(scrollShapeVec[0]);
	BRepTools::Write(scrollShapeVec[0], "scrollShape1.brep");

	return shellVec;
}

// Reversing the created solid shape
TopoDS_Shape GenerateVolute::ReverseShapeIfInsideOut(TopoDS_Solid& solid) 
{
	BRepClass3d_SolidClassifier classify;
	classify.Load(solid);
	BRepTools::Write(solid, "solid_test.brep");
	classify.PerformInfinitePoint(1.0e-6);
	TopAbs_State state = classify.State();
	if ((state == TopAbs_IN && true ) || (state == TopAbs_OUT && false)) solid.Reverse();

	BRepTools::Write(solid, "solid_test1.brep");
	return solid;

}

// converting the sewed volute shape into solid with exit pipe
TopoDS_Solid GenerateVolute::mkSolidOfSewedVoluteWithExitPipe(TopoDS_Shape voluteShape)
{
	TopoDS_Shell sewedShell;
	BRepBuilderAPI_MakeSolid mkSolid;
	for (TopExp_Explorer Expl(voluteShape, TopAbs_SHELL); Expl.More(); Expl.Next())
	{
		sewedShell = TopoDS::Shell(Expl.Current());
		BRepTools::Write(sewedShell, "sewedShell.brep");
		mkSolid.Add(sewedShell);
		//return sewedShell;
	}
	
	TopoDS_Solid solid12 = BRepBuilderAPI_MakeSolid(sewedShell);
	BRepTools::Write(solid12, "solid12.brep");
	TopoDS_Shape sewedShapeVolute;
	TopoDS_Solid sewedSolid;
	if (mkSolid.IsDone())
	{
		sewedSolid = mkSolid.Solid();
		BRepTools::Write(sewedSolid, "sewedSolid.brep");
		ShapeFix_Solid fixSolid(sewedSolid);
		fixSolid.Perform();
		
		sewedSolid = TopoDS::Solid(fixSolid.Solid());
		BRepTools::Write(sewedSolid, "sewedSolid.brep");
	}

	return sewedSolid;
}

// apply 2D fillets to the cross sections without base
TopoDS_Wire GenerateVolute::Apply2dFilletsForWireWithoutBase(TopoDS_Wire wire, double radius)
{
	std::vector<TopoDS_Edge> edgeVec;
	std::vector<gp_Pnt> pointVec1;
	std::vector<gp_Pnt> pointVec2;
	std::vector<gp_Pnt> pointVec3;
	std::vector<TopoDS_Vertex> vertexVec1;
	std::vector<TopoDS_Vertex> vertexVec2;
	std::vector<TopoDS_Vertex> vertexVec3;

	TopoDS_Vertex vertex;

	gp_Pnt commonPoint1;
	gp_Pnt commonPoint2;

	for (TopExp_Explorer explr(wire, TopAbs_EDGE); explr.More(); explr.Next())
	{
		// edge pairs would be elements of edgeVec - [0],[1] & [1],[2]
		TopoDS_Edge edgeFromWire = TopoDS::Edge(explr.Current());
		edgeVec.push_back(edgeFromWire);
		BRepTools::Write(edgeFromWire, "edgeFromWire.brep");

	}
	for ( int i = 0; i < edgeVec.size(); i++) 
	{
		for (TopExp_Explorer explr(edgeVec[i], TopAbs_VERTEX); explr.More(); explr.Next())
		{
			vertex = TopoDS::Vertex(explr.Current());
			gp_Pnt point = BRep_Tool::Pnt(vertex);

			if (i == 0)
			{
				vertexVec1.push_back(vertex);
				BRepTools::Write(vertex, "vertex1.brep");
				pointVec1.push_back(point);
			}
			if (i == 1)
			{
				vertexVec2.push_back(vertex);
				BRepTools::Write(vertex, "vertex2.brep");
				pointVec2.push_back(point);
			}
			if(i == 2)
			{
				vertexVec3.push_back(vertex);
				BRepTools::Write(vertex, "vertex3.brep");
				pointVec3.push_back(point);
			}
		}
	}
	// 1st
	for (int j = 0; j < pointVec1.size(); j++)
	{
		if(pointVec1[j].Distance(pointVec2[0]) < 1.0e-6)
		{
			commonPoint1 = pointVec1[j];
			BRepTools::Write(BRepBuilderAPI_MakeVertex(commonPoint1), "commonvertex3.brep");
			
		}
	}
	for (int j = 0; j < pointVec1.size(); j++)
	{
		if (pointVec1[j].Distance(pointVec2[1]) < 1.0e-6)
		{
			commonPoint1 = pointVec1[j];

		}
	}

	for (int k = 0; k < pointVec3.size(); k++)
	{
		if (pointVec3[k].Distance(pointVec2[0]) < 1.0e-6)
		{
			commonPoint2 = pointVec3[k];
		}
	}
	for (int k = 0; k < pointVec3.size(); k++)
	{
		if (pointVec3[k].Distance(pointVec2[1]) < 1.0e-6)
		{
			commonPoint2 = pointVec3[k];
		}
	}

	//std::vector<TopoDS_Wire> TotalWires_vec;
	gp_Ax2 majorAxis1 = gp_Ax2(commonPoint1, gp_Dir(gp_Vec(0, 1, 0)));
	gp_Pln plane1(majorAxis1);
	ChFi2d_FilletAPI mkFillet1(edgeVec[0], edgeVec[1], plane1);
	mkFillet1.Init(edgeVec[0], edgeVec[1], plane1);
	mkFillet1.Perform(radius);
	TopoDS_Edge curvedEdge1 = mkFillet1.Result(commonPoint1, edgeVec[0], edgeVec[1], -1);
	BRepTools::Write(curvedEdge1, "curvedEdge1.brep");
	BRepTools::Write(edgeVec[0], "edgeVec[0].brep");
	BRepTools::Write(edgeVec[1], "edgeVec[1].brep");

	TopoDS_Edge tempEdge1 = edgeVec[1];
	TopoDS_Edge tempEdge2 = edgeVec[2];

	gp_Ax2 majorAxis2 = gp_Ax2(commonPoint2, gp_Dir(gp_Vec(0, 1, 0)));
	gp_Pln plane(majorAxis2);
	ChFi2d_FilletAPI mkFillet2(edgeVec[1], edgeVec[2], plane);
	mkFillet2.Init(edgeVec[1], edgeVec[2], plane);
	mkFillet2.Perform(radius);
	TopoDS_Edge curvedEdge2 = mkFillet2.Result(commonPoint2, edgeVec[1], edgeVec[2], -1);
	BRepTools::Write(curvedEdge2, "curvedEdge2.brep");
	BRepTools::Write(edgeVec[1], "edgeVec[1].brep");
	BRepTools::Write(edgeVec[2], "edgeVec[2].brep");

	//just for edge2 generation, this part used only for generating rest of the edge2
	//here we trick the fillet algo
	ChFi2d_FilletAPI mkFillet2_tempEdge2(tempEdge2, tempEdge1, plane);
	mkFillet2_tempEdge2.Perform(radius);
	mkFillet2_tempEdge2.Result(commonPoint2, tempEdge2, tempEdge1, -1);
	BRepTools::Write(tempEdge1, "tempEdge1.brep");
	BRepTools::Write(tempEdge2, "tempEdge2.brep");
	
	tempEdge1 = edgeVec[1];
	edgeVec[2] = tempEdge2;

	BRepTools::Write(edgeVec[0], "edgeVec[0].brep");
	BRepTools::Write(curvedEdge1, "curvedEdge1.brep");
	BRepTools::Write(edgeVec[1], "edgeVec[1].brep");
	BRepTools::Write(curvedEdge2, "curvedEdge2.brep");
	BRepTools::Write(edgeVec[2], "edgeVec[2].brep");

	BRepBuilderAPI_MakeWire makeWholeWire;
	makeWholeWire.Add(edgeVec[0]);
	makeWholeWire.Add(curvedEdge1);
	makeWholeWire.Add(edgeVec[1]);
	makeWholeWire.Add(curvedEdge2);
	makeWholeWire.Add(edgeVec[2]);
	makeWholeWire.Build();

	TopoDS_Wire wholeWire = makeWholeWire.Wire();
	BRepTools::Write(wholeWire, "wholeWire.brep");

	return wholeWire;
}

// create wire vector applying 2D fillets for the cross sections without base
std::vector<TopoDS_Wire> GenerateVolute::createFilletedCrossSections(std::vector<TopoDS_Wire> wireVec)
{
	TopoDS_Wire filleted2dwire;
	std::vector<TopoDS_Wire> wholeWireVec;

	for (int i = 0; i < wireVec.size(); i++)
	{
		filleted2dwire = Apply2dFilletsForWireWithoutBase(wireVec[i], 6);
		BRepTools::Write(filleted2dwire, "filleted2dwire.brep");
		wholeWireVec.push_back(filleted2dwire);
	}

	return wholeWireVec;
}

// creating 2D filleted wire with base
std::vector<TopoDS_Wire> GenerateVolute::Create2dFilletedWireWithBase(std::vector<TopoDS_Wire> filleted2dWireVec)
{
	TopoDS_Wire wireWithBase;
	std::vector<TopoDS_Wire> withBaseWireVec;


	for (int i = 0; i < filleted2dWireVec.size(); i++)
	{
		BRepLib_MakeWire wireMaker;

		wireMaker.Add(filleted2dWireVec[i]);
		BRepTools::Write(filleted2dWireVec[i], "filleted2dWire.brep");
		wireMaker.Add(my_BaseWireList[i]);
		BRepTools::Write(my_BaseWireList[i], "my_BaseWire.brep");

		wireWithBase = wireMaker.Wire();
		BRepTools::Write(wireWithBase, "wireWithBase.brep");

		withBaseWireVec.push_back(wireWithBase);
	}

	return withBaseWireVec;
}

// Applying fillets to one hollow scroll shape
TopoDS_Shape GenerateVolute::ApplyFilletScrollShapes(TopoDS_Shape scrollShape, double radius)
{
	TopoDS_Shape filletShape;

	BRepFilletAPI_MakeFillet mkFillet(scrollShape);
	int counter1 = 0;

	for (TopExp_Explorer ex(scrollShape, TopAbs_EDGE); ex.More(); ex.Next())
	{
		TopoDS_Edge anEdge = TopoDS::Edge(ex.Current());
		BRepTools::Write(anEdge, "topEdgeForFillet.brep");

		counter1++;

		if (counter1 == 2 | counter1 == 6)
		{
			mkFillet.Add(radius, anEdge);
		}
	}
	filletShape = mkFillet.Shape();

	return filletShape;
}

// Applying fillets to hollow scroll shape vector
std::vector<TopoDS_Shape> GenerateVolute::mkFilletToScrollShapes(std::vector<TopoDS_Shape> scrollShapeVec)
{
	TopoDS_Shape filletedShape;
	std::vector<TopoDS_Shape> scrollShapes;
	for (int i = 0; i < scrollShapeVec.size(); i++)
	{
		filletedShape = ApplyFilletNewScrollShapes(scrollShapeVec[i], 6);
		BRepTools::Write(filletedShape, "filletedScrollShape.brep");
		scrollShapes.push_back(filletedShape);
	}

	return scrollShapes;
}

// Apply fillets to one scroll - new shape
TopoDS_Shape GenerateVolute::ApplyFilletNewScrollShapes(TopoDS_Shape scrollShape, double radius)
{
	TopoDS_Shape filletShape;

	BRepFilletAPI_MakeFillet mkFillet(scrollShape);
	int counter1 = 0;

	for (TopExp_Explorer ex(scrollShape, TopAbs_EDGE); ex.More(); ex.Next())
	{
		TopoDS_Edge anEdge = TopoDS::Edge(ex.Current());
		BRepTools::Write(anEdge, "topEdgeForFillet.brep");

		counter1++;

		if (counter1 == 1)
		{
			TopoDS_Edge edge = TopoDS::Edge(ex.Current());
			BRepTools::Write(edge, "topEdge.brep");
			mkFillet.Add(radius, anEdge);
		}
	}
	filletShape = mkFillet.Shape();

	return filletShape;
}

// Applying fillets to one solid pipe shape
TopoDS_Shape GenerateVolute::ApplyFilletSolidPipe(TopoDS_Shape solidPipe, double radius)
{
	TopoDS_Shape filletedSolid;

	BRepFilletAPI_MakeFillet mkFillet(solidPipe);
	int counter1 = 0;

	for (TopExp_Explorer ex(solidPipe, TopAbs_EDGE); ex.More(); ex.Next())
	{
		TopoDS_Edge anEdge = TopoDS::Edge(ex.Current());
		BRepTools::Write(anEdge, "topEdgeForFillet.brep");

		counter1++;

		if (counter1 == 10 | counter1 == 12)
		{
			mkFillet.Add(radius, anEdge);
		}
	}
	filletedSolid = mkFillet.Shape();

	return filletedSolid;
}

// Applying fillets to solid pipe vector
TopoDS_Shape GenerateVolute::mkFilletToSolidPipe(std::vector<TopoDS_Shape> solidPipeVec)
{
	TopoDS_Shape filletedShape;
	std::vector<TopoDS_Shape> solidPipe;

	for (int i = 0; i < solidPipeVec.size(); i++)
	{
		filletedShape = ApplyFilletSolidPipe(solidPipeVec[i], 10);
		BRepTools::Write(filletedShape, "filletedSolidScroll.brep");
		solidPipe.push_back(filletedShape);
	}

	return filletedShape;
}

// Applying fillets to one hollow exit shape
TopoDS_Shape GenerateVolute::ApplyFilletExitPipe(TopoDS_Shape exitPipe, double radius)
{
	TopoDS_Shape filletShape;

	BRepFilletAPI_MakeFillet mkFillet(exitPipe);
	int counter1 = 0;

	for (TopExp_Explorer ex(exitPipe, TopAbs_EDGE); ex.More(); ex.Next())
	{
		TopoDS_Edge anEdge = TopoDS::Edge(ex.Current());
		BRepTools::Write(anEdge, "topEdgeForExitPipeFillet.brep");

		counter1++;

		if (counter1 == 10 | counter1 == 12)
		{
			mkFillet.Add(radius, anEdge);

		}
	}
	filletShape = mkFillet.Shape();

	return filletShape;
}

// Applying fillets to hollow exit pipe vector
std::vector<TopoDS_Shape> GenerateVolute::mkFilletToExitPipe(std::vector<TopoDS_Shape> exitPipeVec)
{
	TopoDS_Shape filletedExitPipe;
	std::vector<TopoDS_Shape> pipeVec;

	for (int i = 0; i < exitPipeVec.size(); i++)
	{
		filletedExitPipe = ApplyFilletExitPipe(exitPipeVec[i], 10);
		BRepTools::Write(filletedExitPipe, "filletedExitPipe.brep");
		pipeVec.push_back(filletedExitPipe);
	}

	return pipeVec;
}

// Applying fillets to one solid scroll shape
TopoDS_Shape GenerateVolute::ApplyFilletSolidScroll(TopoDS_Shape scrollShape, double radius)
{
	TopoDS_Shape filletShape;

	BRepFilletAPI_MakeFillet mkFillet(scrollShape);
	int counter1 = 0;

	for (TopExp_Explorer ex(scrollShape, TopAbs_EDGE); ex.More(); ex.Next())
	{
		TopoDS_Edge anEdge = TopoDS::Edge(ex.Current());
		BRepTools::Write(anEdge, "topEdgeForScrollFillet.brep");

		counter1++;

		if (counter1 == 6 | counter1 == 10)
		{
			mkFillet.Add(radius, anEdge);
		}

	}
	filletShape = mkFillet.Shape();

	return filletShape;
}

// Applying fillets to solid scroll shape vector
std::vector<TopoDS_Shape> GenerateVolute::mkFilletToSolidScroll(std::vector<TopoDS_Shape> solidScrollVec)
{
	TopoDS_Shape filletedSolidScroll;
	std::vector<TopoDS_Shape> scrollVec;
	for (int i = 0; i < solidScrollVec.size(); i++)
	{
		filletedSolidScroll = ApplyFilletSolidScroll(solidScrollVec[i], 10);
		BRepTools::Write(filletedSolidScroll, "filletedSolidScroll.brep");

		scrollVec.push_back(filletedSolidScroll);
	}

	return scrollVec;
}

// make spine curve to create curved pipe
TopoDS_Wire GenerateVolute::MakeSpineCurve()
{
	gp_Pnt midPoint = GetMiddlePoint();
	my_circleWire = CreateCircleToGetExitPipe(midPoint, 10000);
	BRepTools::Write(my_circleWire, "my_circleWire.brep");

	gp_Pnt pointTwo(midPoint.X() + 20, -70, midPoint.Z());
	gp_Pnt pointThree(midPoint.X() + 100, -160, midPoint.Z());
	GetVerticesToCreateCurve(midPoint, pointTwo, pointThree);

	TopoDS_Wire curvedWire = CreateBsplineWithThreePoints(midPoint, pointTwo, pointThree);
	BRepTools::Write(curvedWire, "curvedWire.brep");

	return curvedWire;
}

// Create vertices to get the spine curve
void GenerateVolute::GetVerticesToCreateCurve(gp_Pnt pointOne, gp_Pnt pointTwo, gp_Pnt pointThree)
{
	TopoDS_Vertex V1 = BRepBuilderAPI_MakeVertex(pointOne);
	BRepTools::Write(V1, "pointOne.brep");

	TopoDS_Vertex V2 = BRepBuilderAPI_MakeVertex(pointTwo);
	BRepTools::Write(V2, "pointTwo.brep");

	TopoDS_Vertex V3 = BRepBuilderAPI_MakeVertex(pointThree);
	BRepTools::Write(V3, "pointThree.brep");

	TopoDS_Edge Edge1 = BRepBuilderAPI_MakeEdge(V1, V2);
	BRepTools::Write(Edge1, "EdgeOne.brep");
	TopoDS_Edge Edge2 = BRepBuilderAPI_MakeEdge(V2, V3);
	BRepTools::Write(Edge2, "EdgeTwo.brep");
}

// Create curve using three vertices
TopoDS_Wire GenerateVolute::CreateBsplineWithThreePoints(gp_Pnt p1, gp_Pnt p2, gp_Pnt p3)
{
	Handle_TColgp_HArray1OfPnt PointArray = new TColgp_HArray1OfPnt(1, 3);
	PointArray->SetValue(1, p1);
	PointArray->SetValue(2, p2);
	PointArray->SetValue(3, p3);
	GeomAPI_Interpolate left_bottom_Curve_interpolate(PointArray, false, 1.0e-06);
	left_bottom_Curve_interpolate.Perform();
	Handle_Geom_BSplineCurve left_Bottom_Curve;
	left_Bottom_Curve = left_bottom_Curve_interpolate.Curve();

	TopoDS_Edge curvedEdge = BRepBuilderAPI_MakeEdge(left_Bottom_Curve);
	TopoDS_Wire curvedWire = BRepBuilderAPI_MakeWire(curvedEdge);

	return curvedWire;
}

gp_Vec GenerateVolute::CreateVector(TopoDS_Edge edge)
{
	double U1, U2;
	gp_Pnt lastPnt, firstPnt;
	Handle_Geom_Curve bsCurve = BRep_Tool::Curve(edge,U1,U2);
	lastPnt = bsCurve->Value(U1);
	firstPnt = bsCurve->Value(U2);

	gp_Vec vec2(firstPnt, lastPnt);
	TopoDS_Vertex v1 = BRepBuilderAPI_MakeVertex(lastPnt);
	BRepTools::Write(v1, "vertex1.brep");

	TopoDS_Vertex v2 = BRepBuilderAPI_MakeVertex(firstPnt);
	BRepTools::Write(v2, "vertex2.brep");
	//gp_Dir(-vec2);

	return -vec2;
}

// create translated wire for the curved Pipe with the spine curve
TopoDS_Wire GenerateVolute::CreateTrnslWire(TopoDS_Wire curvedWire, TopoDS_Wire wire1)
{
	BRepOffsetAPI_MakePipeShell exitPipeShell(curvedWire);
	BRepTools::Write(curvedWire, "curvedWire.brep");
	exitPipeShell.Add(wire1);
	BRepTools::Write(wire1, "wire1.brep");
	exitPipeShell.Build();
	TopoDS_Shape curvedExitPipe = exitPipeShell.Shape();
	TopoDS_Shape lastWire = exitPipeShell.LastShape();//wire translation

	TopoDS_Wire wire = TopoDS::Wire(lastWire);
	BRepTools::Write(wire, "lastWire.brep");
	
	BRepTools::Write(curvedExitPipe, "curvedExitPipe.brep");
	// pipeshell 

	return wire;
}

// Create pipe shell from translated wire
TopoDS_Shape GenerateVolute::CreatePipeShellFromTrnsWire(TopoDS_Wire curvedWire, TopoDS_Wire wire1, TopoDS_Wire wire2)
{
	BRepOffsetAPI_MakePipeShell exitPipeShell(curvedWire);

	exitPipeShell.Add(wire1);
	exitPipeShell.Add(wire2);
	exitPipeShell.Build();
	
	TopoDS_Shape curvedExitPipe = exitPipeShell.Shape();
	TopoDS_Shape pipe = exitPipeShell.Shape();

	return curvedExitPipe;
}

// Create pipe solid from translated wire
TopoDS_Shape GenerateVolute::CreatePipeSoildFromTrnsWire(TopoDS_Wire curvedWire, TopoDS_Wire wire1, TopoDS_Wire wire2)
{
	BRepOffsetAPI_MakePipeShell exitPipeShell(curvedWire);

	exitPipeShell.Add(wire1);
	exitPipeShell.Add(wire2);
	exitPipeShell.Build();
	exitPipeShell.MakeSolid();
	TopoDS_Shape curvedExitPipe = exitPipeShell.Shape();

	//BRepTools::Write(curvedExitPipe, "curvedExitPipe.brep");

	return curvedExitPipe;
}

// sewing the solid shapes
TopoDS_Shape GenerateVolute::SewVoluteWithCurvedPipe(std::vector<TopoDS_Shape> sewScroll, std::vector<TopoDS_Shape> exitPipe, double tolerance)
{
	BRepBuilderAPI_Sewing sewer;
	sewer.SetTolerance(tolerance);

	//sroll vector loop sewer without last one
	for (int i = 0; i < sewScroll.size(); i++)
	{
		sewer.Add(sewScroll[i]);
	}

	//boolean shapes 
	for (int i = 0; i < exitPipe.size(); i++)
	{
		sewer.Add(exitPipe[i]);
	}
	sewer.Perform();

	//extrat the shell
	TopoDS_Shape sewedShape = sewer.SewedShape();

	return sewedShape;
}

// ButtonClick to start volute program
void CModelingDoc::OnCreateVolute()
{
	GenerateVolute volute;
	
	volute.CreateVoluteWithoutFillets();
	volute.TotalVolumeWithoutInOut();
	volute.CreateRectangularExit();

	volute.CreateNewCrossSection();

	bool /*isFilleted,*/ isCircularExit, isCurvedExitPipe;

	//isFilleted = true;
	isCircularExit = false;
	isCurvedExitPipe = false;

	std::vector<TopoDS_Shape> filletedScrollShape_Vec;
	//std::vector<TopoDS_Shape> nonFilletedScrollShape;

	std::vector<TopoDS_Face> circularExitvec;
	std::vector<TopoDS_Face> filletedRectangularExitVec;
	//std::vector<TopoDS_Face> nonFilletedRectangleVec;

	std::vector<TopoDS_Shape> curvedPipeVecCirExit;
	std::vector<TopoDS_Shape> curvedPipeVecRectExit;
	std::vector<TopoDS_Shape> straightPipeVecCirExit;
	std::vector<TopoDS_Shape> straightPipeVecRectExit;
	//std::vector<TopoDS_Shape> nonFilletedStraightPipeVec;

	TopoDS_Shape filletedStraightPipe;
	//TopoDS_Shape nonFilletedStraightPipe;
	
	//if (isFilleted) 
	//
		// filleted scroll pipe - vector::shape
	filletedScrollShape_Vec = volute.CreateVoluteWith2dFillets();// rectangular volute
	//volute.CreateVoluteWith2dFilletsWithBase();
	volute.getVerticesFromEdges();

	//}
	//else 
	//{
	//	// non-filleted scroll pipe - vector::shape
	//	nonFilletedScrollShape = volute.CreateVolute();
	//
	//}

	if (isCircularExit) 
	{
		// circular exit plane
		TopoDS_Face circularExit = volute.CreateCircularExit();
		BRepTools::Write(circularExit, "circularExit.brep");
		circularExitvec.push_back(circularExit);
	}
	else 
	{
		
		// filleted rectangular exit plane
		TopoDS_Face filletedRectangle = volute.CreateFilletedRectangularExit();
		BRepTools::Write(filletedRectangle, "filletedRectangle.brep");
		filletedRectangularExitVec.push_back(filletedRectangle);

		//else
		//{
		//	// non-filleted rectangular exit plane
		//	TopoDS_Face nonFilletedRectangle = volute.CreateRectangularExit();
		//	BRepTools::Write(nonFilletedRectangle, "nonFilletedRectangle.brep");
		//	nonFilletedRectangleVec.push_back(nonFilletedRectangle);
		//}
		
	}

	if (isCurvedExitPipe)
	{
		
		if (isCircularExit)
		{
			// filleted curved pipe - volute.my_curvedCircularExit;
			TopoDS_Shape curvedPipeWithCircularExit = volute.CreateCurvedPipeWithCircularExit();
			BRepTools::Write(curvedPipeWithCircularExit, "curvedPipeWithCircularExit.brep");
			curvedPipeVecCirExit.push_back(curvedPipeWithCircularExit);
		
		}

		else
		{
			TopoDS_Shape curvedPipeWithRectangularExit = volute.CreateCurvedPipeWithRectExit();
			BRepTools::Write(curvedPipeWithRectangularExit, "curvedPipeWithRectangularExit.brep");
			curvedPipeVecRectExit.push_back(curvedPipeWithRectangularExit);
			
		}
	
	}
	else 
	{
		if (isCircularExit)
		{
			TopoDS_Shape straightPipeWithCircularExit = volute.CreateStraightExitPipeWithCircularExit();
			BRepTools::Write(straightPipeWithCircularExit, "straightPipeWithCircularExit.brep");
			straightPipeVecCirExit.push_back(straightPipeWithCircularExit);
		}
		else
		{
			// filleted straight pipe
			TopoDS_Shape filletedStraightPipe = volute.CreateStraightExitPipeWith2dFilletedWire();
			BRepTools::Write(filletedStraightPipe, "filletedStraightPipe.brep");
			straightPipeVecRectExit.push_back(filletedStraightPipe);

		}

		//else
		//{
		//	//non-filleted straight pipe
		//	TopoDS_Shape nonFilletedStraightPipe = volute.CreateStraightPipe();
		//	BRepTools::Write(nonFilletedStraightPipe, "nonFilletedStraightPipe.brep");
		//	nonFilletedStraightPipeVec.push_back(nonFilletedStraightPipe);
		//}
	}

	// sew functions
	//my_filleted2dScrollShapesStraightPipe or my_filleted2dScrollShellShapesCurvedPipe
	//my_curvedCircularExitVec or my_curvedRectangularExitWithFillets
	TopoDS_Shape sewedVolute = volute.SewBaseScrollWithExitPipe(volute.my_filleted2dScrollShapesStraightPipe, volute.my_inputPlaneFaceList, straightPipeVecRectExit, filletedRectangularExitVec, 1.E-06);
	BRepTools::Write(sewedVolute, "sewedVolute.brep");

	TopoDS_Solid SolidVolute = volute.mkSolidOfSewedVoluteWithExitPipe(sewedVolute);
	BRepTools::Write(SolidVolute, "SolidVolute.brep");

	ShapeFix_Shape fixedShape = ShapeFix_Shape(SolidVolute);

	TopoDS_Shape ReversedSolidVolute = volute.ReverseShapeIfInsideOut(SolidVolute);
	BRepTools::Write(ReversedSolidVolute, "ReversedSolidVolute.brep");
	
}

// Create volute without fillets
std::vector<TopoDS_Shape> GenerateVolute::CreateVoluteWithoutFillets()
{
	// Initializing my_numOfSections
	my_numOfSections = 60;

	// Calling the AreaCalculation function
	std::vector<double> areasOfSections = AreaCalculation(3000, 900);

	// Calling the CreateSection function
	std::vector<TopoDS_Wire> sectionVector = CreateSection(25, areasOfSections);

	std::vector<double> angleVector = CreateAngleVector();
	
	// Calling the CreateSection function
	my_sectionWireList = RotateCrossSections(sectionVector, angleVector);

	// Drawing the wired compound shape
	TopoDS_Shape compoundShapeWithBaseWire = CreateCompoundShape(my_sectionWireList);
	BRepTools::Write(compoundShapeWithBaseWire, "compoundShape.brep");

	// Adding the created shells to a vector
	my_shellVector = CreateShellList(my_sectionWireList, angleVector);

	std::vector<TopoDS_Shape> solidVectorOfScrollShape = CreateSolidList(my_sectionWireList, angleVector);

	TopoDS_Shape baseScroll = CreateBasePlane();

	return my_shellVector;
}

// Create volute with 2D fillets without base
std::vector<TopoDS_Shape> GenerateVolute::TotalVolumeWithoutInOut(/*std::vector<TopoDS_Shape> &thruSectPipe*/)
{
	//CreateVolute();

	std::vector<double> sectionArea = AreaCalculation(3000, 900);
	
	std::vector<TopoDS_Wire> sectionsWithoutBase = CreateSectionWithoutBase(25, sectionArea);

	std::vector<double> angleVector = CreateAngleVector();

	std::vector<TopoDS_Wire> filleted2dWireVec = createFilletedCrossSections(sectionsWithoutBase);

	std::vector<TopoDS_Wire> rotatedWireWithoutBase = RotateCrossSections(sectionsWithoutBase, angleVector);

	// Drawing the wired compound shape
	TopoDS_Shape compoundShape = CreateCompoundShape(rotatedWireWithoutBase);
	BRepTools::Write(compoundShape, "WireWithoutBase.brep");

	//my_scrollVectorWithoutBoolCut.clear();
	my_scrollVectorWithoutBoolCut = CreateShellList(rotatedWireWithoutBase, angleVector);

	// Applying fillet to scroll shapes
	//my_filletedScrollShape.clear();
	my_filletedScrollShape = mkFilletToScrollShapes(my_scrollVectorWithoutBoolCut);

	std::vector<TopoDS_Shape> solidVectorOfScrollShape = CreateSolidList(my_sectionWireList, angleVector);

	// Applying fillet to solid scrolls
	my_filletSolidScrolls = mkFilletToSolidScroll(solidVectorOfScrollShape);

	// creating solid vector of scroll shapes
	std::vector<TopoDS_Shape> solidVec = CreateSolidVectorWithBoolCut(my_filletSolidScrolls[0]);

	// creating the filleted translation
	filletedWire = GetFilletedWireForTrans(my_filletSolidScrolls[0]);
	BRepTools::Write(filletedWire, "filletedWire.brep");
	
	// creating a solid shape vector without boolean fuse
	std::vector<TopoDS_Shape> solidVectorWithoutBoolFuse = CreateSolidScrollVector();

	return my_scrollVectorWithoutBoolCut;

}

// Create volute with 2D fillets with base
std::vector<TopoDS_Shape> GenerateVolute::CreateVoluteWith2dFillets()
{
	std::vector<double> sectionArea = AreaCalculation(3000, 900);

	std::vector<TopoDS_Wire> sectionsWithoutBase = CreateSectionWithoutBase(25, sectionArea);

	std::vector<double> angleVector = CreateAngleVector();

	std::vector<TopoDS_Wire> filleted2dWireVec = createFilletedCrossSections(sectionsWithoutBase);

	std::vector<TopoDS_Wire> wireWithBase2dFillet = Create2dFilletedWireWithBase(filleted2dWireVec);
	BRepTools::Write(wireWithBase2dFillet[50], "wireWithBase2dFillet[50].brep");

	rotated2dFilletWireWithoutBase = RotateCrossSections(filleted2dWireVec, angleVector);

	std::vector<TopoDS_Wire> rotated2dFilletWireWithBase = RotateCrossSections(wireWithBase2dFillet, angleVector);

	// Drawing the wired compound shape
	TopoDS_Shape compoundShape = CreateCompoundShape(rotated2dFilletWireWithoutBase);
	BRepTools::Write(compoundShape, "WireWithoutBase.brep");

	TopoDS_Shape compoundShapeWithBase = CreateCompoundShape(rotated2dFilletWireWithBase);
	BRepTools::Write(compoundShapeWithBase, "compoundShapeWithBase.brep");

	scrollVectorWithoutBoolCut = CreateShellList(rotated2dFilletWireWithoutBase, angleVector);

	solidVector2dFillet = CreateSolidList(rotated2dFilletWireWithBase, angleVector);
	
	return scrollVectorWithoutBoolCut;
}

// Create straight exit pipe with 2D filleted wire
TopoDS_Shape GenerateVolute::CreateStraightExitPipeWith2dFilletedWire()
{
	//getVerticesFromEdges();
	std::vector<TopoDS_Edge> edgesWith2dFillets = create2dFilletsForSquare(6);
	my_rectWire2dFillets = Create2dWire(edgesWith2dFillets);
	BRepTools::Write(my_rectWire2dFillets, "wire2dFillets.brep");

	// Getting the translation of the square
	gp_Vec myVec(0, -200, 0);
	TopoDS_Wire trnsfWire = GetTranslationOfWire(my_rectWire2dFillets, myVec);
	BRepTools::Write(trnsfWire, "trnsf2dWire.brep");
	BRepBuilderAPI_MakeFace squarePlane(trnsfWire, true);
	BRepTools::Write(squarePlane, "squarePlaneWith2dFillets.brep");
	my_exitPlanes.push_back(squarePlane);

	// Getting the largest section of the section wire list
	TopoDS_Wire wireWith2dFillets = GetLargest2dFilletWire();
	BRepTools::Write(wireWith2dFillets, "largestWire.brep");

	// ThruSecting the exit pipe
	TopoDS_Shape ThruSectPipeShape = ThruSectExitPipe(wireWith2dFillets, trnsfWire, Standard_False);
	TopoDS_Shape ThruSectPipeShapeSolid = ThruSectExitPipe(wireWith2dFillets, trnsfWire, Standard_True);
	//my_thruSectPipeVec.push_back(ThruSectPipeShape);
	BRepTools::Write(ThruSectPipeShape, "ThruSectPipeShape2d.brep");

	// boolean cut for pipe
	TopoDS_Shape boolCut2dFilletPipe = GetBooleanResultForSolids(ThruSectPipeShape, solidVector2dFillet[3]);
	BRepTools::Write(solidVector2dFillet[3], "solidVector2dFillet[3].brep");
	std::vector<TopoDS_Shape> curvedPipeVec;
	curvedPipeVec.push_back(boolCut2dFilletPipe);
	BRepTools::Write(boolCut2dFilletPipe, " boolCut2dFilletPipe.brep");

	// boolean cut small scroll
	TopoDS_Shape boolCutForSmallScroll = GetBooleanResultForSolids(scrollVectorWithoutBoolCut[3], ThruSectPipeShapeSolid);
	BRepTools::Write(boolCutForSmallScroll, "boolCutForSmallScroll.brep");

	my_filleted2dScrollShapesStraightPipe = CreateShellVectorWithBoolCut(boolCutForSmallScroll, scrollVectorWithoutBoolCut);

	return boolCut2dFilletPipe;
}

// Create straight exit pipe with circular exit plane
TopoDS_Shape GenerateVolute::CreateStraightExitPipeWithCircularExit()
{
	//getVerticesFromEdges();
	gp_Pnt midPoint = GetMiddlePoint();
	TopoDS_Wire circleWire = CreateCircleToGetExitPipe(midPoint, 10000);

	// Getting the translation of the square
	gp_Vec myVec(0, -200, 0);
	TopoDS_Wire circTrnsfWire = GetTranslationOfWire(circleWire, myVec);
	BRepTools::Write(circTrnsfWire, "circTrnsfWire.brep");
	BRepBuilderAPI_MakeFace circlePlane(circTrnsfWire, true);
	BRepTools::Write(circlePlane, "circlePlane.brep");
	my_exitPlanes.push_back(circlePlane);

	// Getting the largest section of the section wire list
	TopoDS_Wire wireWith2dFillets = GetLargest2dFilletWire();
	BRepTools::Write(wireWith2dFillets, "largestWire.brep");

	// ThruSecting the exit pipe
	TopoDS_Shape ThruSectPipeShape = ThruSectExitPipe(wireWith2dFillets, circTrnsfWire, Standard_False);
	TopoDS_Shape ThruSectPipeShapeSolid = ThruSectExitPipe(wireWith2dFillets, circTrnsfWire, Standard_True);
	//my_thruSectPipeVec.push_back(ThruSectPipeShape);
	BRepTools::Write(ThruSectPipeShape, "ThruSectPipeShape.brep");

	// boolean cut for pipe
	TopoDS_Shape boolCutStraightPipeWithCirExit = GetBooleanResultForSolids(ThruSectPipeShape, solidVector2dFillet[3]);
	//BRepTools::Write(solidVector2dFillet[3], "solidVector2dFillet[3].brep");
	std::vector<TopoDS_Shape> curvedPipeVec;
	curvedPipeVec.push_back(boolCutStraightPipeWithCirExit);
	BRepTools::Write(boolCutStraightPipeWithCirExit, " boolCutStraightPipeWithCirExit.brep");

	// boolean cut small scroll
	TopoDS_Shape boolCutForSmallScroll = GetBooleanResultForSolids(scrollVectorWithoutBoolCut[3], ThruSectPipeShapeSolid);
	BRepTools::Write(boolCutForSmallScroll, "boolCutForSmallScroll.brep");

	my_filleted2dScrollShapesStraightPipe = CreateShellVectorWithBoolCut(boolCutForSmallScroll, scrollVectorWithoutBoolCut);

	return boolCutStraightPipeWithCirExit;
}

// Create straight pipe without fillets
TopoDS_Shape GenerateVolute::CreateStraightPipe()
{
	// Getting the middle point of the square
	gp_Pnt midPoint = GetMiddlePoint();
	//edgesOfSquare.clear();
	TopoDS_Wire squareWire = CreateSquareToGetExitPipe(midPoint, 3000);
	BRepTools::Write(squareWire, "squareWire.brep");

	// Getting the translation of the square
	gp_Vec myVec(0, -200, 0);
	TopoDS_Wire trnsfWire = GetTranslationOfWire(squareWire, myVec);
	BRepTools::Write(trnsfWire, "trnsfWire.brep");
	BRepBuilderAPI_MakeFace squarePlane(trnsfWire, true);
	BRepTools::Write(squarePlane, "squarePlane.brep");
	my_exitPlanes.push_back(squarePlane);

	// Getting the largest section of the section wire list
	TopoDS_Shape largestSection = my_sectionWireList[0];
	TopoDS_Wire wire = TopoDS::Wire(largestSection);

	// ThruSecting the exit pipe
	TopoDS_Shape ThruSectPipeShape = ThruSectExitPipe(wire, trnsfWire, Standard_False);
	my_thruSectPipeVec.push_back(ThruSectPipeShape);
	BRepTools::Write(ThruSectPipeShape, "ThruSectPipeShape.brep");

	// Getting the boolean cut of exit pipe
	TopoDS_Solid smallestSolidOfScrollShape = TopoDS::Solid(my_solidVector[3]);
	BRepTools::Write(smallestSolidOfScrollShape, "solidvec3.brep");
	my_exitPipeBooleanCut = GetSimpleBooleanResult(ThruSectPipeShape, smallestSolidOfScrollShape);
	BRepTools::Write(my_exitPipeBooleanCut, "exitPipeBooleanCut.brep");

	// Getting the boolean cut of the smallest part of the scroll
	my_ThruSectPipeSolidToBe = ThruSectExitPipe(wire, trnsfWire, Standard_True);
	TopoDS_Solid solidPipe = TopoDS::Solid(my_ThruSectPipeSolidToBe);
	my_SmallShellBooleanCut = GetSimpleBooleanResult(my_shellVector[3], solidPipe);
	BRepTools::Write(my_SmallShellBooleanCut, "SmallShellBooleanCut.brep");

	return my_exitPipeBooleanCut;
}

// Create 3D filleted straight exit pipe
TopoDS_Shape GenerateVolute::CreateFilletedStraightPipe()
{
	// Thrusecting the filleted exit pipe
	TopoDS_Shape FilletedThruSectPipeShape = ThruSectExitPipe(filletedWire, my_filletedSqaureWireVec[0], Standard_False);
	my_filletedThruSectPipeVec.push_back(FilletedThruSectPipeShape);
	BRepTools::Write(FilletedThruSectPipeShape, "FilletedThruSectPipe.brep");

	TopoDS_Shape FilletedThruSectSolidPipe = ThruSectExitPipe(filletedWire, my_filletedSqaureWireVec[0], Standard_True);
	// converting the shell pipe to solid 
	TopoDS_Solid squareSolidPipe = TopoDS::Solid(FilletedThruSectSolidPipe);
	BRepTools::Write(squareSolidPipe, "filletedSquareSolidPipe.brep");

	// pushing back solid pipe to solid pipe vector
	my_solidPipeVec.push_back(squareSolidPipe);

	return FilletedThruSectSolidPipe;

}

// Create rectangular exit plane
TopoDS_Face GenerateVolute::CreateRectangularExit()
{
	// Getting the middle point of the square
	gp_Pnt midPoint = GetMiddlePoint();
	edgesOfSquare.clear();
	TopoDS_Wire squareWire = CreateSquareToGetExitPipe(midPoint, 3000);
	BRepTools::Write(squareWire, "squareWire.brep");

	// Getting the translation of the square
	gp_Vec myVec(0, -200, 0);
	TopoDS_Wire trnsfWire = GetTranslationOfWire(squareWire, myVec);
	BRepTools::Write(trnsfWire, "trnsfWire.brep");
	BRepBuilderAPI_MakeFace squarePlane(trnsfWire, true);
	BRepTools::Write(squarePlane, "squarePlane.brep");
	my_exitPlanes.push_back(squarePlane);

	gp_Pnt pointTwo(midPoint.X() + 20, -70, midPoint.Z());
	gp_Pnt pointThree(midPoint.X() + 100, -160, midPoint.Z());
	GetVerticesToCreateCurve(midPoint, pointTwo, pointThree);

	TopoDS_Wire curvedWire = CreateBsplineWithThreePoints(midPoint, pointTwo, pointThree);
	BRepTools::Write(curvedWire, "curvedWire.brep");

	TopoDS_Wire trnslWire = CreateTrnslWire(curvedWire, squareWire);
	BRepTools::Write(trnslWire, "trnslWire.brep");
	TopoDS_Face faceFromTrnsWire = BRepBuilderAPI_MakeFace(trnslWire);
	BRepTools::Write(faceFromTrnsWire, "faceFromTrnsWire.brep");
	my_curvedRectangularExit.push_back(faceFromTrnsWire);

	return squarePlane;
}

// Create 2D filleted rectangular exit
TopoDS_Face GenerateVolute::CreateFilletedRectangularExit()
{
	std::vector<TopoDS_Edge> edgesWith2dFillets = create2dFilletsForSquare(6);
	TopoDS_Wire rectWire2dFillets = Create2dWire(edgesWith2dFillets);
	BRepTools::Write(rectWire2dFillets, "wire2dFillets.brep");

	// Getting the translation of the square
	gp_Vec myVec(0, -200, 0);
	TopoDS_Wire trnsfWire = GetTranslationOfWire(rectWire2dFillets, myVec);
	BRepTools::Write(trnsfWire, "trnsf2dWire.brep");
	TopoDS_Face filletedsquarePlane = BRepBuilderAPI_MakeFace(trnsfWire, true);
	BRepTools::Write(filletedsquarePlane, "squarePlaneWith2dFillets.brep");
	my_exitPlanes.push_back(filletedsquarePlane);

	return filletedsquarePlane;
}

// Create new cross section volute
std::vector<TopoDS_Shape> GenerateVolute::CreateNewCrossSection()
{
	//my_numOfSections = 60;

	std::vector<double> areasOfSections = AreaCalculation(26250, 900);
	
	std::vector<double> angleVector = CreateAngleVector();

	/////// Create new volute without fillets
	std::vector<TopoDS_Wire> newCrossSectionVecWithoutFillets = CreateNewCrossSection(50, 100, 75, 50, 50, areasOfSections);

	std::vector<TopoDS_Wire> rotatedCrossSectionWires = RotateCrossSections(newCrossSectionVecWithoutFillets, angleVector);

	TopoDS_Shape compoundShapeWithBaseWire = CreateCompoundShape(rotatedCrossSectionWires);
	BRepTools::Write(compoundShapeWithBaseWire, "compoundShape.brep");

	std::vector<TopoDS_Shape> shellVector = CreateShellList(rotatedCrossSectionWires, angleVector);

	std::vector<TopoDS_Shape> solidVectorOfScrollShape = CreateSolidList(rotatedCrossSectionWires, angleVector);

	std::vector<TopoDS_Shape> filletedScrollShapes = mkFilletToScrollShapes(shellVector);

	/////// Create new volute with 2D fillets
	std::vector<TopoDS_Wire> newCrossSectionWith2DFillets = createFilletedCrossSections_newShape(newCrossSectionVecWithoutFillets);

	std::vector<TopoDS_Wire> rotatedFilletedCrossSectionWires = RotateCrossSections(newCrossSectionWith2DFillets, angleVector);

	TopoDS_Shape filletedCompoundShapeWithBaseWire = CreateCompoundShape(rotatedFilletedCrossSectionWires);
	BRepTools::Write(filletedCompoundShapeWithBaseWire, "filletedCompoundShapeWithBaseWire.brep");

	std::vector<TopoDS_Shape> filletedShellVector = CreateShellList(rotatedFilletedCrossSectionWires, angleVector);

	std::vector<TopoDS_Shape> filletedSolidVector = CreateSolidList(rotatedFilletedCrossSectionWires, angleVector);

	
	
	std::vector<TopoDS_Edge> edgesWith2dFillets = create2dFilletsForSquare(6);
	TopoDS_Wire rectWire2dFillets = Create2dWire(edgesWith2dFillets);
	BRepTools::Write(my_rectWire2dFillets, "wire2dFillets.brep");

	// Getting the translation of the square
	gp_Vec myVec(0, -200, 0);
	TopoDS_Wire trnsfWire = GetTranslationOfWire(rectWire2dFillets, myVec);
	BRepTools::Write(trnsfWire, "trnsf2dWire.brep");

	// ThruSecting the exit pipe
	TopoDS_Shape ThruSectPipe_NewShape = ThruSectExitPipe(rotatedCrossSectionWires[0], trnsfWire, Standard_False);
	BRepTools::Write(ThruSectPipe_NewShape, "ThruSectPipe_NewShape.brep");

	return shellVector;
}


// Create circular exit plane
TopoDS_Face GenerateVolute::CreateCircularExit()
{
	gp_Pnt midPoint = GetMiddlePoint();
	TopoDS_Wire circleWire = CreateCircleToGetExitPipe(midPoint, 10000);

	// Getting the translation of the square
	gp_Vec myVec(0, -200, 0);
	TopoDS_Wire circTrnsfWire = GetTranslationOfWire(circleWire, myVec);
	BRepTools::Write(circTrnsfWire, "circTrnsfWire.brep");
	BRepBuilderAPI_MakeFace circlePlane(circTrnsfWire, true);
	BRepTools::Write(circlePlane, "circlePlane.brep");
	my_exitPlanes.push_back(circlePlane);

	TopoDS_Face circularExitPlane = circlePlane;

	return circlePlane;
}

// Create the curved exit pipe
TopoDS_Shape GenerateVolute::CreateCurvedPipeWithCircularExit()
{
	TopoDS_Wire curvedWire = MakeSpineCurve();
	TopoDS_Wire wireWith2dFillets = GetLargest2dFilletWire();

	TopoDS_Wire trnslWire = CreateTrnslWire(curvedWire, my_circleWire);
	BRepTools::Write(trnslWire, "trnslWire.brep");
	TopoDS_Face faceFromTrnsWire = BRepBuilderAPI_MakeFace(trnslWire);
	BRepTools::Write(faceFromTrnsWire, "faceFromTrnsWire.brep");
	my_curvedCircularExitVec.push_back(faceFromTrnsWire);

	TopoDS_Shape curvedPipe = CreatePipeShellFromTrnsWire(curvedWire, wireWith2dFillets, trnslWire);
	BRepTools::Write(curvedPipe, "curvedPipe.brep");

	TopoDS_Shape solidCurvedPipe = CreatePipeSoildFromTrnsWire(curvedWire, wireWith2dFillets, trnslWire);
	BRepTools::Write(solidCurvedPipe, "solidCurvedPipe.brep");

	// solid boolean cut small scroll
	TopoDS_Shape solidBoolCutForSmallScroll = GetBooleanResultForSolids(my_filletSolidScrolls[3], solidCurvedPipe);
	BRepTools::Write(solidBoolCutForSmallScroll, "solidBoolCutForSmallScroll.brep");

	// boolean cut curved pipe
	TopoDS_Shape boolCutForCurvedPipe = GetBooleanResultForSolids(curvedPipe, solidBoolCutForSmallScroll);
	std::vector<TopoDS_Shape> curvedPipeVec;
	curvedPipeVec.push_back(boolCutForCurvedPipe);
	BRepTools::Write(boolCutForCurvedPipe, "boolCutForCurvedPipe.brep");

	// solid boolean cut for curved pipe
	TopoDS_Shape solidBoolCutForCurvedPipe = GetBooleanResultForSolids(solidCurvedPipe, my_filletSolidScrolls[3]);
	BRepTools::Write(solidBoolCutForCurvedPipe, "solidBoolCutForCurvedPipe.brep");

	// boolean cut small scroll
	TopoDS_Shape boolCutForSmallScroll = GetBooleanResultForSolids(my_filletedScrollShape[3], solidCurvedPipe);
	BRepTools::Write(boolCutForSmallScroll, "boolCutForSmallScroll.brep");

	my_filleted2dScrollShellShapesCurvedPipe = CreateShellVectorWithBoolCut(boolCutForSmallScroll, my_filletedScrollShape);
	
	return boolCutForCurvedPipe;
}

// Create curved pipe with rectangular exit 
TopoDS_Shape GenerateVolute::CreateCurvedPipeWithRectExit()
{
	TopoDS_Wire curvedWire = MakeSpineCurve();
	TopoDS_Wire wireWith2dFillets = GetLargest2dFilletWire();

	TopoDS_Wire trnslWire = CreateTrnslWire(curvedWire, my_rectWire2dFillets);
	BRepTools::Write(trnslWire, "trnslWire.brep");
	TopoDS_Face faceFromTrnsWire = BRepBuilderAPI_MakeFace(trnslWire);
	BRepTools::Write(faceFromTrnsWire, "faceFromTrnsWire.brep");
	my_curvedRectangularExitWithFillets.push_back(faceFromTrnsWire);

	TopoDS_Shape curvedPipe = CreatePipeShellFromTrnsWire(curvedWire, wireWith2dFillets, trnslWire);
	BRepTools::Write(curvedPipe, "curvedPipe.brep");

	TopoDS_Shape solidCurvedPipe = CreatePipeSoildFromTrnsWire(curvedWire, wireWith2dFillets, trnslWire);
	BRepTools::Write(solidCurvedPipe, "solidCurvedPipe.brep");

	// solid boolean cut small scroll
	TopoDS_Shape solidBoolCutForSmallScroll = GetBooleanResultForSolids(my_filletSolidScrolls[3], solidCurvedPipe);
	BRepTools::Write(solidBoolCutForSmallScroll, "solidBoolCutForSmallScroll.brep");

	// boolean cut curved pipe
	TopoDS_Shape boolCutForCurvedPipe = GetBooleanResultForSolids(curvedPipe, solidBoolCutForSmallScroll);
	std::vector<TopoDS_Shape> curvedPipeVec;
	curvedPipeVec.push_back(boolCutForCurvedPipe);
	BRepTools::Write(boolCutForCurvedPipe, "boolCutForCurvedPipe.brep");

	// solid boolean cut for curved pipe
	TopoDS_Shape solidBoolCutForCurvedPipe = GetBooleanResultForSolids(solidCurvedPipe, my_filletSolidScrolls[3]);
	BRepTools::Write(solidBoolCutForCurvedPipe, "solidBoolCutForCurvedPipe.brep");

	// boolean cut small scroll
	TopoDS_Shape boolCutForSmallScroll = GetBooleanResultForSolids(my_filletedScrollShape[3], solidCurvedPipe);
	BRepTools::Write(boolCutForSmallScroll, "boolCutForSmallScroll.brep");

	my_filleted2dScrollShellShapesCurvedPipe = CreateShellVectorWithBoolCut(boolCutForSmallScroll, my_filletedScrollShape);

	return boolCutForCurvedPipe;
}

// Creating the base planes / input planes
TopoDS_Shape GenerateVolute::CreateBasePlane()
{
	// Calculating area to create input planes
	std::vector<double> sectionArea = AreaCalculation(3000, 900);
	
	// Creating base sections 
	std::vector<TopoDS_Wire> baseSections = CreateBaseSections(25, sectionArea);
	
	std::vector<double> angleVector = CreateAngleVector();

	// Rotating the cross-section wires
	BaseRotatedWire = RotateCrossSections(baseSections, angleVector);

	// Drawing the wired compound shape
	TopoDS_Shape compoundShape = CreateCompoundShape(BaseRotatedWire);
	BRepTools::Write(compoundShape, "BasePlaneWire.brep");

	// Adding the created shells to a vector
	my_inputPlaneList = CreateInputPlanes(BaseRotatedWire, angleVector);
	// Creating a faceList out of the shellList
	my_inputPlaneFaceList = CreateInputPlaneFaces(my_inputPlaneList);

	// Sewing the shells
	TopoDS_Shape baseShell = SewingScrollShells(my_inputPlaneList, 1.E-06);
	BRepTools::Write(baseShell, "BaseShell.brep");

	TopoDS_Solid solidBaseShell = mkSolidOfSewedVoluteWithExitPipe(baseShell);
	BRepTools::Write(solidBaseShell, "solidBaseShell.brep");

	TopoDS_Shape reversedSolidBaseShell = ReverseShapeIfInsideOut(solidBaseShell);
	BRepTools::Write(reversedSolidBaseShell, "reversedSolidBaseShell.brep");

	return baseShell;
}

// Make bottle
TopoDS_Shape CModelingDoc::MakeBottle(const Standard_Real myWidth, const Standard_Real myHeight,
	const Standard_Real myThickness)
{
	
	gp_Pnt aPnt1(-myWidth / 2., 0, 0);
	gp_Pnt aPnt2(-myWidth / 2., -myThickness / 4., 0);
	gp_Pnt aPnt3(0, -myThickness / 2., 0);
	gp_Pnt aPnt4(myWidth / 2., -myThickness / 4., 0);
	gp_Pnt aPnt5(myWidth / 2., 0, 0);

	// Defining the Geometry
	Handle(Geom_TrimmedCurve) anArcOfCircle = GC_MakeArcOfCircle(aPnt2, aPnt3, aPnt4);
	Handle(Geom_TrimmedCurve) aSegment1 = GC_MakeSegment(aPnt1, aPnt2);
	Handle(Geom_TrimmedCurve) aSegment2 = GC_MakeSegment(aPnt4, aPnt5);

	// Defining the Topology
	TopoDS_Edge anEdge1 = BRepBuilderAPI_MakeEdge(aSegment1);
	TopoDS_Edge anEdge2 = BRepBuilderAPI_MakeEdge(anArcOfCircle);
	TopoDS_Edge anEdge3 = BRepBuilderAPI_MakeEdge(aSegment2);
	TopoDS_Wire aWire = BRepBuilderAPI_MakeWire(anEdge1, anEdge2, anEdge3);

	gp_Ax1 xAxis = gp::OX();
	gp_Trsf aTrsf;
	aTrsf.SetMirror(xAxis);
	BRepBuilderAPI_Transform aBRepTrsf(aWire, aTrsf);
	TopoDS_Shape aMirroredShape = aBRepTrsf.Shape();
	TopoDS_Wire aMirroredWire = TopoDS::Wire(aMirroredShape);

	BRepBuilderAPI_MakeWire mkWire;
	mkWire.Add(aWire);
	mkWire.Add(aMirroredWire);
	TopoDS_Wire myWireProfile = mkWire.Wire();

	TopoDS_Face myFaceProfile = BRepBuilderAPI_MakeFace(myWireProfile);
	gp_Vec aPrismVec(0, 0, myHeight);
	TopoDS_Shape myBody = BRepPrimAPI_MakePrism(myFaceProfile, aPrismVec);

	BRepFilletAPI_MakeFillet mkFillet(myBody);
	TopExp_Explorer anEdgeExplorer(myBody, TopAbs_EDGE);
	while (anEdgeExplorer.More())
	{
		TopoDS_Edge anEdge = TopoDS::Edge(anEdgeExplorer.Current());
	
		mkFillet.Add(myThickness / 12., anEdge);
		anEdgeExplorer.Next();
	}

	myBody = mkFillet.Shape();

	// Adding the Neck
	gp_Pnt neckLocation(0, 0, myHeight);
	gp_Dir neckAxis = gp::DZ();
	gp_Ax2 neckAx2(neckLocation, neckAxis);

	Standard_Real myNeckRadius = myThickness / 4.;
	Standard_Real myNeckHeight = myHeight / 10.;

	BRepPrimAPI_MakeCylinder MKCylinder(neckAx2, myNeckRadius, myNeckHeight);
	TopoDS_Shape myNeck = MKCylinder.Shape();

	myBody = BRepAlgoAPI_Fuse(myBody, myNeck);

	
	TopoDS_Face faceToRemove;
	Standard_Real zMax = -1;

	for (TopExp_Explorer aFaceExplorer(myBody, TopAbs_FACE); aFaceExplorer.More(); aFaceExplorer.Next()) {
		TopoDS_Face aFace = TopoDS::Face(aFaceExplorer.Current());
		// Check if <aFace> is the top face of the bottle's neck 
		Handle(Geom_Surface) aSurface = BRep_Tool::Surface(aFace);
		if (aSurface->DynamicType() == STANDARD_TYPE(Geom_Plane)) {
			Handle(Geom_Plane) aPlane = Handle(Geom_Plane)::DownCast(aSurface);
			gp_Pnt aPnt = aPlane->Location();
			Standard_Real aZ = aPnt.Z();
			if (aZ > zMax) {
				zMax = aZ;
				faceToRemove = aFace;
			}
		}
	}

	TopTools_ListOfShape facesToRemove;
	facesToRemove.Append(faceToRemove);
	BRepOffsetAPI_MakeThickSolid BodyMaker;
	BodyMaker.MakeThickSolidByJoin(myBody, facesToRemove, -myThickness / 50, 1.e-3);
	myBody = BodyMaker.Shape();
	// Threading : Create Surfaces
	Handle(Geom_CylindricalSurface) aCyl1 = new Geom_CylindricalSurface(neckAx2, myNeckRadius * 0.99);
	Handle(Geom_CylindricalSurface) aCyl2 = new Geom_CylindricalSurface(neckAx2, myNeckRadius * 1.05);

	// Threading : Define 2D Curves
	gp_Pnt2d aPnt(2. * M_PI, myNeckHeight / 2.);
	gp_Dir2d aDir(2. * M_PI, myNeckHeight / 4.);
	gp_Ax2d anAx2d(aPnt, aDir);

	Standard_Real aMajor = 2. * M_PI;
	Standard_Real aMinor = myNeckHeight / 10;

	Handle(Geom2d_Ellipse) anEllipse1 = new Geom2d_Ellipse(anAx2d, aMajor, aMinor);
	Handle(Geom2d_Ellipse) anEllipse2 = new Geom2d_Ellipse(anAx2d, aMajor, aMinor / 4);
	Handle(Geom2d_TrimmedCurve) anArc1 = new Geom2d_TrimmedCurve(anEllipse1, 0, M_PI);
	Handle(Geom2d_TrimmedCurve) anArc2 = new Geom2d_TrimmedCurve(anEllipse2, 0, M_PI);
	gp_Pnt2d anEllipsePnt1 = anEllipse1->Value(0);
	gp_Pnt2d anEllipsePnt2 = anEllipse1->Value(M_PI);

	Handle(Geom2d_TrimmedCurve) aSegment = GCE2d_MakeSegment(anEllipsePnt1, anEllipsePnt2);
	// Building Edges and Wires
	TopoDS_Edge anEdge1OnSurf1 = BRepBuilderAPI_MakeEdge(anArc1, aCyl1);
	TopoDS_Edge anEdge2OnSurf1 = BRepBuilderAPI_MakeEdge(aSegment, aCyl1);
	TopoDS_Edge anEdge1OnSurf2 = BRepBuilderAPI_MakeEdge(anArc2, aCyl2);
	TopoDS_Edge anEdge2OnSurf2 = BRepBuilderAPI_MakeEdge(aSegment, aCyl2);
	TopoDS_Wire threadingWire1 = BRepBuilderAPI_MakeWire(anEdge1OnSurf1, anEdge2OnSurf1);
	TopoDS_Wire threadingWire2 = BRepBuilderAPI_MakeWire(anEdge1OnSurf2, anEdge2OnSurf2);
	BRepLib::BuildCurves3d(threadingWire1);
	BRepLib::BuildCurves3d(threadingWire2);

	BRepOffsetAPI_ThruSections aTool(Standard_True);
	aTool.AddWire(threadingWire1);
	aTool.AddWire(threadingWire2);
	aTool.CheckCompatibility(Standard_False);

	TopoDS_Shape myThreading = aTool.Shape();

	// Building the Resulting Compound 
	TopoDS_Compound aRes;
	BRep_Builder aBuilder;
	aBuilder.MakeCompound(aRes);
	aBuilder.Add(aRes, myBody);
	aBuilder.Add(aRes, myThreading);

	return aRes;
}

void CModelingDoc::OnBottle()
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for (aListIterator.Initialize(aList); aListIterator.More(); aListIterator.Next()) {
		myAISContext->Remove(aListIterator.Value(), Standard_False);
	}
	TopoDS_Shape S = MakeBottle(50, 70, 30);
	Handle(AIS_Shape) ais1 = new AIS_Shape(S);
	myAISContext->SetColor(ais1, Quantity_NOC_PINK, Standard_False);
	myAISContext->SetMaterial(ais1, Graphic3d_NOM_PLASTIC, Standard_False);
	myAISContext->Display(ais1, Standard_False);
	
	Fit();

}

void CModelingDoc::ReadSTEP1(const Handle(AIS_InteractiveContext)& anInteractiveContext)
{
	Handle(TopTools_HSequenceOfShape) aSequence = CModelingDoc::ReadSTEP1();
	if (!aSequence.IsNull()) {
		for (int i = 1; i <= aSequence->Length(); i++)
			anInteractiveContext->Display(new AIS_Shape(aSequence->Value(i)), Standard_False);
	}
}

Handle(TopTools_HSequenceOfShape) CModelingDoc::ReadSTEP1()// not by reference --> the sequence is created here !!
{
	CFileDialog dlg(TRUE,
					NULL,
					NULL,
					OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
					L"STEP Files (*.stp;*.step)|*.stp; *.step|All Files (*.*)|*.*||",
					NULL);

	CString SHAREPATHValue;
	SHAREPATHValue.GetEnvironmentVariable(L"CSF_OCCTDataPath");
	CString initdir = (SHAREPATHValue + "\\step");

	dlg.m_ofn.lpstrInitialDir = initdir;

	Handle(TopTools_HSequenceOfShape) aSequence = new TopTools_HSequenceOfShape();
	if (dlg.DoModal() == IDOK)
	{
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
		TCollection_AsciiString aFileName((const wchar_t*)dlg.GetPathName());
		IFSelect_ReturnStatus ReturnStatus = ReadSTEP1(aFileName.ToCString(), aSequence);
		switch (ReturnStatus)
		{
		case IFSelect_RetError:
			MessageBoxW(AfxGetApp()->m_pMainWnd->m_hWnd, L"Not a valid Step file", L"ERROR", MB_ICONWARNING);
			break;
		case IFSelect_RetFail:
			MessageBoxW(AfxGetApp()->m_pMainWnd->m_hWnd, L"Reading has failed", L"ERROR", MB_ICONWARNING);
			break;
		case IFSelect_RetVoid:
			MessageBoxW(AfxGetApp()->m_pMainWnd->m_hWnd, L"Nothing to transfer", L"ERROR", MB_ICONWARNING);
			break;
		}
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	}
	return aSequence;

}

IFSelect_ReturnStatus CModelingDoc::ReadSTEP1(const Standard_CString& aFileName, Handle(TopTools_HSequenceOfShape)& aHSequenceOfShape)
{
	aHSequenceOfShape->Clear();

	// create additional log file
	STEPControl_Reader aReader;
	IFSelect_ReturnStatus status = aReader.ReadFile(aFileName);
	if (status != IFSelect_RetDone)
		return status;

	aReader.WS()->TransferReader()->TransientProcess()->SetTraceLevel(2); // increase default trace level

	Standard_Boolean failsonly = Standard_False;
	aReader.PrintCheckLoad(failsonly, IFSelect_ItemsByEntity);

	// Root transfers
	Standard_Integer nbr = aReader.NbRootsForTransfer();
	aReader.PrintCheckTransfer(failsonly, IFSelect_ItemsByEntity);
	for (Standard_Integer n = 1; n <= nbr; n++) {
		//Standard_Boolean ok =
		aReader.TransferRoot(n);
	}

	// Collecting resulting entities
	Standard_Integer nbs = aReader.NbShapes();
	if (nbs == 0) {
		return IFSelect_RetVoid;
	}
	for (Standard_Integer i = 1; i <= nbs; i++) {
		aHSequenceOfShape->Append(aReader.Shape(i));
	}

	return status;
}

void CModelingDoc::OnStep()
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for (aListIterator.Initialize(aList); aListIterator.More(); aListIterator.Next()) {
		myAISContext->Remove(aListIterator.Value(), Standard_False);
	}
	Handle(TopTools_HSequenceOfShape) aSequence = CModelingDoc::ReadSTEP1();
	if (!aSequence.IsNull()) {
		for (int i = 1; i <= aSequence->Length(); i++)

			myAISContext->Display(new AIS_Shape(aSequence->Value(i)), Standard_False);
	}

	Fit();

	int faceCounter = 0;
	Handle_AIS_Shape hs;
	AIS_ListOfInteractive aListOfIO;
	myAISContext->DisplayedObjects(aListOfIO);
	
	for (aListIterator.Initialize(aListOfIO); aListIterator.More(); aListIterator.Next())
	{
		if ((aListIterator.Value()->IsKind(STANDARD_TYPE(AIS_Shape))))
		{
			hs = Handle_AIS_Shape::DownCast(aListIterator.Value());
			BRepTools::Write(hs->Shape(), "shape.brep");
		}	
	}

	BladeShapes.clear();
	for (TopExp_Explorer explr(hs->Shape(), TopAbs_FACE); explr.More(); explr.Next())
	{
		TopoDS_Face tempFace = TopoDS::Face(explr.Current());
		BRepTools::Write(tempFace, "tempFace.brep");
		faceCounter++;
		
		if (faceCounter == 4 | faceCounter == 5 | faceCounter == 6 | faceCounter == 7)
		{
			BladeShapes.push_back(tempFace); //ps,le,ss,te
		}
		if (faceCounter == 8)
		{
			FinalBladeShapes.push_back(tempFace);
			for (int i = 0; i < BladeShapes.size(); i++)
			{
				FinalBladeShapes.push_back(BladeShapes[i]);
			}
		}
		
	}

	// Caling CreateIsoCurves function
	psEdgeArray = CreateIsoCurves(TopoDS::Face(BladeShapes[0]), 11, 0);
	leEdgeArray = CreateIsoCurves(TopoDS::Face(BladeShapes[1]), 11, 0);
	ssEdgeArray = CreateIsoCurves(TopoDS::Face(BladeShapes[2]), 11, 0);
	teEdgeArray = CreateIsoCurves(TopoDS::Face(BladeShapes[3]), 11, 0);

	// Calling CreateFaceFromWires function
	TopoDS_Shape psFace = CreateFaceFromWires(psEdgeArray);
	TopoDS_Shape ssFace = CreateFaceFromWires(ssEdgeArray);
	TopoDS_Shape leFace = CreateFaceFromWires(leEdgeArray);
	TopoDS_Shape teFace = CreateFaceFromWires(teEdgeArray);
	BRepTools::Write(psFace, "psFace.brep");
	BRepTools::Write(ssFace, "ssFace.brep");
	
	TopoDS_Face facePS, faceSS; 
	for (TopExp_Explorer explr(psFace, TopAbs_FACE); explr.More(); explr.Next())
	{
		facePS = TopoDS::Face(explr.Current());
	}
	for (TopExp_Explorer explr(ssFace, TopAbs_FACE); explr.More(); explr.Next())
	{
		faceSS = TopoDS::Face(explr.Current());
	}

	// creating the hubline
	CreateHubline(facePS,faceSS);

	// Calling GetPointsFromEdge function
	vector<gp_Pnt> psPoints = GetPointsFromEdge(horEdgeArray[0], 10);
	vector<gp_Pnt> ssPoints = GetPointsFromEdge(horEdgeArray[1], 10);
	vector<gp_Pnt> midPntArray;

	for (int i = 0; i < psPoints.size() - 1; i++)
	{
		gp_Pnt midPnt = gp_Pnt((psPoints[i].X() + ssPoints[i].X()) / 2, (psPoints[i].Y() + ssPoints[i].Y()) / 2, (psPoints[i].Z() + ssPoints[i].Z()) / 2);
		midPntArray.push_back(midPnt);
	}
	
	// Getting the midEdge
	Handle(Geom_BSplineCurve) curve = MkBSplinecurve();
	TopoDS_Edge midEdge = BRepBuilderAPI_MakeEdge(curve);
	BRepTools::Write(midEdge, "midEdge.brep");

	// Extending the Edge
	TopoDS_Edge extEdge = ExtendEdge(midEdge, 1, 1);
	BRepTools::Write(extEdge, "extendEdge.brep");
	
	// Translating  the extended edge
	gp_Vec myVec(-15, 0, 0);
	TopoDS_Shape trsfEdge = GetTranslation(extEdge, myVec);
	BRepTools::Write(trsfEdge, "trsfEdge.brep");
	
	TopoDS_Edge translatedEdge;
	for (TopExp_Explorer explr(trsfEdge, TopAbs_EDGE); explr.More(); explr.Next())
	{
		translatedEdge = TopoDS::Edge(explr.Current());
	}
	// Creating the hubFace
	TopoDS_Shape HubFace = CreateHubFace(extEdge, translatedEdge);
	BRepTools::Write(HubFace, "hubFace.brep");

	// Rotating the hubFace
	TopoDS_Shape rotatedFace = Rotating(0.1, HubFace);
	BRepTools::Write(rotatedFace, "rotatedFace.brep");

	// Revolving the hubFace
	TopoDS_Shape revolvedShape = Revolving(rotatedFace);
	BRepTools::Write(revolvedShape, "revolvedFace.brep");

	// Drawing le, te edges
	BRepTools::Write(leFace, "leFace.brep");
	BRepTools::Write(teFace, "teFace.brep");
	TopoDS_Face faceLE, faceTE;
	for (TopExp_Explorer explr(leFace, TopAbs_FACE); explr.More(); explr.Next())
	{
		faceLE = TopoDS::Face(explr.Current());
	}
	for (TopExp_Explorer explr(teFace, TopAbs_FACE); explr.More(); explr.Next())
	{
		faceTE = TopoDS::Face(explr.Current());
	}
	BladeBaseEdges(faceLE, faceTE);

	// Connecting blade base edges from a wire	
	TopoDS_Wire baseWire = ConnectBladeBaseEdges();
	BRepTools::Write(baseWire, "baseWire.brep");

	// calling topHubface function
	TopoDS_Face topFace = TopHubFace(revolvedShape);
	BRepTools::Write(topFace, "topHubFace.brep");
	

	// check T or F
	CheckForReverseWire(topFace, baseWire);

	// Creating holedFace
	TopoDS_Face holedHubFace = CreateHoledFaceModified(topFace, wireList);
	BRepTools::Write(holedHubFace, "holedFace.brep");

	vector<TopoDS_Shape> finalBladeHub = FinalShapes(revolvedShape, topFace, wireList);

	TopoDS_Shape TheBlade = makeVolumeShape(finalBladeHub, 1.E-06);
	BRepTools::Write(TheBlade, "TheBlade.brep");
}

vector<TopoDS_Edge> CModelingDoc::CreateIsoCurves(TopoDS_Face face, int numSections, int uIso)
{
	vector<TopoDS_Edge> edgeArray;
	Handle_Geom_Surface surf = BRep_Tool::Surface(face);

	Standard_Real dUmax, dUmin, dVmax, dVmin;
	BRepTools::UVBounds(face, dUmin, dUmax, dVmin, dVmax);

	for (int i = 0; i < numSections; i++)
	{
		Handle_Geom_Curve theGeomCurve;
		double uParam = (!uIso) ? dVmin + i*(dVmax - dVmin) / (numSections - 1) : dUmin + i*(dUmax - dUmin) / (numSections - 1);
		theGeomCurve = (!uIso) ? surf->VIso(uParam) : surf->UIso(uParam);
		theGeomCurve = new Geom_TrimmedCurve(theGeomCurve, (!uIso) ? dUmin : dVmin, (!uIso) ? dUmax : dVmax);
	
		BRepBuilderAPI_MakeEdge theEdgeBuilder(theGeomCurve);
		BRepTools::Write(theEdgeBuilder.Edge(), "tempEdge.brep");
		edgeArray.push_back(theEdgeBuilder.Edge());
	}

	return edgeArray;
}

TopoDS_Shape CModelingDoc::CreateFaceFromWires(vector<TopoDS_Edge> edgeArray)
{
	BRepOffsetAPI_ThruSections BTS1(Standard_False, Standard_False, 1.0e-10);
	BTS1.SetMaxDegree(2);
	
	for (int j = 0; j < edgeArray.size() - 1; j++)
	{
		TopoDS_Wire aWire = BRepBuilderAPI_MakeWire(edgeArray[j]);
		BTS1.AddWire(aWire);
	}

	BTS1.Build();
	TopoDS_Shape sideFaces = BTS1.Shape();
	
	return sideFaces;
}

TopoDS_Edge CModelingDoc::CreateHubline(TopoDS_Face ps, TopoDS_Face ss)
{
	int edgeCounter = 0;
	horEdgeArray.clear();
	TopoDS_Edge tempEdge;
	
	for (TopExp_Explorer explr(ps, TopAbs_EDGE); explr.More(); explr.Next())
	{
		tempEdge = TopoDS::Edge(explr.Current());
		BRepTools::Write(tempEdge, "edge.brep");
		edgeCounter++;
		
		if (edgeCounter == 1)				
		{
			horEdgeArray.push_back(tempEdge);
		}
	}
	int edgeCounter1 = 0;
	for (TopExp_Explorer explr(ss, TopAbs_EDGE); explr.More(); explr.Next())
	{
		tempEdge = TopoDS::Edge(explr.Current());
		BRepTools::Write(tempEdge, "edge.brep");
		edgeCounter1++;
		
		if (edgeCounter1 == 1)				
		{
			horEdgeArray.push_back(tempEdge);
		}
	}
	return tempEdge;
}

vector<gp_Pnt> CModelingDoc::GetPointsFromEdge(TopoDS_Edge edge, int n)
{
	Standard_Real first, last;
	gp_Pnt p;
	Handle(Geom_Curve) c = BRep_Tool::Curve(edge, first, last);
	GProp_GProps system;
	BRepGProp::LinearProperties(edge, system);
	BRepAdaptor_Curve gac(edge);
	//vector<gp_Pnt> gp_Pnt_list;
	GCPnts_UniformAbscissa algo(gac, n); // create n new points
	if (algo.IsDone())
	{
		// show the new points
		if (gp_Pnt_list.size() == 0)
		{
			for (int i = 1; i <= algo.NbPoints(); i++)
			{
				c->D0(algo.Parameter(i), p);
				gp_Pnt_list.push_back(p);
				/*TopoDS_Vertex V = BRepBuilderAPI_MakeVertex(p);
				BRepTools::Write(V, "vertices.brep");*/
			}
		}
	}
	return gp_Pnt_list;
}

Handle(Geom_BSplineCurve) CModelingDoc::MkBSplinecurve()
{ 
	Standard_Integer numPt = static_cast<Standard_Integer>(gp_Pnt_list.size());
	Handle_TColgp_HArray1OfPnt pPoints = new TColgp_HArray1OfPnt(1, numPt);

	for (auto i = 1; i <= numPt; i++)
	{
		pPoints->SetValue(i, gp_Pnt_list[i - 1]);
	}
	GeomAPI_Interpolate interp(pPoints, Standard_False, 1.0e-6);
	interp.Perform();
	Handle(Geom_BSplineCurve) BSplinecurve;
	if (interp.IsDone())
	{
		BSplinecurve = interp.Curve();
	}
	return BSplinecurve;
}

TopoDS_Edge CModelingDoc::ExtendEdge(TopoDS_Edge edgeToExtend, Standard_Boolean extendAfter, bool extendStart)
{
	TopoDS_Edge extendedEdge;
	Standard_Real u1, u2;
	gp_Vec v1, v2;
	gp_Pnt firstPnt1, lastPnt1, firstPnt2, lastPnt2, pntTemp1, pntTemp2;
	Handle(Geom_Curve) geomCurve = BRep_Tool::Curve(edgeToExtend, u1, u2);
	Handle(Geom_TrimmedCurve) trimedCurve = new Geom_TrimmedCurve(geomCurve, u1, u2);
	if (extendAfter)
	{
		geomCurve->D1(u2, lastPnt1, v2);
		pntTemp2 = lastPnt1;
		pntTemp2.Translate(7.0*v2);
		GeomLib::ExtendCurveToPoint(trimedCurve, pntTemp2, 1, extendAfter);
	}
	if (extendStart)
	{
		geomCurve->D1(u1, firstPnt1, v1);
		pntTemp1 = firstPnt1;
		pntTemp1.Translate(-7.0*v1);
		GeomLib::ExtendCurveToPoint(trimedCurve, pntTemp1, 1, false);
	}
	BRepBuilderAPI_MakeEdge mkedge(trimedCurve);

	return mkedge.Edge();
}

TopoDS_Shape CModelingDoc::GetTranslation(TopoDS_Shape shape, gp_Vec vec)
{
	gp_Trsf translation1;
	translation1.SetTranslation(vec);

	BRepBuilderAPI_Transform frontTrans(shape, translation1, true);

	return frontTrans.Shape();
}

TopoDS_Shape CModelingDoc::CreateHubFace(TopoDS_Edge extendedEdge, TopoDS_Edge translatedEdge)
{

	BRepOffsetAPI_ThruSections BTS1(Standard_False, Standard_False, 1.0e-10);
	BTS1.SetMaxDegree(2);

	TopoDS_Wire wire1 = BRepBuilderAPI_MakeWire(extendedEdge);
	TopoDS_Wire wire2 = BRepBuilderAPI_MakeWire(translatedEdge);
	BTS1.AddWire(wire1);
	BTS1.AddWire(wire2);

	BTS1.Build();
	TopoDS_Shape hubFace = BTS1.Shape();
	
	return hubFace;
}

TopoDS_Shape CModelingDoc::Rotating(Standard_Real arcWidth, TopoDS_Shape shape)
{	
	gp_Trsf theTransformation;
	gp_Ax1 axe = gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(0., 0., 1.));

	theTransformation.SetRotation(axe, arcWidth/2);
	BRepBuilderAPI_Transform myBRepTransformation(shape, theTransformation);
	TopoDS_Shape rotatedShape = myBRepTransformation.Shape();

	return rotatedShape;
}

TopoDS_Shape CModelingDoc::Revolving(TopoDS_Shape myShape)
{
	gp_Ax1 axe = gp_Ax1(gp_Pnt(0., 0., 0.), gp_Dir(0., 0., 1.));

	TopoDS_Shape RevShape = BRepPrimAPI_MakeRevol(myShape, axe, -5*M_PI / 180);

	return RevShape;
}

TopoDS_Face CModelingDoc::TopHubFace(TopoDS_Shape shape)
{
	int faceCounter = 0;
	TopoDS_Face topFace;

	for (TopExp_Explorer explr(shape, TopAbs_FACE); explr.More(); explr.Next())
	{
		topFace = TopoDS::Face(explr.Current());
		faceCounter++;
		if (faceCounter == 1)
		{
			return topFace;
		}
	}
	return topFace;
}

TopoDS_Edge CModelingDoc::BladeBaseEdges(TopoDS_Face le, TopoDS_Face te)
{
	int edgeCounter = 0;
	horEdgeArray2.clear();
	TopoDS_Edge tempEdge;

	for (TopExp_Explorer explr(le, TopAbs_EDGE); explr.More(); explr.Next())
	{
		tempEdge = TopoDS::Edge(explr.Current());
		BRepTools::Write(tempEdge, "edge2.brep");
		edgeCounter++;

		if (edgeCounter == 1)
		{
			horEdgeArray2.push_back(tempEdge);
		}
	}

	int edgeCounter1 = 0;
	for (TopExp_Explorer explr(te, TopAbs_EDGE); explr.More(); explr.Next())
	{
		tempEdge = TopoDS::Edge(explr.Current());
		BRepTools::Write(tempEdge, "edge2.brep");
		edgeCounter1++;

		if (edgeCounter1 == 1)
		{
			horEdgeArray2.push_back(tempEdge);
		}
	}
	
	return tempEdge;
}

TopoDS_Wire CModelingDoc::ConnectBladeBaseEdges()
{
	vector<TopoDS_Edge> bladeBase;
	bladeBase.clear();

	TopoDS_Wire wire;
	BRepLib_MakeWire wireMaker;
	bladeBase.push_back(horEdgeArray[0]);
	bladeBase.push_back(horEdgeArray2[0]);
	bladeBase.push_back(horEdgeArray[1]);
	bladeBase.push_back(horEdgeArray2[1]);
	
	for (int i = 0; i < bladeBase.size(); i++)
	{
		wireMaker.Add(bladeBase[i]);
	}
	wire = wireMaker.Wire();

	wireList.push_back(wire);
	return wire;
	
}

bool CModelingDoc::CheckForReverseWire(TopoDS_Face face, TopoDS_Wire wire)
{
	//Handle_Geom_Surface baseSurface = BRep_Tool::Surface(face);
	TopoDS_Face face1, face2;
	double surfArea1 = -1.0e-10;
	double surfArea2 = -1.0e-10;
	BRepBuilderAPI_MakeFace mkBaseFace1(face, wire);
	wire.Reverse();
	BRepBuilderAPI_MakeFace mkBaseFace2(face, wire);

	if (mkBaseFace1.IsDone())
	{
		face1 = mkBaseFace1.Face();
		BRepTools::Write(face1, "mkBaseFace1.brep");
		ShapeFix_Face fixFace(face1);
		fixFace.Perform();
		face1 = fixFace.Face();
		BRepTools::Write(face1, "face1.brep");
		GProp_GProps SProps1;
		BRepGProp::SurfaceProperties(face1, SProps1);
		surfArea1 = SProps1.Mass();
	}
	if (mkBaseFace2.IsDone())
	{
		face2 = mkBaseFace2.Face();
		BRepTools::Write(face1, "mkBaseFace2.brep");
		ShapeFix_Face fixFace(face2);
		fixFace.Perform();
		face2 = fixFace.Face();
		BRepTools::Write(face2, "face2.brep");
		GProp_GProps SProps2;
		BRepGProp::SurfaceProperties(face2, SProps2);
		surfArea2 = SProps2.Mass();
	}

	if (surfArea1 > surfArea2)
		return true;
	else
		return false;
}

TopoDS_Face CModelingDoc::CreateHoledFaceModified(TopoDS_Face inputFace, vector<TopoDS_Wire> &holeWireList)
{
	if (holeWireList.size() == 0) return inputFace;

	TopoDS_Face holedFace;
	holedFace.Nullify();
	try {
		OCC_CATCH_SIGNALS
		BRepBuilderAPI_MakeFace faceMaker(inputFace);
		//CADGeomBuilderInternal::WRITESHAPE(inputFace);
		for (auto &wire : holeWireList)
		{
			BRepTools::Write(wire, "holeWire.brep");
			ShapeFix_Wire fixWire(wire, inputFace, 1.0e-10);
			fixWire.Perform();
			TopoDS_Wire w1 = fixWire.Wire();
			BRepTools::Write(w1, "w1.brep");
			bool reverse = CheckForReverseWire(inputFace, w1);
			if (reverse)
				w1.Reverse();
			faceMaker.Add(w1);
		}
		faceMaker.Build();
		BRepTools::Write(faceMaker, "faceMaker.brep");
		if (faceMaker.IsDone())
		{
			ShapeFix_Face fixFace(faceMaker.Face());
			fixFace.Perform();
			holedFace = fixFace.Face();
			return holedFace;
		}

	}
	catch (...)
	{

	}
	return holedFace;
}

vector<TopoDS_Shape> CModelingDoc::FinalShapes(TopoDS_Shape shape, TopoDS_Face face, vector<TopoDS_Wire> wireList)
{
	vector<TopoDS_Shape> TotalShapes;

	TotalShapes.clear();
	FinalHubShapes.clear();
	FinalHubShapes.push_back(CreateHoledFaceModified(face, wireList));

	int faceCounter = 0;
	TopoDS_Face hubFaces;

	for (TopExp_Explorer explr(shape, TopAbs_FACE); explr.More(); explr.Next())
	{
		hubFaces = TopoDS::Face(explr.Current());
		faceCounter++;
		if (faceCounter == 2 | faceCounter == 3 | faceCounter == 4 | faceCounter == 5 | faceCounter == 6)
		{
			FinalHubShapes.push_back(hubFaces);
		}
	}
	for (int i = 0; i < FinalBladeShapes.size(); i++)
	{
		TotalShapes.push_back(FinalBladeShapes[i]);
	}
	for (int i = 0; i < FinalHubShapes.size(); i++)
	{
		TotalShapes.push_back(FinalHubShapes[i]);

	}

	return TotalShapes;
}

TopoDS_Shape CModelingDoc::makeVolumeShape(vector<TopoDS_Shape> faceList, double tolerance)
{
	BRepBuilderAPI_Sewing sewer;
	sewer.SetTolerance(tolerance);

	for (int i = 0; i < faceList.size(); i++)
	{
		sewer.Add(faceList[i]);
	}

	sewer.Perform();

	//extrat the shell
	TopoDS_Shell sewedShell;
	for (TopExp_Explorer Expl(sewer.SewedShape(), TopAbs_SHELL); Expl.More(); Expl.Next())
	{
		sewedShell = TopoDS::Shell(Expl.Current());
	}

	//return sewedShell;
	ShapeAnalysis_Shell SAS;
	SAS.LoadShells(sewedShell);
	if (SAS.HasFreeEdges()) //can't make a closed volume
		return sewedShell;

	BRepBuilderAPI_MakeSolid mkSolid(sewedShell);
	if (mkSolid.IsDone())
	{
		TopoDS_Solid sewedSolid = mkSolid.Solid();
		ShapeFix_Solid fixSolid(sewedSolid);
		fixSolid.Perform();
		TopoDS_Shape sewedShape = fixSolid.Solid();
		return sewedShape;
	}

	return sewedShell;

}

void CModelingDoc::OnCreateBox()
{
	GenerateBox1 dialogBox(this);
	dialogBox.DoModal();

}

void CModelingDoc::OnMirroraxis() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}
	TopoDS_Shape S = BRepPrimAPI_MakeWedge(60.,100.,80.,20.).Shape(); 
	Handle(AIS_Shape) ais1 = new AIS_Shape(S);
	myAISContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais1,Standard_False);

	gp_Trsf theTransformation;
	gp_Ax1 axe = gp_Ax1(gp_Pnt(110,60,60),gp_Dir(0.,1.,0.));
	Handle(Geom_Axis1Placement) Gax1 = new Geom_Axis1Placement(axe);
	Handle (AIS_Axis) ax1 = new AIS_Axis(Gax1);
	myAISContext->Display(ax1,Standard_False);
	theTransformation.SetMirror(axe);
	BRepBuilderAPI_Transform myBRepTransformation(S,theTransformation);
	TopoDS_Shape S2 = myBRepTransformation.Shape();
	Handle(AIS_Shape) ais2 = new AIS_Shape(S2);
	myAISContext->SetColor(ais2,Quantity_NOC_BLUE1,Standard_False); 
	myAISContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais2,Standard_False);
	Fit();

    TCollection_AsciiString Message ("\
\n\
TopoDS_Shape S = BRepBuilderAPI_MakeWedge(60.,100.,80.,20.); \n\
gp_Trsf theTransformation; \n\
gp_Ax1 Axis = gp_Ax1(gp_Pnt(110,60,60),gp_Dir(0.,1.,0.)); \n\
theTransformation.SetMirror(Axis);\n\
BRepBuilderAPI_Transform myBRepTransformation(S,theTransformation);\n\
TopoDS_Shape TransformedShape = myBRepTransformation.Shape();	\n");

	PocessTextInDialog("Transform a Shape with Mirror and One axis.", Message);

}

void CModelingDoc::OnRotate() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}
	TopoDS_Shape S = BRepPrimAPI_MakeWedge(60.,100.,80.,20.).Shape(); 
	Handle(AIS_Shape) ais1 = new AIS_Shape(S);
	myAISContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais1,Standard_False);
	gp_Trsf theTransformation;
	gp_Ax1 axe = gp_Ax1(gp_Pnt(200,60,60),gp_Dir(0.,1.,0.));
	Handle(Geom_Axis1Placement) Gax1 = new Geom_Axis1Placement(axe);
	Handle (AIS_Axis) ax1 = new AIS_Axis(Gax1);
	myAISContext->Display(ax1,Standard_False);
	theTransformation.SetRotation(axe,30*M_PI/180);
	BRepBuilderAPI_Transform myBRepTransformation(S,theTransformation);
	TopoDS_Shape S2 = myBRepTransformation.Shape();
	Handle(AIS_Shape) ais2 = new AIS_Shape(S2);
	myAISContext->SetColor(ais2,Quantity_NOC_BLUE1,Standard_False); 
	myAISContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais2,Standard_False);
	Fit();

    TCollection_AsciiString Message ("\
\n\
TopoDS_Shape S = BRepBuilderAPI_MakeWedge(60.,100.,80.,20.); \n\
gp_Trsf theTransformation; \n\
gp_Ax1 Axis = gp_Ax1(gp_Pnt(200,60,60),gp_Dir(0.,1.,0.)); \n\
theTransformation.SetRotation(Axis,30*PI/180); // Rotation of 30 degrees \n\
BRepBuilderAPI_Transform myBRepTransformation(S,theTransformation);\n\
TopoDS_Shape TransformedShape = myBRepTransformation.Shape();	\n");

	PocessTextInDialog("Transform a Shape with Rotation.", Message);
	
}

void CModelingDoc::OnScale() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}
	TopoDS_Shape S = BRepPrimAPI_MakeWedge(60.,100.,80.,20.).Shape(); 
	Handle(AIS_Shape) ais1 = new AIS_Shape(S);
	myAISContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais1,Standard_False);
	gp_Trsf theTransformation;
	gp_Pnt theCenterOfScale(200,60,60);
	Handle(AIS_Point) aispnt = new AIS_Point(new Geom_CartesianPoint(theCenterOfScale));

	myAISContext->Display(aispnt,Standard_False);
	theTransformation.SetScale(theCenterOfScale,0.5);
	BRepBuilderAPI_Transform myBRepTransformation(S,theTransformation);
	TopoDS_Shape S2 = myBRepTransformation.Shape();

	Handle(AIS_Shape) ais2 = new AIS_Shape(S2);
	myAISContext->SetColor(ais2,Quantity_NOC_BLUE1,Standard_False); 
	myAISContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais2,Standard_False);
	Fit();

    TCollection_AsciiString Message ("\
\n\
TopoDS_Shape S = BRepBuilderAPI_MakeWedge(60.,100.,80.,20.); \n\
gp_Trsf theTransformation; \n\
gp_Pnt theCenterOfScale(200,60,60); \n\
theTransformation.SetScale(theCenterOfScale,0.5); // Scale : value = 0.5 \n\
BRepBuilderAPI_Transform myBRepTransformation(S,theTransformation);\n\
TopoDS_Shape TransformedShape = myBRepTransformation.Shape();	\n");

	PocessTextInDialog("Scale a Shape with One point.", Message);
	

}

void CModelingDoc::OnTranslation() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}
	TopoDS_Shape S = BRepPrimAPI_MakeWedge(6.,10.,8.,2.).Shape(); 
	Handle(AIS_Shape) ais1 = new AIS_Shape(S);
	myAISContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);
	myAISContext->Display(ais1,Standard_False);
	gp_Trsf theTransformation;
	gp_Vec theVectorOfTranslation(-6,-6,6);

	Handle(ISession_Direction) aDirection1 = new ISession_Direction(gp_Pnt(0,0,0),theVectorOfTranslation);
	myAISContext->Display(aDirection1,Standard_False);

	theTransformation.SetTranslation(theVectorOfTranslation);
	BRepBuilderAPI_Transform myBRepTransformation(S,theTransformation);
	TopoDS_Shape S2 = myBRepTransformation.Shape();

	Handle(AIS_Shape) ais2 = new AIS_Shape(S2);
	myAISContext->SetColor(ais2,Quantity_NOC_BLUE1,Standard_False); 
	myAISContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais2,Standard_False);

	Fit();

    TCollection_AsciiString Message ("\
\n\
TopoDS_Shape S = BRepBuilderAPI_MakeWedge(6.,10.,8.,2.); \n\
gp_Trsf theTransformation; \n\
gp_Vec theVectorOfTranslation(6,6,6); \n\
theTransformation.SetTranslation(theVectorOfTranslation); \n\
BRepBuilderAPI_Transform myBRepTransformation(S,theTransformation);\n\
TopoDS_Shape TransformedShape = myBRepTransformation.Shape();	\n");

	PocessTextInDialog("Translate a Shape with One vector.", Message);
	
}

void CModelingDoc::OnDisplacement() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}
  TopoDS_Shape S = BRepPrimAPI_MakeWedge(60., 100., 80., 20.).Shape();
	Handle(AIS_Shape) ais1 = new AIS_Shape(S);
	myAISContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais1,Standard_False);
	gp_Trsf theTransformation;

	gp_Ax3 ax3_1(gp_Pnt(0,0,0),gp_Dir(0,0,1));
	gp_Ax3 ax3_2(gp_Pnt(60,60,60),gp_Dir(1,1,1));

	theTransformation.SetDisplacement(ax3_1,ax3_2);
	BRepBuilderAPI_Transform myBRepTransformation(S,theTransformation);
	TopoDS_Shape TransformedShape = myBRepTransformation.Shape();
	Handle(AIS_Shape) ais2 = new AIS_Shape(TransformedShape);
	myAISContext->SetColor(ais2,Quantity_NOC_BLUE1,Standard_False); 
	myAISContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais2,Standard_False);
	Fit();
    TCollection_AsciiString Message ("\
\n\
TopoDS_Shape S = BRepBuilderAPI_MakeWedge(60.,100.,80.,20.); \n\
gp_Trsf theTransformation; \n\
gp_Ax3 ax3_1(gp_Pnt(0,0,0),gp_Dir(0,0,1)); \n\
gp_Ax3 ax3_2(gp_Pnt(60,60,60),gp_Dir(1,1,1)); \n\
theTransformation.SetDisplacement(ax3_1,ax3_2); \n\
BRepBuilderAPI_Transform myBRepTransformation(S,theTransformation);\n\
TopoDS_Shape TransformedShape = myBRepTransformation.Shape();	\n");

	PocessTextInDialog("Displace a Shape with Two coordinate systems.", Message);
	
	
}


void CModelingDoc::OnDeform() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}
  TopoDS_Shape S = BRepPrimAPI_MakeWedge(60., 100., 80., 20.).Shape();
	Handle(AIS_Shape) ais1 = new AIS_Shape(S);
	myAISContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais1,Standard_False);

	gp_GTrsf theTransformation;
	gp_Mat rot(1, 0, 0, 0, 0.5, 0, 0, 0, 1.5);
	theTransformation.SetVectorialPart(rot);
	theTransformation.SetTranslationPart(gp_XYZ(5,5,5));

	BRepBuilderAPI_GTransform myBRepTransformation(S,theTransformation);
	TopoDS_Shape S2 = myBRepTransformation.Shape();

	Handle(AIS_Shape) ais2 = new AIS_Shape(S2);
	myAISContext->SetColor(ais2,Quantity_NOC_BLUE1,Standard_False); 
	myAISContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais2,Standard_False);
	Fit();
    TCollection_AsciiString Message ("\
\n\
TopoDS_Shape S = BRepBuilderAPI_MakeWedge(60.,100.,80.,20.); \n\
gp_GTrsf theTransformation; \n\
gp_Mat rot(1, 0, 0, 0, 0.5, 0, 0, 0, 1.5); // scaling : 100% on X ; 50% on Y ; 150% on Z . \n\
theTransformation.SetVectorialPart(rot); \n\
theTransformation.SetTranslationPart(gp_XYZ(5,5,5)); \n\
BRepBuilderAPI_GTransform myBRepGTransformation(S,theTransformation);\n\
TopoDS_Shape TransformedShape = myBRepGTransformation.Shape();	\n");

	PocessTextInDialog("Deform a Shape with One matrix of deformation and One translation.", Message);
}

/* =================================================================================
   ====================   P R I M I T I V E S   ====================================
   ================================================================================= */

void CModelingDoc::OnBox() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}
  TopoDS_Shape B1 = BRepPrimAPI_MakeBox(200., 150., 100.).Shape();
	Handle(AIS_Shape) aBox1 = new AIS_Shape(B1);
	myAISContext->SetMaterial(aBox1,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->SetColor(aBox1,Quantity_NOC_GREEN,Standard_False);	
	myAISContext->Display(aBox1,Standard_False);
	TopoDS_Shape B2 = BRepPrimAPI_MakeBox (gp_Ax2(gp_Pnt(-200.,-80.,-70.),
                                         gp_Dir(1.,2.,1.)),
                                         80., 90., 120.).Shape();
	Handle(AIS_Shape) aBox2 = new AIS_Shape(B2);
	myAISContext->SetMaterial(aBox2,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->SetColor(aBox2,Quantity_NOC_RED,Standard_False); 
	myAISContext->Display(aBox2,Standard_False);
	Fit();
    TCollection_AsciiString Message ("\
		\n\
TopoDS_Shape B1 = BRepPrimAPI_MakeBox (200.,150.,100.); \n\
TopoDS_Shape B2 = BRepPrimAPI_MakeBox (gp_Ax2(gp_Pnt(-200.,-80.,-70.), \n\
                                          gp_Dir(1.,2.,1.)), \n\
                                   80.,90.,120.); \n\
		\n");
	PocessTextInDialog("Make a topological box", Message);
}

void CModelingDoc::OnCylinder() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

  TopoDS_Shape C1 = BRepPrimAPI_MakeCylinder(50., 200.).Shape();
	Handle(AIS_Shape) aCyl1 = new AIS_Shape(C1);
	myAISContext->SetMaterial(aCyl1,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->SetColor(aCyl1,Quantity_NOC_RED,Standard_False); 
	myAISContext->Display(aCyl1,Standard_False);
	TopoDS_Shape C2 = BRepPrimAPI_MakeCylinder (gp_Ax2(gp_Pnt(200.,200.,0.),
												                      gp_Dir(0.,0.,1.)),
                                              40., 110., 210.*M_PI / 180).Shape();
	Handle(AIS_Shape) aCyl2 = new AIS_Shape(C2);
	myAISContext->SetMaterial(aCyl2,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->SetColor(aCyl2,Quantity_NOC_MATRABLUE,Standard_False); 	
	myAISContext->Display(aCyl2,Standard_False);
	Fit();

    TCollection_AsciiString Message ("\
		\n\
TopoDS_Shape C1 = BRepPrimAPI_MakeCylinder (50.,200.); \n\
TopoDS_Shape C2 = BRepPrimAPI_MakeCylinder (gp_Ax2(gp_Pnt(200.,200.,0.), \n\
                                        gp_Dir(0.,0.,1.)), \n\
                                        40.,110.,210.*PI180.); \n\
		\n");
	PocessTextInDialog("Make a cylinder", Message);
}


void CModelingDoc::OnCone() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}
  TopoDS_Shape C1 = BRepPrimAPI_MakeCone(50., 25., 200.).Shape();
	Handle(AIS_Shape) ais1 = new AIS_Shape(C1);
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->SetColor(ais1,Quantity_NOC_MATRABLUE,Standard_False); 		
	myAISContext->Display(ais1,Standard_False);
	TopoDS_Shape C2 = BRepPrimAPI_MakeCone(gp_Ax2(gp_Pnt(100.,100.,0.),
												                 gp_Dir(0.,0.,1.)),
                                         60., 0., 150., 210.*M_PI / 180).Shape();
	Handle(AIS_Shape) ais2 = new AIS_Shape(C2);
	myAISContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->SetColor(ais2,Quantity_NOC_GREEN,Standard_False); 
	myAISContext->Display(ais2,Standard_False);
	Fit();

    TCollection_AsciiString Message ("\
		\n\
TopoDS_Shape C1 = BRepPrimAPI_MakeCone (50.,25.,200.); \n\
TopoDS_Shape C2 = BRepPrimAPI_MakeCone(gp_Ax2(gp_Pnt(100.,100.,0.), \n\
                                          gp_Dir(0.,0.,1.)), \n\
                                   605.,0.,150.,210.*PI180); \n\
		\n");
	PocessTextInDialog("Make a cone", Message);
}

void CModelingDoc::OnSphere() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

  TopoDS_Shape S1 = BRepPrimAPI_MakeSphere(gp_Pnt(-200., -250., 0.), 80.).Shape();
	Handle(AIS_Shape) ais1 = new AIS_Shape(S1);
	myAISContext->SetColor(ais1,Quantity_NOC_AZURE,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(ais1,Standard_False);
  TopoDS_Shape S2 = BRepPrimAPI_MakeSphere(100., 120.*M_PI / 180).Shape();
	Handle(AIS_Shape) ais2 = new AIS_Shape(S2);
	myAISContext->SetColor(ais2,Quantity_NOC_GREEN,Standard_False); 
	myAISContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(ais2,Standard_False);
	TopoDS_Shape S3 = BRepPrimAPI_MakeSphere(gp_Pnt(200.,250.,0.),100.,
                                           -60.*M_PI / 180, 60.*M_PI / 180).Shape();
	Handle(AIS_Shape) ais3 = new AIS_Shape(S3);
	myAISContext->SetColor(ais3,Quantity_NOC_RED,Standard_False); 
	myAISContext->SetMaterial(ais3,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(ais3,Standard_False);
	TopoDS_Shape S4 = BRepPrimAPI_MakeSphere(gp_Pnt(0.,0.,-300.),150.,
                                           -45.*M_PI / 180, 45.*M_PI / 180, 45.*M_PI / 180).Shape();
	Handle(AIS_Shape) ais4 = new AIS_Shape(S4);
	myAISContext->SetColor(ais4,Quantity_NOC_MATRABLUE,Standard_False); 
	myAISContext->SetMaterial(ais4,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(ais4,Standard_False);
	Fit();

    TCollection_AsciiString Message ("\
		\n\
TopoDS_Shape S1 = BRepPrimAPI_MakeSphere(gp_Pnt(-200.,-250.,0.),80.); \n\
TopoDS_Shape S2 = BRepPrimAPI_MakeSphere(100.,120.*PI180); \n\
TopoDS_Shape S3 = BRepPrimAPI_MakeSphere(gp_Pnt(200.,250.,0.),100., \n\
                                     -60.*PI180, 60.*PI180); \n\
TopoDS_Shape S4 = BRepPrimAPI_MakeSphere(gp_Pnt(0.,0.,-300.),150., \n\
                                     -45.*PI180, 45.*PI180, 45.*PI180); \n\
		\n");
	PocessTextInDialog("Make a sphere", Message);
}

void CModelingDoc::OnTorus() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

  TopoDS_Shape S1 = BRepPrimAPI_MakeTorus(60., 20.).Shape();
	Handle(AIS_Shape) ais1 = new AIS_Shape(S1);
	myAISContext->SetColor(ais1,Quantity_NOC_AZURE,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(ais1,Standard_False);
	TopoDS_Shape S2 = BRepPrimAPI_MakeTorus(gp_Ax2(gp_Pnt(100.,100.,0.),gp_Dir(1.,1.,1.)),
                                          50., 20., 210.*M_PI / 180).Shape();
	Handle(AIS_Shape) ais2 = new AIS_Shape(S2);
	myAISContext->SetColor(ais2,Quantity_NOC_GREEN,Standard_False); 
	myAISContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(ais2,Standard_False);
	TopoDS_Shape S3 = BRepPrimAPI_MakeTorus(gp_Ax2(gp_Pnt(-200.,-150.,-100),gp_Dir(0.,1.,0.)),
                                          60., 20., -45.*M_PI / 180, 45.*M_PI / 180, 90.*M_PI / 180).Shape();
	Handle(AIS_Shape) ais3= new AIS_Shape(S3);
	myAISContext->SetColor(ais3,Quantity_NOC_CORAL,Standard_False); 
	myAISContext->SetMaterial(ais3,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(ais3,Standard_False);
	Fit();

    TCollection_AsciiString Message ("\
		\n\
TopoDS_Shape S1 = BRepPrimAPI_MakeTorus(60.,20.); \n\
TopoDS_Shape S2 = BRepPrimAPI_MakeTorus(gp_Ax2(gp_Pnt(100.,100.,0.),gp_Dir(1.,1.,1.)), \n\
                                    50.,20.,210.*PI180); \n\
TopoDS_Shape S3 = BRepPrimAPI_MakeTorus(gp_Ax2(gp_Pnt(-200.,-150.,-100),gp_Dir(0.,1.,0.)), \n\
                                    60.,20.,-45.*PI180,45.*PI180,90.*PI180); \n\
		\n");
	PocessTextInDialog("Make a torus", Message);
}

void CModelingDoc::OnWedge() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

  TopoDS_Shape S1 = BRepPrimAPI_MakeWedge(60., 100., 80., 20.).Shape();
	Handle(AIS_Shape) ais1 = new AIS_Shape(S1);
	myAISContext->SetColor(ais1,Quantity_NOC_AZURE,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(ais1,Standard_False);
	TopoDS_Shape S2 = BRepPrimAPI_MakeWedge(gp_Ax2(gp_Pnt(100.,100.,0.),gp_Dir(0.,0.,1.)),
                                          60., 50., 80., 25., -10., 40., 70.).Shape();
	Handle(AIS_Shape) ais2 = new AIS_Shape(S2);
	myAISContext->SetColor(ais2,Quantity_NOC_CORAL2,Standard_False); 
	myAISContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(ais2,Standard_False);
	Fit();

    TCollection_AsciiString Message ("\
		\n\
TopoDS_Shape S1 = BRepPrimAPI_MakeWedge(60.,100.,80.,20.); \n\
TopoDS_Shape S2 = BRepPrimAPI_MakeWedge(gp_Ax2(gp_Pnt(100.,100.,0.),gp_Dir(0.,0.,1.)), \n\
                                    60.,50.,80.,25.,-10.,40.,70.); \n\
		\n");
	PocessTextInDialog("Make a wedge", Message);
}

void CModelingDoc::OnPrism() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

	TopoDS_Vertex V1 = BRepBuilderAPI_MakeVertex(gp_Pnt(-200.,-200.,0.));
	Handle(AIS_Shape) ais1 = new AIS_Shape(V1);
	myAISContext->Display(ais1,Standard_False);
	TopoDS_Shape S1 = BRepPrimAPI_MakePrism(V1,gp_Vec(0.,0.,100.));
	Handle(AIS_Shape) ais2 = new AIS_Shape(S1);
	myAISContext->Display(ais2,Standard_False);

	TopoDS_Edge E = BRepBuilderAPI_MakeEdge(gp_Pnt(-150.,-150,0.), gp_Pnt(-50.,-50,0.));
	Handle(AIS_Shape) ais3 = new AIS_Shape(E);
	myAISContext->Display(ais3,Standard_False);
	TopoDS_Shape S2 = BRepPrimAPI_MakePrism(E,gp_Vec(0.,0.,100.));
	Handle(AIS_Shape) ais4 = new AIS_Shape(S2);
	myAISContext->SetColor(ais4,Quantity_NOC_CORAL2,Standard_False); 
	myAISContext->SetMaterial(ais4,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(ais4,Standard_False);

	TopoDS_Edge E1 = BRepBuilderAPI_MakeEdge(gp_Pnt(0.,0.,0.), gp_Pnt(50.,0.,0.));
	TopoDS_Edge E2 = BRepBuilderAPI_MakeEdge(gp_Pnt(50.,0.,0.), gp_Pnt(50.,50.,0.));
	TopoDS_Edge E3 = BRepBuilderAPI_MakeEdge(gp_Pnt(50.,50.,0.), gp_Pnt(0.,0.,0.));
	TopoDS_Wire W = BRepBuilderAPI_MakeWire(E1,E2,E3);
	TopoDS_Shape S3 = BRepPrimAPI_MakePrism(W,gp_Vec(0.,0.,100.));
	Handle(AIS_Shape) ais5 = new AIS_Shape(W);
	myAISContext->Display(ais5,Standard_False);
	Handle(AIS_Shape) ais6 = new AIS_Shape(S3);
	myAISContext->SetColor(ais6,Quantity_NOC_GREEN,Standard_False); 
	myAISContext->SetMaterial(ais6,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(ais6,Standard_False);

	gp_Circ c = gp_Circ(gp_Ax2(gp_Pnt(200.,200.,0.),gp_Dir(0.,0.,1.)), 80.);
	TopoDS_Edge Ec = BRepBuilderAPI_MakeEdge(c);
	TopoDS_Wire Wc = BRepBuilderAPI_MakeWire(Ec);
	TopoDS_Face F = BRepBuilderAPI_MakeFace(gp_Pln(gp::XOY()),Wc);
	Handle(AIS_Shape) ais7 = new AIS_Shape(F);
	myAISContext->Display(ais7,Standard_False);
	TopoDS_Shape S4 = BRepPrimAPI_MakePrism(F,gp_Vec(0.,0.,100.));
	Handle(AIS_Shape) ais8 = new AIS_Shape(S4);
	myAISContext->SetColor(ais8,Quantity_NOC_MATRABLUE,Standard_False); 
	myAISContext->SetMaterial(ais8,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(ais8,Standard_False);
	Fit();

    TCollection_AsciiString Message ("\
		\n\
--- Prism a vertex -> result is an edge --- \n\
\n\
TopoDS_Vertex V1 = BRepBuilderAPI_MakeVertex(gp_Pnt(-200.,-200.,0.)); \n\
TopoDS_Shape S1 = BRepBuilderAPI_MakePrism(V1,gp_Vec(0.,0.,100.)); \n\
\n\
--- Prism an edge -> result is a face --- \n\
\n\
TopoDS_Edge E = BRepBuilderAPI_MakeEdge(gp_Pnt(-150.,-150,0.), gp_Pnt(-50.,-50,0.)); \n\
TopoDS_Shape S2 = BRepPrimAPI_MakePrism(E,gp_Vec(0.,0.,100.)); \n\
\n\
--- Prism an wire -> result is a shell --- \n\
\n\
TopoDS_Edge E1 = BREpBuilderAPI_MakeEdge(gp_Pnt(0.,0.,0.), gp_Pnt(50.,0.,0.)); \n\
TopoDS_Edge E2 = BREpBuilderAPI_MakeEdge(gp_Pnt(50.,0.,0.), gp_Pnt(50.,50.,0.)); \n\
TopoDS_Edge E3 = BREpBuilderAPI_MakeEdge(gp_Pnt(50.,50.,0.), gp_Pnt(0.,0.,0.)); \n\
TopoDS_Wire W = BRepBuilderAPI_MakeWire(E1,E2,E3); \n\
TopoDS_Shape S3 = BRepPrimAPI_MakePrism(W,gp_Vec(0.,0.,100.)); \n\
\n\
--- Prism a face or a shell -> result is a solid --- \n\
\n\
gp_Circ c = gp_Circ(gp_Ax2(gp_Pnt(200.,200.,0.gp_Dir(0.,0.,1.)), 80.); \n\
TopoDS_Edge Ec = BRepBuilderAPI_MakeEdge(c); \n\
TopoDS_Wire Wc = BRepBuilderAPI_MakeWire(Ec); \n\
TopoDS_Face F = BRepBuilderAPI_MakeFace(gp::XOY(),Wc); \n\
TopoDS_Shape S4 = BRepBuilderAPI_MakePrism(F,gp_Vec(0.,0.,100.)); \n\
		\n");
	PocessTextInDialog("Make a prism", Message);
}

void CModelingDoc::OnRevol() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

	TopoDS_Vertex V1 = BRepBuilderAPI_MakeVertex(gp_Pnt(-200.,-200.,0.));
	Handle(AIS_Shape) ais1 = new AIS_Shape(V1);
	myAISContext->Display(ais1,Standard_False);
	gp_Ax1 axe = gp_Ax1(gp_Pnt(-170.,-170.,0.),gp_Dir(0.,0.,1.));
	Handle(Geom_Axis1Placement) Gax1 = new Geom_Axis1Placement(axe);
	Handle (AIS_Axis) ax1 = new AIS_Axis(Gax1);
	myAISContext->Display(ax1,Standard_False);
	TopoDS_Shape S1 = BRepPrimAPI_MakeRevol(V1,axe);
	Handle(AIS_Shape) ais2 = new AIS_Shape(S1);
	myAISContext->Display(ais2,Standard_False);

	TopoDS_Edge E = BRepBuilderAPI_MakeEdge(gp_Pnt(-120.,-120,0.), gp_Pnt(-120.,-120,100.));
	Handle(AIS_Shape) ais3 = new AIS_Shape(E);
	myAISContext->Display(ais3,Standard_False);
	axe = gp_Ax1(gp_Pnt(-100.,-100.,0.),gp_Dir(0.,0.,1.));
	Handle(Geom_Axis1Placement) Gax2 = new Geom_Axis1Placement(axe);
	Handle (AIS_Axis) ax2 = new AIS_Axis(Gax2);
	myAISContext->Display(ax2,Standard_False);
	TopoDS_Shape S2 = BRepPrimAPI_MakeRevol(E,axe);
	Handle(AIS_Shape) ais4 = new AIS_Shape(S2);
	myAISContext->SetColor(ais4,Quantity_NOC_YELLOW,Standard_False); 
	myAISContext->SetMaterial(ais4,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(ais4,Standard_False);

	TopoDS_Edge E1 = BRepBuilderAPI_MakeEdge(gp_Pnt(0.,0.,0.), gp_Pnt(50.,0.,0.));
	TopoDS_Edge E2 = BRepBuilderAPI_MakeEdge(gp_Pnt(50.,0.,0.), gp_Pnt(50.,50.,0.));
	TopoDS_Edge E3 = BRepBuilderAPI_MakeEdge(gp_Pnt(50.,50.,0.), gp_Pnt(0.,0.,0.));
	TopoDS_Wire W = BRepBuilderAPI_MakeWire(E1,E2,E3);
	axe = gp_Ax1(gp_Pnt(0.,0.,30.),gp_Dir(0.,1.,0.));
	Handle(Geom_Axis1Placement) Gax3 = new Geom_Axis1Placement(axe);
	Handle (AIS_Axis) ax3 = new AIS_Axis(Gax3);
	myAISContext->Display(ax3,Standard_False);
	TopoDS_Shape S3 = BRepPrimAPI_MakeRevol(W,axe, 210.*M_PI/180);
	Handle(AIS_Shape) ais5 = new AIS_Shape(W);
	myAISContext->Display(ais5,Standard_False);
	Handle(AIS_Shape) ais6 = new AIS_Shape(S3);
	myAISContext->SetColor(ais6,Quantity_NOC_GREEN,Standard_False); 
	myAISContext->SetMaterial(ais6,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(ais6,Standard_False);

	gp_Circ c = gp_Circ(gp_Ax2(gp_Pnt(200.,200.,0.),gp_Dir(0.,0.,1.)), 80.);
	TopoDS_Edge Ec = BRepBuilderAPI_MakeEdge(c);
	TopoDS_Wire Wc = BRepBuilderAPI_MakeWire(Ec);
	TopoDS_Face F = BRepBuilderAPI_MakeFace(gp_Pln(gp::XOY()),Wc);
	axe = gp_Ax1(gp_Pnt(290,290.,0.),gp_Dir(0.,1,0.));
	Handle(Geom_Axis1Placement) Gax4 = new Geom_Axis1Placement(axe);
	Handle (AIS_Axis) ax4 = new AIS_Axis(Gax4);
	myAISContext->Display(ax4,Standard_False);
	TopoDS_Shape S4 = BRepPrimAPI_MakeRevol(F,axe, 90.*M_PI/180);
	Handle(AIS_Shape) ais8 = new AIS_Shape(S4);
	myAISContext->SetColor(ais8,Quantity_NOC_MATRABLUE,Standard_False); 
	myAISContext->SetMaterial(ais8,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(ais8,Standard_False);
	Fit();

	TCollection_AsciiString Message ("\
		\n\
--- Revol of a vertex -> result is an edge --- \n\
\n\
TopoDS_Vertex V1 = BRepBuilderAPI_MakeVertex(gp_Pnt(-200.,-200.,0.)); \n\
gp_Ax1 axe = gp_Ax1(gp_Pnt(-170.,-170.,0.),gp_Dir(0.,0.,1.)); \n\
TopoDS_Shape S1 = BRepPrimAPI_MakeRevol(V1,axe); \n\
\n\
--- Revol of an edge -> result is a face --- \n\
\n\
TopoDS_Edge E = BRepBuilderAPI_MakeEdge(gp_Pnt(-120.,-120,0.), gp_Pnt(-120.,-120,100.)); \n\
axe = gp_Ax1(gp_Pnt(-100.,-100.,0.),gp_Dir(0.,0.,1.)); \n\
TopoDS_Shape S2 = BRepPrimAPI_MakeRevol(E,axe); \n\
\n\
--- Revol of a wire -> result is a shell --- \n\
\n\
TopoDS_Edge E1 = BRepBuilderAPI_MakeEdge(gp_Pnt(0.,0.,0.), gp_Pnt(50.,0.,0.)); \n\
TopoDS_Edge E2 = BRepBuilderAPI_MakeEdge(gp_Pnt(50.,0.,0.), gp_Pnt(50.,50.,0.)); \n\
TopoDS_Edge E3 = BRepBuilderAPI_MakeEdge(gp_Pnt(50.,50.,0.), gp_Pnt(0.,0.,0.)); \n\
TopoDS_Wire W = BRepBuilderAPI_MakeWire(E1,E2,E3); \n\
axe = gp_Ax1(gp_Pnt(0.,0.,30.),gp_Dir(0.,1.,0.)); \n\
TopoDS_Shape S3 = BRepPrimAPI_MakeRevol(W,axe, 210.*PI180); \n\
\n\
--- Revol of a face -> result is a solid --- \n\
\n\
gp_Circ c = gp_Circ(gp_Ax2(gp_Pnt(200.,200.,0.),gp_Dir(0.,0.,1.)), 80.); \n\
TopoDS_Edge Ec = BRepBuilderAPI_MakeEdge(c); \n\
TopoDS_Wire Wc = BRepBuilderPI_MakeWire(Ec); \n\
TopoDS_Face F = BRepBuilderAPI_MakeFace(gp_Pln(gp::XOY()),Wc); \n\
axe = gp_Ax1(gp_Pnt(290,290.,0.),gp_Dir(0.,1,0.)); \n\
TopoDS_Shape S4 = BRepPrimAPI_MakeRevol(F,axe, 90.*PI180); \n\
		\n");
  PocessTextInDialog("Make a revol", Message);
}

void CModelingDoc::OnPipe() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

	TColgp_Array1OfPnt CurvePoles(1,4);
	gp_Pnt pt = gp_Pnt(0.,0.,0.);
	CurvePoles(1) = pt;
	pt = gp_Pnt(20.,50.,0.);
	CurvePoles(2) = pt;
	pt = gp_Pnt(60.,100.,0.);
	CurvePoles(3) = pt;
	pt = gp_Pnt(150.,0.,0.);
	CurvePoles(4) = pt;
	Handle(Geom_BezierCurve) curve = new Geom_BezierCurve(CurvePoles);
	TopoDS_Edge E = BRepBuilderAPI_MakeEdge(curve);
	TopoDS_Wire W = BRepBuilderAPI_MakeWire(E);
	Handle(AIS_Shape) ais1 = new AIS_Shape(W);
	myAISContext->Display(ais1,Standard_False);
	Fit();
	Sleep(500);
	gp_Circ c = gp_Circ(gp_Ax2(gp_Pnt(0.,0.,0.),gp_Dir(0.,1.,0.)),10.);
	TopoDS_Edge Ec = BRepBuilderAPI_MakeEdge(c);
	TopoDS_Wire Wc = BRepBuilderAPI_MakeWire(Ec);
	Handle(AIS_Shape) ais3 = new AIS_Shape(Wc);
	myAISContext->Display(ais3,Standard_False);
	TopoDS_Face F = BRepBuilderAPI_MakeFace(gp_Pln(gp::ZOX()),Wc);
	TopoDS_Shape S = BRepOffsetAPI_MakePipe(W,F);
	Handle(AIS_Shape) ais2 = new AIS_Shape(S);
	myAISContext->SetColor(ais2,Quantity_NOC_MATRABLUE,Standard_False); 
	myAISContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais2,Standard_False);
	Fit();

	TCollection_AsciiString Message ("\
		\n\
TColgp_Array1OfPnt CurvePoles(1,6);\n\
gp_Pnt pt = gp_Pnt(0.,0.,0.);\n\
CurvePoles(1) = pt;\n\
pt = gp_Pnt(20.,50.,0.);\n\
CurvePoles(2) = pt;\n\
pt = gp_Pnt(60.,100.,0.);\n\
CurvePoles(3) = pt;\n\
pt = gp_Pnt(150.,0.,0.);\n\
CurvePoles(4) = pt;\n\
Handle(Geom_BezierCurve) curve = new Geom_BezierCurve(CurvePoles);\n\
TopoDS_Edge E = BRepBuilderAPI_MakeEdge(curve);\n\
TopoDS_Wire W = BRepBuilderAPI_MakeWire(E);\n\
gp_Circ c = gp_Circ(gp_Ax2(gp_Pnt(0.,0.,0.),gp_Dir(0.,1.,0.)),10.);\n\
TopoDS_Edge Ec = BRepBuilderAPI_MakeEdge(c);\n\
TopoDS_Wire Wc = BRepBuilderAPI_MakeWire(Ec);\n\
TopoDS_Face F = BRepBuilderAPI_MakeFace(gp_Pln(gp::ZOX()),Wc);\n\
TopoDS_Shape S = BRepBuilderAPI_MakePipe(W,F);\n\
		\n");
  PocessTextInDialog("Make a pipe", Message);

}

void CModelingDoc::OnThru() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

	gp_Circ c1 = gp_Circ(gp_Ax2(gp_Pnt(-100.,0.,-100.),gp_Dir(0.,0.,1.)),40.);
	TopoDS_Edge E1 = BRepBuilderAPI_MakeEdge(c1);
	TopoDS_Wire W1 = BRepBuilderAPI_MakeWire(E1);
	Handle(AIS_Shape) sec1 = new AIS_Shape(W1);
	myAISContext->Display(sec1,Standard_False);
	gp_Circ c2 = gp_Circ(gp_Ax2(gp_Pnt(-10.,0.,-0.),gp_Dir(0.,0.,1.)),40.);
	TopoDS_Edge E2 = BRepBuilderAPI_MakeEdge(c2);
	TopoDS_Wire W2 = BRepBuilderAPI_MakeWire(E2);
	Handle(AIS_Shape) sec2 = new AIS_Shape(W2);
	myAISContext->Display(sec2,Standard_False);	
	gp_Circ c3 = gp_Circ(gp_Ax2(gp_Pnt(-75.,0.,100.),gp_Dir(0.,0.,1.)),40.);
	TopoDS_Edge E3 = BRepBuilderAPI_MakeEdge(c3);
	TopoDS_Wire W3 = BRepBuilderAPI_MakeWire(E3);
	Handle(AIS_Shape) sec3 = new AIS_Shape(W3);
	myAISContext->Display(sec3,Standard_False);
	gp_Circ c4= gp_Circ(gp_Ax2(gp_Pnt(0.,0.,200.),gp_Dir(0.,0.,1.)),40.);
	TopoDS_Edge E4 = BRepBuilderAPI_MakeEdge(c4);
	TopoDS_Wire W4 = BRepBuilderAPI_MakeWire(E4);
	Handle(AIS_Shape) sec4 = new AIS_Shape(W4);
	myAISContext->Display(sec4,Standard_False);
	BRepOffsetAPI_ThruSections generator(Standard_False,Standard_True);
	generator.AddWire(W1);
	generator.AddWire(W2);
	generator.AddWire(W3);
	generator.AddWire(W4);
	generator.Build();
	TopoDS_Shape S1 = generator.Shape();
	Handle(AIS_Shape) ais1 = new AIS_Shape(S1);
	myAISContext->SetColor(ais1,Quantity_NOC_MATRABLUE,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais1,Standard_False);

	gp_Circ c1b = gp_Circ(gp_Ax2(gp_Pnt(100.,0.,-100.),gp_Dir(0.,0.,1.)),40.);
	TopoDS_Edge E1b = BRepBuilderAPI_MakeEdge(c1b);
	TopoDS_Wire W1b = BRepBuilderAPI_MakeWire(E1b);
	Handle(AIS_Shape) sec1b = new AIS_Shape(W1b);
	myAISContext->Display(sec1b,Standard_False);
	gp_Circ c2b = gp_Circ(gp_Ax2(gp_Pnt(210.,0.,-0.),gp_Dir(0.,0.,1.)),40.);
	TopoDS_Edge E2b = BRepBuilderAPI_MakeEdge(c2b);
	TopoDS_Wire W2b = BRepBuilderAPI_MakeWire(E2b);
	Handle(AIS_Shape) sec2b = new AIS_Shape(W2b);
	myAISContext->Display(sec2b,Standard_False);	
	gp_Circ c3b = gp_Circ(gp_Ax2(gp_Pnt(275.,0.,100.),gp_Dir(0.,0.,1.)),40.);
	TopoDS_Edge E3b = BRepBuilderAPI_MakeEdge(c3b);
	TopoDS_Wire W3b = BRepBuilderAPI_MakeWire(E3b);
	Handle(AIS_Shape) sec3b = new AIS_Shape(W3b);
	myAISContext->Display(sec3b,Standard_False);
	gp_Circ c4b= gp_Circ(gp_Ax2(gp_Pnt(200.,0.,200.),gp_Dir(0.,0.,1.)),40.);
	TopoDS_Edge E4b = BRepBuilderAPI_MakeEdge(c4b);
	TopoDS_Wire W4b = BRepBuilderAPI_MakeWire(E4b);
	Handle(AIS_Shape) sec4b = new AIS_Shape(W4b);
	myAISContext->Display(sec4b,Standard_False);
	BRepOffsetAPI_ThruSections generatorb(Standard_True,Standard_False);
	generatorb.AddWire(W1b);
	generatorb.AddWire(W2b);
	generatorb.AddWire(W3b);
	generatorb.AddWire(W4b);
	generatorb.Build();
	TopoDS_Shape S2 = generatorb.Shape();
	Handle(AIS_Shape) ais2 = new AIS_Shape(S2);
	myAISContext->SetColor(ais2,Quantity_NOC_ALICEBLUE,Standard_False); 
	myAISContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais2,Standard_False);
	Fit();

	TCollection_AsciiString Message ("\
		\n\
---------- ruled -------------- \n\
\n\
gp_Circ c1 = gp_Circ(gp_Ax2(gp_Pnt(-100.,0.,-100.),gp_Dir(0.,0.,1.)),40.);\n\
TopoDS_Edge E1 = BRepBuilderAPI_MakeEdge(c1);\n\
TopoDS_Wire W1 = BRepBuilderAPI_MakeWire(E1);\n\
gp_Circ c2 = gp_Circ(gp_Ax2(gp_Pnt(-10.,0.,-0.),gp_Dir(0.,0.,1.)),40.);\n\
TopoDS_Edge E2 = BRepBuilderAPI_MakeEdge(c2);\n\
TopoDS_Wire W2 = BRepBuilderAPI_MakeWire(E2);\n\
gp_Circ c3 = gp_Circ(gp_Ax2(gp_Pnt(-75.,0.,100.),gp_Dir(0.,0.,1.)),40.);\n\
TopoDS_Edge E3 = BRepBuilderAPI_MakeEdge(c3);\n\
TopoDS_Wire W3 = BRepBuilderAPI_MakeWire(E3);\n\
gp_Circ c4= gp_Circ(gp_Ax2(gp_Pnt(0.,0.,200.),gp_Dir(0.,0.,1.)),40.);\n\
TopoDS_Edge E4 = BRep>BuilderAPI_MakeEdge(c4);\n\
TopoDS_Edge E4 = BRepBuilderAPI_MakeEdge(c4);\n\
TopoDS_Edge E4 = BRepBuilderAPI_MakeEdge(c4);\n\
TopoDS_Wire W4 = BRepBuilderAPI_MakeWire(E4);\n\
BRepOffsetAPI_ThruSections generator(Standard_False,Standard_True);\n\
generator.AddWire(W1);\n\
generator.AddWire(W2);\n\
generator.AddWire(W3);\n\
generator.AddWire(W4);\n\
generator.Build();\n\
TopoDS_Shape S1 = generator.Shape();\n\
\n\
---------- smooth -------------- \n\
\n\
gp_Circ c1b = gp_Circ(gp_Ax2(gp_Pnt(100.,0.,-100.),gp_Dir(0.,0.,1.)),40.); \n\
TopoDS_Edge E1b = BRepBuilderAPI_MakeEdge(c1b); \n\
TopoDS_Wire W1b = BRepBuilderAPI_MakeWire(E1b); \n\
gp_Circ c2b = gp_Circ(gp_Ax2(gp_Pnt(210.,0.,-0.),gp_Dir(0.,0.,1.)),40.); \n\
TopoDS_Edge E2b = BRepBuilderAPI_MakeEdge(c2b);\n\
TopoDS_Wire W2b = BRepBuilderAPI_MakeWire(E2b); \n\
gp_Circ c3b = gp_Circ(gp_Ax2(gp_Pnt(275.,0.,100.),gp_Dir(0.,0.,1.)),40.);\n\
TopoDS_Edge E3b = BRepBuilderAPI_MakeEdge(c3b);\n\
TopoDS_Wire W3b = BRepBuilderAPI_MakeWire(E3b);\n\
gp_Circ c4b= gp_Circ(gp_Ax2(gp_Pnt(200.,0.,200.),gp_Dir(0.,0.,1.)),40.);\n\
TopoDS_Edge E4b = BRepBuilderAPI_MakeEdge(c4b);\n\
TopoDS_Wire W4b = BRepBuilderAPI_MakeWire(E4b);\n\
BRepOffsetAPI_ThruSections generatorb(Standard_True,Standard_False);\n\
generatorb.AddWire(W1b);\n\
generatorb.AddWire(W2b);\n\
generatorb.AddWire(W3b);\n\
generatorb.AddWire(W4b);\n\
generatorb.Build();\n\
TopoDS_Shape S2 = generatorb.Shape();\n\
		\n");
  PocessTextInDialog("Make a Thru sections", Message);

}

void CModelingDoc::OnEvolved() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

	BRepBuilderAPI_MakePolygon P;
	P.Add(gp_Pnt(0.,0.,0.));
	P.Add(gp_Pnt(200.,0.,0.));
	P.Add(gp_Pnt(200.,200.,0.));
	P.Add(gp_Pnt(0.,200.,0.));
	P.Add(gp_Pnt(0.,0.,0.));
	TopoDS_Wire W = P.Wire();
	
	Handle(AIS_Shape) ais1 = new AIS_Shape(W);
	myAISContext->Display(ais1,Standard_False);
	
	TopoDS_Wire wprof = BRepBuilderAPI_MakePolygon(gp_Pnt(0.,0.,0.),gp_Pnt(-60.,-60.,-200.));
	
	Handle(AIS_Shape) ais3 = new AIS_Shape(wprof);
	myAISContext->Display(ais3,Standard_False);
	Fit();
	Sleep(500);
	TopoDS_Shape S = BRepOffsetAPI_MakeEvolved(W,wprof,GeomAbs_Arc,Standard_True,Standard_False,Standard_True,0.0001);
	
	Handle(AIS_Shape) ais2 = new AIS_Shape(S);
	myAISContext->SetColor(ais2,Quantity_NOC_MATRABLUE,Standard_False); 
	myAISContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais2,Standard_False);
	Fit();
	
	TCollection_AsciiString Message ("\
		\n\
---------- Evolved shape -------------- \n\
\n\
BRepBuilderAPI_MakePolygon P;\n\
P.Add(gp_Pnt(0.,0.,0.));\n\
P.Add(gp_Pnt(200.,0.,0.));\n\
P.Add(gp_Pnt(200.,200.,0.));\n\
P.Add(gp_Pnt(0.,200.,0.));\n\
P.Add(gp_Pnt(0.,0.,0.));\n\
TopoDS_Wire W = P.Wire();\n\
TopoDS_Wire wprof = BRepBuilderAPI_MakePolygon(gp_Pnt(0.,0.,0.),gp_Pnt(-60.,-60.,-200.));\n\
TopoDS_Shape S = BRepBuilderAPI_MakeEvolved(W,wprof,GeomAbs_Arc,Standard_True,Standard_False,Standard_True,0.0001);\n\
		\n");
  PocessTextInDialog("Make an evolved shape", Message);

}

void CModelingDoc::OnDraft() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

  TopoDS_Shape S = BRepPrimAPI_MakeBox(200., 300., 150.).Shape();
	Handle(AIS_Shape) ais1 = new AIS_Shape(S);
	myAISContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais1,Standard_False);
	Fit();
	Sleep(500);
	BRepOffsetAPI_DraftAngle adraft(S);
	TopExp_Explorer Ex;
	for (Ex.Init(S,TopAbs_FACE); Ex.More(); Ex.Next()) {
		TopoDS_Face F = TopoDS::Face(Ex.Current());
		Handle(Geom_Plane) surf = Handle(Geom_Plane)::DownCast(BRep_Tool::Surface(F));
		gp_Pln apln = surf->Pln();
		gp_Dir dirF = apln.Axis().Direction();
		if (dirF.IsNormal(gp_Dir(0.,0.,1.),Precision::Angular()))
			adraft.Add(F, gp_Dir(0.,0.,1.), 15.*M_PI/180, gp_Pln(gp::XOY()));
	}
	ais1->Set(adraft.Shape());
	myAISContext->Redisplay(ais1,Standard_False);
	Fit();

	TCollection_AsciiString Message ("\
		\n\
---------- Tapered shape -------------- \n\
\n\
TopoDS_Shape S = BRepPrimAPI_MakeBox(200.,300.,150.);\n\
BRepOffsetAPI_DraftAngle adraft(S);\n\
TopExp_Explorer Ex;\n\
for (Ex.Init(S,TopAbs_FACE); Ex.More(); Ex.Next()) {\n\
	TopoDS_Face F = TopoDS::Face(Ex.Current());\n\
	Handle(Geom_Plane) surf = Handle(Geom_Plane)::DownCast(BRep_Tool::Surface(F));\n\
	gp_Pln apln = surf->Pln();\n\
	gp_Dir dirF = apln.Axis().Direction();\n\
	if (dirF.IsNormal(gp_Dir(0.,0.,1.),Precision::Angular()))\n\
		adraft.Add(F, gp_Dir(0.,0.,1.), 15.*PI180, gp_Pln(gp::XOY()));\n\
}\n\
		\n");
  PocessTextInDialog("Make a tapered shape", Message);

}

/* =================================================================================
   ====================   O P E R A T I O N S   ====================================
   ================================================================================= */

void CModelingDoc::OnCut() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

  TopoDS_Shape theBox = BRepPrimAPI_MakeBox(200, 60, 60).Shape();

Handle (AIS_Shape)	ais1 = new AIS_Shape(theBox);
myAISContext->SetDisplayMode(ais1,1,Standard_False);
myAISContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False);
myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);
myAISContext->Display(ais1,Standard_False);
const Handle(AIS_InteractiveObject)& anIO1 = ais1;
myAISContext->SetSelected (anIO1, Standard_False);
Fit();
Sleep(1000);


TopoDS_Shape theSphere = BRepPrimAPI_MakeSphere(gp_Pnt(100, 20, 20), 80).Shape();
Handle (AIS_Shape)	ais2 = new AIS_Shape(theSphere);
myAISContext->SetDisplayMode(ais2,1,Standard_False);
myAISContext->SetColor(ais2,Quantity_NOC_YELLOW,Standard_False);
myAISContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);
myAISContext->Display(ais2,Standard_False);
const Handle(AIS_InteractiveObject)& anIO2 = ais2;
myAISContext->SetSelected (anIO2, Standard_False);
Fit();
Sleep(1000);

TopoDS_Shape ShapeCut = BRepAlgoAPI_Cut(theSphere,theBox);

myAISContext->Erase(ais1,Standard_False);
myAISContext->Erase(ais2,Standard_False);

Handle (AIS_Shape)	aSection = new AIS_Shape(ShapeCut);
myAISContext->SetDisplayMode(aSection,1,Standard_False);
myAISContext->SetColor(aSection,Quantity_NOC_RED,Standard_False);
myAISContext->SetMaterial(aSection,Graphic3d_NOM_PLASTIC,Standard_False);
myAISContext->Display(aSection,Standard_False);
const Handle(AIS_InteractiveObject)& anIOSection = aSection;
myAISContext->SetSelected (anIOSection, Standard_False);
Fit();

    TCollection_AsciiString Message ("\
		\n\
TopoDS_Shape theBox = BRepPrimAPI_MakeBox(200,40,40); \n\
 \n\
TopoDS_Shape theSphere = BRepPrimAPI_MakeSphere(gp_Pnt(100,20,20),80); \n\
 \n\
TopoDS_Shape ShapeCut = BRepAlgoAPI_Cut(theSphere,theBox); \n\
 \n");
	PocessTextInDialog("Cut the sphere with a box", Message);


}










void CModelingDoc::OnFuse() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

gp_Pnt P(-5,5,-5);
TopoDS_Shape theBox1 = BRepPrimAPI_MakeBox(60, 200, 70).Shape();
Handle (AIS_Shape)	ais1 = new AIS_Shape(theBox1);
myAISContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False);
myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);
myAISContext->Display(ais1,Standard_False);
const Handle(AIS_InteractiveObject)& anIO1 = ais1;
myAISContext->SetSelected (anIO1, Standard_False);
Fit();
Sleep(1000);

TopoDS_Shape theBox2 = BRepPrimAPI_MakeBox(P, 20, 150, 110).Shape();
Handle (AIS_Shape)	ais2 = new AIS_Shape(theBox2);
myAISContext->SetColor(ais2,Quantity_NOC_YELLOW,Standard_False);
myAISContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);
myAISContext->Display(ais2,Standard_False);
const Handle(AIS_InteractiveObject)& anIO2 = ais2;
myAISContext->SetSelected (anIO2, Standard_False);
Fit();
Sleep(1000);

TopoDS_Shape FusedShape = BRepAlgoAPI_Fuse(theBox1,theBox2);

myAISContext->Erase(ais1,false);
myAISContext->Erase(ais2,false);

Handle (AIS_Shape)	aFusion = new AIS_Shape(FusedShape);
myAISContext->SetDisplayMode(aFusion,1,Standard_False);
myAISContext->SetColor(aFusion,Quantity_NOC_RED,Standard_False);
myAISContext->SetMaterial(aFusion,Graphic3d_NOM_PLASTIC,Standard_False);
myAISContext->Display(aFusion,Standard_False);
const Handle(AIS_InteractiveObject)& anIOFusion = aFusion;
myAISContext->SetSelected (anIOFusion, Standard_False);
myAISContext->UpdateCurrentViewer();

    TCollection_AsciiString Message ("\
		\n\
TopoDS_Shape theBox1 = BRepPrimAPI_MakeBox(50,200,70); \n\
 \n\
TopoDS_Shape theBox2 = BRepPrimAPI_MakeBox(-30,150,70); \n\
 \n\
TopoDS_Shape FusedShape = BRepAlgoAPI_Fuse(theBox1,theBox2); \n");
	PocessTextInDialog("Fuse the boxes", Message);

}

void CModelingDoc::OnCommon() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

gp_Ax2 axe(gp_Pnt(10,10,10),gp_Dir(1,2,1));
TopoDS_Shape theBox = BRepPrimAPI_MakeBox(axe, 60, 80, 100).Shape();

Handle(AIS_Shape) aboxshape=new AIS_Shape(theBox);
myAISContext->SetColor(aboxshape,Quantity_NOC_YELLOW,Standard_False);
myAISContext->SetMaterial(aboxshape,Graphic3d_NOM_PLASTIC,Standard_False);    
myAISContext->SetTransparency(aboxshape,0.2,Standard_False);
myAISContext->Display(aboxshape, AIS_Shaded, 0, Standard_False);
const Handle(AIS_InteractiveObject)& anIOBoxShape = aboxshape;
myAISContext->SetSelected (anIOBoxShape, Standard_False);
Fit();
Sleep(500);

TopoDS_Shape theWedge = BRepPrimAPI_MakeWedge(60., 100., 80., 20.).Shape();

Handle(AIS_Shape) awedge = new AIS_Shape(theWedge);
myAISContext->SetColor(awedge,Quantity_NOC_RED,Standard_False);
myAISContext->SetMaterial(awedge,Graphic3d_NOM_PLASTIC,Standard_False);    
myAISContext->SetTransparency(awedge,0.0,Standard_False);
myAISContext->Display(awedge,Standard_False);
const Handle(AIS_InteractiveObject)& anIOWedge = awedge;
myAISContext->SetSelected (anIOWedge, Standard_False);
myAISContext->UpdateCurrentViewer();
Sleep(500);

TopoDS_Shape theCommonSurface = BRepAlgoAPI_Common(theBox,theWedge);

myAISContext->Erase(aboxshape, false);
myAISContext->Erase(awedge, false);

Handle(AIS_Shape) acommon = new AIS_Shape(theCommonSurface);
myAISContext->SetColor(acommon,Quantity_NOC_GREEN,Standard_False); 
myAISContext->SetMaterial(acommon,Graphic3d_NOM_PLASTIC,Standard_False);    
myAISContext->Display (acommon, AIS_Shaded, 0,Standard_False);
const Handle(AIS_InteractiveObject)& anIOCommon = acommon;
myAISContext->SetSelected (anIOCommon, Standard_False);
myAISContext->UpdateCurrentViewer();

   TCollection_AsciiString Message ("\
		\n\
gp_Ax2 axe(gp_Pnt(10,10,10),gp_Dir(1,2,1)); \n\
 \n\
TopoDS_Shape theBox = BRepPrimAPI_MakeBox(axe,60,80,100); \n\
 \n\
TopoDS_Shape theWedge = BRepPrimAPI_MakeWedge(60.,100.,80.,20.); \n\
 \n\
TopoDS_Shape theCommonSurface = BRepAlgoAPI_Common(theBox,theWedge); \n\
 \n");

	PocessTextInDialog("Compute the common surface ", Message);

}

void CModelingDoc::OnSection() 
{

	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

  TopoDS_Shape atorus = BRepPrimAPI_MakeTorus(120, 20).Shape();

    Handle(AIS_Shape) ashape=new AIS_Shape(atorus);
    myAISContext->SetColor(ashape,Quantity_NOC_RED,Standard_False);
    myAISContext->SetMaterial(ashape,Graphic3d_NOM_PLASTIC,Standard_False);    
    myAISContext->SetDisplayMode(ashape,1,Standard_False);
	myAISContext->SetTransparency(ashape,0.1,Standard_False);
    myAISContext->Display(ashape,Standard_False);

gp_Vec V1(1,1,1);
Standard_Real radius = 120;
Standard_Integer i=-3;

for(i;i<=3;i++) {
    TopoDS_Shape asphere = BRepPrimAPI_MakeSphere(gp_Pnt(26 * 3 * i, 0, 0), radius).Shape();

    Handle (AIS_Shape) theShape=new AIS_Shape (asphere);
    myAISContext->SetTransparency(theShape,0.1,Standard_False);
    myAISContext->SetColor(theShape,Quantity_NOC_WHITE,Standard_False);
    myAISContext->SetDisplayMode(theShape,1,Standard_False);
    myAISContext->Display(theShape,Standard_False);
	Fit();

    Standard_Boolean PerformNow=Standard_False; 

    BRepAlgoAPI_Section section(atorus,asphere,PerformNow);
    section.ComputePCurveOn1(Standard_True);
    section.Approximation(TopOpeBRepTool_APPROX);
    section.Build();

    Handle(AIS_Shape) asection=new AIS_Shape(section.Shape());
    myAISContext->SetDisplayMode (asection, 0, Standard_False);
    myAISContext->SetColor (asection, Quantity_NOC_WHITE, Standard_False);
    myAISContext->Display (asection, Standard_False);
    if(i<3) {
    myAISContext->Remove (theShape, Standard_False);
	}
}
  myAISContext->UpdateCurrentViewer();
   TCollection_AsciiString Message ("\
		\n\
TopoDS_Shape atorus = BRepPrimAPI_MakeTorus(120,20); \n\
gp_Vec V1(1,1,1); \n\
Standard_Real radius = 120; \n\
Standard_Integer i=-3; \n\
for(i;i<=3;i++) { \n\
    TopoDS_Shape asphere = BRepPrimAPI_MakeSphere(gp_Pnt(78*i,0,0),radius); \n\
    Standard_Boolean PerformNow=Standard_False; \n\
    BRepAlgoAPI_Section section(atorus,asphere,PerformNow); \n\
    section.ComputePCurveOn1(Standard_True); \n\
    section.Approximation(TopOpeBRepTool_APPROX); \n\
    section.Build(); \n\
	\n");

	PocessTextInDialog("Compute the sections ", Message);

}

void CModelingDoc::OnPsection() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

TopoDS_Shape theTorus = BRepPrimAPI_MakeTorus(35, 8).Shape();
Handle(AIS_Shape) atorus = new AIS_Shape(theTorus);
myAISContext->SetColor(atorus,Quantity_NOC_YELLOW,Standard_False); 
myAISContext->SetMaterial(atorus,Graphic3d_NOM_PLASTIC,Standard_False);
myAISContext->SetTransparency(atorus,0.1,Standard_False);
myAISContext->Display(atorus,Standard_False);
const Handle(AIS_InteractiveObject)& anIOTorus = atorus;
myAISContext->SetSelected (anIOTorus, Standard_False);
Fit();
Sleep(500);

gp_Pln aplane(1,0.25,3,4);
Handle (Geom_Plane) thePlane = new Geom_Plane(aplane);
Handle (AIS_Plane) ais1 = new AIS_Plane(thePlane);
myAISContext->Display(ais1,Standard_False);
const Handle(AIS_InteractiveObject)& anIO1 = ais1;
myAISContext->SetSelected (anIO1, Standard_False);
Fit();
Sleep(300);

BRepAlgoAPI_Section section(theTorus,thePlane,Standard_False);
section.ComputePCurveOn1(Standard_True);
section.Approximation(TopOpeBRepTool_APPROX);
section.Build();

Handle(AIS_Shape) asection=new AIS_Shape(section.Shape());
myAISContext->SetDisplayMode(asection ,0,Standard_False);
myAISContext->SetColor(asection,Quantity_NOC_WHITE,Standard_False); 
myAISContext->Display(asection,Standard_False);
Fit();

   TCollection_AsciiString Message ("\
		\n\
TopoDS_Shape theTorus = BRepPrimAPI_MakeTorus(60.,20.); \n\
 \n\
gp_Pln P(1,2,1,-15); \n\
 \n\
TopoDS_Shape Psection = BRepAlgoAPI_Section(theTorus,P);  \n\
\n");

	PocessTextInDialog("Compute the plane section ", Message);

}

void CModelingDoc::OnBlend() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

TopoDS_Shape Box = BRepPrimAPI_MakeBox(gp_Pnt(-400,0,0),200,230,180).Shape();
Handle(AIS_Shape) ais1 = new AIS_Shape(Box);
myAISContext->SetColor(ais1,Quantity_NOC_YELLOW,Standard_False); 
myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False); 
myAISContext->Display(ais1,Standard_False);
const Handle(AIS_InteractiveObject)& anIO1 = ais1;
myAISContext->SetSelected (anIO1, Standard_False);
Fit();
Sleep(500);

BRepFilletAPI_MakeFillet fillet(Box);

for (TopExp_Explorer ex(Box,TopAbs_EDGE); ex.More(); ex.Next()) {
	TopoDS_Edge Edge =TopoDS::Edge(ex.Current());
	fillet.Add(20,Edge);
}

myAISContext->Remove(ais1,Standard_False);

TopoDS_Shape blendedBox = fillet.Shape();
Handle(AIS_Shape) aBlendbox = new AIS_Shape(blendedBox);
myAISContext->SetColor(aBlendbox,Quantity_NOC_YELLOW,Standard_False); 
myAISContext->SetMaterial(aBlendbox,Graphic3d_NOM_PLASTIC,Standard_False); 
myAISContext->Display(aBlendbox,Standard_False);
const Handle(AIS_InteractiveObject)& anIOBlendBox = aBlendbox;
myAISContext->SetSelected (anIOBlendBox, Standard_False);
Fit();
Sleep(500);


gp_Pnt P1(250,150,75);
TopoDS_Shape S1 = BRepPrimAPI_MakeBox(300, 200, 200).Shape();
TopoDS_Shape S2 = BRepPrimAPI_MakeBox(P1, 120, 180, 70).Shape();

TopoDS_Shape fusedShape = BRepAlgoAPI_Fuse(S1,S2);
Handle(AIS_Shape) ais2 = new AIS_Shape(fusedShape);
myAISContext->SetColor(ais2,Quantity_NOC_RED,Standard_False); 
myAISContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);  
myAISContext->Display(ais2,Standard_False);
const Handle(AIS_InteractiveObject)& anIO2 = ais2;
myAISContext->SetSelected (anIO2, Standard_False);
Fit();

BRepFilletAPI_MakeFillet fill(fusedShape);

for (TopExp_Explorer ex1(fusedShape,TopAbs_EDGE); ex1.More(); ex1.Next()) {
	TopoDS_Edge E =TopoDS::Edge(ex1.Current());
	fill.Add(E);
}

for (Standard_Integer i = 1;i<=fill.NbContours();i++) {
	Standard_Real longueur(fill.Length(i));
	Standard_Real Rad(0.15*longueur);
	fill.SetRadius(Rad,i, 1);
}

TopoDS_Shape blendedFusedSolids = fill.Shape();

Handle(AIS_Shape) aBlend = new AIS_Shape(blendedFusedSolids);
myAISContext->SetColor(aBlend,Quantity_NOC_RED,Standard_False); 
myAISContext->SetMaterial(aBlend,Graphic3d_NOM_PLASTIC,Standard_False);  
myAISContext->Display(aBlend,Standard_False);

myAISContext->Remove(ais2,Standard_False);
Fit();


 TCollection_AsciiString Message ("\
		\n\
//THE YELLOW BOX\n\
TopoDS_Shape Box = BRepPrimAPI_MakeBox(gp_Pnt(-400,0,0),200,230,180);\n\
\n\
BRepPrimAPI_MakeFillet fillet(Box);\n\
\n\
for (TopExp_Explorer ex(Box,TopAbs_EDGE); ex.More(); ex.Next()) {\n\
	TopoDS_Edge Edge =TopoDS::Edge(ex.Current());\n\
	fillet.Add(20,Edge);\n\
}\n\
TopoDS_Shape blendedBox = fillet.Shape();\n\
\n\
////////////////////////////////////////////////////////////////////////\n\
\n\
//THE RED SOLID\n\
 \n\
//Warning : On the acute angles of the boxes a fillet is created. \n\
On the angles of fusion a blend is created. \n\
\n\
gp_Pnt P1(150,150,75);\n\
TopoDS_Shape S1 = BRepPrimAPI_MakeBox(300,200,200);\n\
TopoDS_Shape S2 = BRepPrimAPI_MakeBox(P1,100,200,70);\n\
\n\
TopoDS_Shape fusedShape = BRepAlgoAPI_Fuse(S1,S2);\n\
BRepPrimAPI_MakeFillet fill(fusedShape);\n\
\n\
for (TopExp_Explorer ex1(fusedShape,TopAbs_EDGE); ex1.More(); ex1.Next()) {\n\
	TopoDS_Edge E =TopoDS::Edge(ex1.Current());\n\
	fill.Add(E);\n\
}\n\
\n\
for (Standard_Integer i = 1;i<=fill.NbContours();i++) {\n\
	Standard_Real longueur(fill.Length(i));\n\
	Standard_Real Rad(0.2*longueur);\n\
	fill.SetRadius(Rad,i);\n\
}\n\
\n\
TopoDS_Shape blendedFusedSolids = fill.Shape(); \n");

	PocessTextInDialog("Compute the blend on the edges ", Message);
}

void CModelingDoc::OnEvolvedblend() 
{

	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

TopoDS_Shape theBox = BRepPrimAPI_MakeBox(200, 200, 200).Shape();
Handle(AIS_Shape) ais1 = new AIS_Shape(theBox);
myAISContext->SetColor(ais1,Quantity_NOC_BROWN,Standard_False); 
myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False); 
myAISContext->Display(ais1,Standard_False);
const Handle(AIS_InteractiveObject)& anIO1 = ais1;
myAISContext->SetSelected (anIO1, Standard_False);
Fit();
Sleep(500);

BRepFilletAPI_MakeFillet Rake(theBox);

TopExp_Explorer ex(theBox,TopAbs_EDGE);
ex.Next();
ex.Next();
ex.Next();
ex.Next();
Rake.Add(8,50,TopoDS::Edge(ex.Current()));
Rake.Build();
if (Rake.IsDone() ){
	TopoDS_Shape evolvedBox = Rake.Shape();
	ais1->Set(evolvedBox);
	myAISContext->Redisplay(ais1,Standard_False);
	myAISContext->SetSelected(anIO1, Standard_False);
	Fit();
	Sleep(500);

}

TopoDS_Shape theCylinder = BRepPrimAPI_MakeCylinder(gp_Ax2(gp_Pnt(-300, 0, 0), gp::DZ()), 100, 200).Shape();
Handle(AIS_Shape) ais3 = new AIS_Shape(theCylinder);
myAISContext->SetColor(ais3,Quantity_NOC_GREEN,Standard_False); 
myAISContext->SetMaterial(ais3,Graphic3d_NOM_PLASTIC,Standard_False);    
myAISContext->Display(ais3,Standard_False);
const Handle(AIS_InteractiveObject)& anIO3 = ais3;
myAISContext->SetSelected (anIO3, Standard_False);
Fit();
Sleep(500);

BRepFilletAPI_MakeFillet fillet(theCylinder);

TColgp_Array1OfPnt2d TabPoint2(1,20);

for (Standard_Integer i=0; i<=19; i++) {
	gp_Pnt2d Point2d(i*2*M_PI/19,60*cos(i*M_PI/19-M_PI/2)+10);
	TabPoint2.SetValue(i+1,Point2d);
}

TopExp_Explorer exp2(theCylinder,TopAbs_EDGE);
fillet.Add(TabPoint2,TopoDS::Edge(exp2.Current()));
fillet.Build();
if (fillet.IsDone() ){
	TopoDS_Shape LawEvolvedCylinder = fillet.Shape();
	ais3->Set(LawEvolvedCylinder);
	myAISContext->Redisplay(ais3,Standard_False);
	myAISContext->SetSelected(anIO3,Standard_False);
	Fit();
	Sleep(500);
}

gp_Pnt P(350,0,0);
TopoDS_Shape theBox2 = BRepPrimAPI_MakeBox(P, 200, 200, 200).Shape();
Handle(AIS_Shape) ais2 = new AIS_Shape(theBox2);
myAISContext->SetColor(ais2,Quantity_NOC_RED,Standard_False); 
myAISContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);    
myAISContext->Display(ais2,Standard_False);
const Handle(AIS_InteractiveObject)& anIO2 = ais2;
myAISContext->SetSelected (anIO2, Standard_False);
Fit();
Sleep(500);


BRepFilletAPI_MakeFillet afillet(theBox2);

TColgp_Array1OfPnt2d TabPoint(1,6);

gp_Pnt2d P1(0.,8.);
gp_Pnt2d P2(0.2,16.);
gp_Pnt2d P3(0.4,25.);
gp_Pnt2d P4(0.6,55.);
gp_Pnt2d P5(0.8,28.);
gp_Pnt2d P6(1.,20.);
TabPoint.SetValue(1,P1);
TabPoint.SetValue(2,P2);
TabPoint.SetValue(3,P3);
TabPoint.SetValue(4,P4);
TabPoint.SetValue(5,P5);
TabPoint.SetValue(6,P6);

TopExp_Explorer exp(theBox2,TopAbs_EDGE);
exp.Next();
exp.Next();
exp.Next();
exp.Next();

afillet.Add(TabPoint, TopoDS::Edge(exp.Current()));

afillet.Build();
if (afillet.IsDone() ){
	TopoDS_Shape LawevolvedBox = afillet.Shape();
	ais2->Set(LawevolvedBox);
	myAISContext->Redisplay(ais2,Standard_False);
	myAISContext->SetSelected(anIO2,Standard_False);
	Fit();
	
}

   TCollection_AsciiString Message ("\
		\n\
//THE BROWN BOX \n\
\n\
TopoDS_Shape theBox = BRepPrimAPI_MakeBox(200,200,200);	\n\
	\n\
BRepPrimAPI_MakeFillet Rake(theBox);	\n\
ChFi3d_FilletShape FSh = ChFi3d_Rational;	\n\
Rake.SetFilletShape(FSh);	\n\
	\n\
TopExp_Explorer ex(theBox,TopAbs_EDGE);	\n\
ex.Next();   //in order to recover the front edge	\n\
ex.Next();	\n\
ex.Next();	\n\
ex.Next();	\n\
Rake.Add(8,50,TopoDS::Edge(ex.Current()));	\n\
 	\n\
Rake.Build();  \n\
if (Rake.IsDone() )  \n\
	TopoDS_Shape theBrownBox = Rake.Shape();	\n\
	\n\
//////////////////////////////////////////////////////////	\n\
	\n\
//THE GREEN CYLINDER	\n\
	\n\
TopoDS_Shape theCylinder = BRepPrimAPI_MakeCylinder(gp_Ax2(gp_Pnt(-300,0,0),gp_Dir(0,0,1)),100,200);	\n\
	\n\
BRepPrimAPI_MakeFillet fillet(theCylinder);	\n\
	\n\
TColgp_Array1OfPnt2d TabPoint2(1,20);	\n\
	\n\
for (Standard_Integer i=0; i<=19; i++) {	\n\
	gp_Pnt2d Point2d(i*2*PI/19,60*cos(i*PI/19-PI/2)+10);	\n\
	TabPoint2.SetValue(i+1,Point2d);	\n\
}	\n\
	\n\
TopExp_Explorer exp2(theCylinder,TopAbs_EDGE);	\n\
fillet.Add(TabPoint2,TopoDS::Edge(exp2.Current()));	\n\
 	\n\
fillet.Build();  \n\
if (fillet.IsDone() )  \n\
	TopoDS_Shape LawEvolvedCylinder = fillet.Shape();	\n\
	\n\
////////////////////////////////////////////////////////////	\n\
	\n\
	//THE RED BOX \n\
 \n\
gp_Pnt P(350,0,0);	\n\
TopoDS_Shape theBox2 = BRepPrimAPI_MakeBox(P,200,200,200);	\n\
	\n\
BRepPrimAPI_MakeFillet fill(theBox2);	\n\
	\n\
TColgp_Array1OfPnt2d TabPoint(1,6);	\n\
gp_Pnt2d P1(0,8);	\n\
gp_Pnt2d P2(0.2,16);	\n\
gp_Pnt2d P3(0.4,25);	\n\
gp_Pnt2d P4(0.6,55);	\n\
gp_Pnt2d P5(0.8,28);	\n\
gp_Pnt2d P6(1,20);	\n\
TabPoint.SetValue(1,P1);	\n\
TabPoint.SetValue(2,P2);	\n\
TabPoint.SetValue(3,P3);	\n\
TabPoint.SetValue(4,P4);	\n\
TabPoint.SetValue(5,P5);	\n\
TabPoint.SetValue(6,P6);	\n\
	\n\
TopExp_Explorer exp(theBox2,TopAbs_EDGE);	\n\
exp.Next();  //in order to trcover the front edge	\n\
exp.Next();	\n\
exp.Next();	\n\
exp.Next();	\n\
fill.Add(TabPoint,TopoDS::Edge(exp.Current()));	\n\
 	\n\
fill.Build();   \n\
if (fillet.IsDone() )  \n\
	TopoDS_Shape theRedBox = fill.Shape();	\n\
\n");

	PocessTextInDialog("Compute evolutiv blend on an edge ", Message);

}

void CModelingDoc::OnChamf() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

TopoDS_Shape theBox = BRepPrimAPI_MakeBox(60,200,70).Shape();
Handle(AIS_Shape) ais1 = new AIS_Shape(theBox);
myAISContext->SetColor(ais1,Quantity_NOC_YELLOW,Standard_False); 
myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);    
myAISContext->Display(ais1,Standard_False);
const Handle(AIS_InteractiveObject)& anIO1 = ais1;
myAISContext->SetSelected (anIO1, Standard_False);
Fit();
Sleep(500);

BRepFilletAPI_MakeChamfer MC(theBox);
// add all the edges to chamfer
TopTools_IndexedDataMapOfShapeListOfShape M;
TopExp::MapShapesAndAncestors(theBox,TopAbs_EDGE,TopAbs_FACE,M);
for (Standard_Integer i = 1;i<=M.Extent();i++) {
	TopoDS_Edge E = TopoDS::Edge(M.FindKey(i));
	TopoDS_Face F = TopoDS::Face(M.FindFromIndex(i).First());
	MC.Add(5,5,E,F);
	}

TopoDS_Shape ChanfrenedBox = MC.Shape();
Handle(AIS_Shape) aBlendedBox = new AIS_Shape(ChanfrenedBox);
myAISContext->SetColor(aBlendedBox,Quantity_NOC_YELLOW,Standard_False); 
myAISContext->SetMaterial(aBlendedBox,Graphic3d_NOM_PLASTIC,Standard_False);    
myAISContext->Display(aBlendedBox,Standard_False);
const Handle(AIS_InteractiveObject)& anIOBlendedBox = aBlendedBox;
myAISContext->SetSelected (anIOBlendedBox, Standard_False);
Fit();
Sleep(500);

myAISContext->Erase(ais1,Standard_True);

   TCollection_AsciiString Message ("\
		\n\
TopoDS_Shape theBox = BRepPrimAPI_MakeBox(130,200,170); \n\
BRepFilletAPI_MakeChamfer MC(theBox); \n\
TopTools_IndexedDataMapOfShapeListOfShape M; \n\
TopExp::MapShapesAndAncestors(theBox,TopAbs_EDGE,TopAbs_FACE,M); \n\
for (Standar1d_Integer i;i<M.Extent();i++) { \n\
	TopoDS_Edge E = TopoDS::Edge(M.FindKey(i)); \n\
	TopoDS_Face F = TopoDS::Face(M.FindFromIndex(i).First()); \n\
	MC.Add(15,15,E,F); \n\
	} \n\
TopoDS_Shape ChanfrenedBox = MC.Shape();  \n");

	PocessTextInDialog("Compute the chamfers on all the edges ", Message);
}

/* =================================================================================
   ====================   L O C A L   O P E R A T I O N S   ========================
   ================================================================================= */

void CModelingDoc::OnPrismLocal() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}
  TopoDS_Shape S = BRepPrimAPI_MakeBox(400., 250., 300.).Shape();
	Handle(AIS_Shape) ais1 = new AIS_Shape(S);

	myAISContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais1,Standard_False);
	const Handle(AIS_InteractiveObject)& anIO1 = ais1;
	myAISContext->SetSelected (anIO1, Standard_False);
	Fit();
	Sleep(500);

	TopExp_Explorer Ex;
	Ex.Init(S,TopAbs_FACE);
	Ex.Next();
	TopoDS_Face F = TopoDS::Face(Ex.Current());
	Handle(Geom_Surface) surf = BRep_Tool::Surface(F);
	Handle(Geom_Plane) Pl = Handle(Geom_Plane)::DownCast(surf);
	gp_Dir D = Pl->Pln().Axis().Direction();
// new in 2.0 ..use the trigonometric orientation to make the extrusion.
	D.Reverse();
	gp_Pnt2d p1,p2;
	Handle(Geom2d_Curve) aline;
	BRepBuilderAPI_MakeWire MW;
	p1 = gp_Pnt2d(200.,-100.);
	p2 = gp_Pnt2d(100.,-100.);
	aline = GCE2d_MakeLine(p1,p2).Value();
	MW.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));
	p1 = p2;
	p2 = gp_Pnt2d(100.,-200.);
	aline = GCE2d_MakeLine(p1,p2).Value();
	MW.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));
	p1 = p2;
	p2 = gp_Pnt2d(200.,-200.);
	aline = GCE2d_MakeLine(p1,p2).Value();
	MW.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));
	p1 = p2;
	p2 = gp_Pnt2d(200.,-100.);
	aline = GCE2d_MakeLine(p1,p2).Value();
	MW.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));
	BRepBuilderAPI_MakeFace MKF;
	MKF.Init(surf,Standard_False, Precision::Confusion());
	MKF.Add(MW.Wire());
	TopoDS_Shape FP = MKF.Face();
	BRepLib::BuildCurves3d(FP);
	BRepFeat_MakePrism MKP(S,FP,F,D,0,Standard_True);
	MKP.Perform(200.);
	TopoDS_Shape res1 = MKP.Shape();
	ais1->Set(res1);

	myAISContext->Redisplay(ais1,Standard_False);
	myAISContext->SetSelected(anIO1,Standard_False);
	Fit();
	Sleep(500);

	Ex.Next();
	TopoDS_Face F2 = TopoDS::Face(Ex.Current());
	surf = BRep_Tool::Surface(F2);
	Pl = Handle(Geom_Plane)::DownCast(surf);
	D = Pl->Pln().Axis().Direction();
	D.Reverse();
	BRepBuilderAPI_MakeWire MW2;
	p1 = gp_Pnt2d(100.,100.);
	p2 = gp_Pnt2d(200.,100.);
	aline = GCE2d_MakeLine(p1,p2).Value();
	MW2.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));
	p1 = p2;
	p2 = gp_Pnt2d(150.,200.);
	aline = GCE2d_MakeLine(p1,p2).Value();
	MW2.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));
	p1 = p2;
	p2 = gp_Pnt2d(100.,100.);
	aline = GCE2d_MakeLine(p1,p2).Value();
	MW2.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));
	BRepBuilderAPI_MakeFace MKF2;
	MKF2.Init(surf,Standard_False, Precision::Confusion());
	MKF2.Add(MW2.Wire());
	FP = MKF2.Face();
	BRepLib::BuildCurves3d(FP);
	BRepFeat_MakePrism MKP2(res1,FP,F2,D,1,Standard_True);
	MKP2.Perform(100.);
	TopoDS_Shape res2 = MKP2.Shape();
	ais1->Set(res2);

	myAISContext->Redisplay (ais1, Standard_False);
	myAISContext->SetSelected (anIO1, Standard_False);
	Fit();

	TCollection_AsciiString Message ("\
	\n\
--- Extrusion ---\n\
	\n\
TopoDS_Shape S = BRepPrimAPI_MakeBox(400.,250.,300.);\n\
TopExp_Explorer Ex;\n\
Ex.Init(S,TopAbs_FACE);\n\
Ex.Next();\n\
TopoDS_Face F = TopoDS::Face(Ex.Current());\n\
Handle(Geom_Surface) surf = BRep_Tool::Surface(F);\n\
Handle(Geom_Plane) Pl = Handle(Geom_Plane)::DownCast(surf);\n\
gp_Dir D = Pl->Pln().Axis().Direction();\n\
D.Reverse();\n\
gp_Pnt2d p1,p2;\n\
Handle(Geom2d_Curve) aline;\n\
BRepBuilderAPI_MakeWire MW;\n\
p1 = gp_Pnt2d(200.,-100.);\n\
p2 = gp_Pnt2d(100.,-100.);\n\
aline = GCE2d_MakeLine(p1,p2).Value();\n\
MW.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));\n\
p1 = p2;\n\
p2 = gp_Pnt2d(100.,-200.);\n\
aline = GCE2d_MakeLine(p1,p2).Value();\n\
MW.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));\n\
p1 = p2;\n\
p2 = gp_Pnt2d(200.,-200.);\n\
aline = GCE2d_MakeLine(p1,p2).Value();\n\
MW.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));\n\
p1 = p2;\n\
p2 = gp_Pnt2d(200.,-100.);\n\
aline = GCE2d_MakeLine(p1,p2).Value();\n\
MW.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));\n\
BRepBuilderAPI_MakeFace MKF;\n\
MKF.Init(surf,Standard_False);\n\
MKF.Add(MW.Wire());\n\
TopoDS_Shape FP = MKF.Face();\n\
BRepLib::BuildCurves3d(FP);\n\
BRepFeat_MakePrism MKP(S,FP,F,D,0,Standard_True);\n\
MKP.Perform(200);\n\
TopoDS_Shape res1 = MKP.Shape();\n\
	\n");
		Message += "\n\
--- Protrusion --- \n\
\n\
Ex.Next();\n\
TopoDS_Face F2 = TopoDS::Face(Ex.Current());\n\
surf = BRep_Tool::Surface(F2);\n\
Pl = Handle(Geom_Plane)::DownCast(surf);\n\
D = Pl->Pln().Axis().Direction();\n\
D.Reverse();\n\
BRepBuilderAPI_MakeWire MW2;\n\
p1 = gp_Pnt2d(100.,100.);\n\
p2 = gp_Pnt2d(200.,100.);\n\
aline = GCE2d_MakeLine(p1,p2).Value();\n\
MW2.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));\n\
p1 = p2;\n\
p2 = gp_Pnt2d(150.,200.);\n\
aline = GCE2d_MakeLine(p1,p2).Value();\n\
MW2.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));\n\
p1 = p2;\n\
p2 = gp_Pnt2d(100.,100.);\n\
aline = GCE2d_MakeLine(p1,p2).Value();\n\
MW2.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));\n\
BRepBuilderAPI_MakeFace MKF2;\n\
MKF2.Init(surf,Standard_False);\n\
MKF2.Add(MW2.Wire());\n\
FP = MKF2.Face();\n\
BRepLib::BuildCurves3d(FP);\n\
BRepFeat_MakePrism MKP2(res1,FP,F2,D,1,Standard_True);\n\
MKP2.Perform(100.);\n\
TopoDS_Shape res2 = MKP2.Shape();\n\
	\n";
	PocessTextInDialog("Make an extrusion or a protrusion", Message);
}

//
// BRepFeat_MakeDPrism
//
void CModelingDoc::OnDprismLocal() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

  TopoDS_Shape S = BRepPrimAPI_MakeBox(400., 250., 300.).Shape();
	Handle(AIS_Shape) ais1 = new AIS_Shape(S);

	myAISContext->SetColor(ais1,Quantity_NOC_RED,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
	
	TopExp_Explorer Ex;
	Ex.Init(S,TopAbs_FACE);
	Ex.Next();
	Ex.Next();
	Ex.Next();
	Ex.Next();
	Ex.Next();
	TopoDS_Face F = TopoDS::Face(Ex.Current());
	Handle(Geom_Surface) surf = BRep_Tool::Surface(F);
	gp_Circ2d c(gp_Ax2d(gp_Pnt2d(200.,130.),gp_Dir2d(1.,0.)),50.);
	BRepBuilderAPI_MakeWire MW;
	Handle(Geom2d_Curve) aline = new Geom2d_Circle(c);
	MW.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,M_PI));
	MW.Add(BRepBuilderAPI_MakeEdge(aline,surf,M_PI,2.*M_PI));
	BRepBuilderAPI_MakeFace MKF;
	MKF.Init(surf,Standard_False, Precision::Confusion());
	MKF.Add(MW.Wire());
	TopoDS_Face FP = MKF.Face();
	BRepLib::BuildCurves3d(FP);
	BRepFeat_MakeDPrism MKDP(S,FP,F,10*M_PI/180,1,Standard_True);
	MKDP.Perform(200);
	TopoDS_Shape res1 = MKDP.Shape();

	myAISContext->Display(ais1,Standard_False);
	const Handle(AIS_InteractiveObject)& anIO1 = ais1;
	myAISContext->SetSelected (anIO1, Standard_False);
	Fit();
	Sleep(500);

	ais1->Set(res1);
	
	myAISContext->Redisplay(ais1,Standard_False);
	myAISContext->SetSelected(anIO1,Standard_False);

	Fit();

	TCollection_AsciiString Message ("\
	\n\
--- Protrusion with draft angle --- \n\
	\n\
TopoDS_Shape S = BRepPrimAPI_MakeBox(400.,250.,300.);\n\
TopExp_Explorer Ex;\n\
Ex.Init(S,TopAbs_FACE);\n\
Ex.Next();\n\
Ex.Next();\n\
Ex.Next();\n\
Ex.Next();\n\
Ex.Next();\n\
TopoDS_Face F = TopoDS::Face(Ex.Current());\n\
Handle(Geom_Surface) surf = BRep_Tool::Surface(F);\n\
gp_Circ2d c(gp_Ax2d(gp_Pnt2d(200.,130.),gp_Dir2d(1.,0.)),50.);\n\
BRepBuilderAPI_MakeWire MW;\n\
Handle(Geom2d_Curve) aline = new Geom2d_Circle(c);\n\
MW.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,PI));\n\
MW.Add(BRepBuilderAPI_MakeEdge(aline,surf,PI,2.*PI));\n\
BRepBuilderAPI_MakeFace MKF;\n\
MKF.Init(surf,Standard_False);\n\
MKF.Add(MW.Wire());\n\
TopoDS_Face FP = MKF.Face();\n\
BRepLib::BuildCurves3d(FP);\n\
BRepFeat_MakeDPrism MKDP(S,FP,F,10*PI180,1,Standard_True);\n\
MKDP.Perform(200);\n\
TopoDS_Shape res1 = MKDP.Shape();\n\
	\n");
	PocessTextInDialog("Make an extrusion or a protrusion with a draft angle", Message);
}

void CModelingDoc::OnRevolLocal() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}
  TopoDS_Shape S = BRepPrimAPI_MakeBox(400., 250., 300.).Shape();

	Handle(AIS_Shape) ais1 = new AIS_Shape(S);
	
	myAISContext->SetColor(ais1,Quantity_NOC_CORAL,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais1,Standard_False);
	const Handle(AIS_InteractiveObject)& anIO1 = ais1;
	myAISContext->SetSelected (anIO1, Standard_False);
	Fit();
	Sleep(500);

	TopExp_Explorer Ex;
	Ex.Init(S,TopAbs_FACE);
	Ex.Next();
	Ex.Next();
	TopoDS_Face F1 = TopoDS::Face(Ex.Current());
	Handle(Geom_Surface) surf = BRep_Tool::Surface(F1);
	Handle (Geom_Plane) Pl = Handle(Geom_Plane)::DownCast(surf);
	gp_Ax1 D = gp::OX();
	BRepBuilderAPI_MakeWire MW1;
	gp_Pnt2d p1,p2;
	p1 = gp_Pnt2d(100.,100.);
	p2 = gp_Pnt2d(200.,100.);
	Handle(Geom2d_Line) aline = GCE2d_MakeLine(p1,p2).Value();
	MW1.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));
	p1 = p2;
	p2 = gp_Pnt2d(150.,200.);
	aline = GCE2d_MakeLine(p1,p2).Value();
	MW1.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));
	p1 = p2;
	p2 = gp_Pnt2d(100.,100.);
	aline = GCE2d_MakeLine(p1,p2).Value();
	MW1.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));
	BRepBuilderAPI_MakeFace MKF1;
	MKF1.Init(surf,Standard_False, Precision::Confusion());
	MKF1.Add(MW1.Wire());
	TopoDS_Face FP = MKF1.Face();
	BRepLib::BuildCurves3d(FP);
	BRepFeat_MakeRevol MKrev(S,FP,F1,D,1,Standard_True);
	Ex.Next();
	Ex.Next();
	TopoDS_Face F2 = TopoDS::Face(Ex.Current());
	MKrev.Perform(F2);
	TopoDS_Shape res1 = MKrev.Shape();


	myAISContext->Remove (ais1, Standard_False);
	Handle(AIS_Shape) ais2 = new AIS_Shape(res1);
	myAISContext->Display(ais2,Standard_False);
	const Handle(AIS_InteractiveObject)& anIO2 = ais2;
	myAISContext->SetSelected (anIO2, Standard_False);
	Fit();

	TCollection_AsciiString Message ("\
	\n\
TopoDS_Shape S = BRepPrimAPI_MakeBox(400.,250.,300.);\n\
TopExp_Explorer Ex;\n\
Ex.Init(S,TopAbs_FACE);\n\
Ex.Next();\n\
Ex.Next();\n\
TopoDS_Face F1 = TopoDS::Face(Ex.Current());\n\
Handle(Geom_Surface) surf = BRep_Tool::Surface(F1);\n\
Handle (Geom_Plane) Pl = Handle(Geom_Plane)::DownCast(surf);\n\
gp_Ax1 D = gp::OX();\n\
BRepBuilderAPI_MakeWire MW1;\n\
gp_Pnt2d p1,p2;\n\
p1 = gp_Pnt2d(100.,100.);\n\
p2 = gp_Pnt2d(200.,100.);\n\
Handle(Geom2d_Line) aline = GCE2d_MakeLine(p1,p2).Value();\n\
MW1.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));\n\
p1 = p2;\n\
p2 = gp_Pnt2d(150.,200.);\n\
aline = GCE2d_MakeLine(p1,p2).Value();\n\
MW1.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));\n\
p1 = p2;\n\
p2 = gp_Pnt2d(100.,100.);\n\
aline = GCE2d_MakeLine(p1,p2).Value();\n\
MW1.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));\n\
BRepBuilderAPI_MakeFace MKF1;\n\
MKF1.Init(surf,Standard_False);\n\
MKF1.Add(MW1.Wire());\n\
TopoDS_Face FP = MKF1.Face();\n\
BRepLib::BuildCurves3d(FP);\n\
BRepFeat_MakeRevol MKrev(S,FP,F1,D,1,Standard_True);\n\
Ex.Next();\n\
TopoDS_Face F2 = TopoDS::Face(Ex.Current());\n\
MKrev.Perform(F2);\n\
TopoDS_Shape res1 = MKrev.Shape();\n\
	\n");
  PocessTextInDialog("Make a local revolution", Message);
}

void CModelingDoc::OnGlueLocal() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}
  TopoDS_Shape S1 = BRepPrimAPI_MakeBox(gp_Pnt(-500., -500., 0.), gp_Pnt(-100., -250., 300.)).Shape();

	Handle(AIS_Shape) ais1 = new AIS_Shape(S1);
	myAISContext->SetColor(ais1,Quantity_NOC_ORANGE,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais1,Standard_False);
	const Handle(AIS_InteractiveObject)& anIO1 = ais1;
	myAISContext->SetSelected (anIO1, Standard_False);
	Fit();
	Sleep(1000);
	
	TopExp_Explorer Ex1;
	Ex1.Init(S1,TopAbs_FACE);
	Ex1.Next();
	Ex1.Next();
	Ex1.Next();
	Ex1.Next();
	Ex1.Next();
	TopoDS_Face F1 = TopoDS::Face(Ex1.Current());
  TopoDS_Shape S2 = BRepPrimAPI_MakeBox(gp_Pnt(-400., -400., 300.), gp_Pnt(-200., -300., 500.)).Shape();
	Handle(AIS_Shape) ais2 = new AIS_Shape(S2);

	myAISContext->SetColor(ais2,Quantity_NOC_AZURE,Standard_False); 
	myAISContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais2,Standard_False);
	const Handle(AIS_InteractiveObject)& anIO2 = ais2;
	myAISContext->SetSelected (anIO2, Standard_False);
	Fit();
	Sleep(1000);

	TopExp_Explorer Ex2;
	Ex2.Init(S2,TopAbs_FACE);
	Ex2.Next();
	Ex2.Next();
	Ex2.Next();
	Ex2.Next();
	TopoDS_Face F2 = TopoDS::Face(Ex2.Current());
	BRepFeat_Gluer glue(S2,S1);
	glue.Bind(F2,F1);
	TopoDS_Shape res1 = glue.Shape();
	myAISContext->Erase(ais2,Standard_False);
	
	ais1->Set(res1);

	myAISContext->Redisplay (ais1, Standard_False);
	myAISContext->SetSelected(anIO1,Standard_False);
	Fit();
	Sleep(1000);

  TopoDS_Shape S3 = BRepPrimAPI_MakeBox(500., 400., 300.).Shape();

	Handle(AIS_Shape) ais3 = new AIS_Shape(S3);
	myAISContext->SetColor(ais3,Quantity_NOC_ORANGE,Standard_False); 
	myAISContext->SetMaterial(ais3,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais3,Standard_False);
	const Handle(AIS_InteractiveObject)& anIO3 = ais3;
	myAISContext->SetSelected (anIO3, Standard_False);
	Fit();
	Sleep(1000);

	TopExp_Explorer Ex3;
	Ex3.Init(S3,TopAbs_FACE);
	Ex3.Next();
	Ex3.Next();
	Ex3.Next();
	Ex3.Next();
	Ex3.Next();
	TopoDS_Face F3 = TopoDS::Face(Ex3.Current());
  TopoDS_Shape S4 = BRepPrimAPI_MakeBox(gp_Pnt(0., 0., 300.), gp_Pnt(200., 200., 500.)).Shape();

	Handle(AIS_Shape) ais4 = new AIS_Shape(S4);
	myAISContext->SetColor(ais4,Quantity_NOC_AZURE,Standard_False); 
	myAISContext->SetMaterial(ais4,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais4,Standard_False);
	const Handle(AIS_InteractiveObject)& anIO4 = ais4;
	myAISContext->SetSelected (anIO4, Standard_False);
	Fit();
	Sleep(1000);

	TopExp_Explorer Ex4;
	Ex4.Init(S4,TopAbs_FACE);
	Ex4.Next();
	Ex4.Next();
	Ex4.Next();
	Ex4.Next();
	TopoDS_Face F4 = TopoDS::Face(Ex4.Current());
	BRepFeat_Gluer glue2(S4,S3);
	glue2.Bind(F4,F3);
	LocOpe_FindEdges CommonEdges(F4,F3);
	for (CommonEdges.InitIterator(); CommonEdges.More(); CommonEdges.Next()) 
		glue2.Bind(CommonEdges.EdgeFrom(),CommonEdges.EdgeTo());
	TopoDS_Shape res2 = glue2.Shape();
	myAISContext->Erase(ais3,Standard_False);
	
	ais4->Set(res2);

	myAISContext->Redisplay(ais4,Standard_False);
	myAISContext->SetSelected(anIO4,Standard_False);
	Fit();
	Sleep(1000);

	TCollection_AsciiString Message ("\
	\n\
--- Without common edges ---\n\
	\n\
TopoDS_Shape S1 = BRepPrimAPI_MakeBox(gp_Pnt(-500.,-500.,0.),gp_Pnt(-100.,-250.,300.));\n\
TopExp_Explorer Ex1;\n\
Ex1.Init(S1,TopAbs_FACE);\n\
Ex1.Next();\n\
Ex1.Next();\n\
Ex1.Next();\n\
Ex1.Next();\n\
Ex1.Next();\n\
TopoDS_Face F1 = TopoDS::Face(Ex1.Current());\n\
TopoDS_Shape S2 = BRepPrimAPI_MakeBox(gp_Pnt(-400.,-400.,300.),gp_Pnt(-200.,-300.,500.));\n\
TopExp_Explorer Ex2;\n\
Ex2.Init(S2,TopAbs_FACE);\n\
Ex2.Next();\n\
Ex2.Next();\n\
Ex2.Next();\n\
Ex2.Next();\n\
TopoDS_Face F2 = TopoDS::Face(Ex2.Current());\n\
BRepFeat_Gluer glue(S2,S1);\n\
glue.Bind(F2,F1);\n\
TopoDS_Shape res1 = glue.Shape();\n\
\n\
--- With common edges ---\n\
\n\
TopoDS_Shape S3 = BRepPrimAPI_MakeBox(500.,400.,300.);\n\
TopExp_Explorer Ex3;\n\
Ex3.Init(S3,TopAbs_FACE);\n\
Ex3.Next();\n\
Ex3.Next();\n\
Ex3.Next();\n\
Ex3.Next();\n\
Ex3.Next();\n\
TopoDS_Face F3 = TopoDS::Face(Ex3.Current());\n\
TopoDS_Shape S4 = BRepPrimAPI_MakeBox(gp_Pnt(0.,0.,300.),gp_Pnt(200.,200.,500.));\n\
TopExp_Explorer Ex4;\n\
Ex4.Init(S4,TopAbs_FACE);\n\
Ex4.Next();\n\
Ex4.Next();\n\
Ex4.Next();\n\
Ex4.Next();\n\
TopoDS_Face F4 = TopoDS::Face(Ex4.Current());\n\
BRepFeat_Gluer glue2(S4,S3);\n\
glue2.Bind(F4,F3);\n\
LocOpe_FindEdges CommonEdges(F4,F3);\n\
for (CommonEdges.InitIterator(); CommonEdges.More(); CommonEdges.Next()) \n\
	glue2.Bind(CommonEdges.EdgeFrom(),CommonEdges.EdgeTo());\n\
TopoDS_Shape res2 = glue2.Shape();\n\
	\n");
  PocessTextInDialog("Glue two solids", Message);
}


void CModelingDoc::OnPipeLocal() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}
  TopoDS_Shape S = BRepPrimAPI_MakeBox(400., 250., 300.).Shape();
	Handle(AIS_Shape) ais1 = new AIS_Shape(S);

	myAISContext->SetColor(ais1,Quantity_NOC_CORAL,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais1,Standard_False);
	const Handle(AIS_InteractiveObject)& anIO1 = ais1;
	myAISContext->SetSelected (anIO1, Standard_False);
	Fit();
	Sleep(500);

	TopExp_Explorer Ex;
	Ex.Init(S,TopAbs_FACE);
	Ex.Next();
	Ex.Next();
	TopoDS_Face F1 = TopoDS::Face(Ex.Current());
	Handle(Geom_Surface) surf = BRep_Tool::Surface(F1);
	BRepBuilderAPI_MakeWire MW1;
	gp_Pnt2d p1,p2;
	p1 = gp_Pnt2d(100.,100.);
	p2 = gp_Pnt2d(200.,100.);
	Handle(Geom2d_Line) aline = GCE2d_MakeLine(p1,p2).Value();
	MW1.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));
	p1 = p2;
	p2 = gp_Pnt2d(150.,200.);
	aline = GCE2d_MakeLine(p1,p2).Value();
	MW1.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));
	p1 = p2;
	p2 = gp_Pnt2d(100.,100.);
	aline = GCE2d_MakeLine(p1,p2).Value();
	MW1.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));
	BRepBuilderAPI_MakeFace MKF1;
	MKF1.Init(surf,Standard_False, Precision::Confusion());
	MKF1.Add(MW1.Wire());
	TopoDS_Face FP = MKF1.Face();
	BRepLib::BuildCurves3d(FP);
	TColgp_Array1OfPnt CurvePoles(1,3);
	gp_Pnt pt = gp_Pnt(150.,0.,150.);
	CurvePoles(1) = pt;
	pt = gp_Pnt(200.,-100.,150.);
	CurvePoles(2) = pt;
	pt = gp_Pnt(150.,-200.,150.);
	CurvePoles(3) = pt;
	Handle(Geom_BezierCurve) curve = new Geom_BezierCurve(CurvePoles);
	TopoDS_Edge E = BRepBuilderAPI_MakeEdge(curve);
	TopoDS_Wire W = BRepBuilderAPI_MakeWire(E);
	BRepFeat_MakePipe MKPipe(S,FP,F1,W,1,Standard_True);
	MKPipe.Perform();
	TopoDS_Shape res1 = MKPipe.Shape();
	ais1->Set(res1);

	myAISContext->Redisplay(ais1,Standard_False); 
	myAISContext->SetSelected(anIO1,Standard_False);
	Fit();
	
	TCollection_AsciiString Message ("\
	\n\
TopoDS_Shape S = BRepPrimAPI_MakeBox(400.,250.,300.);\n\
TopExp_Explorer Ex;\n\
Ex.Init(S,TopAbs_FACE);\n\
Ex.Next();\n\
Ex.Next();\n\
TopoDS_Face F1 = TopoDS::Face(Ex.Current());\n\
Handle(Geom_Surface) surf = BRep_Tool::Surface(F1);\n\
BRepBuilderAPI_MakeWire MW1;\n\
gp_Pnt2d p1,p2;\n\
p1 = gp_Pnt2d(100.,100.);\n\
p2 = gp_Pnt2d(200.,100.);\n\
Handle(Geom2d_Line) aline = GCE2d_MakeLine(p1,p2).Value();\n\
MW1.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));\n\
p1 = p2;\n\
p2 = gp_Pnt2d(150.,200.);\n\
aline = GCE2d_MakeLine(p1,p2).Value();\n\
MW1.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));\n\
p1 = p2;\n\
p2 = gp_Pnt2d(100.,100.);\n\
aline = GCE2d_MakeLine(p1,p2).Value();\n\
MW1.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));\n\
BRepBuilderAPI_MakeFace MKF1;\n\
MKF1.Init(surf,Standard_False);\n\
TopoDS_Face FP = MKF1.Face();\n\
BRepLib::BuildCurves3d(FP);\n\
TColgp_Array1OfPnt CurvePoles(1,3);\n\
gp_Pnt pt = gp_Pnt(150.,0.,150.);\n\
CurvePoles(1) = pt;\n\
pt = gp_Pnt(200.,-100.,150.);\n\
CurvePoles(2) = pt;\n\
pt = gp_Pnt(150.,-200.,150.);\n\
CurvePoles(3) = pt;\n\
Handle(Geom_BezierCurve) curve = new Geom_BezierCurve(CurvePoles);\n\
TopoDS_Edge E = BRepBuilderAPI_MakeEdge(curve);\n\
TopoDS_Wire W = BRepBuilderAPI_MakeWire(E);\n\
BRepFeat_MakePipe MKPipe(S,FP,F1,W,1,Standard_True);\n\
MKPipe.Perform();\n\
TopoDS_Shape res1 = MKPipe.Shape();\n\
	\n");
  PocessTextInDialog("Make a local pipe", Message);
}


void CModelingDoc::OnLinearLocal() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}
	BRepBuilderAPI_MakeWire mkw;
	gp_Pnt p1 = gp_Pnt(0.,0.,0.);
	gp_Pnt p2 = gp_Pnt(200.,0.,0.);
	mkw.Add(BRepBuilderAPI_MakeEdge(p1,p2));
	p1 = p2;
	p2 = gp_Pnt(200.,0.,50.);
	mkw.Add(BRepBuilderAPI_MakeEdge(p1,p2));
	p1 = p2;
	p2 = gp_Pnt(50.,0.,50.);
	mkw.Add(BRepBuilderAPI_MakeEdge(p1,p2));
	p1 = p2;
	p2 = gp_Pnt(50.,0.,200.);
	mkw.Add(BRepBuilderAPI_MakeEdge(p1,p2));
	p1 = p2;
	p2 = gp_Pnt(0.,0.,200.);
	mkw.Add(BRepBuilderAPI_MakeEdge(p1,p2));
	p1 = p2;
	mkw.Add(BRepBuilderAPI_MakeEdge(p2,gp_Pnt(0.,0.,0.)));
	
	TopoDS_Shape S = BRepPrimAPI_MakePrism(BRepBuilderAPI_MakeFace(mkw.Wire()), 
									   gp_Vec(gp_Pnt(0.,0.,0.),gp_Pnt(0.,100.,0.)));

	Handle(AIS_Shape) ais1 = new AIS_Shape(S);
	myAISContext->SetColor(ais1,Quantity_NOC_CYAN2,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais1,Standard_False);
	const Handle(AIS_InteractiveObject)& anIO1 = ais1;
	myAISContext->SetSelected (anIO1, Standard_False);
	Fit();
	Sleep(500);

	TopoDS_Wire W = BRepBuilderAPI_MakeWire(BRepBuilderAPI_MakeEdge(gp_Pnt(50.,45.,100.),
													  gp_Pnt(100.,45.,50.)));	
	Handle(Geom_Plane) aplane = new Geom_Plane(0.,1.,0.,-45.);
	BRepFeat_MakeLinearForm aform(S, W, aplane, gp_Vec(0.,10.,0.), gp_Vec(0.,0.,0.),
								  1, Standard_True);
	aform.Perform(/*10.*/); // new in 2.0

	TopoDS_Shape res = aform.Shape();
	ais1->Set(res);
	myAISContext->Redisplay(ais1,Standard_False);
	myAISContext->SetSelected (anIO1, Standard_False);
	Fit();

	TCollection_AsciiString Message ("\
	\n\
BRepBuilderAPI_MakeWire mkw;\n\
gp_Pnt p1 = gp_Pnt(0.,0.,0.);\n\
gp_Pnt p2 = gp_Pnt(200.,0.,0.);\n\
mkw.Add(BRepBuilderAPI_MakeEdge(p1,p2));\n\
p1 = p2;\n\
p2 = gp_Pnt(200.,0.,50.);\n\
mkw.Add(BRepBuilderAPI_MakeEdge(p1,p2));\n\
p1 = p2;\n\
p2 = gp_Pnt(50.,0.,50.);\n\
mkw.Add(BRepBuilderAPI_MakeEdge(p1,p2));\n\
p1 = p2;\n\
p2 = gp_Pnt(50.,0.,200.);\n\
mkw.Add(BRepBuilderAPI_MakeEdge(p1,p2));\n\
p1 = p2;\n\
p2 = gp_Pnt(0.,0.,200.);\n\
mkw.Add(BRepBuilderAPI_MakeEdge(p1,p2));\n\
p1 = p2;\n\
mkw.Add(BRepBuilderAPI_MakeEdge(p2,gp_Pnt(0.,0.,0.)));\n\
TopoDS_Shape S = BRepPrimAPI_MakePrism(BRepBuilderAPI_MakeFace(mkw.Wire()), \n\
								gp_Vec(gp_Pnt(0.,0.,0.),gp_Pnt(0.,100.,0.)));\n\
TopoDS_Wire W = BRepBuilderAPI_MakeWire(BRepBuilderAPI_MakeEdge(gp_Pnt(50.,45.,100.),\n\
												gp_Pnt(100.,45.,50.)));\n\
Handle(Geom_Plane) aplane = new Geom_Plane(0.,1.,0.,-45.);\n\
BRepFeat_MakeLinearForm aform(S, W, aplane, gp_Dir(0.,10.,0.), gp_Dir(0.,0.,0.),\n\
							1, Standard_True);\n\
aform.Perform(10.);\n\
TopoDS_Shape res = aform.Shape();\n\
	\n");
  PocessTextInDialog("Make a rib", Message);
}


void CModelingDoc::OnSplitLocal() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

  TopoDS_Shape S = BRepPrimAPI_MakeBox(gp_Pnt(-100, -60, -80), 150, 200, 170).Shape();
	
	Handle(AIS_Shape) ais1 = new AIS_Shape(S);
	myAISContext->SetColor(ais1,Quantity_NOC_RED,Standard_False);
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);
	myAISContext->Display(ais1,Standard_False);
	const Handle(AIS_InteractiveObject)& anIO1 = ais1;
	myAISContext->SetSelected (anIO1, Standard_False);
	Fit();
	Sleep(500);

	BRepAlgoAPI_Section asect(S, gp_Pln(1,2,1,-15),Standard_False);
	asect.ComputePCurveOn1(Standard_True);
	asect.Approximation(Standard_True);
	asect.Build();
	TopoDS_Shape R = asect.Shape();

	BRepFeat_SplitShape asplit(S);
	
	for (TopExp_Explorer Ex(R,TopAbs_EDGE); Ex.More(); Ex.Next()) {
		TopoDS_Shape anEdge = Ex.Current();
		TopoDS_Shape aFace;
		if (asect.HasAncestorFaceOn1(anEdge,aFace)) {
			TopoDS_Face F = TopoDS::Face(aFace);
			TopoDS_Edge E = TopoDS::Edge(anEdge);
			asplit.Add(E,F);
		}
	}

	asplit.Build();

	//Sleep(1000);
	myAISContext->Erase(ais1,Standard_False);
	//Fit();

	TopoDS_Shape Result = asplit.Shape();
	
	Handle(AIS_Shape) ais2 = new AIS_Shape(Result);

	myAISContext->SetColor(ais2,Quantity_NOC_RED,Standard_False);
	myAISContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);
	myAISContext->SetDisplayMode(ais2,1,Standard_False);
	myAISContext->Display(ais2,Standard_False);
	const Handle(AIS_InteractiveObject)& anIO2 = ais2;
	myAISContext->SetSelected (anIO2, Standard_False);
	Fit();
		TCollection_AsciiString Message ("\
	\n\
TopoDS_Shape S = BRepPrimAPI_MakeBox(gp_Pnt(-100,-60,-80),150,200,170); 	\n\
		\n\
BRepBuilderAPI_Section asect(S, gp_Pln(1,2,1,-15),Standard_False);	\n\
asect.ComputePCurveOn1(Standard_True);	\n\
asect.Approximation(Standard_True);	\n\
asect.Build();	\n\
TopoDS_Shape R = asect.Shape();	\n\
	\n\
BRepFeat_SplitShape asplit(S);	\n\
	\n\
for (TopExp_Explorer Ex(R,TopAbs_EDGE); Ex.More(); Ex.Next()) {	\n\
TopoDS_Shape anEdge = Ex.Current();	\n\
	TopoDS_Shape aFace;	\n\
	if (asect.HasAncestorFaceOn1(anEdge,aFace)) {	\n\
		TopoDS_Face F = TopoDS::Face(aFace);	\n\
		TopoDS_Edge E = TopoDS::Edge(anEdge);	\n\
		asplit.Add(E,F);	\n\
	}	\n\
}	\n\
	\n\
asplit.Build();	\n\
	\n\
TopoDS_Shape Result = asplit.Shape();	\n\
	\n\
\n");

PocessTextInDialog("Split a shape", Message);
}



void CModelingDoc::OnThickLocal() 
{
	AIS_ListOfInteractive L;
	myAISContext->DisplayedObjects(L);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(L);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

  TopoDS_Shape S1 = BRepPrimAPI_MakeBox(150, 200, 110).Shape();
	
	Handle(AIS_Shape) abox1 = new AIS_Shape(S1);
	myAISContext->SetColor (abox1, Quantity_NOC_WHITE, Standard_False);
	myAISContext->SetMaterial(abox1,Graphic3d_NOM_PLASTIC,Standard_False);
	myAISContext->Display(abox1,Standard_False);
	const Handle(AIS_InteractiveObject)& anIOBox1 = abox1;
	myAISContext->SetSelected (anIOBox1, Standard_False);
	Fit();
	Sleep(1000);

	TopTools_ListOfShape aList;
	TopExp_Explorer Ex(S1,TopAbs_FACE);
	Ex.Next();	//this is the front face
	TopoDS_Shape aFace = Ex.Current();
	aList.Append(aFace);
	
    BRepOffsetAPI_MakeThickSolid aSolidMaker;
    aSolidMaker.MakeThickSolidByJoin(S1,aList,10,0.01);
	TopoDS_Shape aThickSolid = aSolidMaker.Shape();

	Handle(AIS_Shape) ais1 = new AIS_Shape(aThickSolid);
	myAISContext->SetColor(ais1,Quantity_NOC_RED,Standard_False);
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);
	myAISContext->Display(ais1,Standard_False);
	const Handle(AIS_InteractiveObject)& anIO1 = ais1;
	myAISContext->SetSelected (anIO1, Standard_False);
	Fit();
	Sleep(1000);
	
	myAISContext->Erase(abox1,Standard_True);
	Fit();


		TCollection_AsciiString Message ("\
	\n\
TopoDS_Shape S = BRepPrimAPI_MakeBox(150,200,110);	\n\
	\n\
TopTools_ListOfShape aList;	\n\
TopExp_Explorer Ex(S,TopAbs_FACE);	\n\
Ex.Next();	//in order to recover the front face	\n\
TopoDS_Shape aFace = Ex.Current();	\n\
aList.Append(aFace);	\n\
			\n\
TopoDS_Shape aThickSolid = BRepPrimAPI_MakeThickSolid(S,aList,15,0.01);	\n\
	\n\
\n");
PocessTextInDialog("Make a thick solid", Message);
}

void CModelingDoc::OnOffsetLocal() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

  TopoDS_Shape S1 = BRepPrimAPI_MakeBox(150, 200, 110).Shape();

	Handle(AIS_Shape) aisBox1 = new AIS_Shape(S1);
	myAISContext->SetColor(aisBox1,Quantity_NOC_BROWN,Standard_False);
	myAISContext->SetMaterial(aisBox1,Graphic3d_NOM_GOLD,Standard_False);
	myAISContext->Display(aisBox1,Standard_False);
	Fit();
	Sleep(500);

    BRepOffsetAPI_MakeOffsetShape aShapeMaker1;
    aShapeMaker1.PerformByJoin(S1,60,0.01);
	TopoDS_Shape anOffsetShape1 = aShapeMaker1.Shape();

	Handle(AIS_Shape) ais1 = new AIS_Shape(anOffsetShape1);
	myAISContext->SetColor(ais1,Quantity_NOC_MATRABLUE,Standard_False);
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_GOLD,Standard_False);
	myAISContext->SetTransparency(ais1,0.5,Standard_False);
	myAISContext->Display(ais1,Standard_False);
	Fit();
	Sleep(500);

  TopoDS_Shape S2 = BRepPrimAPI_MakeBox(gp_Pnt(500, 0, 0), 220, 140, 180).Shape();
	
	Handle(AIS_Shape) aisBox2 = new AIS_Shape(S2);
	myAISContext->SetColor(aisBox2,Quantity_NOC_WHITE,Standard_False);
	myAISContext->SetMaterial(aisBox2,Graphic3d_NOM_GOLD,Standard_False);
	myAISContext->SetTransparency(aisBox2,0.5,Standard_False);
	myAISContext->Display(aisBox2,Standard_False);
	Fit();
	Sleep(500);

    BRepOffsetAPI_MakeOffsetShape aShapeMaker2;
    aShapeMaker2.PerformByJoin(S2,-40,0.01,
      BRepOffset_Skin,Standard_False,Standard_False,GeomAbs_Arc);
	TopoDS_Shape anOffsetShape2 = aShapeMaker2.Shape();

	Handle(AIS_Shape) ais2 = new AIS_Shape(anOffsetShape2);
	myAISContext->SetColor (ais2, Quantity_NOC_MATRABLUE, Standard_False);
	myAISContext->SetMaterial(ais2,Graphic3d_NOM_GOLD,Standard_False);
	myAISContext->Display (ais2, Standard_False);
	Fit();
	
	TCollection_AsciiString Message ("\
	\n\
\n\
TopoDS_Shape S1 = BRepPrimAPI_MakeBox(150,200,110);	\n\
\n\
TopoDS_Shape anOffsetShape1 = BRepPrimAPI_MakeOffsetShape(S1,60,0.01);	\n\
\n\
//The white box	\n\
	\n\
TopoDS_Shape S2 = BRepPrimAPI_MakeBox(gp_Pnt(300,0,0),220,140,180);	\n\
\n\
TopoDS_Shape anOffsetShape2 = BRepPrimAPI_MakeOffsetShape(S2,-20,0.01,	\n\
	BRepOffset_Skin,Standard_False,Standard_False,GeomAbs_Arc);	\n\
	\n\
\n\
\n");
PocessTextInDialog("Make an offset shape", Message);

}

/* =================================================================================
   ====================   B U I L D I N G   ========================================
   ================================================================================= */


void CModelingDoc::OnVertex() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

	TopoDS_Vertex V1,V2,V3;
	
	V1 = BRepBuilderAPI_MakeVertex(gp_Pnt(0,0,0));
	V2 = BRepBuilderAPI_MakeVertex(gp_Pnt(10,7,25));
	
	gp_Pnt P(-12,8,-4);
	BRepBuilderAPI_MakeVertex MV(P);
	V3 = MV.Vertex();

	Handle(AIS_Shape) Point1 = new AIS_Shape(V1);
	myAISContext->Display(Point1,Standard_False);
	Handle(AIS_Shape) Point2 = new AIS_Shape(V2);
	myAISContext->Display(Point2,Standard_False);
	Handle(AIS_Shape) Point3 = new AIS_Shape(V3);
	myAISContext->Display(Point3,Standard_False);

	Fit();

   TCollection_AsciiString Message ("\
		\n\
TopoDS_Vertex V1,V2,V3;	\n\
		\n\
V1 = BRepBuilderAPI_MakeVertex(gp_Pnt(0,0,0));	\n\
	\n\
V2 = BRepBuilderAPI_MakeVertex(gp_Pnt(10,7,25));	\n\
		\n\
gp_Pnt P(-12,8,-4);	\n\
BRepBuilderAPI_MakeVertex MV(P);	\n\
V3 = MV.Vertex();	\n\
	\n\
\n");

	PocessTextInDialog("Make vertex from point ", Message);

}

void CModelingDoc::OnEdge() 
{
	
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}
	
	
	TopoDS_Edge BlueEdge,YellowEdge,WhiteEdge,RedEdge,GreenEdge;
	TopoDS_Vertex V1,V2,V3,V4;
	
/////////////The blue edge

	BlueEdge = BRepBuilderAPI_MakeEdge(gp_Pnt(-80,-50,-20),gp_Pnt(-30,-60,-60));

/////////////The yellow edge
	
	V1 = BRepBuilderAPI_MakeVertex(gp_Pnt(-20,10,-30));
	V2 = BRepBuilderAPI_MakeVertex(gp_Pnt(10,7,-25));
	YellowEdge = BRepBuilderAPI_MakeEdge(V1,V2);

/////////////The white edge
	
	gp_Lin line(gp_Ax1(gp_Pnt(10,10,10),gp_Dir(1,0,0)));
	WhiteEdge = BRepBuilderAPI_MakeEdge(line,-20,10);

//////////////The red edge

	gp_Elips Elips(gp_Ax2(gp_Pnt(10,0,0),gp_Dir(1,1,1)),60,30);
	RedEdge = BRepBuilderAPI_MakeEdge(Elips,0,M_PI/2);

/////////////The green edge and the both extreme vertex

	gp_Pnt P1(-15,200,10);
	gp_Pnt P2(5,204,0);
	gp_Pnt P3(15,200,0);
	gp_Pnt P4(-15,20,15);
	gp_Pnt P5(-5,20,0);
	gp_Pnt P6(15,20,0);
	gp_Pnt P7(24,120,0);
	gp_Pnt P8(-24,120,12.5);
	TColgp_Array1OfPnt array(1,8);
	array.SetValue(1,P1);
	array.SetValue(2,P2);
	array.SetValue(3,P3); 
	array.SetValue(4,P4); 
	array.SetValue(5,P5); 
	array.SetValue(6,P6); 
	array.SetValue(7,P7); 
	array.SetValue(8,P8); 
	Handle (Geom_BezierCurve) curve = new Geom_BezierCurve(array);
	
	BRepBuilderAPI_MakeEdge ME (curve);
	GreenEdge = ME;
	V3 = ME.Vertex1();
	V4 = ME.Vertex2();

//////////////Display
Handle(AIS_Shape) blue = new AIS_Shape(BlueEdge);
myAISContext->SetColor(blue,Quantity_NOC_MATRABLUE,Standard_False); 
myAISContext->Display(blue,Standard_False);

Handle(AIS_Shape) yellow = new AIS_Shape(YellowEdge);
myAISContext->SetColor(yellow,Quantity_NOC_YELLOW,Standard_False); 
myAISContext->Display(yellow,Standard_False);

Handle(AIS_Shape) white = new AIS_Shape(WhiteEdge);
myAISContext->SetColor(white,Quantity_NOC_WHITE,Standard_False); 
myAISContext->Display(white,Standard_False);

Handle(AIS_Shape) red = new AIS_Shape(RedEdge);
myAISContext->SetColor(red,Quantity_NOC_RED,Standard_False); 
myAISContext->Display(red,Standard_False);

Handle(AIS_Shape) green = new AIS_Shape(GreenEdge);
myAISContext->SetColor(green,Quantity_NOC_GREEN,Standard_False); 
myAISContext->Display(green,Standard_False);

Handle(AIS_Shape) Point1 = new AIS_Shape(V3);
myAISContext->Display(Point1,Standard_False);
Handle(AIS_Shape) Point2 = new AIS_Shape(V4);
myAISContext->Display(Point2,Standard_False);

Fit();

   TCollection_AsciiString Message ("\
		\n\
TopoDS_Edge BlueEdge, YellowEdge, WhiteEdge, RedEdge, GreenEdge;	\n\
TopoDS_Vertex V1,V2,V3,V4;	\n\
		\n\
/////////////The blue edge	\n\
	\n\
BlueEdge = BRepBuilderAPI_MakeEdge(gp_Pnt(-80,-50,-20),gp_Pnt(-30,-60,-60));	\n\
	\n\
/////////////The yellow edge	\n\
		\n\
V1 = BRepBuilderAPI_MakeVertex(gp_Pnt(-20,10,-30));	\n\
V2 = BRepBuilderAPI_MakeVertex(gp_Pnt(10,7,-25));	\n\
YellowEdge = BRepBuilderAPI_MakeEdge(V1,V2);	\n\
	\n\
/////////////The white edge	\n\
		\n\
gp_Lin line(gp_Ax1(gp_Pnt(10,10,10),gp_Dir(1,0,0)));	\n\
WhiteEdge = BRepBuilderAPI_MakeEdge(line,-20,10);	\n\
	\n\
//////////////The red edge	\n\
	\n\
gp_Elips Elips(gp_Ax2(gp_Pnt(10,0,0),gp_Dir(1,1,1)),60,30);	\n\
RedEdge = BRepBuilderAPI_MakeEdge(Elips,0,PI/2);	\n\
	\n\
/////////////The green edge and the both extreme vertex	\n\
	\n\
gp_Pnt P1(-15,200,10);	\n\
gp_Pnt P2(5,204,0);	\n\
gp_Pnt P3(15,200,0);	\n\
gp_Pnt P4(-15,20,15);	\n\
gp_Pnt P5(-5,20,0);	\n\
gp_Pnt P6(15,20,0);	\n\
gp_Pnt P7(24,120,0);	\n\
gp_Pnt P8(-24,120,12.5);	\n\
TColgp_Array1OfPnt array(1,8);	\n\
array.SetValue(1,P1);	\n\
array.SetValue(2,P2);	\n\
array.SetValue(3,P3); 	\n\
array.SetValue(4,P4); 	\n\
array.SetValue(5,P5); 	\n\
array.SetValue(6,P6); 	\n\
array.SetValue(7,P7); 	\n\
array.SetValue(8,P8); 	\n\
Handle (Geom_BezierCurve) curve = new Geom_BezierCurve(array);	\n\
	\n\
BRepBuilderAPI_MakeEdge ME (curve);	\n\
GreenEdge = ME;	\n\
V3 = ME.Vertex1();	\n\
V4 = ME.Vertex2();	\n\
	\n\
\n");

	PocessTextInDialog("Make edge", Message);
	
}

void CModelingDoc::OnWire() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

	TopoDS_Wire RedWire,YellowWire,WhiteWire,
		ExistingWire, ExistingWire2;

	TopoDS_Edge Edge1,Edge2,Edge3,Edge4,Edge5,Edge6,Edge7,LastEdge;
	TopoDS_Vertex LastVertex;

////////////The red wire is build from a single edge

	gp_Elips Elips(gp_Ax2(gp_Pnt(250,0,0),gp_Dir(1,1,1)),160,90);
	Edge1 = BRepBuilderAPI_MakeEdge(Elips,0,M_PI/2);

	RedWire = BRepBuilderAPI_MakeWire(Edge1);

///////////the yellow wire is build from an existing wire and an edge
	
	gp_Circ circle(gp_Ax2(gp_Pnt(-300,0,0),gp_Dir(1,0,0)),80);
	Edge2 = BRepBuilderAPI_MakeEdge(circle,0,M_PI);

	ExistingWire = BRepBuilderAPI_MakeWire(Edge2);

	Edge3 = BRepBuilderAPI_MakeEdge(gp_Pnt(-300,0,-80),gp_Pnt(-90,20,-30));

	BRepBuilderAPI_MakeWire MW1(ExistingWire,Edge3);
	if (MW1.IsDone()) {
			YellowWire = MW1;
	}


//////////the white wire is built with an existing wire and 3 edges.
//////////we use the methods Add, Edge and Vertex from BRepBuilderAPI_MakeWire.

	gp_Circ circle2(gp_Ax2(gp_Pnt(0,0,0),gp_Dir(0,1,0)),200);
	Edge4 = BRepBuilderAPI_MakeEdge(circle2,0,M_PI);

	ExistingWire2 = BRepBuilderAPI_MakeWire(Edge4);

	gp_Pnt P1(0,0,-200);
	gp_Pnt P2(5,204,0);
	Edge5 = BRepBuilderAPI_MakeEdge(P1,P2);

	gp_Pnt P3(-15,20,15);
	Edge6 = BRepBuilderAPI_MakeEdge(P2,P3);
	gp_Pnt P4(15,20,0);	
	Edge7 = BRepBuilderAPI_MakeEdge(P3,P4);

	BRepBuilderAPI_MakeWire MW;
	MW.Add(ExistingWire2);
	MW.Add(Edge5);
	MW.Add(Edge6);
	MW.Add(Edge7);

	if (MW.IsDone()) {
		WhiteWire = MW.Wire();
		LastEdge = MW.Edge();
		LastVertex = MW.Vertex();
	}


Handle(AIS_Shape) red = new AIS_Shape(RedWire);
myAISContext->SetColor(red,Quantity_NOC_RED,Standard_False); 
myAISContext->Display(red,Standard_False);

Handle(AIS_Shape) yellow = new AIS_Shape(YellowWire);
myAISContext->SetColor(yellow,Quantity_NOC_YELLOW,Standard_False); 
myAISContext->Display(yellow,Standard_False);

Handle(AIS_Shape) white = new AIS_Shape(WhiteWire);
myAISContext->SetColor(white,Quantity_NOC_WHITE,Standard_False); 
myAISContext->Display(white,Standard_False);

Handle(AIS_Shape) lastE = new AIS_Shape(LastEdge);
myAISContext->SetWidth(lastE,3,Standard_False);
myAISContext->SetColor(lastE,Quantity_NOC_RED,Standard_False); 
myAISContext->Display(lastE,Standard_False);

Handle(AIS_Shape) lastV = new AIS_Shape(LastVertex);
myAISContext->Display(lastV,Standard_False);

Fit();

   TCollection_AsciiString Message ("\
	\n\
TopoDS_Wire RedWire,YellowWire,WhiteWire,	\n\
ExistingWire, ExistingWire2;	\n\
	\n\
TopoDS_Edge Edge1,Edge2,Edge3,Edge4,Edge5,Edge6,Edge7,LastEdge;	\n\
TopoDS_Vertex LastVertex;	\n\
	\n\
////////////The red wire is build from a single edge	\n\
	\n\
gp_Elips Elips(gp_Ax2(gp_Pnt(10,0,0),gp_Dir(1,1,1)),160,90);	\n\
Edge1 = BRepBuilderAPI_MakeEdge(Elips,0,PI/2);	\n\
	\n\
RedWire = BRepBuilderAPI_MakeWire(Edge1);	\n\
	\n\
///////////the yellow wire is build from an existing wire and an edge	\n\
		\n\
gp_Circ circle(gp_Ax2(gp_Pnt(0,0,0),gp_Dir(1,0,0)),80);	\n\
Edge2 = BRepBuilderAPI_MakeEdge(circle,0,PI);	\n\
	\n\
ExistingWire = BRepBuilderAPI_MakeWire(Edge2);	\n\
	\n\
Edge3 = BRepBuilderAPI_MakeEdge(gp_Pnt(0,0,-80),gp_Pnt(90,20,30));	\n\
	\n\
BRepBuilderAPI_MakeWire MW1(ExistingWire,Edge3);	\n\
if (MW1.IsDone()) {	\n\
		YellowWire = MW1;	\n\
}	\n\
	\n\
///the white wire is built with an existing wire and 3 edges.	\n\
///we use the methods Add, Edge and Vertex from BRepBuilderAPI_MakeWire	\n\
///in order to display the last edge and the last vertices we	\n\
///add to the wire. 	\n\
	\n\
gp_Circ circle2(gp_Ax2(gp_Pnt(0,0,0),gp_Dir(0,1,0)),200);	\n\
Edge4 = BRepBuilderAPI_MakeEdge(circle2,0,PI);	\n\
	\n\
ExistingWire2 = BRepBuilderAPI_MakeWire(Edge4);	\n\
	\n\
gp_Pnt P1(0,0,-200);	\n\
gp_Pnt P2(5,204,0);	\n\
Edge5 = BRepBuilderAPI_MakeEdge(P1,P2);	\n\
	\n\
gp_Pnt P3(-15,20,15);	\n\
Edge6 = BRepBuilderAPI_MakeEdge(P2,P3);	\n\
gp_Pnt P4(15,20,0);		\n\
Edge7 = BRepBuilderAPI_MakeEdge(P3,P4);	\n\
	\n\
BRepBuilderAPI_MakeWire MW;	\n\
MW.Add(ExistingWire2);	\n\
MW.Add(Edge5);	\n\
MW.Add(Edge6);	\n\
MW.Add(Edge7);	\n\
	\n\
if (MW.IsDone()) {	\n\
	WhiteWire = MW.Wire();	\n\
	LastEdge = MW.Edge();	\n\
	LastVertex = MW.Vertex();	\n\
}	\n\
	\n\
\n");

	PocessTextInDialog("Make wire ", Message);
}

void CModelingDoc::OnFace() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}



	TopoDS_Face WhiteFace, BrownFace, RedFace, PinkFace;
	TopoDS_Edge Edge1, Edge2, Edge3, Edge4, Edge5, Edge6, Edge7;
	TopoDS_Wire Wire1;
	gp_Pnt P1, P2, P3, P4, P5, P6, P7;

	gp_Sphere sphere (gp_Ax3(gp_Pnt(0,0,0),gp_Dir(1,0,0)),150);

	WhiteFace = BRepBuilderAPI_MakeFace(sphere,0.1,0.7,0.2,0.9);

//////////////////////////////////

	P1.SetCoord(-15,200,10);
	P2.SetCoord(5,204,0);
	P3.SetCoord(15,200,0);
	P4.SetCoord(-15,20,15);
	P5.SetCoord(-5,20,0);
	P6.SetCoord(15,20,35);
	TColgp_Array2OfPnt array(1,3,1,2);
	array.SetValue(1,1,P1);
	array.SetValue(2,1,P2);
	array.SetValue(3,1,P3); 
	array.SetValue(1,2,P4); 
	array.SetValue(2,2,P5); 
	array.SetValue(3,2,P6);
	Handle (Geom_BSplineSurface) curve = GeomAPI_PointsToBSplineSurface(array,3,8,GeomAbs_C2,0.001);

	RedFace = BRepBuilderAPI_MakeFace(curve, Precision::Confusion());

////////////////////

	gp_Circ circle(gp_Ax2(gp_Pnt(0,0,0),gp_Dir(1,0,0)),80);
	Edge1 = BRepBuilderAPI_MakeEdge(circle,0,M_PI);

	Edge2 = BRepBuilderAPI_MakeEdge(gp_Pnt(0,0,-80),gp_Pnt(0,-10,40));
	Edge3 = BRepBuilderAPI_MakeEdge(gp_Pnt(0,-10,40),gp_Pnt(0,0,80));

	TopoDS_Wire YellowWire;
	BRepBuilderAPI_MakeWire MW1(Edge1,Edge2,Edge3);
	if (MW1.IsDone()) {
			YellowWire = MW1;
	}

	BrownFace = BRepBuilderAPI_MakeFace(YellowWire);


/////////////

	P1.SetCoord(35,-200,40);
	P2.SetCoord(50,-204,30);
	P3.SetCoord(65,-200,30);
	P4.SetCoord(35,-20,45);
	P5.SetCoord(45,-20,30);
	P6.SetCoord(65,-20,65);
	TColgp_Array2OfPnt array2(1,3,1,2);
	array2.SetValue(1,1,P1);
	array2.SetValue(2,1,P2);
	array2.SetValue(3,1,P3); 
	array2.SetValue(1,2,P4); 
	array2.SetValue(2,2,P5); 
	array2.SetValue(3,2,P6);
	
	Handle (Geom_BSplineSurface) BSplineSurf = GeomAPI_PointsToBSplineSurface(array2,3,8,GeomAbs_C2,0.001);
	
	TopoDS_Face aFace = BRepBuilderAPI_MakeFace(BSplineSurf, Precision::Confusion());

	//2d lines
	gp_Pnt2d P12d(0.9,0.1);
	gp_Pnt2d P22d(0.2,0.7);
	gp_Pnt2d P32d(0.02,0.1);

	Handle (Geom2d_Line) line1 = new Geom2d_Line(P12d,gp_Dir2d((0.2-0.9),(0.7-0.1)));
	Handle (Geom2d_Line) line2 = new Geom2d_Line(P22d,gp_Dir2d((0.02-0.2),(0.1-0.7)));
	Handle (Geom2d_Line) line3 = new Geom2d_Line(P32d,gp_Dir2d((0.9-0.02),(0.1-0.1)));


	//Edges are on the BSpline surface
	Edge1 = BRepBuilderAPI_MakeEdge(line1,BSplineSurf,0,P12d.Distance(P22d));
	Edge2 = BRepBuilderAPI_MakeEdge(line2,BSplineSurf,0,P22d.Distance(P32d));
	Edge3 = BRepBuilderAPI_MakeEdge(line3,BSplineSurf,0,P32d.Distance(P12d));

	Wire1 = BRepBuilderAPI_MakeWire(Edge1,Edge2,Edge3);
	Wire1.Reverse();
	PinkFace = BRepBuilderAPI_MakeFace(aFace,Wire1);
	BRepLib::BuildCurves3d(PinkFace);

/////////////Display
	Handle(AIS_Shape) white = new AIS_Shape(WhiteFace);
	myAISContext->SetColor(white,Quantity_NOC_WHITE,Standard_False);
	myAISContext->SetMaterial(white,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(white,Standard_False);
	
	Handle(AIS_Shape) red = new AIS_Shape(RedFace);
	myAISContext->SetColor(red,Quantity_NOC_RED,Standard_False);
	myAISContext->SetMaterial(red,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(red,Standard_False);
	
	Handle(AIS_Shape) brown = new AIS_Shape(BrownFace);
	myAISContext->SetColor(brown,Quantity_NOC_BROWN,Standard_False);
	myAISContext->SetMaterial(brown,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(brown,Standard_False);
	
	Handle(AIS_Shape) pink = new AIS_Shape(PinkFace);
	myAISContext->SetColor(pink,Quantity_NOC_HOTPINK,Standard_False);
	myAISContext->SetMaterial(pink,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(pink,Standard_False);

	Fit();


  TCollection_AsciiString Message ("\
 	\n\
TopoDS_Face WhiteFace, BrownFace, RedFace, PinkFace;	\n\
TopoDS_Edge Edge1, Edge2, Edge3, Edge4, Edge5, Edge6, Edge7;	\n\
TopoDS_Wire Wire1;	\n\
gp_Pnt P1, P2, P3, P4, P5, P6, P7;	\n\
\n\
////////The white Face \n\
\n\
gp_Sphere sphere (gp_Ax3(gp_Pnt(0,0,0),gp_Dir(1,0,0)),150);	\n\
\n\
WhiteFace = BRepBuilderAPI_MakeFace(sphere,0.1,0.7,0.2,0.9);	\n\
\n\
////////The red face	\n\
\n\
P1.SetCoord(-15,200,10);	\n\
P2.SetCoord(5,204,0);	\n\
P3.SetCoord(15,200,0);	\n\
P4.SetCoord(-15,20,15);	\n\
P5.SetCoord(-5,20,0);	\n\
P6.SetCoord(15,20,35);	\n\
TColgp_Array2OfPnt array(1,3,1,2);	\n\
array.SetValue(1,1,P1);	\n\
array.SetValue(2,1,P2);	\n\
array.SetValue(3,1,P3); 	\n\
array.SetValue(1,2,P4); 	\n\
array.SetValue(2,2,P5); 	\n\
array.SetValue(3,2,P6);	\n\
Handle (Geom_BSplineSurface) curve = GeomAPI_PointsToBSplineSurface(array,3,8,GeomAbs_C2,0.001);	\n\
\n\
RedFace = BRepBuilderAPI_MakeFace(curve);	\n\
\n\
////////The brown face	\n\
\n\
gp_Circ circle(gp_Ax2(gp_Pnt(0,0,0),gp_Dir(1,0,0)),80);	\n\
Edge1 = BRepBuilderAPI_MakeEdge(circle,0,PI);	\n\
\n\
Edge2 = BRepBuilderAPI_MakeEdge(gp_Pnt(0,0,-80),gp_Pnt(0,-10,40));	\n\
Edge3 = BRepBuilderAPI_MakeEdge(gp_Pnt(0,-10,40),gp_Pnt(0,0,80));	\n\
\n\
TopoDS_Wire YellowWire;	\n\
BRepBuilderAPI_MakeWire MW1(Edge1,Edge2,Edge3);	\n\
if (MW1.IsDone()) {	\n\
		YellowWire = MW1;	\n\
}	\n\
\n\
BrownFace = BRepBuilderAPI_MakeFace(YellowWire);	\n\
\n");
Message +=("\
////////The pink face	\n\
\n\
P1.SetCoord(35,-200,40);	\n\
P2.SetCoord(50,-204,30);	\n\
P3.SetCoord(65,-200,30);	\n\
P4.SetCoord(35,-20,45);	\n\
P5.SetCoord(45,-20,30);	\n\
P6.SetCoord(65,-20,65);	\n\
TColgp_Array2OfPnt array2(1,3,1,2);	\n\
array2.SetValue(1,1,P1);	\n\
array2.SetValue(2,1,P2);	\n\
array2.SetValue(3,1,P3); 	\n\
array2.SetValue(1,2,P4); 	\n\
array2.SetValue(2,2,P5); 	\n\
array2.SetValue(3,2,P6);	\n\
	\n\
Handle (Geom_BSplineSurface) BSplineSurf = GeomAPI_PointsToBSplineSurface(array2,3,8,GeomAbs_C2,0.001);	\n\
	\n\
TopoDS_Face aFace = BRepBuilderAPI_MakeFace(BSplineSurf);	\n\
\n\
//2d lines	\n\
gp_Pnt2d P12d(0.9,0.1);	\n\
gp_Pnt2d P22d(0.2,0.7);	\n\
gp_Pnt2d P32d(0.02,0.1);	\n\
\n\
Handle (Geom2d_Line) line1=		\n\
	new Geom2d_Line(P12d,gp_Dir2d((0.2-0.9),(0.7-0.1)));	\n\
Handle (Geom2d_Line) line2=		\n\
	new Geom2d_Line(P22d,gp_Dir2d((0.02-0.2),(0.1-0.7)));   \n\
Handle (Geom2d_Line) line3=		\n\
	new Geom2d_Line(P32d,gp_Dir2d((0.9-0.02),(0.1-0.1)));	\n\
		\n\
//Edges are on the BSpline surface	\n\
Edge1 = BRepBuilderAPI_MakeEdge(line1,BSplineSurf,0,P12d.Distance(P22d));	\n\
Edge2 = BRepBuilderAPI_MakeEdge(line2,BSplineSurf,0,P22d.Distance(P32d));	\n\
Edge3 = BRepBuilderAPI_MakeEdge(line3,BSplineSurf,0,P32d.Distance(P12d));	\n\
\n\
Wire1 = BRepBuilderAPI_MakeWire(Edge1,Edge2,Edge3);	\n\
Wire1.Reverse();	\n\
PinkFace = BRepBuilderAPI_MakeFace(aFace,Wire1);	\n\
BRepLib::BuildCurves3d(PinkFace);	\n\
\n\
\n");

PocessTextInDialog("Make face ", Message);
}

void CModelingDoc::OnShell() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

	TColgp_Array2OfPnt Poles(1,2,1,4);
	Poles.SetValue(1,1,gp_Pnt(0,0,0));
	Poles.SetValue(1,2,gp_Pnt(0,10,2));
	Poles.SetValue(1,3,gp_Pnt(0,20,10)); 
	Poles.SetValue(1,4,gp_Pnt(0,30,0)); 
	Poles.SetValue(2,1,gp_Pnt(10,0,5));
	Poles.SetValue(2,2,gp_Pnt(10,10,3));
	Poles.SetValue(2,3,gp_Pnt(10,20,20));
	Poles.SetValue(2,4,gp_Pnt(10,30,0));

	TColStd_Array1OfReal UKnots(1,2);
	UKnots.SetValue(1,0);
	UKnots.SetValue(2,1);

	TColStd_Array1OfInteger UMults(1,2);
	UMults.SetValue(1,2);
	UMults.SetValue(2,2);

	TColStd_Array1OfReal VKnots(1,3);
	VKnots.SetValue(1,0);
	VKnots.SetValue(2,1);
	VKnots.SetValue(3,2);

	TColStd_Array1OfInteger VMults(1,3);
	VMults.SetValue(1,3);
	VMults.SetValue(2,1);
	VMults.SetValue(3,3);

	Standard_Integer UDegree(1);
	Standard_Integer VDegree(2);
	
	Handle (Geom_BSplineSurface) BSpline = new Geom_BSplineSurface(Poles,UKnots,VKnots,UMults,VMults,UDegree,VDegree);
	
	TopoDS_Face WhiteFace = BRepBuilderAPI_MakeFace(BSpline, Precision::Confusion());

	
	Handle(AIS_Shape) white = new AIS_Shape(WhiteFace);
	myAISContext->SetColor (white, Quantity_NOC_WHITE, Standard_False);
	myAISContext->SetMaterial(white,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->SetTransparency (white, 0.7, Standard_False);
	myAISContext->Display(white,Standard_False);
	

	TopoDS_Shell aShell = BRepBuilderAPI_MakeShell(BSpline);
	Handle(AIS_Shape) anAISShell = new AIS_Shape(aShell);
	myAISContext->SetDisplayMode (anAISShell, 0, Standard_False);
	myAISContext->Display(anAISShell,Standard_False);
	//myAISContext->SetSelected(anAISShell);

	Fit();
  
	TCollection_AsciiString Message ("\
	\n\
TColgp_Array2OfPnt Poles(1,2,1,4);	\n\
Poles.SetValue(1,1,gp_Pnt(0,0,0));	\n\
Poles.SetValue(1,2,gp_Pnt(0,10,2));	\n\
Poles.SetValue(1,3,gp_Pnt(0,20,10)); 	\n\
Poles.SetValue(1,4,gp_Pnt(0,30,0)); 	\n\
Poles.SetValue(2,1,gp_Pnt(10,0,5));	\n\
Poles.SetValue(2,2,gp_Pnt(10,10,3));	\n\
Poles.SetValue(2,3,gp_Pnt(10,20,20));	\n\
Poles.SetValue(2,4,gp_Pnt(10,30,0));	\n\
\n\
TColStd_Array1OfReal UKnots(1,2);	\n\
UKnots.SetValue(1,0);	\n\
UKnots.SetValue(2,1);	\n\
\n\
TColStd_Array1OfInteger UMults(1,2);	\n\
UMults.SetValue(1,2);	\n\
UMults.SetValue(2,2);	\n\
\n\
TColStd_Array1OfReal VKnots(1,3);	\n\
VKnots.SetValue(1,0);	\n\
VKnots.SetValue(2,1);	\n\
VKnots.SetValue(3,2);	\n\
\n\
TColStd_Array1OfInteger VMults(1,3);	\n\
VMults.SetValue(1,3);	\n\
VMults.SetValue(2,1);	\n\
VMults.SetValue(3,3);	\n\
\n\
Standard_Integer UDegree(1);	\n\
Standard_Integer VDegree(2);	\n\
	\n\
Handle (Geom_BSplineSurface) BSpline = new Geom_BSplineSurface(Poles,UKnots,VKnots,UMults,VMults,UDegree,VDegree);	\n\
	\n\
TopoDS_Shell aShell = BRepBuilderAPI_MakeShell(BSpline);	\n\
\n\
\n");

  PocessTextInDialog("Make shell", Message);
	
}

void CModelingDoc::OnCompound() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

	BRep_Builder builder;
	TopoDS_Compound Comp;
	builder.MakeCompound(Comp);

	TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(-20,10,-30));
	builder.Add(Comp,aVertex);
	
	gp_Lin line(gp_Ax1(gp_Pnt(10,10,10),gp_Dir(1,0,0)));
	TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(line,-20,10);
	builder.Add(Comp,anEdge);

	gp_Sphere sphere (gp_Ax3(gp_Pnt(-80,0,0),gp_Dir(1,0,0)),150);
	TopoDS_Face aFace = BRepBuilderAPI_MakeFace(sphere,0.1,0.7,0.2,0.9);
	builder.Add(Comp,aFace);

  TopoDS_Shape aBox = BRepPrimAPI_MakeBox(gp_Pnt(-60, 0, 0), 30, 60, 40).Shape();
	builder.Add(Comp,aBox);

	Handle(AIS_Shape) white = new AIS_Shape(Comp);
	myAISContext->SetDisplayMode (white, 0, Standard_False);
	myAISContext->Display(white,Standard_False);

	Fit();


   TCollection_AsciiString Message ("\
		\n\
BRep_Builder builder;	\n\
TopoDS_Compound Comp;	\n\
builder.MakeCompound(Comp);	\n\
\n\
TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(-20,10,-30));	\n\
builder.Add(Comp,aVertex);	\n\
	\n\
gp_Lin line(gp_Ax1(gp_Pnt(10,10,10),gp_Dir(1,0,0)));	\n\
TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(line,-20,10);	\n\
builder.Add(Comp,anEdge);	\n\
	\n\
gp_Sphere sphere (gp_Ax3(gp_Pnt(-80,0,0),gp_Dir(1,0,0)),150);	\n\
TopoDS_Face aFace = BRepBuilderAPI_MakeFace(sphere,0.1,0.7,0.2,0.9);	\n\
builder.Add(Comp,aFace);	\n\
	\n\
TopoDS_Shape aBox = BRepPrimAPI_MakeBox(gp_Pnt(-60,0,0),30,60,40);	\n\
builder.Add(Comp,aBox);	\n\
	\n\
\n");

	PocessTextInDialog("Make compound ", Message);

}





void CModelingDoc::OnSewing() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

	gp_Pnt P(0,0,0);
	gp_Vec V(0,0,1);
	Handle(Geom_Plane) Pi=new Geom_Plane(P,V);
	Handle(Geom_RectangularTrimmedSurface) GeometricSurface=new Geom_RectangularTrimmedSurface(Pi,0.,100.,0.,100.);
	TopoDS_Shape FirstShape = BRepBuilderAPI_MakeFace(GeometricSurface, Precision::Confusion());
	
	Handle(AIS_Shape) white1 = new AIS_Shape(FirstShape);
	
	myAISContext->SetColor(white1,Quantity_NOC_RED,Standard_False);
	myAISContext->SetMaterial(white1,Graphic3d_NOM_PLASTIC,Standard_False); 
	myAISContext->SetTransparency(white1,0.4,Standard_False);
	myAISContext->Display(white1,Standard_False);
	//Sleep(1000);
	
	gp_Pnt P1(0,0,0);
	gp_Pnt P2(50,0,0);
	gp_Pnt P3(100,0,0);
	gp_Pnt P4(25,12,85);
	gp_Pnt P5(100,0,80);
	gp_Pnt P6(135,-12,85);

	TColgp_Array2OfPnt Array(1,3,1,2);
	Array.SetValue(1,1,P1);
	Array.SetValue(2,1,P2);
	Array.SetValue(3,1,P3);
	Array.SetValue(1,2,P4);
	Array.SetValue(2,2,P5);
	Array.SetValue(3,2,P6);

	Handle (Geom_BSplineSurface) aSurf = GeomAPI_PointsToBSplineSurface(Array,3,8,GeomAbs_C2,0.00001);
	TopoDS_Shape SecondShape = BRepBuilderAPI_MakeFace(aSurf, Precision::Confusion());
	
	Handle(AIS_Shape) white2 = new AIS_Shape(SecondShape);
	
	myAISContext->SetColor(white2,Quantity_NOC_YELLOW,Standard_False);
	myAISContext->SetMaterial(white2,Graphic3d_NOM_PLASTIC,Standard_False);  
	myAISContext->SetTransparency(white2,0.4,Standard_False);
	myAISContext->Display(white2,Standard_False);

	//Sleep(1000);
	
	BRepOffsetAPI_Sewing aMethod;
	aMethod.Add(FirstShape);	
	aMethod.Add(SecondShape);

	aMethod.Perform();

	TopoDS_Shape sewedShape = aMethod.SewedShape();

	Handle(AIS_Shape) result = new AIS_Shape(sewedShape);
	myAISContext->SetDisplayMode(result,0,Standard_False);
	myAISContext->Display(result,Standard_False);

	Fit();

   TCollection_AsciiString Message ("\
	\n\
///////The first shape \n\
 \n\
gp_Pnt P(0,0,0);	\n\
gp_Vec V(0,0,1);	\n\
Handle(Geom_Plane) Pi=new Geom_Plane(P,V);	\n\
Handle(Geom_RectangularTrimmedSurface) GeometricSurface=new Geom_RectangularTrimmedSurface(Pi,0.,100.,0.,100.);	\n\
TopoDS_Shape FirstShape = BRepBuilderAPI_MakeFace(GeometricSurface);	\n\
	\n\
///////The second shape \n\
 \n\
gp_Pnt P1(0,0,0);	\n\
gp_Pnt P2(50,0,0);	\n\
gp_Pnt P3(100,0,0);	\n\
gp_Pnt P4(25,12,85);	\n\
gp_Pnt P5(100,0,80);	\n\
gp_Pnt P6(135,-12,85);	\n\
\n\
TColgp_Array2OfPnt Array(1,3,1,2);	\n\
Array.SetValue(1,1,P1);	\n\
Array.SetValue(2,1,P2);	\n\
Array.SetValue(3,1,P3);	\n\
Array.SetValue(1,2,P4);	\n\
Array.SetValue(2,2,P5);	\n\
Array.SetValue(3,2,P6);	\n\
\n\
Handle (Geom_BSplineSurface) aSurf = GeomAPI_PointsToBSplineSurface(Array,3,8,GeomAbs_C2,0.00001);	\n\
TopoDS_Shape SecondShape = BRepBuilderAPI_MakeFace(aSurf);	\n\
	\n\
BRepOffsetAPI_Sewing aMethod;	\n\
aMethod.Add(FirstShape);		\n\
aMethod.Add(SecondShape);	\n\
\n\
aMethod.Perform();	\n\
\n\
TopoDS_Shape sewedShape = aMethod.SewedShape();	\n\
	\n\
\n");

	PocessTextInDialog("Sew faces ", Message);

}







void CModelingDoc::OnBuilder() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

	//The tolerance is the tolerance of confusion
	Standard_Real precision = Precision::Confusion();

	//The builder
	BRep_Builder B;

	//Build the vertices
	TopoDS_Vertex V000, V001, V010, V011, V100, V101, V110, V111;
	B.MakeVertex(V000,gp_Pnt(0,0,0),precision);
	B.MakeVertex(V001,gp_Pnt(0,0,100),precision);
	B.MakeVertex(V010,gp_Pnt(0,150,0),precision);
	B.MakeVertex(V011,gp_Pnt(0,150,100),precision);
	B.MakeVertex(V100,gp_Pnt(200,0,0),precision);
	B.MakeVertex(V101,gp_Pnt(200,0,100),precision);
	B.MakeVertex(V110,gp_Pnt(200,150,0),precision);
	B.MakeVertex(V111,gp_Pnt(200,150,100),precision);

	//Build the edges
	//the edges are oriented as the axis X,Y,Z
	TopoDS_Edge EX00, EX01, EX10, EX11;
	TopoDS_Edge EY00, EY01, EY10, EY11;
	TopoDS_Edge EZ00, EZ01, EZ10, EZ11;
	Handle (Geom_Line) L;

	//Edge X00
	L = new Geom_Line(gp_Pnt(0,0,0),gp_Dir(1,0,0));
	B.MakeEdge(EX00,L,precision);
	V000.Orientation(TopAbs_FORWARD);
	V100.Orientation(TopAbs_REVERSED);
	B.Add(EX00,V000);
	B.Add(EX00,V100);
	//Parameters
	B.UpdateVertex(V000,0,EX00,precision);
	B.UpdateVertex(V100,200,EX00,precision);

	//Edge X10
	L = new Geom_Line(gp_Pnt(0,150,0),gp_Dir(1,0,0));
	B.MakeEdge(EX10,L,precision);
	V010.Orientation(TopAbs_FORWARD);
	V110.Orientation(TopAbs_REVERSED);
	B.Add(EX10,V010);
	B.Add(EX10,V110);
	//Parameters
	B.UpdateVertex(V010,0,EX10,precision);
	B.UpdateVertex(V110,200,EX10,precision);

	//Edge Y00
	L = new Geom_Line(gp_Pnt(0,0,0),gp_Dir(0,1,0));
	B.MakeEdge(EY00,L,precision);
	V000.Orientation(TopAbs_FORWARD);
	V010.Orientation(TopAbs_REVERSED);
	B.Add(EY00,V000);
	B.Add(EY00,V010);
	//Parameters
	B.UpdateVertex(V000,0,EY00,precision);
	B.UpdateVertex(V010,150,EY00,precision);

	//Edge Y10
	L = new Geom_Line(gp_Pnt(200,0,0),gp_Dir(0,1,0));
	B.MakeEdge(EY10,L,precision);
	V100.Orientation(TopAbs_FORWARD);
	V110.Orientation(TopAbs_REVERSED);
	B.Add(EY10,V100);
	B.Add(EY10,V110);
	//Parameters
	B.UpdateVertex(V100,0,EY10,precision);
	B.UpdateVertex(V110,150,EY10,precision);

	//Edge Y01
	L = new Geom_Line(gp_Pnt(0,0,100),gp_Dir(0,1,0));
	B.MakeEdge(EY01,L,precision);
	V001.Orientation(TopAbs_FORWARD);
	V011.Orientation(TopAbs_REVERSED);
	B.Add(EY01,V001);
	B.Add(EY01,V011);
	//Parameters
	B.UpdateVertex(V001,0,EY01,precision);
	B.UpdateVertex(V011,150,EY01,precision);

	//Edge Y11
	L = new Geom_Line(gp_Pnt(200,0,100),gp_Dir(0,1,0));
	B.MakeEdge(EY11,L,precision);
	V101.Orientation(TopAbs_FORWARD);
	V111.Orientation(TopAbs_REVERSED);
	B.Add(EY11,V101);
	B.Add(EY11,V111);
	//Parameters
	B.UpdateVertex(V101,0,EY11,precision);
	B.UpdateVertex(V111,150,EY11,precision);

	//Edge Z00
	L = new Geom_Line(gp_Pnt(0,0,0),gp_Dir(0,0,1));
	B.MakeEdge(EZ00,L,precision);
	V000.Orientation(TopAbs_FORWARD);
	V001.Orientation(TopAbs_REVERSED);
	B.Add(EZ00,V000);
	B.Add(EZ00,V001);
	//Parameters
	B.UpdateVertex(V000,0,EZ00,precision);
	B.UpdateVertex(V001,100,EZ00,precision);

	//Edge Z01
	L = new Geom_Line(gp_Pnt(0,150,0),gp_Dir(0,0,1));
	B.MakeEdge(EZ01,L,precision);
	V010.Orientation(TopAbs_FORWARD);
	V011.Orientation(TopAbs_REVERSED);
	B.Add(EZ01,V010);
	B.Add(EZ01,V011);
	//Parameters
	B.UpdateVertex(V010,0,EZ01,precision);
	B.UpdateVertex(V011,100,EZ01,precision);

	//Edge Z10
	L = new Geom_Line(gp_Pnt(200,0,0),gp_Dir(0,0,1));
	B.MakeEdge(EZ10,L,precision);
	V100.Orientation(TopAbs_FORWARD);
	V101.Orientation(TopAbs_REVERSED);
	B.Add(EZ10,V100);
	B.Add(EZ10,V101);
	//Parameters
	B.UpdateVertex(V100,0,EZ10,precision);
	B.UpdateVertex(V101,100,EZ10,precision);

	//Edge Z11
	L = new Geom_Line(gp_Pnt(200,150,0),gp_Dir(0,0,1));
	B.MakeEdge(EZ11,L,precision);
	V110.Orientation(TopAbs_FORWARD);
	V111.Orientation(TopAbs_REVERSED);
	B.Add(EZ11,V110);
	B.Add(EZ11,V111);
	//Parameters
	B.UpdateVertex(V110,0,EZ11,precision);
	B.UpdateVertex(V111,100,EZ11,precision);


	//Circular Edges
	Handle (Geom_Circle) C;
	//Standard_Real R = 100;

	//Edge EX01
	C = new Geom_Circle(gp_Ax2(gp_Pnt(100,0,100),gp_Dir(0,1,0),gp_Dir(-1,0,0)),100);
	B.MakeEdge(EX01,C,precision);
	V001.Orientation(TopAbs_FORWARD);
	V101.Orientation(TopAbs_REVERSED);
	B.Add(EX01,V001);
	B.Add(EX01,V101);
	//Parameters
	B.UpdateVertex(V001,0,EX01,precision);
	B.UpdateVertex(V101,M_PI,EX01,precision);

	//Edge EX11
	C = new Geom_Circle(gp_Ax2(gp_Pnt(100,150,100),gp_Dir(0,1,0),gp_Dir(-1,0,0)),100);
	B.MakeEdge(EX11,C,precision);
	V011.Orientation(TopAbs_FORWARD);
	V111.Orientation(TopAbs_REVERSED);
	B.Add(EX11,V011);
	B.Add(EX11,V111);
	//Parameters
	B.UpdateVertex(V011,0,EX11,precision);
	B.UpdateVertex(V111,M_PI,EX11,precision);

	//Build wire and faces
	//Faces normals are along the axis X,Y,Z
	TopoDS_Face FXMIN, FXMAX, FYMIN, FYMAX, FZMIN, FZMAX;
	TopoDS_Wire W;
	Handle (Geom_Plane) P;
	Handle (Geom2d_Line) L2d;
	Handle (Geom2d_Circle) C2d;
	Handle (Geom_CylindricalSurface) S;

	//Face FXMAX
	P = new Geom_Plane(gp_Ax2(gp_Pnt(200,0,0),gp_Dir(1,0,0),gp_Dir(0,1,0)));
	B.MakeFace(FXMAX,P,precision);
	//the wire and the edges
	B.MakeWire (W);

	EY10.Orientation(TopAbs_FORWARD);
	B.Add(W,EY10);
	//pcurve
	L2d = new Geom2d_Line(gp_Pnt2d(0,0),gp_Dir2d(1,0));
	B.UpdateEdge(EY10,L2d,FXMAX,precision);

	EZ11.Orientation(TopAbs_FORWARD);
	B.Add(W,EZ11);
	//pcurve
	L2d = new Geom2d_Line(gp_Pnt2d(150,0),gp_Dir2d(0,1));
	B.UpdateEdge(EZ11,L2d,FXMAX,precision);

	EY11.Orientation(TopAbs_REVERSED);
	B.Add(W,EY11);
	//pcurve
	L2d = new Geom2d_Line(gp_Pnt2d(0,100),gp_Dir2d(1,0));
	B.UpdateEdge(EY11,L2d,FXMAX,precision);

	EZ10.Orientation(TopAbs_REVERSED);
	B.Add(W,EZ10);
	//pcurve
	L2d = new Geom2d_Line(gp_Pnt2d(0,0),gp_Dir2d(0,1));
	B.UpdateEdge(EZ10,L2d,FXMAX,precision);

	B.Add(FXMAX,W);

	//Face FXMIN
	P = new Geom_Plane(gp_Ax2(gp_Pnt(0,0,0),gp_Dir(-1,0,0),gp_Dir(0,0,1)));
	B.MakeFace(FXMIN,P,precision);
	//the wire and the edges
	B.MakeWire (W);

	EZ00.Orientation(TopAbs_FORWARD);
	B.Add(W,EZ00);
	//pcurve
	L2d = new Geom2d_Line(gp_Pnt2d(0,0),gp_Dir2d(1,0));
	B.UpdateEdge(EZ00,L2d,FXMIN,precision);

	EY01.Orientation(TopAbs_FORWARD);
	B.Add(W,EY01);
	//pcurve
	L2d = new Geom2d_Line(gp_Pnt2d(100,0),gp_Dir2d(0,1));
	B.UpdateEdge(EY01,L2d,FXMIN,precision);

	EZ01.Orientation(TopAbs_REVERSED);
	B.Add(W,EZ01);
	//pcurve
	L2d = new Geom2d_Line(gp_Pnt2d(0,150),gp_Dir2d(1,0));
	B.UpdateEdge(EZ01,L2d,FXMIN,precision);

	EY00.Orientation(TopAbs_REVERSED);
	B.Add(W,EY00);
	//pcurve
	L2d = new Geom2d_Line(gp_Pnt2d(0,0),gp_Dir2d(0,1));
	B.UpdateEdge(EY00,L2d,FXMIN,precision);


	B.Add(FXMIN,W);
	
	//Face FYMAX

	P = new Geom_Plane(gp_Ax2(gp_Pnt(0,0,0),gp_Dir(0,1,0),gp_Dir(0,0,1)));
	B.MakeFace(FYMAX,P,precision);
	//the wire and the edges
	B.MakeWire (W);

	EZ00.Orientation(TopAbs_FORWARD);
	B.Add(W,EZ00);
	//pcurve
	L2d = new Geom2d_Line(gp_Pnt2d(0,0),gp_Dir2d(1,0));
	B.UpdateEdge(EZ00,L2d,FYMAX,precision);

	EX01.Orientation(TopAbs_FORWARD);
	B.Add(W,EX01);
	//pcurve
	C2d = new Geom2d_Circle(gp_Ax2d(gp_Pnt2d(100,100),gp_Dir2d(0,-1)),100);
	B.UpdateEdge(EX01,C2d,FYMAX,precision);
	B.UpdateVertex(V001,0,EX01,FYMAX,precision);
	B.UpdateVertex(V101,M_PI,EX01,FYMAX,precision);

	EZ10.Orientation(TopAbs_REVERSED);
	B.Add(W,EZ10);
	//pcurve
	L2d = new Geom2d_Line(gp_Pnt2d(0,200),gp_Dir2d(1,0));
	B.UpdateEdge(EZ10,L2d,FYMAX,precision);

	EX00.Orientation(TopAbs_REVERSED);
	B.Add(W,EX00);
	//pcurve
	L2d = new Geom2d_Line(gp_Pnt2d(0,0),gp_Dir2d(0,1));
	B.UpdateEdge(EX00,L2d,FYMAX,precision);


	B.Add(FYMAX,W);



	//Face FYMIN
	P = new Geom_Plane(gp_Ax2(gp_Pnt(0,150,0),gp_Dir(0,1,0),gp_Dir(0,0,1)));
	B.MakeFace(FYMIN,P,precision);
	//the wire and the edges
	B.MakeWire (W);

	EZ01.Orientation(TopAbs_FORWARD);
	B.Add(W,EZ01);
	//pcurve
	L2d = new Geom2d_Line(gp_Pnt2d(0,0),gp_Dir2d(1,0));
	B.UpdateEdge(EZ01,L2d,FYMIN,precision);

	EX11.Orientation(TopAbs_FORWARD);
	B.Add(W,EX11);
	//pcurve
	C2d = new Geom2d_Circle(gp_Ax2d(gp_Pnt2d(100,100),gp_Dir2d(0,-1)),100);
	B.UpdateEdge(EX11,C2d,FYMIN,precision);
	B.UpdateVertex(V011,0,EX11,FYMIN,precision);
	B.UpdateVertex(V111,M_PI,EX11,FYMIN,precision);

	EZ11.Orientation(TopAbs_REVERSED);
	B.Add(W,EZ11);
	//pcurve
	L2d = new Geom2d_Line(gp_Pnt2d(0,200),gp_Dir2d(1,0));
	B.UpdateEdge(EZ11,L2d,FYMIN,precision);

	EX10.Orientation(TopAbs_REVERSED);
	B.Add(W,EX10);
	//pcurve
	L2d = new Geom2d_Line(gp_Pnt2d(0,0),gp_Dir2d(0,1));
	B.UpdateEdge(EX10,L2d,FYMIN,precision);

	B.Add(FYMIN,W);

	//Face FZMAX
	P = new Geom_Plane(gp_Ax2(gp_Pnt(0,0,0),gp_Dir(0,0,-1),gp_Dir(0,1,0)));
	B.MakeFace(FZMAX,P,precision);
	//the wire and the edges
	B.MakeWire (W);

	EY00.Orientation(TopAbs_FORWARD);
	B.Add(W,EY00);
	//pcurve
	L2d = new Geom2d_Line(gp_Pnt2d(0,0),gp_Dir2d(1,0));
	B.UpdateEdge(EY00,L2d,FZMAX,precision);

	EX10.Orientation(TopAbs_FORWARD);
	B.Add(W,EX10);
	//pcurve
	L2d = new Geom2d_Line(gp_Pnt2d(150,0),gp_Dir2d(0,1));
	B.UpdateEdge(EX10,L2d,FZMAX,precision);

	EY10.Orientation(TopAbs_REVERSED);
	B.Add(W,EY10);
	//pcurve
	L2d = new Geom2d_Line(gp_Pnt2d(0,200),gp_Dir2d(1,0));
	B.UpdateEdge(EY10,L2d,FZMAX,precision);

	EX00.Orientation(TopAbs_REVERSED);
	B.Add(W,EX00);
	//pcurve
	L2d = new Geom2d_Line(gp_Pnt2d(0,0),gp_Dir2d(0,1));
	B.UpdateEdge(EX00,L2d,FZMAX,precision);


	B.Add(FZMAX,W);
		
	//Face FZMIN
	S = new Geom_CylindricalSurface(gp_Ax3(gp_Pnt(100,0,100),gp_Dir(0,1,0),gp_Dir(-1,0,0)),100);
	B.MakeFace(FZMIN,S,precision);

	//the wire and the edges
	B.MakeWire (W);

	EX01.Orientation(TopAbs_FORWARD);
	B.Add(W,EX01);
	//pcurve
	L2d = new Geom2d_Line(gp_Ax2d(gp_Pnt2d(0,0),gp_Dir2d(1,0)));
	B.UpdateEdge(EX01,L2d,FZMIN,precision);
	B.UpdateVertex(V001,0,EX01,FZMIN,precision);
	B.UpdateVertex(V101,M_PI,EX01,FZMIN,precision);

	EY11.Orientation(TopAbs_FORWARD);
	B.Add(W,EY11);
	//pcurve
	L2d = new Geom2d_Line(gp_Pnt2d(M_PI,0),gp_Dir2d(0,1));
	B.UpdateEdge(EY11,L2d,FZMIN,precision);

	EX11.Orientation(TopAbs_REVERSED);
	B.Add(W,EX11);
	//pcurve
	L2d = new Geom2d_Line(gp_Ax2d(gp_Pnt2d(0,150),gp_Dir2d(1,0)));
	B.UpdateEdge(EX11,L2d,FZMIN,precision);
	B.UpdateVertex(V111,M_PI,EX11,FZMIN,precision);
	B.UpdateVertex(V011,0,EX11,FZMIN,precision);

	EY01.Orientation(TopAbs_REVERSED);
	B.Add(W,EY01);
	//pcurve
	L2d = new Geom2d_Line(gp_Pnt2d(0,0),gp_Dir2d(0,1));
	B.UpdateEdge(EY01,L2d,FZMIN,precision);

	B.Add(FZMIN,W);

	FYMAX.Orientation(TopAbs_REVERSED);

	//Shell
	TopoDS_Shell Sh;
	B.MakeShell(Sh);
	B.Add(Sh,FXMAX);
	B.Add(Sh,FXMIN);
	B.Add(Sh,FYMAX);
	B.Add(Sh,FYMIN);
	B.Add(Sh,FZMAX);
	B.Add(Sh,FZMIN);

	// Solid
	TopoDS_Solid Sol;
	B.MakeSolid(Sol);
	B.Add(Sol,Sh);

	Handle(AIS_Shape) borne = new AIS_Shape(Sol);
	myAISContext->SetDisplayMode (borne, 1, Standard_False);
	myAISContext->SetColor (borne, Quantity_NOC_RED, Standard_False);
	myAISContext->SetMaterial(borne,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(borne,Standard_False);


	Fit();
   TCollection_AsciiString Message ("\
		\n\
//The tolerance is 0.01 	\n\
Standard_Real precision(0.01);	\n\
\n\
//The builder	\n\
BRep_Builder B;	\n\
\n\
//Build the vertices	\n\
TopoDS_Vertex V000, V001, V010, V011, V100, V101, V110, V111;	\n\
B.MakeVertex(V000,gp_Pnt(0,0,0),precision);	\n\
B.MakeVertex(V001,gp_Pnt(0,0,100),precision);	\n\
B.MakeVertex(V010,gp_Pnt(0,150,0),precision);	\n\
B.MakeVertex(V011,gp_Pnt(0,150,100),precision);	\n\
B.MakeVertex(V100,gp_Pnt(200,0,0),precision);	\n\
B.MakeVertex(V101,gp_Pnt(200,0,100),precision);	\n\
B.MakeVertex(V110,gp_Pnt(200,150,0),precision);	\n\
B.MakeVertex(V111,gp_Pnt(200,150,100),precision);	\n\
\n\
//Build the edges	\n\
//the edges are oriented as the axis X,Y,Z	\n\
TopoDS_Edge EX00, EX01, EX10, EX11;	\n\
TopoDS_Edge EY00, EY01, EY10, EY11;	\n\
TopoDS_Edge EZ00, EZ01, EZ10, EZ11;	\n\
Handle (Geom_Line) L;	\n\
\n\
//Edge X00	\n\
L = new Geom_Line(gp_Pnt(0,0,0),gp_Dir(1,0,0));	\n\
B.MakeEdge(EX00,L,precision);	\n\
V000.Orientation(TopAbs_FORWARD);	\n\
V100.Orientation(TopAbs_REVERSED);	\n\
B.Add(EX00,V000);	\n\
B.Add(EX00,V100);	\n\
//Parameters	\n\
B.UpdateVertex(V000,0,EX00,precision);	\n\
B.UpdateVertex(V100,200,EX00,precision);	\n\
\n\
//Idem for all the linear edges...	\n\
\n\
//Circular Edges	\n\
Handle (Geom_Circle) C;	\n\
Standard_Real R = 100;	\n\
\n\
//Edge EX01	\n\
C = new Geom_Circle(gp_Ax2(gp_Pnt(100,0,100),gp_Dir(0,1,0),gp_Dir(-1,0,0)),100);	\n\
B.MakeEdge(EX01,C,precision);	\n\
V001.Orientation(TopAbs_FORWARD);	\n\
V101.Orientation(TopAbs_REVERSED);	\n\
B.Add(EX01,V001);	\n\
B.Add(EX01,V101);	\n\
//Parameters	\n\
B.UpdateVertex(V001,0,EX01,precision);	\n\
B.UpdateVertex(V101,PI,EX01,precision);	\n\
\n\
//Idem for EX11	\n\
\n\
//Build wire and faces	\n\
//Faces normals are along the axis X,Y,Z	\n\
TopoDS_Face FXMIN, FXMAX, FYMIN, FYMAX, FZMIN, FZMAX;	\n\
TopoDS_Wire W;	\n\
Handle (Geom_Plane) P;	\n\
Handle (Geom2d_Line) L2d;	\n\
Handle (Geom2d_Circle) C2d;	\n\
Handle (Geom_CylindricalSurface) S;	\n\
\n\
//Face FXMAX	\n\
P = new Geom_Plane(gp_Ax2(gp_Pnt(200,0,0),gp_Dir(1,0,0),gp_Dir(0,1,0)));	\n\
B.MakeFace(FXMAX,P,precision);	\n\
//the wire and the edges	\n\
B.MakeWire (W);	\n");
Message += ("\
\n\
EY10.Orientation(TopAbs_FORWARD);	\n\
B.Add(W,EY10);	\n\
//pcurve	\n\
L2d = new Geom2d_Line(gp_Pnt2d(0,0),gp_Dir2d(1,0));	\n\
B.UpdateEdge(EY10,L2d,FXMAX,precision);	\n\
	\n\
EZ11.Orientation(TopAbs_FORWARD);	\n\
B.Add(W,EZ11);	\n\
//pcurve	\n\
L2d = new Geom2d_Line(gp_Pnt2d(150,0),gp_Dir2d(0,1));	\n\
B.UpdateEdge(EZ11,L2d,FXMAX,precision);	\n\
	\n\
EY11.Orientation(TopAbs_REVERSED);	\n\
B.Add(W,EY11);	\n\
//pcurve	\n\
L2d = new Geom2d_Line(gp_Pnt2d(0,100),gp_Dir2d(1,0));	\n\
B.UpdateEdge(EY11,L2d,FXMAX,precision);	\n\
	\n\
EZ10.Orientation(TopAbs_REVERSED);	\n\
B.Add(W,EZ10);	\n\
//pcurve	\n\
L2d = new Geom2d_Line(gp_Pnt2d(0,0),gp_Dir2d(0,1));	\n\
B.UpdateEdge(EZ10,L2d,FXMAX,precision);	\n\
\n\
B.Add(FXMAX,W);	\n\
\n\
//Idem for other faces...	\n\
\n\
//Shell	\n\
TopoDS_Shell Sh;	\n\
B.MakeShell(Sh);	\n\
B.Add(Sh,FXMAX);	\n\
B.Add(Sh,FXMIN);	\n\
B.Add(Sh,FYMAX);	\n\
B.Add(Sh,FYMIN);	\n\
B.Add(Sh,FZMAX);	\n\
B.Add(Sh,FZMIN);	\n\
\n\
// Solid	\n\
TopoDS_Solid Sol;	\n\
B.MakeSolid(Sol);	\n\
B.Add(Sol,Sh);	\n\
\n\
\n");

PocessTextInDialog("Make a shape with a builder", Message);

}

void CModelingDoc::OnGeometrie() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

	//geometrie of a vertex
	TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(0,120,70));
	gp_Pnt GeometricPoint = BRep_Tool::Pnt(aVertex);

	Handle(AIS_Shape) vert = new AIS_Shape(aVertex);
	myAISContext->Display(vert,Standard_False);
	Fit();
	Sleep (500);

	//geometrie of an edge
	TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(gp_Pnt(100,50,250),gp_Pnt(-30,-100,-50));
	Handle(AIS_Shape) yellow = new AIS_Shape(anEdge);
	myAISContext->SetWidth(yellow,2,Standard_False);
	myAISContext->Display(yellow,Standard_False);
	Fit();
	Sleep (500);

	TopLoc_Location location;
	Standard_Real first, last;
	Handle (Geom_Curve) aCurve = BRep_Tool::Curve(anEdge,location,first,last);
	TopoDS_Edge anEdgeDS = BRepBuilderAPI_MakeEdge(aCurve);

	Handle (Geom_Line) aLine = Handle (Geom_Line)::DownCast(aCurve);
	if (!aLine.IsNull()) {
		Handle (AIS_Line) DispLine = new AIS_Line(aLine);
		myAISContext->Display(DispLine,Standard_False);
		Fit();
		Sleep (500);
	}
		
	//geometrie of a face
	gp_Pnt P(-20,-20,-20);
	gp_Vec V(0,0,1);
	Handle(Geom_Plane) Pi=new Geom_Plane(P,V);
	Handle(Geom_RectangularTrimmedSurface) Surface=new Geom_RectangularTrimmedSurface(Pi,0.,100.,0.,100.);
	TopoDS_Face RedFace = BRepBuilderAPI_MakeFace(Surface, Precision::Confusion());

	Handle(AIS_Shape) red = new AIS_Shape(RedFace);
	myAISContext->SetColor(red,Quantity_NOC_RED,Standard_False);
	myAISContext->SetMaterial(red,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(red,Standard_False);
	Fit();
	Sleep (500);

	TopLoc_Location location2;
	Handle (Geom_Surface) aGeometricSurface = BRep_Tool::Surface(RedFace,location2);

	Handle (Geom_Plane) aPlane = Handle (Geom_Plane)::DownCast(aGeometricSurface);
	if (!aPlane.IsNull()) {
		Handle (AIS_Plane) DispPlane = new AIS_Plane(aPlane);
		myAISContext->Display(DispPlane,Standard_False);

	}


	Fit();
	Sleep (500);


   TCollection_AsciiString Message ("\
	\n\
///////geometrie of a vertex	\n\
TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(0,120,70));	\n\
gp_Pnt GeometricPoint = BRep_Tool::Pnt(aVertex);	\n\
\n\
///////geometrie of an edge	\n\
TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(gp_Pnt(100,50,250),gp_Pnt(-30,-100,-50));	\n\
\n\
TopLoc_Location location;	\n\
Standard_Real first, last;	\n\
Handle (Geom_Curve) aCurve = BRep_Tool::Curve(anEdge,location,first,last);	\n\
TopoDS_Edge anEdgeDS = BRepBuilderAPI_MakeEdge(aCurve);	\n\
\n\
Handle (Geom_Line) aLine = Handle (Geom_Line)::DownCast(aCurve);	\n\
if (!aLine.IsNull()) {	\n\
	Handle (AIS_Line) DispLine = new AIS_Line(aLine);	\n\
}	\n\
		\n\
///////geometrie of a face	\n\
gp_Pnt P(-20,-20,-20);	\n\
gp_Vec V(0,0,1);	\n\
Handle(Geom_Plane) Pi=new Geom_Plane(P,V);	\n\
Handle(Geom_RectangularTrimmedSurface) Surface=new Geom_RectangularTrimmedSurface(Pi,0.,100.,0.,100.);	\n\
TopoDS_Face RedFace = BRepBuilderAPI_MakeFace(Surface);	\n\
\n\
TopLoc_Location location2;	\n\
Handle (Geom_Surface) aGeometricSurface = BRep_Tool::Surface(RedFace,location2);	\n\
\n\
Handle (Geom_Plane) aPlane = Handle (Geom_Plane)::DownCast(aGeometricSurface);	\n\
if (!aPlane.IsNull()) {	\n\
	Handle (AIS_Plane) DispPlane = new AIS_Plane(aPlane);	\n\
}	\n\
\n\
\n");

	PocessTextInDialog("Recover the geometrie of vertex, edge and face ", Message);


}

void CModelingDoc::OnExplorer() 
{
	myAISContext->RemoveAll (false);
	
	TopoDS_Shape aBox = BRepPrimAPI_MakeBox(100, 100, 100).Shape();
	Standard_Integer j(8);
	Handle(AIS_ColoredShape) theBox = new AIS_ColoredShape(aBox);
	myAISContext->SetColor(theBox,Quantity_NOC_RED,Standard_False);
	myAISContext->SetMaterial(theBox,Graphic3d_NOM_PLASTIC,Standard_False);  
	myAISContext->Display(theBox, AIS_Shaded, 0,Standard_False);
	Fit();
	Sleep(500);

	for (TopExp_Explorer exp (aBox,TopAbs_FACE);exp.More();exp.Next())
	{
		TopoDS_Face aCurrentFace = TopoDS::Face(exp.Current());
		{
			Handle(AIS_ColoredDrawer) aSubFaceAspects = theBox->CustomAspects (aCurrentFace);
			aSubFaceAspects->SetShadingAspect (new Prs3d_ShadingAspect());
			*aSubFaceAspects->ShadingAspect()->Aspect() = *theBox->Attributes()->ShadingAspect()->Aspect();
			aSubFaceAspects->ShadingAspect()->Aspect()->ChangeFrontMaterial().SetTransparency (0.8f);
			myAISContext->Redisplay (theBox, false);
		}

		//test the orientation of the current face
		TopAbs_Orientation orient = aCurrentFace.Orientation();

		//Recover the geometric plane
		TopLoc_Location location;
		Handle (Geom_Surface) aGeometricSurface = BRep_Tool::Surface(aCurrentFace,location);

		Handle (Geom_Plane) aPlane = Handle (Geom_Plane)::DownCast(aGeometricSurface);

		//Build an AIS_Shape with a new color
		Handle(AIS_Shape) theMovingFace = new AIS_Shape(aCurrentFace);
		Quantity_NameOfColor aCurrentColor = (Quantity_NameOfColor)j;
		myAISContext->SetColor(theMovingFace,aCurrentColor,Standard_False);
		myAISContext->SetMaterial(theMovingFace,Graphic3d_NOM_PLASTIC,Standard_False);  
		//Find the normal vector of each face
		gp_Pln agpPlane = aPlane->Pln();
		gp_Ax1 norm = agpPlane.Axis();
		gp_Dir dir = norm.Direction();
		gp_Vec move(dir);

		TopLoc_Location aLocation;
		Handle (AIS_ConnectedInteractive) theTransformedDisplay = new AIS_ConnectedInteractive();
		theTransformedDisplay->Connect(theMovingFace, aLocation);

		Handle (Geom_Transformation) theMove = new Geom_Transformation(aLocation.Transformation());
        myAISContext->Display(theTransformedDisplay,Standard_False);
		myAISContext->UpdateCurrentViewer();
		Sleep (500);

		for (Standard_Integer i=1;i<=30;i++)
		{
			theMove->SetTranslation(move*i);
			if (orient==TopAbs_FORWARD) myAISContext->SetLocation(theTransformedDisplay,TopLoc_Location(theMove->Trsf()));
			else myAISContext->SetLocation(theTransformedDisplay,TopLoc_Location(theMove->Inverted()->Trsf()));

			myAISContext->Redisplay(theTransformedDisplay,true);
		}
		j+=15;
	}

	myAISContext->UpdateCurrentViewer();
	Sleep (500);

	   TCollection_AsciiString Message ("\
\n\
TopoDS_Shape aBox = BRepPrimAPI_MakeBox(100,100,100);	\n\
\n\
for (TopExp_Explorer exp (aBox,TopAbs_FACE);exp.More();exp.Next()) {	\n\
	TopoDS_Face aCurrentFace = TopoDS::Face(exp.Current());	\n\
\n\
	//Recover the geometric plane	\n\
	TopLoc_Location location;	\n\
	Handle (Geom_Surface) aGeometricSurface = BRep_Tool::Surface(aCurrentFace,location);	\n\
\n\
	Handle (Geom_Plane) aPlane = Handle (Geom_Plane)::DownCast(aGeometricSurface);	\n\
	\n\
\n");
	PocessTextInDialog("Explode a shape in faces ", Message);

}

/* =================================================================================
   ====================   A N A L Y S I S   ========================================
   ================================================================================= */

void CModelingDoc::OnValid() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

  TopoDS_Shape S = BRepPrimAPI_MakeBox(200., 300., 150.).Shape();
	Handle(AIS_Shape) ais1 = new AIS_Shape(S);
	myAISContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais1,Standard_False);
	Fit();

	TCollection_AsciiString Message ("\
		\n\
TopoDS_Shape S = BRepPrimI_MakeBox(200.,300.,150.);\n\
Standard_Boolean theShapeIsValid = BRepAlgo::IsValid(S);\n\
if ( theShapeIsValid )\n\
{\n\
  MessageBox(\"The Shape Is Valid !! \",\"Checking Shape\");\n\
}\n\
else\n\
{\n\
  MessageBox(\"The Shape Is NOT Valid !! \",\"Checking Shape\");\n\
}\n\
\n");
	PocessTextInDialog("Check a shape", Message);

	Standard_Boolean theShapeIsValid = BRepAlgo::IsValid(S);
  MessageBoxW (AfxGetApp()->m_pMainWnd->m_hWnd, theShapeIsValid ? L"The Shape Is Valid !! " : L"The Shape Is NOT Valid !! ", L"Checking Shape", MB_OK);
}


void CModelingDoc::OnLinear() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}


	TColgp_Array1OfPnt Points1(1,4);
	Points1.SetValue(1,gp_Pnt(0,0,0));
	Points1.SetValue(2,gp_Pnt(2,1,0));
	Points1.SetValue(3,gp_Pnt(4,0,0));
	Points1.SetValue(4,gp_Pnt(6,2,0));
	GeomAPI_PointsToBSpline PTBS1(Points1);
	Handle(Geom_BSplineCurve) BSC1 = PTBS1.Curve();
	TopoDS_Edge S = BRepBuilderAPI_MakeEdge(BSC1).Edge();

	Handle(AIS_Shape) ais1 = new AIS_Shape(S);
	myAISContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais1,Standard_False);
	Fit();


	GProp_GProps System;
	BRepGProp::LinearProperties(S,System);
	gp_Pnt G = System.CentreOfMass ();
	Standard_Real Length = System.Mass();
	gp_Mat I = System.MatrixOfInertia();

	TCollection_ExtendedString string("Length Of all the Edges =");
  TCollection_ExtendedString string1(Length);
	
	string += string1;
	string += "\nCenterOfMass : \n   X=";
	string1 = G.X();
	string += string1;
	string += " Y=";
	string1 = G.Y();
	string += string1;
	string += " Z=";
	string1 = G.Z();
	string += string1;
	string +="\n";

	string += "Matrix of Inertia :\n     ";
	string1 = I(1,1);
	string += string1;
	string += " " ;
	string1 = I(1,2);
	string += string1;
	string += " " ;
	string1 = I(1,3);
	string += string1;
	string += "\n     " ;
	string1 = I(2,1);
	string += string1;
	string += " " ;
	string1 = I(2,2);
	string += string1;
	string += " " ;
	string1 = I(2,3);
	string += string1;
	string += "\n     " ;
	string1 = I(3,1);
	string += string1;
	string += " " ;
	string1 = I(3,2);
	string += string1;
	string += " " ;
	string1 = I(3,3);
	string += string1;
	string += "\n" ;

  TCollection_AsciiString Message ("\
		\n\
TColgp_Array1OfPnt Points1(1,4);\n\
Points1.SetValue(1,gp_Pnt(0,0,0));\n\
Points1.SetValue(2,gp_Pnt(2,1,0));\n\
Points1.SetValue(3,gp_Pnt(4,0,0));\n\
Points1.SetValue(4,gp_Pnt(6,2,0));\n\
GeomAPI_PointsToBSpline PTBS1(Points1);\n\
Handle(Geom_BSplineCurve) BSC1 = PTBS1.Curve();\n\
TopoDS_Shape S = BRepBuilderAPI_MakeEdge(BSC1).Edge();\n\
GProp_GProps System;\n\
BRepGProp::LinearProperties(S,System);\n\
gp_Pnt G = System.CentreOfMass ();\n\
Standard_Real Length = System.Mass();\n\
gp_Mat I = System.MatrixOfInertia();\n\
\n");
	PocessTextInDialog("Linear Properties", Message);
	MessageBoxW (AfxGetApp()->m_pMainWnd->m_hWnd, string.ToWideString(), L"Linear Properties", MB_OK);
}

void CModelingDoc::OnSurface() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

	TColgp_Array1OfPnt Pnts1(1,3);
	TColgp_Array1OfPnt Pnts2(1,3);
	TColgp_Array1OfPnt Pnts3(1,3);
	TColgp_Array1OfPnt Pnts4(1,3);

	Pnts1(1) = gp_Pnt(0,0,0);
	Pnts1(2) = gp_Pnt(5,0,0);
	Pnts1(3) = gp_Pnt(10,10,0);

	Pnts2(1) = gp_Pnt(10,10,0);
	Pnts2(2) = gp_Pnt(5,12,4);
	Pnts2(3) = gp_Pnt(0,15,10);

	Pnts3(1) = gp_Pnt(0,15,10);
	Pnts3(2) = gp_Pnt(-12,10,11);
	Pnts3(3) = gp_Pnt(-10,5,13);

	Pnts4(1) = gp_Pnt(-10,5,13);
	Pnts4(2) = gp_Pnt(-2,-2,2);
	Pnts4(3) = gp_Pnt(0,0,0);
	
	GeomAPI_PointsToBSpline PTBS1(Pnts1);
	GeomAPI_PointsToBSpline PTBS2(Pnts2);
	GeomAPI_PointsToBSpline PTBS3(Pnts3);
	GeomAPI_PointsToBSpline PTBS4(Pnts4);
	Handle(Geom_BSplineCurve) C1 = PTBS1.Curve();
	Handle(Geom_BSplineCurve) C2 = PTBS2.Curve();
	Handle(Geom_BSplineCurve) C3 = PTBS3.Curve();
	Handle(Geom_BSplineCurve) C4 = PTBS4.Curve();

	GeomFill_BSplineCurves fill; 
	fill.Init(C1,C2,C3,C4,GeomFill_CoonsStyle);
	Handle(Geom_BSplineSurface) BSS = fill.Surface();

	TopoDS_Shape S = BRepBuilderAPI_MakeFace(BSS, Precision::Confusion()).Face();

	Handle(AIS_Shape) ais1 = new AIS_Shape(S);
	myAISContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais1,Standard_False);
	Fit();


	GProp_GProps System;
	BRepGProp::SurfaceProperties(S,System);
	gp_Pnt G = System.CentreOfMass ();
	Standard_Real Area = System.Mass();
	gp_Mat I = System.MatrixOfInertia();

	TCollection_ExtendedString string("Area Of the Face =");
  TCollection_ExtendedString string1(Area);
	
	string += string1;
	string += "\nCenterOfMass : \n   X=";
	string1 = G.X();
	string += string1;
	string += " Y=";
	string1 = G.Y();
	string += string1;
	string += " Z=";
	string1 = G.Z();
	string += string1;
	string +="\n";

	string += "Matrix of Inertia :\n     ";
	string1 = I(1,1);
	string += string1;
	string += " " ;
	string1 = I(1,2);
	string += string1;
	string += " " ;
	string1 = I(1,3);
	string += string1;
	string += "\n     " ;
	string1 = I(2,1);
	string += string1;
	string += " " ;
	string1 = I(2,2);
	string += string1;
	string += " " ;
	string1 = I(2,3);
	string += string1;
	string += "\n     " ;
	string1 = I(3,1);
	string += string1;
	string += " " ;
	string1 = I(3,2);
	string += string1;
	string += " " ;
	string1 = I(3,3);
	string += string1;
	string += "\n" ;

  TCollection_AsciiString Message ("\
		\n\
TopoDS_Shape S = BRepBuilderAPI_MakeFace(BSplineSurf).Face();\n\
GProp_GProps System;\n\
BRepGProp::SurfaceProperties(S,System);\n\
gp_Pnt G = System.CentreOfMass ();\n\
Standard_Real Area = System.Mass();\n\
gp_Mat I = System.MatrixOfInertia();\n\
\n");
  PocessTextInDialog("Surface Properties", Message);
	MessageBoxW (AfxGetApp()->m_pMainWnd->m_hWnd, string.ToWideString(), L"Surface Properties", MB_OK);

}

void CModelingDoc::OnVolume() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}
	

	TopoDS_Shape S = BRepPrimAPI_MakeWedge(60.,100.,80.,20.).Shape();
	Handle(AIS_Shape) ais1 = new AIS_Shape(S);
	myAISContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais1,Standard_False);
	Fit();


	GProp_GProps System;
	BRepGProp::VolumeProperties(S,System);
	gp_Pnt G = System.CentreOfMass ();
	Standard_Real Volume = System.Mass();
	gp_Mat I = System.MatrixOfInertia();

	TCollection_ExtendedString string("Volume Of all the Shape =");
  TCollection_ExtendedString string1(Volume);
	
	string += string1;
	string += "\nCenterOfMass : \n   X=";
	string1 = G.X();
	string += string1;
	string += " Y=";
	string1 = G.Y();
	string += string1;
	string += " Z=";
	string1 = G.Z();
	string += string1;
	string +="\n";

	string += "Matrix of Inertia :\n     ";
	string1 = I(1,1);
	string += string1;
	string += " " ;
	string1 = I(1,2);
	string += string1;
	string += " " ;
	string1 = I(1,3);
	string += string1;
	string += "\n     " ;
	string1 = I(2,1);
	string += string1;
	string += " " ;
	string1 = I(2,2);
	string += string1;
	string += " " ;
	string1 = I(2,3);
	string += string1;
	string += "\n     " ;
	string1 = I(3,1);
	string += string1;
	string += " " ;
	string1 = I(3,2);
	string += string1;
	string += " " ;
	string1 = I(3,3);
	string += string1;
	string += "\n" ;

	TCollection_AsciiString Message ("\
		\n\
TopoDS_Shape S = BRepBuilderAPI_MakeWedge(60.,100.,80.,20.);;\n\
GProp_GProps System;\n\
BRepGProp::VolumeProperties(S,System);\n\
gp_Pnt G = System.CentreOfMass ();\n\
Standard_Real Volume = System.Mass();\n\
gp_Mat I = System.MatrixOfInertia();\n\
\n");
  PocessTextInDialog("Volume Properties", Message);
	MessageBoxW (AfxGetApp()->m_pMainWnd->m_hWnd, string.ToWideString(), L"Volume Properties", MB_OK);
}


void CModelingDoc::OnButtonFill() 
{
	// TODO: Add your command handler code here
	myAISContext->InitSelected();
	if (myAISContext->MoreSelected()) {
		AIS1 = Handle(AIS_Shape)::DownCast(myAISContext->SelectedInteractive());
		myAISContext->Unhilight (AIS1, Standard_True);
		myAISContext->Activate(AIS1,2);
		myState = SELECT_EDGE_PLATE;
		((OCC_MainFrame*)AfxGetMainWnd())->SetStatusMessage("Select hole contour edges and then press right mouse button");	}
	else {
		AIS_ListOfInteractive LI;
		myAISContext->DisplayedObjects(LI);
		if(LI.IsEmpty()){
			if(OnFileImportBrep_WithInitDir (L"HoleFilling") == 1)
				return;
		myAISContext->DisplayedObjects(LI);
		myAISContext->SetSelected(LI.First(), Standard_True);
			OnButtonFill();
			return;
		}
	AfxMessageBox (L"Select a shape before!");
	}
}

void CModelingDoc::OnStopStop() 
{
	// TODO: Add your command handler code here
	// Stop selection
	if (myState == SELECT_EDGE_PLATE ) {
		Standard_Integer nbedges = 0;
		for (myAISContext->InitSelected(); myAISContext->MoreSelected();
		     myAISContext->NextSelected()) {
			nbedges++;
			
		}
                Handle(GeomPlate_HArray1OfHCurve) Fronts =
                        new GeomPlate_HArray1OfHCurve(1, nbedges);
		Handle(TColStd_HArray1OfInteger) Tang = 
			new TColStd_HArray1OfInteger(1,nbedges);
		Handle(TColStd_HArray1OfInteger) NbPtsCur = 
			new TColStd_HArray1OfInteger(1,nbedges);
		Standard_Integer i = 0;
		TopoDS_Shape S1 = AIS1->Shape();
		TopTools_IndexedDataMapOfShapeListOfShape M;
		TopExp::MapShapesAndAncestors(S1, TopAbs_EDGE, TopAbs_FACE, M);

		for (myAISContext->InitSelected(); myAISContext->MoreSelected();myAISContext->NextSelected()) {
			i++;
			Tang->SetValue(i,1);
			NbPtsCur->SetValue(i,10);
			TopoDS_Edge E = TopoDS::Edge(myAISContext->SelectedShape());
			TopoDS_Face F = TopoDS::Face(M.FindFromKey(E).First());

			BRepAdaptor_Surface S(F);
			GeomAdaptor_Surface aGAS = S.Surface();
			Handle(GeomAdaptor_HSurface) aHGAS = new GeomAdaptor_HSurface(aGAS);

			Handle(BRepAdaptor_HCurve2d) C = new BRepAdaptor_HCurve2d();
			C->ChangeCurve2d().Initialize(E,F);

			Adaptor3d_CurveOnSurface ConS(C,aHGAS);

			Handle (Adaptor3d_HCurveOnSurface) HConS = new Adaptor3d_HCurveOnSurface(ConS);
			Fronts->SetValue(i,HConS);
		}
		GeomPlate_BuildPlateSurface abuildplate(NbPtsCur,Fronts,Tang,3);
		abuildplate.Perform();
		if (!abuildplate.IsDone()){ // New in 2.0
			MessageBoxW (AfxGetApp()->m_pMainWnd->m_hWnd, L"Error : Build plate not valid!", L"CasCade Error", MB_ICONERROR);
			return;
		}
		Handle(GeomPlate_Surface) aplate = abuildplate.Surface();

		GeomPlate_MakeApprox aMKS(aplate, Precision::Approximation(), 4, 7, 0.001, 1);
		Handle(Geom_BSplineSurface) support = aMKS.Surface();
		BRepBuilderAPI_MakeWire MW;
		TopTools_Array1OfShape tab(1,nbedges);
		for (i=1 ; i<=nbedges ; i++) {
			if (abuildplate.Sense()->Value(abuildplate.Order()->Value(i))==1) {
				BRepBuilderAPI_MakeEdge ME(abuildplate.Curves2d()->Value(abuildplate.Order()->Value(i)),
									support,
									Fronts->Value(abuildplate.Order()->Value(i))->LastParameter(),
									Fronts->Value(abuildplate.Order()->Value(i))->FirstParameter());
				TopoDS_Edge E = ME.Edge();
				BRepLib::BuildCurves3d(E);
				tab(abuildplate.Order()->Value(i)) = E;
				//MW.Add(E);
			}
			else {
				BRepBuilderAPI_MakeEdge ME(abuildplate.Curves2d()->Value(abuildplate.Order()->Value(i)),
									support,
									Fronts->Value(abuildplate.Order()->Value(i))->FirstParameter(),
									Fronts->Value(abuildplate.Order()->Value(i))->LastParameter());
				TopoDS_Edge E = ME.Edge();
				BRepLib::BuildCurves3d(E);
				tab(abuildplate.Order()->Value(i)) = E;
			}	
		}
		for (i=1 ; i<=nbedges ; i++) 
			MW.Add(TopoDS::Edge(tab(i)));
		TopoDS_Wire W;
		try{
		W=MW.Wire();
		}
		
		catch(StdFail_NotDone)
		{
			AfxMessageBox (L"Can't build wire!");
			return;
		}

		if (!(W.Closed())){
			AfxMessageBox (L"Wire is not closed!");
			return;
			//throw Standard_Failure("Wire is not closed");
			
		}
		BRepBuilderAPI_MakeFace MF(support,W,Standard_True);
		TopoDS_Face aface;
		aface = MF.Face();
		BRepTopAdaptor_FClass2d clas2d(aface,Precision::Confusion());
		if (clas2d.PerformInfinitePoint() == TopAbs_IN) {
			W.Reverse();
			BRepBuilderAPI_MakeFace MF1(support,W,Standard_True);
			aface = MF1.Face();
		}
		if (!BRepAlgo::IsValid(aface))
			MessageBoxW (AfxGetApp()->m_pMainWnd->m_hWnd, L"Error : The plate face is not valid!", L"CasCade Error", MB_ICONERROR);
		myState = -1;
		Handle(AIS_Shape) anAISShape = new AIS_Shape(aface);
		myAISContext->SetColor (anAISShape,Quantity_NOC_AZURE, Standard_False);
		myAISContext->SetMaterial (anAISShape, Graphic3d_NOM_SILVER, Standard_False);
    myAISContext->SetDisplayMode (anAISShape, 1, Standard_False);
		myAISContext->Display (anAISShape, Standard_True);
	}
}

void CModelingDoc::OnFillwithtang() 
{
	static BOOL flag = 0;
	if (flag == 1){
		flag = 0;
		Handle(AIS_InteractiveObject) aObject;
		myAISContext ->InitSelected();
		if(myAISContext->MoreSelected())
			aObject = myAISContext->SelectedInteractive();
		((OCC_MainFrame*)AfxGetMainWnd())->SetStatusMessage("Select a file with second face");
		if(OnFileImportBrep_WithInitDir (L"TangentSurface") == 1){
			((OCC_MainFrame*)AfxGetMainWnd())->SetStatusMessage("");
				AIS_ListOfInteractive aList;
				myAISContext->DisplayedObjects(aList);
				AIS_ListIteratorOfListOfInteractive aListIterator;
				for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
					myAISContext->Remove (aListIterator.Value(), Standard_False);
				}
        myAISContext->UpdateCurrentViewer();
				return;
			}
		myAISContext->SetSelected(aObject, Standard_True);
	}

	myAISContext->InitSelected();
	if (myAISContext->MoreSelected()) {
		Handle(AIS_Shape) ashape = Handle(AIS_Shape)::DownCast(myAISContext->SelectedInteractive());
		try {
                  THE_F1 = TopoDS::Face(ashape->Shape());
		}
	    catch(Standard_Failure){}
		if (THE_F1.IsNull())
                {
                    AfxMessageBox (L"Current object is not a face!\n\
Please, select a face to continue\nthe creation of a tangent surface.");
                    return;
                }
		myAISContext->Activate(ashape,2);
		myState = SELECT_EDGE_PLATE_TGTES_1;
		
		((OCC_MainFrame*)AfxGetMainWnd())->SetStatusMessage("Select an edge on the first face");
	}
	else {
		AIS_ListOfInteractive LI;
		myAISContext->DisplayedObjects(LI);
		if(LI.IsEmpty()){
			((OCC_MainFrame*)AfxGetMainWnd())->SetStatusMessage("Select a file with first face");
			if(OnFileImportBrep_WithInitDir (L"TangentSurface") == 1){
				((OCC_MainFrame*)AfxGetMainWnd())->SetStatusMessage("");
				AIS_ListOfInteractive aList;
				myAISContext->DisplayedObjects(aList);
				AIS_ListIteratorOfListOfInteractive aListIterator;
				for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
					myAISContext->Remove(aListIterator.Value(), Standard_False);
				}
        myAISContext->UpdateCurrentViewer();
				return;
			}
			((OCC_MainFrame*)AfxGetMainWnd())->SetStatusMessage("");
			myAISContext->DisplayedObjects(LI);
			myAISContext->SetSelected (LI.First(), Standard_True);
			Sleep(700);
			flag = 1;
			OnFillwithtang();
			return;
		}
		AfxMessageBox (L"Select a face before");
	}
}

void CModelingDoc::InputEvent(const Standard_Integer /*x*/,
                              const Standard_Integer /*y*/,
                              const Handle(V3d_View)& /*aView*/)
{
    myAISContext->Select (Standard_True);
	if (myState == SELECT_EDGE_PLATE_TGTES_1) {
		myAISContext->InitSelected();
 		if (myAISContext->MoreSelected()) {
 			THE_E1 = TopoDS::Edge(myAISContext->SelectedShape());
 			myState = SELECT_EDGE_PLATE_TGTES_2;
 			
			AIS_ListOfInteractive aLI;
			myAISContext->DisplayedObjects(aLI);
			if(aLI.Extent() == 2){
				myState = SELECT_EDGE_PLATE_TGTES_2;
				if (myAISContext->IsSelected(aLI.First()))
					myAISContext->SetSelected (aLI.Last(), Standard_True);
				else
					myAISContext->SetSelected (aLI.First(), Standard_True);
				myAISContext->InitSelected();
				Handle(AIS_Shape) ashape = Handle(AIS_Shape)::DownCast(myAISContext->SelectedInteractive());
 				THE_F2 = TopoDS::Face(ashape->Shape());
				myAISContext->Activate(ashape,2);
				myState = SELECT_EDGE_PLATE_TGTES_3;
				((OCC_MainFrame*)AfxGetMainWnd())->SetStatusMessage("Select an edge on the second face");
				return;
			}
			
			((OCC_MainFrame*)AfxGetMainWnd())->SetStatusMessage("Select second face");
			AIS_ListOfInteractive LI;
			myAISContext->DisplayedObjects(LI);
			if(LI.Extent() == 1){
				if(OnFileImportBrep_WithInitDir (L"TangentSurface") == 1)
				return;
			}
 		}
 		else
 			AfxMessageBox (L"Select an edge on the face!");
 
 	}
 	else if (myState == SELECT_EDGE_PLATE_TGTES_2) {
		myAISContext->InitSelected();
		if (myAISContext->MoreSelected()) {
			Handle(AIS_Shape) ashape = Handle(AIS_Shape)::DownCast(myAISContext->SelectedInteractive());
			THE_F2 = TopoDS::Face(ashape->Shape());
			myAISContext->Activate(ashape,2);
			myState = SELECT_EDGE_PLATE_TGTES_3;
			((OCC_MainFrame*)AfxGetMainWnd())->SetStatusMessage("Select an edge on the second face");
		}
		else 
			AfxMessageBox (L"Select the second face!");
	}
	else if (myState == SELECT_EDGE_PLATE_TGTES_3) {
		myAISContext->InitSelected();
		if (myAISContext->MoreSelected()) {
			THE_E2 = TopoDS::Edge(myAISContext->SelectedShape());

			Standard_Integer i, nbPntsOnFaces=10;
			Standard_Real u,First, Last, Delta, Tol=0.001, TolProj;
			Plate_Plate aPlate;
			gp_Vec V1,V2,W1,W2;
			gp_Pnt2d P2d;
			gp_Pnt P, PP;

			//get the pcurve, curve and surface
			BRepAdaptor_Curve   Curve3d1(THE_E1), Curve3d2(THE_E2);
			BRepAdaptor_Curve2d Curve2d1(THE_E1,THE_F1), Curve2d2(THE_E2,THE_F2);
			BRepAdaptor_Surface Surf1(THE_F1), Surf2(THE_F2);

			//compute the average plane : initial surface
			Handle(TColgp_HArray1OfPnt) theTanPoints = new
				TColgp_HArray1OfPnt (1,2*nbPntsOnFaces );

			Delta = (Curve3d1.LastParameter()-Curve3d1.FirstParameter())/(nbPntsOnFaces-1);
			for (u=Curve3d1.FirstParameter(),i=1;i<=nbPntsOnFaces; i++,u+=Delta)
				theTanPoints->SetValue(i,Curve3d1.Value(u));

			Delta = (Curve3d2.LastParameter()-Curve3d2.FirstParameter())/(nbPntsOnFaces-1);
			for (u=Curve3d2.FirstParameter(),i=1;i<=nbPntsOnFaces; i++,u+=Delta)
				theTanPoints->SetValue(nbPntsOnFaces+i,Curve3d2.Value(u));

			//Building an initial plane
			GeomPlate_BuildAveragePlane aMkPlane (theTanPoints,int(Tol),1,1,1);
			Handle(Geom_Plane) aPlane = aMkPlane.Plane();
			gp_Pln aPln = aPlane->Pln();
			gp_XYZ aNormale = aPln.Axis().Direction().XYZ();
			gp_Trsf aTrsf; // to compute the U and V of the points
			aTrsf.SetTransformation(aPln.Position());

			aPlane->D1(0,0,P,W1,W2); // extract plane DU & DV

			// 1st surface tangencies constraints
			Delta = (Curve3d1.LastParameter()-Curve3d1.FirstParameter())/(nbPntsOnFaces-1);
			for (u=Curve3d1.FirstParameter(),i=1; i<=nbPntsOnFaces; i++,u+=Delta) {
				P = Curve3d1.Value(u).Transformed(aTrsf);
				gp_XY UV(P.X(),P.Y());
				aPlate.Load(Plate_PinpointConstraint(UV,aNormale*P.Z()));
				Curve2d1.D0(u,P2d);
				Surf1.D1(P2d.X(),P2d.Y(),P,V1,V2);  // extract surface UV of the point
				aPlate.Load(Plate_GtoCConstraint(UV,
												 Plate_D1(W1.XYZ(),W2.XYZ()),
												 Plate_D1(V1.XYZ(),V2.XYZ())));
			}
			// 2nd surface 
			Delta = (Curve3d2.LastParameter()-Curve3d2.FirstParameter())/(nbPntsOnFaces-1);
			for (u=Curve3d2.FirstParameter(),i=1; i<=nbPntsOnFaces; i++,u+=Delta) {
				P = Curve3d2.Value(u).Transformed(aTrsf);
				gp_XY UV(P.X(),P.Y());
				aPlate.Load(Plate_PinpointConstraint(UV,aNormale*P.Z()));

				Curve2d2.D0(u,P2d);
				Surf2.D1(P2d.X(),P2d.Y(),P,V1,V2); 
				aPlate.Load(Plate_GtoCConstraint(UV,
												 Plate_D1(W1.XYZ(),W2.XYZ()),
												 Plate_D1(V1.XYZ()*-1,V2.XYZ()*-1)));
			}

			((OCC_MainFrame*)AfxGetMainWnd())->SetStatusMessage("Select a file with passing points");
			//Some passing points
			CFileDialog dlg(TRUE,
					NULL,
					NULL,
					OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
					L"Points Files (*.pass)|*.pass; |All Files (*.*)|*.*||", 
					NULL);

			CString initdir;
			initdir.GetEnvironmentVariable (L"CSF_OCCTDataPath");
			initdir += L"\\occ";

			dlg.m_ofn.lpstrInitialDir = initdir;

			if (dlg.DoModal() == IDOK) 
			{
				((OCC_MainFrame*)AfxGetMainWnd())->SetStatusMessage("Building the tangent surface...");
				SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
				CString filename = dlg.GetPathName();
				std::filebuf fic;
				std::istream in(&fic);  
				if (!fic.open(filename, std::ios::in))
					MessageBoxW (AfxGetApp()->m_pMainWnd->m_hWnd, L"Error : Unable to open file", L"CasCade Error", MB_ICONERROR);
				Standard_Real x,y,z;
				BRep_Builder B;
				TopoDS_Compound C;
				B.MakeCompound(C);
				while (!in.fail()|| !in.eof()){
					if (in >> x && in >> y && in >> z){
						PP = gp_Pnt(x, y, z);
						P = PP.Transformed(aTrsf);
						aPlate.Load(Plate_PinpointConstraint(gp_XY(P.X(),P.Y()),
															aNormale*P.Z()));
						BRepBuilderAPI_MakeVertex V(PP);
						B.Add(C,V.Vertex());	    
					}
				}
				fic.close();
 				Handle(AIS_Shape) anAISCompound = new AIS_Shape(C);
				myAISContext->Display(anAISCompound, Standard_False);
				Fit();
				Sleep(500);
			}
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
			((OCC_MainFrame*)AfxGetMainWnd())->SetStatusMessage("Building the tangent surface...");
			//Solving ... 
			Standard_Integer Order = 3; // constraints continuity + 2
			aPlate.SolveTI(Order,1.);
			if (!aPlate.IsDone()){
				MessageBoxW (AfxGetApp()->m_pMainWnd->m_hWnd, L"Error : Build plate not valid!", L"CasCade Error", MB_ICONERROR);
				return;
			}
			//Plate Surface creation 
			Handle(GeomPlate_Surface) aPlateSurface = new GeomPlate_Surface(aPlane,aPlate);
			//BSplineSurface approximation 
			GeomPlate_MakeApprox aMkSurf(aPlateSurface,Tol,1,8,0.1,1);
			Handle(Geom_Surface) theSurface =aMkSurf.Surface();

			// Face building ... 
			Handle(Geom2d_Curve)C1,C2,C3,C4;
			Handle(Geom_Curve)C;
			C = BRep_Tool::Curve(THE_E1,First,Last);
			TolProj = 0.01;
			C1 = GeomProjLib::Curve2d(C,First,Last,theSurface,TolProj);
			TopoDS_Edge Ed1 = BRepBuilderAPI_MakeEdge(C1,theSurface).Edge();

			C = BRep_Tool::Curve(THE_E2,First,Last);
			TolProj = 0.01;
			C3 = GeomProjLib::Curve2d(C,First,Last,theSurface,TolProj);
			TopoDS_Edge Ed3 = BRepBuilderAPI_MakeEdge(C3,theSurface).Edge();
			
			C2 = GCE2d_MakeSegment(C1->Value(C1->FirstParameter()),
								C3->Value(C3->FirstParameter())).Value();
			TopoDS_Edge Ed2 = BRepBuilderAPI_MakeEdge(C2,theSurface).Edge();
			C4 = GCE2d_MakeSegment(C1->Value(C1->LastParameter()),
								C3->Value(C3->LastParameter())).Value();
			TopoDS_Edge Ed4 = BRepBuilderAPI_MakeEdge(C4,theSurface).Edge();
			Ed2.Reverse();
			Ed3.Reverse();
			TopoDS_Wire theWire = BRepBuilderAPI_MakeWire(Ed1,Ed2,Ed3,Ed4);
			TopoDS_Face theFace = BRepBuilderAPI_MakeFace(theWire);
			BRepLib::BuildCurves3d(theFace);
			if (!BRepAlgo::IsValid(theFace)){
				C2 = GCE2d_MakeSegment(C1->Value(C1->LastParameter()),
										C3->Value(C3->FirstParameter())).Value();
				Ed2 = BRepBuilderAPI_MakeEdge(C2,theSurface).Edge();
				C4 = GCE2d_MakeSegment(C3->Value(C3->LastParameter()),
								C1->Value(C1->FirstParameter())).Value();
				Ed4 = BRepBuilderAPI_MakeEdge(C4,theSurface).Edge();
				Ed3.Reverse();
				theWire = BRepBuilderAPI_MakeWire(Ed1,Ed2,Ed3,Ed4);
				theFace = BRepBuilderAPI_MakeFace(theWire);
				BRepLib::BuildCurves3d(theFace);
				if (!BRepAlgo::IsValid(theFace))
					MessageBoxW (AfxGetApp()->m_pMainWnd->m_hWnd, L"Error : The plate surface is not valid!!!", L"CasCade Error", MB_ICONERROR);
			}

			Handle(AIS_Shape) anAISShape=new AIS_Shape(theFace);
			myAISContext->SetColor (anAISShape, Quantity_NOC_BLUE1, Standard_False);
			myAISContext->SetMaterial (anAISShape, Graphic3d_NOM_SILVER, Standard_False);
			myAISContext->SetDisplayMode (anAISShape, 1, Standard_False);
			myAISContext->Display (anAISShape, Standard_False);
			myState = -1;
		}
		else
			AfxMessageBox (L"Select an edge on the second face!");
		((OCC_MainFrame*)AfxGetMainWnd())->SetStatusMessage("");

	}
}

void CModelingDoc::Popup(const Standard_Integer  x,
					     const Standard_Integer  y ,
                         const Handle(V3d_View)& aView)
{
  Standard_Integer PopupMenuNumber=0;
  myAISContext->InitSelected();
  if (myState == SELECT_EDGE_PLATE) 
    PopupMenuNumber=2;
  else if (myAISContext->MoreSelected())
    PopupMenuNumber=1;

  CMenu menu;
  VERIFY(menu.LoadMenu(IDR_Popup3D));
  CMenu* pPopup = menu.GetSubMenu(PopupMenuNumber);
  ASSERT(pPopup != NULL);

  if (PopupMenuNumber == 1) // more than 1 object.
  {
    bool OneOrMoreInShading = false;
	for (myAISContext->InitSelected();myAISContext->MoreSelected ();myAISContext->NextSelected ())
    if (myAISContext->IsDisplayed(myAISContext->SelectedInteractive(),1)) OneOrMoreInShading=true;
	if(!OneOrMoreInShading)
   	pPopup->EnableMenuItem(5, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
   }

 
  POINT winCoord = { x , y };
  Handle(WNT_Window) aWNTWindow=
  Handle(WNT_Window)::DownCast(aView->Window());
  ClientToScreen ( (HWND)(aWNTWindow->HWindow()),&winCoord);
  pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON , winCoord.x, winCoord.y , 
                         AfxGetMainWnd());
}


