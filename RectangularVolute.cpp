#include "stdafx.h"
#include "RectangularVolute.h"

#include <iostream>
#include <fstream>
#include <vector>

#include <BRepBuilderAPI_Sewing.hxx>
#include <ShapeAnalysis_Shell.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <ShapeFix_Solid.hxx>
#include <ChFi2d_FilletAPI.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepOffsetAPI_MakePipeShell.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <GeomAPI_Interpolate.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <ShapeFix_Shape.hxx>

// ButtonClick to start volute program
TopoDS_Shape RectangularVolute::createRectangularVolute()
{
	CreateVoluteWithoutFillets();
	TotalVolumeWithoutInOut();
	CreateRectangularExit();

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
	filletedScrollShape_Vec = CreateVoluteWith2dFillets();// rectangular volute
	//volute.CreateVoluteWith2dFilletsWithBase();
	getVerticesFromEdges();

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
		TopoDS_Face circularExit = CreateCircularExit();
		BRepTools::Write(circularExit, "circularExit.brep");
		circularExitvec.push_back(circularExit);
	}
	else
	{

		// filleted rectangular exit plane
		TopoDS_Face filletedRectangle = CreateFilletedRectangularExit();
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
			TopoDS_Shape curvedPipeWithCircularExit = CreateCurvedPipeWithCircularExit();
			BRepTools::Write(curvedPipeWithCircularExit, "curvedPipeWithCircularExit.brep");
			curvedPipeVecCirExit.push_back(curvedPipeWithCircularExit);

		}

		else
		{
			TopoDS_Shape curvedPipeWithRectangularExit = CreateCurvedPipeWithRectExit();
			BRepTools::Write(curvedPipeWithRectangularExit, "curvedPipeWithRectangularExit.brep");
			curvedPipeVecRectExit.push_back(curvedPipeWithRectangularExit);

		}

	}
	else
	{
		if (isCircularExit)
		{
			TopoDS_Shape straightPipeWithCircularExit = CreateStraightExitPipeWithCircularExit();
			BRepTools::Write(straightPipeWithCircularExit, "straightPipeWithCircularExit.brep");
			straightPipeVecCirExit.push_back(straightPipeWithCircularExit);
		}
		else
		{
			// filleted straight pipe
			TopoDS_Shape filletedStraightPipe = CreateStraightExitPipeWith2dFilletedWire();
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
	TopoDS_Shape sewedVolute = SewBaseScrollWithExitPipe(my_filleted2dScrollShapesStraightPipe, my_inputPlaneFaceList, straightPipeVecRectExit, filletedRectangularExitVec, 1.E-06);
	BRepTools::Write(sewedVolute, "sewedVolute.brep");

	TopoDS_Solid SolidVolute = GenVolute->mkSolidOfSewedVoluteWithExitPipe(sewedVolute);
	BRepTools::Write(SolidVolute, "SolidVolute.brep");

	ShapeFix_Shape fixedShape = ShapeFix_Shape(SolidVolute);

	TopoDS_Shape ReversedSolidVolute = GenVolute->ReverseShapeIfInsideOut(SolidVolute);
	BRepTools::Write(ReversedSolidVolute, "ReversedSolidVolute.brep");

	return ReversedSolidVolute;

}


// Create volute without fillets
std::vector<TopoDS_Shape> RectangularVolute::CreateVoluteWithoutFillets()
{
	GenVolute = new GenerateVolute1();

	// Initializing my_numOfSections
	GenVolute->my_numOfSections = 60;

	// Calling the AreaCalculation function
	std::vector<double> areasOfSections = GenVolute->AreaCalculation(3000, 900);

	// Calling the CreateSection function
	std::vector<TopoDS_Wire> sectionVector = CreateSection(25, areasOfSections);

	std::vector<double> angleVector = GenVolute->CreateAngleVector();

	// Calling the CreateSection function
	my_sectionWireList = GenVolute->RotateCrossSections(sectionVector, angleVector);

	// Drawing the wired compound shape
	TopoDS_Shape compoundShapeWithBaseWire = GenVolute->CreateCompoundShape(my_sectionWireList);
	BRepTools::Write(compoundShapeWithBaseWire, "compoundShape.brep");

	// Adding the created shells to a vector
	GenVolute->my_shellVector = GenVolute->CreateShellList(my_sectionWireList, angleVector);

	std::vector<TopoDS_Shape> solidVectorOfScrollShape = GenVolute->CreateSolidList(my_sectionWireList, angleVector);

	TopoDS_Shape baseScroll = CreateBasePlane();

	return solidVectorOfScrollShape;
}

// Create volute with 2D fillets without base
std::vector<TopoDS_Shape> RectangularVolute::TotalVolumeWithoutInOut(/*std::vector<TopoDS_Shape> &thruSectPipe*/)
{

	std::vector<double> sectionArea = GenVolute->AreaCalculation(3000, 900);

	std::vector<TopoDS_Wire> sectionsWithoutBase = CreateSectionWithoutBase(25, sectionArea);

	std::vector<double> angleVector = GenVolute->CreateAngleVector();

	std::vector<TopoDS_Wire> filleted2dWireVec = createFilletedCrossSections(sectionsWithoutBase);

	std::vector<TopoDS_Wire> rotatedWireWithoutBase = GenVolute->RotateCrossSections(sectionsWithoutBase, angleVector);

	// Drawing the wired compound shape
	TopoDS_Shape compoundShape = GenVolute->CreateCompoundShape(rotatedWireWithoutBase);
	BRepTools::Write(compoundShape, "WireWithoutBase.brep");

	//my_scrollVectorWithoutBoolCut.clear();
	my_scrollVectorWithoutBoolCut = GenVolute->CreateShellList(rotatedWireWithoutBase, angleVector);

	// Applying fillet to scroll shapes
	//my_filletedScrollShape.clear();
	my_filletedScrollShape = mkFilletToScrollShapes(my_scrollVectorWithoutBoolCut);

	std::vector<TopoDS_Shape> solidVectorOfScrollShape = GenVolute->CreateSolidList(my_sectionWireList, angleVector);

	// Applying fillet to solid scrolls
	my_filletSolidScrolls = mkFilletToSolidScroll(solidVectorOfScrollShape);

	// creating solid vector of scroll shapes
	std::vector<TopoDS_Shape> solidVec = CreateSolidVectorWithBoolCut(my_filletSolidScrolls[0]);

	// creating the filleted translation
	GenVolute->my_filletedWireForTrans = GetFilletedWireForTrans(my_filletSolidScrolls[0]);
	BRepTools::Write(GenVolute->my_filletedWireForTrans, "filletedWire.brep");

	// creating a solid shape vector without boolean fuse
	std::vector<TopoDS_Shape> solidVectorWithoutBoolFuse = CreateSolidScrollVector();

	return my_scrollVectorWithoutBoolCut;

}

// Create volute with 2D fillets with base
std::vector<TopoDS_Shape> RectangularVolute::CreateVoluteWith2dFillets()
{


	std::vector<double> sectionArea = GenVolute->AreaCalculation(3000, 900);

	std::vector<TopoDS_Wire> sectionsWithoutBase = CreateSectionWithoutBase(25, sectionArea);

	std::vector<double> angleVector = GenVolute->CreateAngleVector();

	std::vector<TopoDS_Wire> filleted2dWireVec = createFilletedCrossSections(sectionsWithoutBase);

	std::vector<TopoDS_Wire> wireWithBase2dFillet = Create2dFilletedWireWithBase(filleted2dWireVec);
	BRepTools::Write(wireWithBase2dFillet[50], "wireWithBase2dFillet[50].brep");

	rotated2dFilletWireWithoutBase = GenVolute->RotateCrossSections(filleted2dWireVec, angleVector);

	std::vector<TopoDS_Wire> rotated2dFilletWireWithBase = GenVolute->RotateCrossSections(wireWithBase2dFillet, angleVector);

	// Drawing the wired compound shape
	TopoDS_Shape compoundShape = GenVolute->CreateCompoundShape(rotated2dFilletWireWithoutBase);
	BRepTools::Write(compoundShape, "WireWithoutBase.brep");

	TopoDS_Shape compoundShapeWithBase = GenVolute->CreateCompoundShape(rotated2dFilletWireWithBase);
	BRepTools::Write(compoundShapeWithBase, "compoundShapeWithBase.brep");

	scrollVectorWithoutBoolCut = GenVolute->CreateShellList(rotated2dFilletWireWithoutBase, angleVector);

	solidVector2dFillet = GenVolute->CreateSolidList(rotated2dFilletWireWithBase, angleVector);

	return scrollVectorWithoutBoolCut;
}

// Create straight exit pipe with 2D filleted wire
TopoDS_Shape RectangularVolute::CreateStraightExitPipeWith2dFilletedWire()
{
	//getVerticesFromEdges();
	std::vector<TopoDS_Edge> edgesWith2dFillets = create2dFilletsForSquare(6);
	my_rectWire2dFillets = Create2dWire(edgesWith2dFillets);
	BRepTools::Write(my_rectWire2dFillets, "wire2dFillets.brep");

	// Getting the translation of the square
	gp_Vec myVec(0, -200, 0);
	TopoDS_Wire trnsfWire = GenVolute->GetTranslationOfWire(my_rectWire2dFillets, myVec);
	BRepTools::Write(trnsfWire, "trnsf2dWire.brep");
	BRepBuilderAPI_MakeFace squarePlane(trnsfWire, true);
	BRepTools::Write(squarePlane, "squarePlaneWith2dFillets.brep");
	my_exitPlanes.push_back(squarePlane);

	// Getting the largest section of the section wire list
	TopoDS_Wire wireWith2dFillets = GetLargest2dFilletWire();
	BRepTools::Write(wireWith2dFillets, "largestWire.brep");

	// ThruSecting the exit pipe
	TopoDS_Shape ThruSectPipeShape = GenVolute->ThruSectExitPipe(wireWith2dFillets, trnsfWire, Standard_False);
	TopoDS_Shape ThruSectPipeShapeSolid = GenVolute->ThruSectExitPipe(wireWith2dFillets, trnsfWire, Standard_True);
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
TopoDS_Shape RectangularVolute::CreateStraightExitPipeWithCircularExit()
{
	//getVerticesFromEdges();
	gp_Pnt midPoint = GetMiddlePoint(my_sectionWireList[0]);
	TopoDS_Wire circleWire = CreateCircleToGetExitPipe(midPoint, 10000);

	// Getting the translation of the square
	gp_Vec myVec(0, -200, 0);
	TopoDS_Wire circTrnsfWire = GenVolute->GetTranslationOfWire(circleWire, myVec);
	BRepTools::Write(circTrnsfWire, "circTrnsfWire.brep");
	BRepBuilderAPI_MakeFace circlePlane(circTrnsfWire, true);
	BRepTools::Write(circlePlane, "circlePlane.brep");
	my_exitPlanes.push_back(circlePlane);

	// Getting the largest section of the section wire list
	TopoDS_Wire wireWith2dFillets = GetLargest2dFilletWire();
	BRepTools::Write(wireWith2dFillets, "largestWire.brep");

	// ThruSecting the exit pipe
	TopoDS_Shape ThruSectPipeShape = GenVolute->ThruSectExitPipe(wireWith2dFillets, circTrnsfWire, Standard_False);
	TopoDS_Shape ThruSectPipeShapeSolid = GenVolute->ThruSectExitPipe(wireWith2dFillets, circTrnsfWire, Standard_True);
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
TopoDS_Shape RectangularVolute::CreateStraightPipe()
{


	// Getting the middle point of the square
	gp_Pnt midPoint = GetMiddlePoint(my_sectionWireList[0]);
	//edgesOfSquare.clear();
	TopoDS_Wire squareWire = CreateSquareToGetExitPipe(midPoint, 3000);
	BRepTools::Write(squareWire, "squareWire.brep");

	// Getting the translation of the square
	gp_Vec myVec(0, -200, 0);
	TopoDS_Wire trnsfWire = GenVolute->GetTranslationOfWire(squareWire, myVec);
	BRepTools::Write(trnsfWire, "trnsfWire.brep");
	BRepBuilderAPI_MakeFace squarePlane(trnsfWire, true);
	BRepTools::Write(squarePlane, "squarePlane.brep");
	my_exitPlanes.push_back(squarePlane);

	// Getting the largest section of the section wire list
	TopoDS_Shape largestSection = my_sectionWireList[0];
	TopoDS_Wire wire = TopoDS::Wire(largestSection);

	// ThruSecting the exit pipe
	TopoDS_Shape ThruSectPipeShape = GenVolute->ThruSectExitPipe(wire, trnsfWire, Standard_False);
	my_thruSectPipeVec.push_back(ThruSectPipeShape);
	BRepTools::Write(ThruSectPipeShape, "ThruSectPipeShape.brep");

	// Getting the boolean cut of exit pipe
	TopoDS_Solid smallestSolidOfScrollShape = TopoDS::Solid(GenVolute->my_solidVector[3]);
	BRepTools::Write(smallestSolidOfScrollShape, "solidvec3.brep");
	my_exitPipeBooleanCut = GetSimpleBooleanResult(ThruSectPipeShape, smallestSolidOfScrollShape);
	BRepTools::Write(my_exitPipeBooleanCut, "exitPipeBooleanCut.brep");

	// Getting the boolean cut of the smallest part of the scroll
	my_ThruSectPipeSolidToBe = GenVolute->ThruSectExitPipe(wire, trnsfWire, Standard_True);
	TopoDS_Solid solidPipe = TopoDS::Solid(my_ThruSectPipeSolidToBe);
	my_SmallShellBooleanCut = GetSimpleBooleanResult(GenVolute->my_shellVector[3], solidPipe);
	BRepTools::Write(my_SmallShellBooleanCut, "SmallShellBooleanCut.brep");

	return my_exitPipeBooleanCut;
}

// Create 3D filleted straight exit pipe
TopoDS_Shape RectangularVolute::CreateFilletedStraightPipe()
{

	// Thrusecting the filleted exit pipe
	TopoDS_Shape FilletedThruSectPipeShape = GenVolute->ThruSectExitPipe(GenVolute->my_filletedWireForTrans, my_filletedSqaureWireVec[0], Standard_False);
	my_filletedThruSectPipeVec.push_back(FilletedThruSectPipeShape);
	BRepTools::Write(FilletedThruSectPipeShape, "FilletedThruSectPipe.brep");

	TopoDS_Shape FilletedThruSectSolidPipe = GenVolute->ThruSectExitPipe(GenVolute->my_filletedWireForTrans, my_filletedSqaureWireVec[0], Standard_True);
	// converting the shell pipe to solid 
	TopoDS_Solid squareSolidPipe = TopoDS::Solid(FilletedThruSectSolidPipe);
	BRepTools::Write(squareSolidPipe, "filletedSquareSolidPipe.brep");

	// pushing back solid pipe to solid pipe vector
	my_solidPipeVec.push_back(squareSolidPipe);

	return FilletedThruSectSolidPipe;

}

// Create rectangular exit plane
TopoDS_Face RectangularVolute::CreateRectangularExit()
{

	// Getting the middle point of the square
	gp_Pnt midPoint = GetMiddlePoint(my_sectionWireList[0]);
	edgesOfSquare.clear();
	TopoDS_Wire squareWire = CreateSquareToGetExitPipe(midPoint, 3000);
	BRepTools::Write(squareWire, "squareWire.brep");

	// Getting the translation of the square
	gp_Vec myVec(0, -200, 0);
	TopoDS_Wire trnsfWire = GenVolute->GetTranslationOfWire(squareWire, myVec);
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
TopoDS_Face RectangularVolute::CreateFilletedRectangularExit()
{
	std::vector<TopoDS_Edge> edgesWith2dFillets = create2dFilletsForSquare(6);
	TopoDS_Wire rectWire2dFillets = Create2dWire(edgesWith2dFillets);
	BRepTools::Write(rectWire2dFillets, "wire2dFillets.brep");

	// Getting the translation of the square
	gp_Vec myVec(0, -200, 0);
	TopoDS_Wire trnsfWire = GenVolute->GetTranslationOfWire(rectWire2dFillets, myVec);
	BRepTools::Write(trnsfWire, "trnsf2dWire.brep");
	TopoDS_Face filletedsquarePlane = BRepBuilderAPI_MakeFace(trnsfWire, true);
	BRepTools::Write(filletedsquarePlane, "squarePlaneWith2dFillets.brep");
	my_exitPlanes.push_back(filletedsquarePlane);

	return filletedsquarePlane;
}

TopoDS_Shape RectangularVolute::CreateThruSect(TopoDS_Wire transitionStartWire, TopoDS_Solid smallScrollSolid, TopoDS_Shape smallScrollShell)
{
	// Getting the middle point of the square
	gp_Pnt midPoint = GetMiddlePoint(my_sectionWireList[0]);
	//edgesOfSquare.clear();
	TopoDS_Wire squareWire = CreateSquareToGetExitPipe(midPoint, 12000);
	BRepTools::Write(squareWire, "squareWire.brep");

	// Getting the translation of the square
	gp_Vec myVec(0, -400, 0);
	TopoDS_Wire trnsfWire = GenVolute->GetTranslationOfWire(squareWire, myVec);
	BRepTools::Write(trnsfWire, "trnsfWire.brep");

	// non-filleted ThruSecting the exit pipe
	TopoDS_Shape ThruSectPipe_NewShape = GenVolute->ThruSectExitPipe(transitionStartWire, trnsfWire, Standard_False);
	BRepTools::Write(ThruSectPipe_NewShape, "ThruSectPipe_NewShape.brep");

	TopoDS_Shape ThruSectPipeShapeSolid = GenVolute->ThruSectExitPipe(transitionStartWire, trnsfWire, Standard_True);
	TopoDS_Solid ThruSectPipeShapeSolidConverted = TopoDS::Solid(ThruSectPipeShapeSolid);
	BRepTools::Write(ThruSectPipeShapeSolidConverted, "ThruSectPipeShapeSolid.brep");

	TopoDS_Shape exitPipeBooleanCut = GetSimpleBooleanResult(ThruSectPipe_NewShape, smallScrollSolid);
	BRepTools::Write(exitPipeBooleanCut, "exitPipeBooleanCut.brep");

	TopoDS_Shape smallScrollBooleanCut = GetSimpleBooleanResult(smallScrollShell, ThruSectPipeShapeSolidConverted);
	BRepTools::Write(smallScrollBooleanCut, "smallScrollBooleanCut.brep");

	return exitPipeBooleanCut;
}

// Create circular exit plane
TopoDS_Face RectangularVolute::CreateCircularExit()
{
	gp_Pnt midPoint = GetMiddlePoint(my_sectionWireList[0]);
	TopoDS_Wire circleWire = CreateCircleToGetExitPipe(midPoint, 10000);

	// Getting the translation of the square
	gp_Vec myVec(0, -200, 0);
	TopoDS_Wire circTrnsfWire = GenVolute->GetTranslationOfWire(circleWire, myVec);
	BRepTools::Write(circTrnsfWire, "circTrnsfWire.brep");
	BRepBuilderAPI_MakeFace circlePlane(circTrnsfWire, true);
	BRepTools::Write(circlePlane, "circlePlane.brep");
	my_exitPlanes.push_back(circlePlane);

	TopoDS_Face circularExitPlane = circlePlane;

	return circlePlane;
}

// Create the curved exit pipe
TopoDS_Shape RectangularVolute::CreateCurvedPipeWithCircularExit()
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
TopoDS_Shape RectangularVolute::CreateCurvedPipeWithRectExit()
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
TopoDS_Shape RectangularVolute::CreateBasePlane()
{
	// Calculating area to create input planes
	std::vector<double> sectionArea = GenVolute->AreaCalculation(3000, 900);

	// Creating base sections 
	std::vector<TopoDS_Wire> baseSections = CreateBaseSections(25, sectionArea);

	std::vector<double> angleVector = GenVolute->CreateAngleVector();

	// Rotating the cross-section wires
	BaseRotatedWire = GenVolute->RotateCrossSections(baseSections, angleVector);

	// Drawing the wired compound shape
	TopoDS_Shape compoundShape = GenVolute->CreateCompoundShape(BaseRotatedWire);
	BRepTools::Write(compoundShape, "BasePlaneWire.brep");

	// Adding the created shells to a vector
	GenVolute->my_inputPlaneList = GenVolute->CreateInputPlanes(BaseRotatedWire, angleVector);
	// Creating a faceList out of the shellList
	my_inputPlaneFaceList = CreateInputPlaneFaces(GenVolute->my_inputPlaneList);

	// Sewing the shells
	TopoDS_Shape baseShell = SewingScrollShells(GenVolute->my_inputPlaneList, 1.E-06);
	BRepTools::Write(baseShell, "BaseShell.brep");

	TopoDS_Solid solidBaseShell = GenVolute->mkSolidOfSewedVoluteWithExitPipe(baseShell);
	BRepTools::Write(solidBaseShell, "solidBaseShell.brep");

	TopoDS_Shape reversedSolidBaseShell = GenVolute->ReverseShapeIfInsideOut(solidBaseShell);
	BRepTools::Write(reversedSolidBaseShell, "reversedSolidBaseShell.brep");

	return baseShell;
}


// Creating cross-sections with the linear decrement of areas with input faces
std::vector<TopoDS_Wire> RectangularVolute::CreateSection(double width, std::vector<double> area)
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

	delete GenVolute;
}

// Creating cross-sections with the linear decrement of areas without input faces
std::vector<TopoDS_Wire> RectangularVolute::CreateSectionWithoutBase(double width, std::vector<double> area)
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
std::vector<TopoDS_Wire> RectangularVolute::CreateBaseSections(double width, std::vector<double> area)
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


// Sewing the created scroll shells to form one shape
TopoDS_Shape RectangularVolute::SewingScrollShells(std::vector<TopoDS_Shape> shellList, double tolerance)
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
gp_Pnt RectangularVolute::GetMiddlePoint(TopoDS_Wire sectionWires)
{
	//my_sectionWireList[0]
	BRepBuilderAPI_MakeFace mkFace(sectionWires, true);
	TopoDS_Face planeFace = mkFace.Face();
	GProp_GProps SProps;

	//double height = area / my_widthNewShape;

	gp_Pnt centre_pnt(150, 0, 25);
	BRepGProp::SurfaceProperties(planeFace, SProps);
	centre_pnt = SProps.CentreOfMass();
	TopoDS_Vertex V = BRepBuilderAPI_MakeVertex(centre_pnt);
	BRepTools::Write(V, "centrePnt.brep");

	return centre_pnt;
}

// Create a square for the exit plane
TopoDS_Wire RectangularVolute::CreateSquareToGetExitPipe(gp_Pnt centrePnt, double area)
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
TopoDS_Wire RectangularVolute::GetLargest2dFilletWire()
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
void RectangularVolute::getVerticesFromEdges()
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


// Applying 2D Fillets for the rectangle
std::vector<TopoDS_Edge> RectangularVolute::create2dFilletsForSquare(double radius)
{
	gp_Pnt commonPoint1 = GenVolute->getCommonPoint(pointVec1, pointVec2);
	gp_Pnt commonPoint2 = GenVolute->getCommonPoint(pointVec2, pointVec3);
	gp_Pnt commonPoint3 = GenVolute->getCommonPoint(pointVec3, pointVec4);
	gp_Pnt commonPoint4 = GenVolute->getCommonPoint(pointVec4, pointVec1);

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
TopoDS_Wire RectangularVolute::Create2dWire(std::vector<TopoDS_Edge> edgeVec)
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

// Create circular exit wire
TopoDS_Wire RectangularVolute::CreateCircleToGetExitPipe(gp_Pnt centrePnt, double area)
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
TopoDS_Face RectangularVolute::CreateSquareToFormFilletPipe(std::vector<TopoDS_Shape> exitPipe)
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

// Get the 3D filleted wire for translated square
TopoDS_Wire RectangularVolute::GetFilletedWireForTrans(TopoDS_Shape shape)
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



// Get the sewed exit pipe
TopoDS_Shape RectangularVolute::SewedExitPipe(TopoDS_Shape shape1, TopoDS_Shape shape2, double tolerance)
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



// boolean cut for a shell without fillets
TopoDS_Shape RectangularVolute::GetSimpleBooleanResult(TopoDS_Shape shape, TopoDS_Solid solid)
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
TopoDS_Shape RectangularVolute::GetSimpleBooleanResultForFilletedShapes(TopoDS_Shape shape, TopoDS_Shape solid)
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
TopoDS_Shape RectangularVolute::GetBooleanResultForSolids(TopoDS_Shape shape, TopoDS_Shape solid)
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
TopoDS_Shape RectangularVolute::GetBooleanFuseForSolids(TopoDS_Shape shape, TopoDS_Shape solid)
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

// Create input plane faces from input plane shapes
std::vector<TopoDS_Face> RectangularVolute::CreateInputPlaneFaces(std::vector<TopoDS_Shape> baseScroll)
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

// Applying fillets to hollow scroll shape vector
std::vector<TopoDS_Shape> RectangularVolute::mkFilletToScrollShapes(std::vector<TopoDS_Shape> scrollShapeVec)
{
	TopoDS_Shape filletedShape;
	std::vector<TopoDS_Shape> scrollShapes;
	for (int i = 0; i < scrollShapeVec.size(); i++)
	{
		filletedShape = ApplyFilletScrollShapes(scrollShapeVec[i], 6);
		BRepTools::Write(filletedShape, "filletedScrollShape.brep");
		scrollShapes.push_back(filletedShape);
	}

	return scrollShapes;
}

// Create pipe shell from translated wire
TopoDS_Shape RectangularVolute::CreatePipeShellFromTrnsWire(TopoDS_Wire curvedWire, TopoDS_Wire wire1, TopoDS_Wire wire2)
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
TopoDS_Shape RectangularVolute::CreatePipeSoildFromTrnsWire(TopoDS_Wire curvedWire, TopoDS_Wire wire1, TopoDS_Wire wire2)
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
TopoDS_Shape RectangularVolute::SewVoluteWithCurvedPipe(std::vector<TopoDS_Shape> sewScroll, std::vector<TopoDS_Shape> exitPipe, double tolerance)
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

// sewing scroll shapes, exit pipe and input planes
TopoDS_Shape RectangularVolute::SewBaseScrollWithExitPipe(std::vector<TopoDS_Shape> scrollShells, std::vector<TopoDS_Face> inputPlaneList, std::vector<TopoDS_Shape> exitPipe, std::vector<TopoDS_Face> exitPlane, double tolerance)
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
TopoDS_Shape RectangularVolute::SewVoluteFaces(std::vector<TopoDS_Face> sewScroll, std::vector<TopoDS_Face> baseScroll, std::vector<TopoDS_Face> exitPipe, std::vector<TopoDS_Face> exitPlane, double tolerance)
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
TopoDS_Shape RectangularVolute::SewSolidVolute(std::vector<TopoDS_Shape> sewScroll, std::vector<TopoDS_Shape> exitPipe, double tolerance)
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
std::vector<TopoDS_Face> RectangularVolute::GetFacesFromShapes(std::vector<TopoDS_Shape> shapeVec)
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
std::vector<TopoDS_Shape> RectangularVolute::CreateShellVectorAfterBooleanCut()
{
	//GenerateVolute volute;
	std::vector<TopoDS_Shape> shellVectorWithBooleanCut;
	shellVectorWithBooleanCut.clear();

	// adding exitPipeBooleanCut to a separate vector
	my_vectorExitPipeBooleanCut.push_back(my_exitPipeBooleanCut);

	shellVectorWithBooleanCut.push_back(my_exitPipeBooleanCut);
	shellVectorWithBooleanCut.push_back(my_SmallShellBooleanCut);
	shellVectorWithBooleanCut.push_back(GenVolute->my_shellVector[2]);
	BRepTools::Write(GenVolute->my_shellVector[2], "testShell2.brep");
	shellVectorWithBooleanCut.push_back(GenVolute->my_shellVector[1]);
	BRepTools::Write(GenVolute->my_shellVector[1], "tetsShell3.brep");
	shellVectorWithBooleanCut.push_back(GenVolute->my_shellVector[0]);
	BRepTools::Write(GenVolute->my_shellVector[0], "testShell4.brep");

	return shellVectorWithBooleanCut;
}

gp_Vec RectangularVolute::CreateVector(TopoDS_Edge edge)
{
	double U1, U2;
	gp_Pnt lastPnt, firstPnt;
	Handle_Geom_Curve bsCurve = BRep_Tool::Curve(edge, U1, U2);
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
TopoDS_Wire RectangularVolute::CreateTrnslWire(TopoDS_Wire curvedWire, TopoDS_Wire wire1)
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


// Applying fillets to one solid pipe shape
TopoDS_Shape RectangularVolute::ApplyFilletSolidPipe(TopoDS_Shape solidPipe, double radius)
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
TopoDS_Shape RectangularVolute::mkFilletToSolidPipe(std::vector<TopoDS_Shape> solidPipeVec)
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
TopoDS_Shape RectangularVolute::ApplyFilletExitPipe(TopoDS_Shape exitPipe, double radius)
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
std::vector<TopoDS_Shape> RectangularVolute::mkFilletToExitPipe(std::vector<TopoDS_Shape> exitPipeVec)
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
TopoDS_Shape RectangularVolute::ApplyFilletSolidScroll(TopoDS_Shape scrollShape, double radius)
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
std::vector<TopoDS_Shape> RectangularVolute::mkFilletToSolidScroll(std::vector<TopoDS_Shape> solidScrollVec)
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
TopoDS_Wire RectangularVolute::MakeSpineCurve()
{
	gp_Pnt midPoint = GetMiddlePoint(my_sectionWireList[0]);
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
void RectangularVolute::GetVerticesToCreateCurve(gp_Pnt pointOne, gp_Pnt pointTwo, gp_Pnt pointThree)
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
TopoDS_Wire RectangularVolute::CreateBsplineWithThreePoints(gp_Pnt p1, gp_Pnt p2, gp_Pnt p3)
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


// Creating a solid scroll vector with the boolean cut
std::vector<TopoDS_Shape> RectangularVolute::CreateSolidVectorWithBoolCut(TopoDS_Shape smallBoolCutShape)
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
std::vector<TopoDS_Shape> RectangularVolute::CreateSolidScrollVector()
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
std::vector<TopoDS_Shape> RectangularVolute::CreateShellVectorWithBoolCut(TopoDS_Shape smallBoolCutShape, std::vector<TopoDS_Shape> scrollShapeVec)
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

// apply 2D fillets to the cross sections without base
TopoDS_Wire RectangularVolute::Apply2dFilletsForWireWithoutBase(TopoDS_Wire wire, double radius)
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
	for (int i = 0; i < edgeVec.size(); i++)
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
			if (i == 2)
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
		if (pointVec1[j].Distance(pointVec2[0]) < 1.0e-6)
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
std::vector<TopoDS_Wire> RectangularVolute::createFilletedCrossSections(std::vector<TopoDS_Wire> wireVec)
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
std::vector<TopoDS_Wire> RectangularVolute::Create2dFilletedWireWithBase(std::vector<TopoDS_Wire> filleted2dWireVec)
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
TopoDS_Shape RectangularVolute::ApplyFilletScrollShapes(TopoDS_Shape scrollShape, double radius)
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


