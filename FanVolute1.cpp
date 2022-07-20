#include "stdafx.h"
#include "FanVolute1.h"
#include <GenerateVolute1.h>
#include <RectangularVolute.h>

#include "ChFi2d_FilletAPI.hxx"
#include <BRepOffsetAPI_ThruSections.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <BRepBuilderAPI_Sewing.hxx>


// creating the transition start section of the fanVolute
TopoDS_Wire FanVolute1::CreateTransitionStartSection(double L1, double L3, double L5, double area)
{
	std::vector<TopoDS_Edge> edgeVec;

	//my_widthNewShape = L1 + L3 + L5;
	double height = area / my_widthNewShape; 

	gp_Pnt P1(150, 0, 0);
	gp_Pnt P2(150, 0, 0 - L1);
	gp_Pnt P3(150, 0, L3 + L5);

	gp_Pnt P7(150 + height, 0, 0 - L1);
	gp_Pnt P8(150 + height, 0, L3 + L5);

	TopoDS_Edge Edge1 = BRepBuilderAPI_MakeEdge(P1, P2);
	BRepTools::Write(Edge1, "Edge1.brep");
	TopoDS_Edge Edge2 = BRepBuilderAPI_MakeEdge(P2, P7);
	BRepTools::Write(Edge2, "Edge2.brep");
	TopoDS_Edge Edge3 = BRepBuilderAPI_MakeEdge(P7, P8);
	BRepTools::Write(Edge3, "Edge3.brep");
	TopoDS_Edge Edge4 = BRepBuilderAPI_MakeEdge(P8, P3);
	BRepTools::Write(Edge4, "Edge4.brep");
	TopoDS_Edge Edge5 = BRepBuilderAPI_MakeEdge(P3, P1);
	BRepTools::Write(Edge5, "Edge5.brep");

	//transitionStartSection
	edgeVec.push_back(Edge1);
	edgeVec.push_back(Edge2);
	edgeVec.push_back(Edge3);
	edgeVec.push_back(Edge4);
	edgeVec.push_back(Edge5);

	BRepBuilderAPI_MakeWire makeWholeWire;

	for (int i = 0; i < edgeVec.size(); i++)
	{
		makeWholeWire.Add(edgeVec[i]);

	}
	makeWholeWire.Build();

	TopoDS_Wire transitionStartSection = makeWholeWire.Wire();
	BRepTools::Write(transitionStartSection, "transitionStartSection.brep");

	return transitionStartSection;

}

std::vector<gp_Pnt> FanVolute1::GetTopVerticesOfRotatedSection(std::vector<TopoDS_Wire> wireVec)
{

	//std::vector<gp_Pnt> TopWireVertexVec1;
	//std::vector<gp_Pnt> TopWireVertexVec2;

	for (int i = 0; i < wireVec.size(); i++)
	{
		int counter = 0;

		for (TopExp_Explorer ex(wireVec[i], TopAbs_VERTEX); ex.More(); ex.Next())
		{
			TopoDS_Vertex vertex = TopoDS::Vertex(ex.Current());
			BRepTools::Write(vertex, "TopWireVertex.brep");

			counter++;
			if (counter == 6)
			{
				gp_Pnt topPnt1 = BRep_Tool::Pnt(vertex);
				BRepTools::Write(vertex, "TopWireVertex1.brep");
				TopWire_gpPntVec1.push_back(topPnt1);
			}
			if (counter == 8)
			{
				gp_Pnt topPnt2 = BRep_Tool::Pnt(vertex);
				BRepTools::Write(vertex, "TopWireVertex2.brep");
				TopWire_gpPntVec2.push_back(topPnt2);
			}
		}
	}

	return TopWire_gpPntVec1;
}

