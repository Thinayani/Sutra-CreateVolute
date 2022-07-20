#pragma once

#include <vector>
#include <iostream>
#include <OCC_3dDoc.h>
#include <TopoDS_Shape.hxx>
#include <GenerateVolute1.h>



class RectangularVolute
{
public:
	GenerateVolute1* GenVolute;

public:
	TopoDS_Shape createRectangularVolute();

	std::vector<TopoDS_Edge> create2dFilletsForSquare(double radius);

	std::vector<TopoDS_Wire> CreateSection(double width, std::vector<double> array);
	std::vector<TopoDS_Wire> CreateSectionWithoutBase(double width, std::vector<double> area);
	std::vector<TopoDS_Wire> CreateBaseSections(double width, std::vector<double> area);
	std::vector<TopoDS_Wire> Create2dFilletedWireWithBase(std::vector<TopoDS_Wire> filleted2dWireVec);
	std::vector<TopoDS_Wire> createFilletedCrossSections(std::vector<TopoDS_Wire> wireVec);
	TopoDS_Wire Create2dWire(std::vector<TopoDS_Edge> edgeVec);
	TopoDS_Wire GetLargest2dFilletWire();
	TopoDS_Wire MakeSpineCurve();
	TopoDS_Wire CreateSquareToGetExitPipe(gp_Pnt centerPnt, double area);
	TopoDS_Wire CreateCircleToGetExitPipe(gp_Pnt centrePnt, double area);
	//TopoDS_Wire GetTranslationOfWire(TopoDS_Shape shape, gp_Vec vec);
	TopoDS_Wire GetFilletedWireForTrans(TopoDS_Shape shape);
	TopoDS_Wire Apply2dFilletsForWireWithoutBase(TopoDS_Wire wire, double radius);
	TopoDS_Wire CreateBsplineWithThreePoints(gp_Pnt p1, gp_Pnt p2, gp_Pnt p3);
	TopoDS_Wire CreateTrnslWire(TopoDS_Wire curvedWire, TopoDS_Wire wire1);

	std::vector<TopoDS_Shape> CreateVoluteWithoutFillets();
	std::vector<TopoDS_Shape> TotalVolumeWithoutInOut(/*std::vector<TopoDS_Shape> &thruSectPipe*/);
	std::vector<TopoDS_Shape> CreateVoluteWith2dFillets();
	std::vector<TopoDS_Shape> mkFilletToScrollShapes(std::vector<TopoDS_Shape> scrollShapeVec);
	std::vector<TopoDS_Shape> mkFilletToExitPipe(std::vector<TopoDS_Shape> exitPipeVec);
	std::vector<TopoDS_Shape> mkFilletToSolidScroll(std::vector<TopoDS_Shape> solid);
	std::vector<TopoDS_Shape> CreateShellVectorAfterBooleanCut();
	std::vector<TopoDS_Shape> CreateShellVectorWithBoolCut(TopoDS_Shape smallBoolCutShape, std::vector<TopoDS_Shape> scrollShapeVec);
	std::vector<TopoDS_Shape> CreateSolidVectorWithBoolCut(TopoDS_Shape smallBoolCutShape);
	std::vector<TopoDS_Shape> CreateSolidScrollVector();
	TopoDS_Shape SewingScrollShells(std::vector<TopoDS_Shape> shellList, double tolerance);
	//TopoDS_Shape ThruSectExitPipe(TopoDS_Wire wire1, TopoDS_Wire wire2, Standard_Boolean isSolid);
	TopoDS_Shape SewedExitPipe(TopoDS_Shape shape1, TopoDS_Shape shape2, double tolerance);
	TopoDS_Shape GetSimpleBooleanResultForFilletedShapes(TopoDS_Shape shape, TopoDS_Shape solid);
	TopoDS_Shape GetBooleanResultForSolids(TopoDS_Shape shape, TopoDS_Shape solid);
	TopoDS_Shape GetBooleanFuseForSolids(TopoDS_Shape shape, TopoDS_Shape solid);
	//TopoDS_Shape ReverseShapeIfInsideOut(TopoDS_Solid& solid);
	TopoDS_Shape SewBaseScrollWithExitPipe(std::vector<TopoDS_Shape> scrollShells, std::vector<TopoDS_Face> baseScroll, std::vector<TopoDS_Shape> exitPipe, std::vector<TopoDS_Face> exitPlane, double tolerance);
	TopoDS_Shape SewVoluteFaces(std::vector<TopoDS_Face> sewScroll, std::vector<TopoDS_Face> baseScroll, std::vector<TopoDS_Face> exitPipe, std::vector<TopoDS_Face> exitPlane, double tolerance);
	TopoDS_Shape SewSolidVolute(std::vector<TopoDS_Shape> sewScroll, std::vector<TopoDS_Shape> exitPipe, double tolerance);
	TopoDS_Shape CreateThruSect(TopoDS_Wire transitionStartWire, TopoDS_Solid smallScrollSolid, TopoDS_Shape smallScrollShell);
	TopoDS_Shape CreateBasePlane();
	TopoDS_Shape CreateFilletedStraightPipe();
	TopoDS_Shape CreateStraightExitPipeWithCircularExit();
	TopoDS_Shape CreateStraightPipe();
	TopoDS_Shape CreateCurvedPipeWithCircularExit();
	TopoDS_Shape CreateCurvedPipeWithRectExit();
	TopoDS_Shape CreateStraightExitPipeWith2dFilletedWire();
	TopoDS_Shape ApplyFilletScrollShapes(TopoDS_Shape scrollShape, double radius);
	TopoDS_Shape ApplyFilletSolidPipe(TopoDS_Shape solidPipe, double radius);
	TopoDS_Shape ApplyFilletExitPipe(TopoDS_Shape exitPipe, double radius);
	TopoDS_Shape ApplyFilletSolidScroll(TopoDS_Shape exitPipe, double radius);
	TopoDS_Shape mkFilletToSolidPipe(std::vector<TopoDS_Shape> solidPipeVec);
	TopoDS_Shape CreatePipeShellFromTrnsWire(TopoDS_Wire curvedWire, TopoDS_Wire wire1, TopoDS_Wire wire2);
	TopoDS_Shape CreatePipeSoildFromTrnsWire(TopoDS_Wire curvedWire, TopoDS_Wire wire1, TopoDS_Wire wire2);
	TopoDS_Shape SewVoluteWithCurvedPipe(std::vector<TopoDS_Shape> sewScroll, std::vector<TopoDS_Shape> exitPipe, double tolerance);
	TopoDS_Shape GetSimpleBooleanResult(TopoDS_Shape shape, TopoDS_Solid solid);

