#pragma once

#include <vector>
#include <iostream>
#include <OCC_3dDoc.h>
#include <TopoDS_Shape.hxx>

class GenerateVolute1
{

public:

	gp_Pnt getCommonPoint(std::vector<gp_Pnt> pointVec1, std::vector<gp_Pnt> pointVec2);
	TopoDS_Wire GetTranslationOfWire(TopoDS_Shape shape, gp_Vec vec);
	TopoDS_Shape ThruSectExitPipe(TopoDS_Wire wire1, TopoDS_Wire wire2, Standard_Boolean isSolid);
	TopoDS_Shape ReverseShapeIfInsideOut(TopoDS_Solid& solid);

	int my_numOfSections;

	std::vector<double> AreaCalculation(double startArea, double endArea);
	std::vector<double> CreateAngleVector();

	std::vector<TopoDS_Wire> RotateCrossSections(std::vector<TopoDS_Wire> wireVec, std::vector<double> angleVec);
	TopoDS_Wire RotateWire(TopoDS_Wire wire, Standard_Real angle);

	std::vector<TopoDS_Shape> CreateInputPlanes(std::vector<TopoDS_Wire> wireVec, std::vector<double> angleVec);
	std::vector<TopoDS_Shape> CreateShellList(std::vector<TopoDS_Wire> wireVec, std::vector<double> angleVec);
	std::vector<TopoDS_Shape> CreateSolidList(std::vector<TopoDS_Wire> wireVec, std::vector<double> angleVec);
	TopoDS_Shape CreateCompoundShape(std::vector<TopoDS_Wire> list);
	TopoDS_Shape CreateSolidThruSect(std::vector<TopoDS_Wire> wireVector, Standard_Real startAngle, Standard_Real endAngle, std::vector<double> angleVec);
	TopoDS_Shape CreateShellThruSect(std::vector<TopoDS_Wire> wireArray, Standard_Real startAngle, Standard_Real endAngle, std::vector<double> angleVec);
	TopoDS_Solid mkSolidOfSewedVoluteWithExitPipe(TopoDS_Shape voluteShape);

	TopoDS_Wire my_filletedWireForTrans;

	std::vector<TopoDS_Shape> my_shellVector;
	std::vector<TopoDS_Shape> my_solidVector;
	std::vector<TopoDS_Shape> my_inputPlaneList;
};