// get vertices for new cross section
void FanVolute1::getVerticesFromEdges_New(TopoDS_Wire wire)
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
TopoDS_Wire FanVolute1::Apply2dFilletsForNewCrossSection(/*std::vector<TopoDS_Edge> newEdgeVec*/ TopoDS_Wire wire, double radius)
{

	gp_Pnt commonPoint1 = GenVolute->getCommonPoint(newShape_pointVec1, newShape_pointVec2);
	gp_Pnt commonPoint2 = GenVolute->getCommonPoint(newShape_pointVec2, newShape_pointVec3);
	gp_Pnt commonPoint3 = GenVolute->getCommonPoint(newShape_pointVec3, newShape_pointVec4);
	gp_Pnt commonPoint4 = GenVolute->getCommonPoint(newShape_pointVec4, newShape_pointVec5);

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


std::vector<TopoDS_Wire> FanVolute1::CreateWiresForAirExit(double angleOfSmallScroll)
{

	std::vector<TopoDS_Wire> transWireVec;

	numberOfSectionWires = angleOfSmallScroll / 360 * 60 + 1;
	int sectionNumber = 61 - numberOfSectionWires;

	for (int i = 60; i > sectionNumber - 1; i--)
	{
		TopoDS_Wire transWire = CreateRectangleForAirExit(i);
		BRepTools::Write(transWire, "transWire.brep");
		transWireVec.push_back(transWire);
	}

	return transWireVec;
}

// Create a rectangle for the exit plane
TopoDS_Wire FanVolute1::CreateRectangleForAirExit(int numSections)
{
	TopoDS_Wire WireWithBase;
	TopoDS_Wire WireWithoutBase;
	std::vector<TopoDS_Wire> wiresOfRectangle;
	std::vector<TopoDS_Edge> edgesOfRectangle;

	gp_Pnt p1(TopWire_gpPntVec1[numSections]);
	gp_Pnt p2(TopWire_gpPntVec2[numSections]);
	gp_Pnt p3(TopWire_gpPntVec2[0]);
	gp_Pnt p4(TopWire_gpPntVec1[0]);

	TopoDS_Vertex V1 = BRepBuilderAPI_MakeVertex(p1);
	BRepTools::Write(V1, "pointOne.brep");
	TopoDS_Vertex V2 = BRepBuilderAPI_MakeVertex(p2);
	BRepTools::Write(V2, "pointTwo.brep");

	double XcoordDif1 = TopWire_gpPntVec2[0].Y() - TopWire_gpPntVec2[numSections].Y();
	double XcoordDif2 = TopWire_gpPntVec1[0].Y() - TopWire_gpPntVec1[numSections].Y();

	gp_Vec myVec1(0, -XcoordDif1, 0);
	gp_Vec myVec2(0, -XcoordDif2, 0);

	p3.Translate(myVec1);
	p4.Translate(myVec2);

	TopoDS_Vertex V3 = BRepBuilderAPI_MakeVertex(p3);
	BRepTools::Write(V3, "pointThree.brep");
	TopoDS_Vertex V4 = BRepBuilderAPI_MakeVertex(p4);
	BRepTools::Write(V4, "pointFour.brep");

	TopoDS_Edge Edge1 = BRepBuilderAPI_MakeEdge(p1, p2);
	TopoDS_Edge Edge2 = BRepBuilderAPI_MakeEdge(p2, p3);
	TopoDS_Edge Edge3 = BRepBuilderAPI_MakeEdge(p3, p4);
	TopoDS_Edge Edge4 = BRepBuilderAPI_MakeEdge(p4, p1);

	WireWithBase = BRepBuilderAPI_MakeWire(Edge1, Edge2, Edge3, Edge4);
	BRepTools::Write(WireWithBase, "WireWithBase.brep");
	wiresOfRectangle.push_back(WireWithBase);

	WireWithoutBase = BRepBuilderAPI_MakeWire(Edge2, Edge3, Edge4);
	BRepTools::Write(WireWithoutBase, "WireWithoutBase.brep");
	wiresVecWithoutBase.push_back(WireWithoutBase);

	edgesOfRectangle.push_back(Edge1);
	BRepTools::Write(Edge1, "Edge_1.brep");
	edgesOfRectangle.push_back(Edge2);
	BRepTools::Write(Edge2, "Edge_2.brep");
	edgesOfRectangle.push_back(Edge3);
	BRepTools::Write(Edge3, "Edge_3.brep");
	edgesOfRectangle.push_back(Edge4);
	BRepTools::Write(Edge4, "Edge_4.brep");

	// Creating wires 
	TopoDS_Wire wholeWire;
	BRepLib_MakeWire wireMaker;

	for (int i = 0; i < wiresOfRectangle.size(); i++)
	{
		wireMaker.Add(wiresOfRectangle[i]);
	}
	wholeWire = wireMaker.Wire();
	BRepTools::Write(wholeWire, "wire_test.brep");

	return wholeWire;
}

// Creating thruSection of Air Exit pipe
TopoDS_Shape FanVolute1::CreateExitPipeThruSect(std::vector<TopoDS_Wire> wireVector, Standard_Boolean isSolid)
{
	BRepOffsetAPI_ThruSections BTS1(isSolid, Standard_False, 1.0e-10);
	BTS1.SetMaxDegree(2);

	for (int i = 0; i < wireVector.size(); i++)
	{
		TopoDS_Wire aWire = wireVector[i];
		BTS1.AddWire(aWire);

	}
	BTS1.Build();
	TopoDS_Shape shellShape = BTS1.Shape();

	return shellShape;
}

TopoDS_Shape FanVolute1::ApplyFilletsToExitPipeWithoutBase(TopoDS_Shape airExitPipe, Standard_Real radius)
{
	TopoDS_Shape filletShape;

	BRepFilletAPI_MakeFillet mkFillet(airExitPipe);
	int counter1 = 0;
	for (TopExp_Explorer ex(airExitPipe, TopAbs_EDGE); ex.More(); ex.Next())
	{
		TopoDS_Edge anEdge = TopoDS::Edge(ex.Current());
		BRepTools::Write(anEdge, "EdgeToFillet.brep");

		counter1++;

		if (counter1 == 2 || counter1 == 6)
		{
			BRepTools::Write(anEdge, "FilletEdge.brep");
			mkFillet.Add(radius, anEdge);
		}
	}
	mkFillet.Build();
	filletShape = mkFillet.Shape();

	return filletShape;
}

// creating the corss section for the fan volute
std::vector<TopoDS_Wire> FanVolute1::CreateNewCrossSection(double L1, double L2, double L3, double L4, double L5, std::vector<double> area)
{

	TopoDS_Wire NewCrossSectionWire;
	//std::vector<TopoDS_Wire> TotalWires_vec;
	std::vector<TopoDS_Wire> NewCrossSectionWireList;

	/*std::vector<TopoDS_Edge> edge_vec;*/

	gp_Pnt P1(150, 0, 0);
	gp_Pnt P2(150 - L2, 0, 0);
	gp_Pnt P3(150 - L2, 0, 0 - L1);
	gp_Pnt P4(150, 0, L3);
	gp_Pnt P5(150 - L4, 0, L3);
	gp_Pnt P6(150 - L4, 0, L3 + L5);

	my_widthNewShape = L1 + L3 + L5;

	for (int i = 0; i < area.size(); i++)
	{
		double heightNewShape = area[i] / my_widthNewShape;

		gp_Pnt P7(150 + heightNewShape, 0, 0 - L1);
		gp_Pnt P8(150 + heightNewShape, 0, L3 + L5);

		gpPntP7vec.push_back(P7);
		gpPntP8vec.push_back(P8);

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

		for (int j = 0; j < my_newCrossSectionEdgeVec.size(); j++)
		{
			makeWholeWire.Add(my_newCrossSectionEdgeVec[j]);

		}
		makeWholeWire.Build();

		TopoDS_Wire wholeWire = makeWholeWire.Wire();
		BRepTools::Write(wholeWire, "wholeWire.brep");
		NewCrossSectionWireList.push_back(wholeWire);
	}

	return NewCrossSectionWireList;
}

TopoDS_Shape FanVolute1::ApplyFilletsToaShape(TopoDS_Shape shape, double radius)
{
	TopoDS_Shape filletShape;

	BRepFilletAPI_MakeFillet mkFillet(shape);
	int counter1 = 0;
	for (TopExp_Explorer ex(shape, TopAbs_EDGE); ex.More(); ex.Next())
	{
		TopoDS_Edge anEdge = TopoDS::Edge(ex.Current());
		BRepTools::Write(anEdge, "EdgeToFillet.brep");

		counter1++;

		if (counter1 == 3)
		{
			BRepTools::Write(anEdge, "FilletEdge.brep");
			mkFillet.Add(radius, anEdge);
		}
	}
	mkFillet.Build();
	filletShape = mkFillet.Shape();

	return filletShape;
}

TopoDS_Shape FanVolute1::ApplyFilletsToTranisition(TopoDS_Shape shape, double radius)
{
	TopoDS_Shape filletShape;

	BRepFilletAPI_MakeFillet mkFillet(shape);
	int counter1 = 0;
	for (TopExp_Explorer ex(shape, TopAbs_EDGE); ex.More(); ex.Next())
	{
		TopoDS_Edge anEdge = TopoDS::Edge(ex.Current());
		BRepTools::Write(anEdge, "EdgeToFillet.brep");

		counter1++;

		if (counter1 == 2 || counter1 == 12 || counter1 == 6 || counter1 == 10 || counter1 == 21 || counter1 == 24)
		{
			BRepTools::Write(anEdge, "FilletEdge.brep");
			mkFillet.Add(radius, anEdge);
		}
	}
	mkFillet.Build();
	filletShape = mkFillet.Shape();

	return filletShape;
}

std::vector<TopoDS_Face> FanVolute1::IterateFacesOfTransitionPipe(TopoDS_Shape transitionShape)
{
	std::vector<TopoDS_Face> transitionFaces;

	int counter = 0;
	for (TopExp_Explorer ex(transitionShape, TopAbs_FACE); ex.More(); ex.Next())
	{
		TopoDS_Face face = TopoDS::Face(ex.Current());
		BRepTools::Write(face, "face.brep");

		counter++;

		if (counter == 1)
		{
			BRepTools::Write(face, "face.brep");

		}
		else
		{
			transitionFaces.push_back(face);
		}

	}

	return transitionFaces;
}

// create wire vector applying 2D fillets for the new cross sections
std::vector<TopoDS_Wire> FanVolute1::createFilletedCrossSections_newShape(std::vector<TopoDS_Wire> wireVec)
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

TopoDS_Edge FanVolute1::IterateEdgesOfaShape(TopoDS_Shape shape)
{
	TopoDS_Edge anEdge;
	int counter = 0;

	for (TopExp_Explorer ex(shape, TopAbs_EDGE); ex.More(); ex.Next())
	{
		anEdge = TopoDS::Edge(ex.Current());
		BRepTools::Write(anEdge, "EdgeForFillet.brep");

		counter++;
		if (counter == 1/* ||counter == 2 || counter == 4 || counter == 6 || counter == 10*/)
		{
			edgesToBeFilleted.push_back(anEdge);
			BRepTools::Write(anEdge, "EdgeForFillet.brep");
		}
	}

	return anEdge;
}

gp_Pnt FanVolute1::GetMiddlePointForAirExit(TopoDS_Wire sectionWires, double L3)
{
	//my_sectionWireList[0]
	BRepBuilderAPI_MakeFace mkFace(sectionWires, true);
	TopoDS_Face planeFace = mkFace.Face();
	GProp_GProps SProps;

	//double height = area / my_widthNewShape;

	//gp_Pnt centre_pnt(150, 0,25);
	gp_Pnt centre_pnt(150, 0, +L3 / 2);
	BRepGProp::SurfaceProperties(planeFace, SProps);
	centre_pnt = SProps.CentreOfMass();
	TopoDS_Vertex V = BRepBuilderAPI_MakeVertex(centre_pnt);
	BRepTools::Write(V, "centrePnt.brep");

	return centre_pnt;
}

std::vector<TopoDS_Shape> FanVolute1::GetFaceListFromExitShape(TopoDS_Shape shape)
{

	std::vector<TopoDS_Shape> faceVec;

	int counter1 = 0;
	for (TopExp_Explorer ex(shape, TopAbs_FACE); ex.More(); ex.Next())
	{
		TopoDS_Face face = TopoDS::Face(ex.Current());
		BRepTools::Write(face, "face.brep");

		counter1++;

		if (counter1 == 1 || counter1 == 4 || counter1 == 5 || counter1 == 9 || counter1 == 10 || counter1 == 14 || counter1 == 15 || counter1 == 18)
		{
			faceVec.push_back(face);
		}
	}

	return faceVec;
}

TopoDS_Wire FanVolute1::CreateFilletedTranisitionExitWire(TopoDS_Shape shape)
{
	std::vector<TopoDS_Wire> wires;
	std::vector<TopoDS_Edge> edgeVec;

	int counter1 = 0;
	for (TopExp_Explorer ex(shape, TopAbs_EDGE); ex.More(); ex.Next())
	{
		TopoDS_Edge anEdge = TopoDS::Edge(ex.Current());
		BRepTools::Write(anEdge, "EdgeToFillet.brep");

		counter1++;

		if (counter1 == 6 || counter1 == 10 || counter1 == 14 || counter1 == 18 || counter1 == 22 || counter1 == 26 || counter1 == 30 || counter1 == 36)
		{
			edgeVec.push_back(anEdge);
		}
	}

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

void FanVolute1::IterateEdgesOfSewedShape(TopoDS_Shape shape)
{

	int counter1 = 0;
	for (TopExp_Explorer ex(shape, TopAbs_EDGE); ex.More(); ex.Next())
	{
		TopoDS_Edge anEdge = TopoDS::Edge(ex.Current());
		BRepTools::Write(anEdge, "EdgeToFillet.brep");

		counter1++;

		if (counter1 == 5 || counter1 == 15 || counter1 == 29)
		{
			BRepTools::Write(anEdge, "FilletEdge.brep");
			edgeVec1.push_back(anEdge);
			//smallTriangleShapeWires.push_back(anEdge);
		}

		if (counter1 == 8 || counter1 == 22 || counter1 == 26)
		{
			BRepTools::Write(anEdge, "FilletEdge.brep");
			edgeVec2.push_back(anEdge);
			//smallTriangleShapeWires.push_back(anEdge);
		}
	}

}

TopoDS_Wire FanVolute1::MakeWireFromEdges(std::vector<TopoDS_Edge> edgeVec)
{
	TopoDS_Wire wireFromEdges;
	std::vector<TopoDS_Wire> wireVec;

	for (int i = 0; i < edgeVec.size(); i++)
	{
		wireFromEdges = BRepBuilderAPI_MakeWire(edgeVec[i]);
		wireVec.push_back(wireFromEdges);
	}

	TopoDS_Wire totalWire;
	BRepLib_MakeWire wireMaker;

	for (int i = 0; i < wireVec.size(); i++)
	{
		wireMaker.Add(wireVec[i]);

	}
	totalWire = wireMaker.Wire();

	return totalWire;
}

TopoDS_Face FanVolute1::MakeFaceFromWireAndBaseSurface(TopoDS_Wire wire)
{

	TopoDS_Face faceCut;

	TopoDS_Face filledFace = BRepBuilderAPI_MakeFace(wire);

	BRepTools::Write(filledFace, "filledFace.brep");

	smallholeFacVec.push_back(filledFace);

	return faceCut;
}

TopoDS_Face FanVolute1::MakeNewFaceFromWires(TopoDS_Wire wire)
{

	TopoDS_Face faceCut;

	TopoDS_Face newFace = BRepBuilderAPI_MakeFace(wire);

	BRepTools::Write(newFace, "newFace.brep");

	my_newFaceVec.push_back(newFace);
	my_newFaceShapeVec.push_back(newFace);

	return faceCut;
}

TopoDS_Shape FanVolute1::ApplyFilletsToSewedShape(TopoDS_Shape shape, double radius)
{
	TopoDS_Shape filletShape;
	smallTriangleShapeWires;

	BRepFilletAPI_MakeFillet mkFillet(shape);
	int counter1 = 0;
	for (TopExp_Explorer ex(shape, TopAbs_EDGE); ex.More(); ex.Next())
	{
		TopoDS_Edge anEdge = TopoDS::Edge(ex.Current());
		BRepTools::Write(anEdge, "EdgeToFillet.brep");

		counter1++;

		if (counter1 == 1 /*|| counter1 == 4*/)
		{
			BRepTools::Write(anEdge, "FilletEdge.brep");
			mkFillet.Add(radius, anEdge);
		}
		/*else if (counter1 == 1 || (counter1 == 1 || (counter1 == 1)
		{

		}*/
	}
	mkFillet.Build();
	filletShape = mkFillet.Shape();

	return filletShape;
}


// Apply fillets to the new volute shape
TopoDS_Shape FanVolute1::ApplyFilletNewVolute(TopoDS_Shape shapeToBeFilleted, std::vector<TopoDS_Edge> edgeVec, double radius)
{
	TopoDS_Shape filletShape;

	BRepFilletAPI_MakeFillet mkFillet(shapeToBeFilleted);
	//int counter1 = 0;

	for (int i = 0; i < edgeVec.size(); i++)
	{
		BRepTools::Write(edgeVec[i], "FilletEdge.brep");
		mkFillet.Add(radius, edgeVec[i]);

	}
	mkFillet.Build();
	filletShape = mkFillet.Shape();

	return filletShape;
}
void FanVolute1::IterateEdgesFromTransitionExit(TopoDS_Shape shape1)
{

	int counter1 = 0;
	for (TopExp_Explorer ex(shape1, TopAbs_EDGE); ex.More(); ex.Next())
	{
		TopoDS_Edge Edge = TopoDS::Edge(ex.Current());
		BRepTools::Write(Edge, "Edge.brep");

		counter1++;

		if (counter1 == 5 || counter1 == 12 || counter1 == 15 || counter1 == 28 || counter1 == 31 || counter1 == 32)
		{
			newFaceEdgeVec1.push_back(Edge);
		}
		if (counter1 == 8 || counter1 == 9 || counter1 == 22 || counter1 == 23 || counter1 == 24 || counter1 == 25)
		{
			newFaceEdgeVec2.push_back(Edge);
		}
	}

}

std::vector<TopoDS_Shape> FanVolute1::IterateFacesFromaShape(TopoDS_Shape shape1, TopoDS_Shape shape2)
{
	std::vector<TopoDS_Shape> faceVec;

	int counter1 = 0;
	for (TopExp_Explorer ex(shape1, TopAbs_FACE); ex.More(); ex.Next())
	{
		TopoDS_Face scrollFace = TopoDS::Face(ex.Current());
		BRepTools::Write(scrollFace, "scrollFace.brep");

		counter1++;

		if (counter1 == 4)
		{
			BRepTools::Write(scrollFace, "scrollFace.brep");
			faceVec.push_back(scrollFace);
		}
	}

	int counter2 = 0;
	for (TopExp_Explorer ex(shape2, TopAbs_FACE); ex.More(); ex.Next())
	{
		TopoDS_Face transitionFace = TopoDS::Face(ex.Current());
		BRepTools::Write(transitionFace, "transitionFace.brep");

		counter2++;

		if (counter2 == 1)
		{
			BRepTools::Write(transitionFace, "transitionFace.brep");
			faceVec.push_back(transitionFace);
		}
		else if (counter2 == 2)
		{
			transitionFaceVec.push_back(transitionFace);
		}
	}

	return faceVec;
}

TopoDS_Shape FanVolute1::SewFacesToGetShape(std::vector<TopoDS_Shape> faceVec, double tolerance)
{

	BRepBuilderAPI_Sewing sewer;
	sewer.SetTolerance(tolerance);

	//sroll vector loop sewer without last one
	for (int i = 0; i < faceVec.size(); i++)
	{
		sewer.Add(faceVec[i]);
		BRepTools::Write(faceVec[i], "face.brep");
	}

	sewer.Perform();

	//extrat the shell
	TopoDS_Shape sewedShape = sewer.SewedShape();
	BRepTools::Write(sewedShape, "sewedShape.brep");

	return sewedShape;
}

// Apply fillets to one scroll - new shape
TopoDS_Shape FanVolute1::ApplyFilletNewScrollShapes(TopoDS_Shape scrollShape, double radius)
{
	TopoDS_Shape filletShape;

	BRepFilletAPI_MakeFillet mkFillet(scrollShape);
	int counter1 = 0;

	for (TopExp_Explorer ex(scrollShape, TopAbs_EDGE); ex.More(); ex.Next())
	{
		TopoDS_Edge anEdge = TopoDS::Edge(ex.Current());
		BRepTools::Write(anEdge, "topEdgeForFillet.brep");

		counter1++;

		if (counter1 == 10 || counter1 == 14)
		{
			//TopoDS_Edge edge = TopoDS::Edge(ex.Current());
			BRepTools::Write(anEdge, "topEdge.brep");
			edgesToBeFilleted.push_back(anEdge);
			mkFillet.Add(radius, anEdge);
		}
	}
	filletShape = mkFillet.Shape();

	return filletShape;
}

// Applying fillets to hollow scroll shape vector
std::vector<TopoDS_Shape> FanVolute1::mkFilletToNewScrollShapes(std::vector<TopoDS_Shape> scrollShapeVec, double radius)
{
	TopoDS_Shape filletedShape;
	std::vector<TopoDS_Shape> scrollShapes;

	for (int i = 0; i < scrollShapeVec.size(); i++)
	{
		filletedShape = ApplyFilletNewScrollShapes(scrollShapeVec[i], radius);
		BRepTools::Write(filletedShape, "filletedScrollShape.brep");
		scrollShapes.push_back(filletedShape);
	}

	return scrollShapes;
}

// sewing scroll shapes, exit pipe and input planes of the volute with new cross section
TopoDS_Shape FanVolute1::SewVoluteWithTransitionPipe(std::vector<TopoDS_Shape> scrollShells, std::vector<TopoDS_Face> smallScrollFaces, std::vector<TopoDS_Shape> exitPipe, std::vector<TopoDS_Face> newFaceVector, TopoDS_Face transitionExit, TopoDS_Shape filletedSewedFaces, /*std::vector<TopoDS_Shape> shapeFillingVec,*/ TopoDS_Shape transitionPart, double tolerance)
{
	BRepBuilderAPI_Sewing sewer;
	sewer.SetTolerance(tolerance);

	//sroll vector loop sewer without last one
	for (int i = 0; i < scrollShells.size() - 2; i++)
	{
		sewer.Add(scrollShells[i]);
		BRepTools::Write(scrollShells[i], "scrollShells_test.brep");
	}

	//inputplanelist 
	for (int i = 0; i < smallScrollFaces.size(); i++)
	{
		sewer.Add(smallScrollFaces[i]);
		BRepTools::Write(smallScrollFaces[i], "smallScrollFaces_test.brep");
	}

	//boolean shapes 
	for (int i = 0; i < exitPipe.size(); i++)
	{
		sewer.Add(exitPipe[i]);
		BRepTools::Write(exitPipe[i], "exitPipe_test.brep");
	}

	//exit plane list
	for (int i = 0; i < newFaceVector.size(); i++)
	{
		sewer.Add(newFaceVector[i]);
		BRepTools::Write(newFaceVector[i], "newFaceVector_test.brep");
	}

	sewer.Add(transitionPart);
	sewer.Add(transitionExit);
	sewer.Add(filletedSewedFaces);

	sewer.Perform();

	//extrat the shell
	TopoDS_Shape sewedShape = sewer.SewedShape();
	BRepTools::Write(sewedShape, "sewedShape.brep");


	TopoDS_Shell sewedShellWithoutExitPlane;

	for (TopExp_Explorer Expl(sewer.SewedShape(), TopAbs_SHELL); Expl.More(); Expl.Next())
	{
		sewedShellWithoutExitPlane = TopoDS::Shell(Expl.Current());

	}

	BRepTools::Write(sewedShellWithoutExitPlane, "sewedShellWithoutExitPlane.brep");

	return sewedShape;
}

TopoDS_Shape FanVolute1::SewVoluteWith3DFillets(std::vector<TopoDS_Shape> scrollShells, std::vector<TopoDS_Face> smallScrollFaces, std::vector<TopoDS_Shape> exitPipe, TopoDS_Face transitionExit, TopoDS_Shape filletedSewedFaces, double tolerance)
{
	BRepBuilderAPI_Sewing sewer;
	sewer.SetTolerance(tolerance);

	//sroll vector loop sewer without last one
	for (int i = 0; i < scrollShells.size() - 2; i++)
	{
		sewer.Add(scrollShells[i]);
		BRepTools::Write(scrollShells[i], "scrollShells_test.brep");
	}

	//inputplanelist 
	for (int i = 0; i < smallScrollFaces.size(); i++)
	{
		sewer.Add(smallScrollFaces[i]);
		BRepTools::Write(smallScrollFaces[i], "smallScrollFaces_test.brep");
	}

	//boolean shapes 
	for (int i = 0; i < exitPipe.size(); i++)
	{
		sewer.Add(exitPipe[i]);
		BRepTools::Write(exitPipe[i], "exitPipe_test.brep");
	}

	sewer.Add(transitionExit);
	sewer.Add(filletedSewedFaces);

	sewer.Perform();

	//extrat the shell
	TopoDS_Shape sewedShape = sewer.SewedShape();
	BRepTools::Write(sewedShape, "sewedShape.brep");


	TopoDS_Shell sewedShellWithoutExitPlane;

	for (TopExp_Explorer Expl(sewer.SewedShape(), TopAbs_SHELL); Expl.More(); Expl.Next())
	{
		sewedShellWithoutExitPlane = TopoDS::Shell(Expl.Current());

	}

	BRepTools::Write(sewedShellWithoutExitPlane, "sewedShellWithoutExitPlane.brep");

	return sewedShape;
}

std::vector<TopoDS_Face> FanVolute1::RemoveTopFaceOfScroll(/*TopoDS_Shape scrollShape, */std::vector<TopoDS_Shape> scrollShapeVec)
{
	//TopoDS_Face topFace;
	TopoDS_Face scrollFace;
	std::vector<TopoDS_Face> scrollFaceVector;

	for (int i = 3; i >= scrollShapeVec.size() - 2; i--)
	{
		int faceCounter = 0;

		for (TopExp_Explorer explr(scrollShapeVec[i], TopAbs_FACE); explr.More(); explr.Next())
		{
			scrollFace = TopoDS::Face(explr.Current());
			BRepTools::Write(scrollFace, "scrollFace.brep");

			faceCounter++;
			if (faceCounter == 4)
			{

				BRepTools::Write(scrollFace, "smallScrollFace.brep");

			}
			else
			{
				scrollFaceVector.push_back(scrollFace);

			}
		}
	}

	return scrollFaceVector;
}

std::vector<TopoDS_Shape> FanVolute1::CreateFilletedScrollShapes_FanVolute(double radius, double startArea, double endArea)
{
	GenVolute = new GenerateVolute1();

	GenVolute->my_numOfSections = 60;

	// create area vector according to a regular decrement
	std::vector<double> areasOfSections = GenVolute->AreaCalculation(startArea, endArea);

	// create angle vector with a regular decrement
	std::vector<double> angleVector = GenVolute->CreateAngleVector();

	// create the new cross section
	std::vector<TopoDS_Wire> newCrossSectionVecWithoutFillets = CreateNewCrossSection(50, 100, 75, 50, 50, areasOfSections);

	// rotate the new cross sections
	std::vector<TopoDS_Wire> rotatedCrossSectionWires = GenVolute->RotateCrossSections(newCrossSectionVecWithoutFillets, angleVector);

	// create the transition exit
	TopoDS_Wire transitionStartSection = CreateTransitionStartSection(50, 75, 50, startArea);
	BRepTools::Write(transitionStartSection, "transitionStartSection.brep");

	// create the compound shape with the rotated wire vector
	TopoDS_Shape compoundShapeWithBaseWire = GenVolute->CreateCompoundShape(rotatedCrossSectionWires);
	BRepTools::Write(compoundShapeWithBaseWire, "compoundShape.brep");

	// create shell shapes with the given angle
	std::vector<TopoDS_Shape> newShapeShellVector = GenVolute->CreateShellList(rotatedCrossSectionWires, angleVector);

	// apply fillets to the scroll shapes
	std::vector<TopoDS_Shape> filletedScrollShapes = mkFilletToNewScrollShapes(newShapeShellVector, radius);

	// remove the top faces of the last 2 scroll shapes
	std::vector<TopoDS_Face> faceVectorWithoutTop = RemoveTopFaceOfScroll(newShapeShellVector);
	// Get the top vertices of rotated wires
	std::vector<gp_Pnt> verticesOfTopWire = GetTopVerticesOfRotatedSection(rotatedCrossSectionWires);

	// Get the middle point of the transition/air exit
	gp_Pnt midPoint = GetMiddlePointForAirExit(transitionStartSection, 75);

	// create transition cross section wires to create the exit pipe
	std::vector<TopoDS_Wire> transCrossSections = CreateWiresForAirExit(60);

	// thru sect the exit pipe
	TopoDS_Shape airExitPipeThruSection = CreateExitPipeThruSect(transCrossSections, Standard_False);
	BRepTools::Write(airExitPipeThruSection, "airExitPipeThruSection.brep");

	// thru sect air exit pipe without the base
	TopoDS_Shape airExitPipeThruSectionWithoutBase = CreateExitPipeThruSect(wiresVecWithoutBase, Standard_False);
	BRepTools::Write(airExitPipeThruSectionWithoutBase, "airExitPipeThruSectionWithoutBase.brep");

	// filleted air exit pipe without base
	TopoDS_Shape filletedExitPipe = ApplyFilletsToExitPipeWithoutBase(airExitPipeThruSectionWithoutBase, radius);
	BRepTools::Write(filletedExitPipe, "filletedExitPipe.brep");
	airExitPipeThruSectionWithoutBase_vec.push_back(filletedExitPipe);

	// create transition exit wire
	gp_Vec myVec(0, -100, 0);
	TopoDS_Wire transitionExitWire = GenVolute->GetTranslationOfWire(transCrossSections[numberOfSectionWires - 1], myVec);
	BRepTools::Write(transitionExitWire, "transitionExitWire.brep");
	// make face from transition wire
	BRepBuilderAPI_MakeFace transitionExit(transitionExitWire, true);
	TopoDS_Face transitionExitPlane = transitionExit;
	BRepTools::Write(transitionExitPlane, "transitionExitPlane.brep");

	TopoDS_Shape transitionExitPart = GenVolute->ThruSectExitPipe(transCrossSections[numberOfSectionWires - 1], transitionExitWire, Standard_False);
	BRepTools::Write(transitionExitPart, "transitionExitPart.brep");
	transitionExitPart_vec.push_back(transitionExitPart);
	std::vector<TopoDS_Face> transitionFaceVec = IterateFacesOfTransitionPipe(transitionExitPart);
	IterateEdgesOfaShape(transitionExitPart);

	// iterate the shell and transition exit pipe to get the faces to be sewed
	std::vector<TopoDS_Shape> facesToBeSewed = IterateFacesFromaShape(newShapeShellVector[2], transitionExitPart);

	// sew faces 
	TopoDS_Shape sewedShape = SewFacesToGetShape(facesToBeSewed, 1.E-06);
	BRepTools::Write(sewedShape, "sewedShape.brep");

	// make fillet in the first sewed shape
	TopoDS_Shape filletedShapeWithTwoFaces = ApplyFilletsToaShape(sewedShape, radius);
	BRepTools::Write(filletedShapeWithTwoFaces, "filletedShapeWithTwoFaces.brep");

	// add faces to be sewed to a vector
	std::vector<TopoDS_Shape> shapesToBeSewed;
	shapesToBeSewed.push_back(filletedShapeWithTwoFaces);
	shapesToBeSewed.push_back(faceVectorWithoutTop[9]);
	shapesToBeSewed.push_back(faceVectorWithoutTop[10]);
	shapesToBeSewed.push_back(transitionFaceVec[0]);
	shapesToBeSewed.push_back(transitionFaceVec[2]);

	// sew the faces
	TopoDS_Shape sewedShape2 = SewFacesToGetShape(shapesToBeSewed, 1.E-06);
	BRepTools::Write(sewedShape2, "sewedShape2.brep");

	IterateEdgesOfSewedShape(sewedShape2);
	// make wire from the triangular gap 1
	TopoDS_Wire fillingWire1 = MakeWireFromEdges(edgeVec1);
	BRepTools::Write(fillingWire1, "fillingWire1.brep");

	// make wire from the triangular gap 2
	TopoDS_Wire fillingWire2 = MakeWireFromEdges(edgeVec2);
	BRepTools::Write(fillingWire2, "fillingWire2.brep");

	// make face from triangular wire 1
	TopoDS_Face faceToBeFilled1 = MakeFaceFromWireAndBaseSurface(fillingWire1);
	BRepTools::Write(faceToBeFilled1, "faceToBeFilled1.brep");

	// make face from triangular wire 2
	TopoDS_Face faceToBeFilled2 = MakeFaceFromWireAndBaseSurface(fillingWire2);
	BRepTools::Write(faceToBeFilled2, "faceToBeFilled2.brep");

	IterateEdgesFromTransitionExit(sewedShape2);
	TopoDS_Wire newFaceWire1 = MakeWireFromEdges(newFaceEdgeVec1);
	BRepTools::Write(newFaceWire1, "newFaceWire1.brep");

	TopoDS_Wire newFaceWire2 = MakeWireFromEdges(newFaceEdgeVec2);
	BRepTools::Write(newFaceWire2, "newFaceWire2.brep");

	TopoDS_Face newFace1 = MakeNewFaceFromWires(newFaceWire1);
	BRepTools::Write(newFace1, "newFace1.brep");

	TopoDS_Face newFace2 = MakeNewFaceFromWires(newFaceWire2);
	BRepTools::Write(newFace2, "newFace2.brep");

	my_newFaceShapeVec.push_back(transitionFaceVec[1]);
	my_newFaceShapeVec.push_back(filletedShapeWithTwoFaces);
	my_newFaceShapeVec.push_back(faceVectorWithoutTop[9]);
	my_newFaceShapeVec.push_back(faceVectorWithoutTop[10]);
	TopoDS_Shape sewedTranisitionExit = SewFacesToGetShape(my_newFaceShapeVec, 1.E-06);
	BRepTools::Write(sewedTranisitionExit, "sewedTranisitionExit.brep");

	TopoDS_Shape filletedTransition = ApplyFilletsToTranisition(sewedTranisitionExit, radius);
	BRepTools::Write(filletedTransition, "filletedTransition.brep");


	std::vector<TopoDS_Shape> transitionFaceList = GetFaceListFromExitShape(filletedTransition);

	TopoDS_Shape sewedFilletedTranisitionExit = SewFacesToGetShape(transitionFaceList, 1.E-06);
	BRepTools::Write(sewedFilletedTranisitionExit, "sewedFilletedTranisitionExit.brep");

	TopoDS_Wire filletedTransitionExitWire = CreateFilletedTranisitionExitWire(sewedFilletedTranisitionExit);
	BRepTools::Write(filletedTransitionExitWire, "filletedTransitionExitWire.brep");

	BRepBuilderAPI_MakeFace filletedTransitionExit(filletedTransitionExitWire, true);
	TopoDS_Face filletedTransitionExitPlane = filletedTransitionExit;
	BRepTools::Write(filletedTransitionExitPlane, "filletedTransitionExitPlane.brep");

	// remove the 10th element in the face vector
	faceVectorWithoutTop.erase(faceVectorWithoutTop.begin() + 9);
	faceVectorWithoutTop.erase(faceVectorWithoutTop.begin() + 9);

	// sew the whole shape with fillets
	TopoDS_Shape newSewedVoluteShapeWithFillets = SewVoluteWith3DFillets(filletedScrollShapes, faceVectorWithoutTop, airExitPipeThruSectionWithoutBase_vec, filletedTransitionExitPlane, filletedTransition, 1.E-01);
	BRepTools::Write(newSewedVoluteShapeWithFillets, "newSewedVoluteShapeWithFillets.brep");

	// create the solid volute with fillets
	TopoDS_Solid SolidNewVoluteShape = GenVolute->mkSolidOfSewedVoluteWithExitPipe(newSewedVoluteShapeWithFillets);
	BRepTools::Write(SolidNewVoluteShape, "SolidNewVoluteShape.brep");

	// create the reversed solid volute with fillets
	TopoDS_Shape ReversedSolidVolute = GenVolute->ReverseShapeIfInsideOut(SolidNewVoluteShape);
	BRepTools::Write(ReversedSolidVolute, "ReversedSolidVolute.brep");

	//TopoDS_Shape filletedNewVoluteShape = ApplyFilletNewVolute(ReversedSolidVolute, edgesToBeFilleted, 6);
	//BRepTools::Write(filletedNewVoluteShape, "filletedNewVoluteShape.brep");

	//TopoDS_Shape booleanCutExitPipe = CreateThruSect(transitionStartSection, solidSmallScroll, newShapeShellVector[3]);

	/////// -------------- Create new volute with 2D fillets -----------------///////
	/*std::vector<TopoDS_Wire> newCrossSectionWith2DFillets = createFilletedCrossSections_newShape(newCrossSectionVecWithoutFillets);

	std::vector<TopoDS_Wire> rotatedFilletedCrossSectionWires = volute.RotateCrossSections(newCrossSectionWith2DFillets, angleVector);

	TopoDS_Shape filletedCompoundShapeWithBaseWire = volute.CreateCompoundShape(rotatedFilletedCrossSectionWires);
	BRepTools::Write(filletedCompoundShapeWithBaseWire, "filletedCompoundShapeWithBaseWire.brep");

	std::vector<TopoDS_Shape> filletedShellVector = volute.CreateShellList(rotatedFilletedCrossSectionWires, angleVector);

	std::vector<TopoDS_Shape> filletedSolidVector = volute.CreateSolidList(rotatedFilletedCrossSectionWires, angleVector);*/

	// create filleted rectangle
	//std::vector<TopoDS_Edge> edgesWith2dFillets = create2dFilletsForSquare(6);
	//TopoDS_Wire rectWire2dFillets = Create2dWire(edgesWith2dFillets);
	//BRepTools::Write(rectWire2dFillets, "wire2dFillets.brep");

	// Getting the translation of the square
	//gp_Vec myVec1(0, -400, 0);
	//TopoDS_Wire filletedTrnsfWire = GetTranslationOfWire(rectWire2dFillets, myVec1);
	//BRepTools::Write(filletedTrnsfWire, "trnsf2dWire.brep");

	// Filleted exit pipe
	//TopoDS_Shape FilletedThruSectPipe_NewShape = ThruSectExitPipe(rotatedFilletedCrossSectionWires[0], filletedTrnsfWire, Standard_False);
	//BRepTools::Write(FilletedThruSectPipe_NewShape, "FilletedThruSectPipe_NewShape.brep");

	return filletedScrollShapes;
}

