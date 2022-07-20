#include "stdafx.h"
#include "GenerateVolute1.h"

#include <BRepBuilderAPI_Transform.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <ShapeFix_Solid.hxx>
#include <BRepClass3d_SolidClassifier.hxx>

// Calculate areas in a linear decrement
std::vector<double> GenerateVolute1::AreaCalculation(double startArea, double endArea)
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

// Rotating a wire according to the given angle
TopoDS_Wire GenerateVolute1::RotateWire(TopoDS_Wire wire, Standard_Real angle)
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
std::vector<double> GenerateVolute1::CreateAngleVector()
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
std::vector<TopoDS_Wire> GenerateVolute1::RotateCrossSections(std::vector<TopoDS_Wire> wireVec, std::vector<double> angleVec)
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
TopoDS_Shape GenerateVolute1::CreateCompoundShape(std::vector<TopoDS_Wire> list)
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
TopoDS_Shape GenerateVolute1::CreateShellThruSect(std::vector<TopoDS_Wire> wireVector, Standard_Real startAngle, Standard_Real endAngle, std::vector<double> angleVec)
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
std::vector<TopoDS_Shape> GenerateVolute1::CreateShellList(std::vector<TopoDS_Wire> wireVec, std::vector<double> angleVec)
{
	my_shellVector.clear();
	// Calling the CreateShell function 4 times 
	TopoDS_Shape shell = CreateShellThruSect(wireVec, 0, 60, angleVec);
	BRepTools::Write(shell, "shell.brep");
	TopoDS_Shape shell2 = CreateShellThruSect(wireVec, 60, 180, angleVec);
	BRepTools::Write(shell2, "shell2.brep");
	TopoDS_Shape shell3 = CreateShellThruSect(wireVec, 180, 300, angleVec);
	BRepTools::Write(shell3, "shell3.brep");
	TopoDS_Shape shell4 = CreateShellThruSect(wireVec, 300, 360, angleVec);
	BRepTools::Write(shell4, "shell4.brep");

	my_shellVector.push_back(shell);
	my_shellVector.push_back(shell2);
	my_shellVector.push_back(shell3);
	my_shellVector.push_back(shell4);

	return my_shellVector;
}

// Creating the list of solid scrolls
std::vector<TopoDS_Shape> GenerateVolute1::CreateSolidList(std::vector<TopoDS_Wire> wireVec, std::vector<double> angleVec)
{

	// Calling the CreateShell function 4 times 
	my_solidVector.clear();
	TopoDS_Shape solid = CreateSolidThruSect(wireVec, 0, 60, angleVec);
	BRepTools::Write(solid, "solid.brep");
	TopoDS_Shape solid2 = CreateSolidThruSect(wireVec, 60, 210, angleVec);
	BRepTools::Write(solid2, "solid2.brep");
	TopoDS_Shape solid3 = CreateSolidThruSect(wireVec, 210, 330, angleVec);
	BRepTools::Write(solid3, "solid3.brep");
	TopoDS_Shape solid4 = CreateSolidThruSect(wireVec, 330, 360, angleVec);
	BRepTools::Write(solid4, "solid4.brep");

	my_solidVector.push_back(solid);
	my_solidVector.push_back(solid2);
	my_solidVector.push_back(solid3);
	my_solidVector.push_back(solid4);

	return my_solidVector;
}


// create the list of input plane list
std::vector<TopoDS_Shape> GenerateVolute1::CreateInputPlanes(std::vector<TopoDS_Wire> wireVec, std::vector<double> angleVec)
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

// Creating the solid scrolls for the rotated wires with an angle gap
TopoDS_Shape GenerateVolute1::CreateSolidThruSect(std::vector<TopoDS_Wire> wireVector, Standard_Real startAngle, Standard_Real endAngle, std::vector<double> angleVec)
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


// converting the sewed volute shape into solid with exit pipe
TopoDS_Solid GenerateVolute1::mkSolidOfSewedVoluteWithExitPipe(TopoDS_Shape voluteShape)
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


// Getting the common point of two vertices
gp_Pnt GenerateVolute1::getCommonPoint(std::vector<gp_Pnt> pointVec1, std::vector<gp_Pnt> pointVec2)
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


// Get translation of the square
TopoDS_Wire GenerateVolute1::GetTranslationOfWire(TopoDS_Shape shape, gp_Vec vec)
{
	gp_Trsf translation1;
	translation1.SetTranslation(vec);

	BRepBuilderAPI_Transform frontTrans(shape, translation1, true);

	TopoDS_Shape trnsShape = frontTrans.Shape();
	TopoDS_Wire wire = TopoDS::Wire(trnsShape);

	return wire;
}

// Get thruSection of square to create the exit pipe
TopoDS_Shape GenerateVolute1::ThruSectExitPipe(TopoDS_Wire wire1, TopoDS_Wire wire2, Standard_Boolean isSolid)
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

	//transitionExitPart_vec.push_back(shape);

	return shape;
}

// Reversing the created solid shape
TopoDS_Shape GenerateVolute1::ReverseShapeIfInsideOut(TopoDS_Solid& solid)
{
	BRepClass3d_SolidClassifier classify;
	classify.Load(solid);
	BRepTools::Write(solid, "solid_test.brep");
	classify.PerformInfinitePoint(1.0e-6);
	TopAbs_State state = classify.State();
	if ((state == TopAbs_IN && true) || (state == TopAbs_OUT && false)) solid.Reverse();

	BRepTools::Write(solid, "solid_test1.brep");
	return solid;

}


