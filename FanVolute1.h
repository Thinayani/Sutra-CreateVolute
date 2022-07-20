#pragma once

#include <vector>
#include <iostream>
#include <OCC_3dDoc.h>
#include <TopoDS_Shape.hxx>
#include <GenerateVolute1.h>

class FanVolute1 : public OCC_3dDoc
{
public:
	GenerateVolute1* GenVolute;
	//RectangularVolute * rectVol;

public:
	void IterateEdgesOfSewedShape(TopoDS_Shape shape);
	void getVerticesFromEdges_New(TopoDS_Wire wire);
	void IterateEdgesFromTransitionExit(TopoDS_Shape shape1);

	TopoDS_Edge IterateEdgesOfaShape(TopoDS_Shape shape);

	std::vector<TopoDS_Wire> CreateWiresForAirExit(double angleOfSmallScroll);
	std::vector<TopoDS_Wire> createFilletedCrossSections_newShape(std::vector<TopoDS_Wire> wireVec);
	std::vector<TopoDS_Wire> CreateNewCrossSection(double L1, double L2, double L3, double L4, double L5, std::vector<double> area);
	TopoDS_Wire Apply2dFilletsForNewCrossSection(/*std::vector<TopoDS_Edge> newEdgeVec, */TopoDS_Wire wire, double radius);
	TopoDS_Wire CreateTransitionStartSection(double L1, double L2, double L3, double area);
	TopoDS_Wire CreateRectangleForAirExit(int numSections);
	TopoDS_Wire MakeWireFromEdges(std::vector<TopoDS_Edge> edgeVec);
	TopoDS_Wire CreateFilletedTranisitionExitWire(TopoDS_Shape shape);

	//std::vector<TopoDS_Shape> CreateNewCrossSection(int radius);
	std::vector<TopoDS_Shape> GetFaceListFromExitShape(TopoDS_Shape shape);
	std::vector<TopoDS_Shape> IterateFacesFromaShape(TopoDS_Shape shape1, TopoDS_Shape shape2);
	std::vector<TopoDS_Shape> mkFilletToNewScrollShapes(std::vector<TopoDS_Shape> scrollShapeVec, double radius);
	TopoDS_Shape ApplyFilletNewScrollShapes(TopoDS_Shape scrollShape, double radius);
	TopoDS_Shape ApplyFilletsToSewedShape(TopoDS_Shape shape, double radius);
	TopoDS_Shape ApplyFilletsToTranisition(TopoDS_Shape shape, double radius);
	TopoDS_Shape SewVoluteWith3DFillets(std::vector<TopoDS_Shape> scrollShells, std::vector<TopoDS_Face> smallScrollFaces, std::vector<TopoDS_Shape> exitPipe, TopoDS_Face transitionExit, TopoDS_Shape filletedSewedFaces, double tolerance);
	TopoDS_Shape CreateExitPipeThruSect(std::vector<TopoDS_Wire> wireVector, Standard_Boolean isSolid);
	TopoDS_Shape SewVoluteWithTransitionPipe(std::vector<TopoDS_Shape> scrollShells, std::vector<TopoDS_Face> smallScrollFaces, std::vector<TopoDS_Shape> exitPipe, std::vector<TopoDS_Face> newFaceVector, TopoDS_Face transitionExit, TopoDS_Shape filletedSewedFaces,/* std::vector<TopoDS_Shape> shapeFillingVec,*/ TopoDS_Shape transitionPart, double tolerance);
	TopoDS_Shape ApplyFilletNewVolute(TopoDS_Shape shapeToBeFilleted, std::vector<TopoDS_Edge> edgeVec, double radius);
	TopoDS_Shape SewFacesToGetShape(std::vector<TopoDS_Shape> faceVec, double tolerance);
	TopoDS_Shape ApplyFilletsToaShape(TopoDS_Shape shape, double radius);
	TopoDS_Shape ApplyFilletsToExitPipeWithoutBase(TopoDS_Shape airExitPipe, Standard_Real radius);

	std::vector<TopoDS_Face> IterateFacesOfTransitionPipe(TopoDS_Shape transitionShape);
	std::vector<TopoDS_Face> RemoveTopFaceOfScroll(/*TopoDS_Shape scrollShape*/ std::vector<TopoDS_Shape> scrollShapeVec);
	TopoDS_Face MakeFaceFromWireAndBaseSurface(TopoDS_Wire wire);
	TopoDS_Face MakeNewFaceFromWires(TopoDS_Wire wire);

	std::vector<gp_Pnt> GetTopVerticesOfRotatedSection(std::vector<TopoDS_Wire> wireVec);
	gp_Pnt GetMiddlePointForAirExit(TopoDS_Wire sectionWires, double L3);

	std::vector<gp_Pnt> gpPntP7vec;
	std::vector<gp_Pnt> gpPntP8vec;
	std::vector<gp_Pnt> TopWire_gpPntVec1;
	std::vector<gp_Pnt> TopWire_gpPntVec2;
	std::vector<gp_Pnt> newShape_pointVec1;
	std::vector<gp_Pnt> newShape_pointVec2;
	std::vector<gp_Pnt> newShape_pointVec3;
	std::vector<gp_Pnt> newShape_pointVec4;
	std::vector<gp_Pnt> newShape_pointVec5;

	std::vector<TopoDS_Edge> edgeVec1;
	std::vector<TopoDS_Edge> edgeVec2;
	std::vector<TopoDS_Edge> newFaceEdgeVec1;
	std::vector<TopoDS_Edge> newFaceEdgeVec2;
	std::vector<TopoDS_Edge> smallTriangleShapeWires;
	std::vector<TopoDS_Edge> my_newCrossSectionEdgeVec;
	std::vector<TopoDS_Edge> edgesToBeFilleted;

	std::vector<TopoDS_Wire> wiresVecWithoutBase;

	std::vector<TopoDS_Face> my_newFaceVec;

	std::vector<TopoDS_Shape> my_newFaceShapeVec;
	std::vector<TopoDS_Shape> smallholeFacVec;
	std::vector<TopoDS_Shape> airExitPipeThruSectionWithoutBase_vec;
	std::vector<TopoDS_Shape> transitionExitPart_vec;
	std::vector<TopoDS_Shape> transitionFaceVec;

	int numberOfSectionWires;
	double my_widthNewShape;
	double my_heightNewShape;

	std::vector<TopoDS_Shape> CreateFilletedScrollShapes_FanVolute(double radius, double startArea, double endArea);

};

