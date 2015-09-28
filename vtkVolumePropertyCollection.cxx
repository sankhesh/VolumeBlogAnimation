/*=============================================================================
Copyright and License information
=============================================================================*/

// VolumeBlogAnimation includes
#include "vtkVolumePropertyCollection.h"

// VTK includes
#include "vtkObjectFactory.h"
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>

vtkStandardNewMacro(vtkVolumePropertyCollection);

//-----------------------------------------------------------------------------
vtkVolumePropertyCollection::vtkVolumePropertyCollection()
{
  vtkNew<vtkColorTransferFunction> bonesCTF;
  bonesCTF->AddRGBPoint(-1000,76/255.0,76/255.0,1.0);
  bonesCTF->AddRGBPoint(-508,76/255.0,1.0,76/255.0);
  bonesCTF->AddRGBPoint(443.28,168/255.0,140/255.0,155/255.0);
  bonesCTF->AddRGBPoint(639.15,1.0,233/255.0,9/255.0);
  bonesCTF->AddRGBPoint(953,1.0,76/255.0,76/255.0);

  vtkNew<vtkPiecewiseFunction> bonesOF;
  bonesOF->AddPoint(-1000, 0.0);
  bonesOF->AddPoint(132.19, 0.0);
  bonesOF->AddPoint(258.93, 0.19);
  bonesOF->AddPoint(952, 0.2);

  Bones->SetColor(bonesCTF.GetPointer());
  Bones->SetScalarOpacity(bonesOF.GetPointer());
  Bones->ShadeOff();
  Bones->SetDisableGradientOpacity(1);


  vtkNew<vtkColorTransferFunction> lungCTF;
  lungCTF->AddRGBPoint(-1000,0.3,0.3,1.0);
  lungCTF->AddRGBPoint(-600,0,0,1.0);
  lungCTF->AddRGBPoint(-530,0.13,0.78,0.07);
  lungCTF->AddRGBPoint(-460,0.93,1.0,0.11);
  lungCTF->AddRGBPoint(-400,0.89,0.25,0.02);
  lungCTF->AddRGBPoint(2952,1.0,0.3,0.3);

  vtkNew<vtkPiecewiseFunction> lungOF;
  lungOF->AddPoint(-1000, 0.0);
  lungOF->AddPoint(-600.1, 0.0);
  lungOF->AddPoint(-600, 0.17);
  lungOF->AddPoint(-400.1, 0.17);
  lungOF->AddPoint(-400, 0.0);
  lungOF->AddPoint(2952, 0.0);

  Lung->SetColor(lungCTF.GetPointer());
  Lung->SetScalarOpacity(lungOF.GetPointer());
  Lung->ShadeOff();
  Lung->SetDisableGradientOpacity(1);

}

//-----------------------------------------------------------------------------
vtkVolumePropertyCollection::~vtkVolumePropertyCollection()
{
}

//----------------------------------------------------------------------------
void vtkVolumePropertyCollection::PrintSelf(ostream &os, vtkIndent indent)
{
  //os << indent << " = " << this-> << endl;
}