	std::vector<TopoDS_Face> CreateInputPlaneFaces(std::vector<TopoDS_Shape> baseScroll);
	std::vector<TopoDS_Face> GetFacesFromShapes(std::vector<TopoDS_Shape> shapeVec);
	TopoDS_Face CreateRectangularExit();
	TopoDS_Face CreateFilletedRectangularExit();
	TopoDS_Face CreateCircularExit();
	TopoDS_Face CreateSquareToFormFilletPipe(std::vector<TopoDS_Shape> exitPipe);

	void getVerticesFromEdges();
	void GetVerticesToCreateCurve(gp_Pnt pointOne, gp_Pnt pointTwo, gp_Pnt pointThree);
	gp_Vec CreateVector(TopoDS_Edge edge);
	//gp_Pnt getCommonPoint(std::vector<gp_Pnt> pointVec1, std::vector<gp_Pnt> pointVec2);
	gp_Pnt GetMiddlePoint(TopoDS_Wire sectionWires);

	std::vector<TopoDS_Edge> edgesOfSquare;
	std::vector<TopoDS_Wire> rotated2dFilletWireWithoutBase;
	std::vector<TopoDS_Wire> my_sectionWireList;
	std::vector<TopoDS_Wire> my_RotatedWireWithoutBaseWire;
	std::vector<TopoDS_Wire> my_filletedSqaureWireVec;
	std::vector<TopoDS_Wire> my_squareEdgeVec;
	std::vector<TopoDS_Wire> filleted2dWireVecWithBase;
	std::vector<TopoDS_Wire> BaseRotatedWire;
	std::vector<TopoDS_Wire> my_BaseWireList;
	TopoDS_Wire my_rectWire2dFillets;
	TopoDS_Wire my_circleWire;

	std::vector<TopoDS_Face> my_inputPlaneFaceList;
	std::vector<TopoDS_Face> my_filletedExitPlaneVec;
	std::vector<TopoDS_Face> my_curvedCircularExitVec;
	std::vector<TopoDS_Face> my_curvedRectangularExitWithFillets;
	std::vector<TopoDS_Face> my_curvedRectangularExit;

	std::vector<TopoDS_Shape> solidVector2dFillet;
	std::vector<TopoDS_Shape> scrollVectorWithoutBoolCut;
	std::vector<TopoDS_Shape> my_scrollVectorWithoutBoolCut;
	std::vector<TopoDS_Shape> my_exitPlanes;
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
	TopoDS_Shape my_exitPipeBooleanCut;
	TopoDS_Shape my_SmallShellBooleanCut;
	TopoDS_Shape my_ThruSectPipeSolidToBe;

	std::vector<gp_Pnt> pointVec1;
	std::vector<gp_Pnt> pointVec2;
	std::vector<gp_Pnt> pointVec3;
	std::vector<gp_Pnt> pointVec4;


};

