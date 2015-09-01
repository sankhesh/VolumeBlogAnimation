/*=============================================================================
Copyright and License information
=============================================================================*/

#include "pugixml/pugixml.hpp"
#include "vtkAnimationUtils.h"

// VTK includes
#include <vtkObjectFactory.h>
#include <vtkNew.h>

vtkStandardNewMacro(vtkAnimationUtils);

//-----------------------------------------------------------------------------
vtkAnimationUtils::vtkAnimationUtils()
{
  this->Camera = NULL;
  this->Interpolator = NULL;
  this->RenderWindow = NULL;
  this->VolumeProperty = NULL;
}

//-----------------------------------------------------------------------------
vtkAnimationUtils::~vtkAnimationUtils()
{
  if (this->Camera)
    {
    this->Camera->Delete();
    this->Camera = NULL;
    }
  if (this->Interpolator)
    {
    this->Interpolator->Delete();
    this->Interpolator = NULL;
    }
  if (this->RenderWindow)
    {
    this->RenderWindow->Delete();
    this->RenderWindow = NULL;
    }
  if (this->VolumeProperty)
    {
    this->VolumeProperty->Delete();
    this->VolumeProperty = NULL;
    }
}

//-----------------------------------------------------------------------------
void vtkAnimationUtils::AddCurrentView()
{
  if (!this->Interpolator || !this->Camera)
    {
    return;
    }
  double t = static_cast<double> (this->Interpolator->GetNumberOfCameras());
  this->Interpolator->AddCamera(t, this->Camera);
}

//-----------------------------------------------------------------------------
void vtkAnimationUtils::Animate()
{
  if (!this->Interpolator || !this->RenderWindow || !this->Camera)
    {
    return;
    }
  int numSteps = 100;
  double min = this->Interpolator->GetMinimumT();
  double max = this->Interpolator->GetMaximumT();
  int i = 0;
  while (i <= numSteps)
    {
    this->VolumeProperty->ShadeOn();
    if (i >= numSteps / 2.0)
      {
      this->VolumeProperty->ShadeOff();
      }
    double t = static_cast<double>(i) * (max - min) / numSteps;
    this->Interpolator->InterpolateCamera(t, this->Camera);
    this->RenderWindow->Render();
    i++;
    }
}

//-----------------------------------------------------------------------------
void vtkAnimationUtils::LoadState()
{    
  if (!this->Interpolator)
    {
    return;
    }

  std::cout << "Reading VolumeBlogAnimation.xml..." << std::endl;

  pugi::xml_document document;
  if (!document.load_file("VolumeBlogAnimation.xml"))
    {
    std::cerr << "Error loading file: VolumeBlogAnimation.xml" << std::endl;
    return;
    }
  pugi::xml_node cameraInterpNode =
    document.child("VolumeBlogAnimationParameters").child("CameraInterpolator");
  int minT = cameraInterpNode.attribute("MinimumT").as_int();
  int maxT = cameraInterpNode.attribute("MaximumT").as_int();

  // Clean up the interpolator
  std::cout << "Cleaning up the interpolator" << std::endl;
  int min = static_cast<int>(this->Interpolator->GetMinimumT());
  int max = static_cast<int>(this->Interpolator->GetMaximumT());
  for (int i = min; i < max; ++i)
    {
    this->Interpolator->RemoveCamera(static_cast<double>(i));
    }

  // Add all the cameras
  std::cout << "Loading camera states..." << std::endl;
  int counter = minT;
  for (pugi::xml_node camNode = cameraInterpNode.child("Camera");
       camNode; camNode = camNode.next_sibling("Camera"))
    {
    if (counter > maxT)
      {
      break;
      }
    vtkNew<vtkCamera> cam;

    double pos[3];
    pos[0] = camNode.child("Position").attribute("X").as_double();
    pos[1] = camNode.child("Position").attribute("Y").as_double();
    pos[2] = camNode.child("Position").attribute("Z").as_double();
    cam->SetPosition(pos);

    double fp[3];
    fp[0] = camNode.child("FocalPoint").attribute("X").as_double();
    fp[1] = camNode.child("FocalPoint").attribute("Y").as_double();
    fp[2] = camNode.child("FocalPoint").attribute("Z").as_double();
    cam->SetFocalPoint(fp);

    double vup[3];
    vup[0] = camNode.child("ViewUp").attribute("X").as_double();
    vup[1] = camNode.child("ViewUp").attribute("Y").as_double();
    vup[2] = camNode.child("ViewUp").attribute("Z").as_double();
    cam->SetViewUp(vup);

    double cr[3];
    cr[0] = camNode.child("ClippingRange").attribute("Min").as_double();
    cr[1] = camNode.child("ClippingRange").attribute("Max").as_double();
    cam->SetClippingRange(cr);

    cam->SetViewAngle(camNode.attribute("ViewAngle").as_double());
    cam->SetParallelScale(camNode.attribute("ParallelScale").as_double());

    this->Interpolator->AddCamera(static_cast<double>(counter), cam.GetPointer());
    counter++;
    }
  std::cout << "Loading camera states...Done" << std::endl;
  std::cout << "Loaded " << counter << " camera state(s)" << std::endl;
  std::cout << "Reading VolumeBlogAnimation.xml...Done" << std::endl << std::endl;
}

//-----------------------------------------------------------------------------
void vtkAnimationUtils::SaveState()
{
  if (!this->Interpolator)
    {
    return;
    }
  std::cout << "Writing VolumeBlogAnimation.xml..." << std::endl;
  pugi::xml_document document;
  pugi::xml_node root = document.append_child("VolumeBlogAnimationParameters");
  root.append_attribute("version").set_value("0.0a");

  pugi::xml_node camInterpNode = root.append_child("CameraInterpolator");
  camInterpNode.append_attribute("MinimumT").set_value(
                                        this->Interpolator->GetMinimumT());
  camInterpNode.append_attribute("MaximumT").set_value(
                                        this->Interpolator->GetMaximumT());

  int min = static_cast<int>(this->Interpolator->GetMinimumT());
  int max = static_cast<int>(this->Interpolator->GetMaximumT());
  std::cout << "Exporting camera states..." << std::endl;
  for (int i = min; i <= max; ++i)
    {
    vtkNew<vtkCamera> cam;
    this->Interpolator->InterpolateCamera(static_cast<double>(i),
                                          cam.GetPointer());
    pugi::xml_node camNode = camInterpNode.append_child("Camera");
    pugi::xml_node pos = camNode.append_child("Position");
    pos.append_attribute("X").set_value(cam->GetPosition()[0]);
    pos.append_attribute("Y").set_value(cam->GetPosition()[1]);
    pos.append_attribute("Z").set_value(cam->GetPosition()[2]);

    pugi::xml_node fp = camNode.append_child("FocalPoint");
    fp.append_attribute("X").set_value(cam->GetFocalPoint()[0]);
    fp.append_attribute("Y").set_value(cam->GetFocalPoint()[1]);
    fp.append_attribute("Z").set_value(cam->GetFocalPoint()[2]);

    pugi::xml_node vup = camNode.append_child("ViewUp");
    vup.append_attribute("X").set_value(cam->GetViewUp()[0]);
    vup.append_attribute("Y").set_value(cam->GetViewUp()[1]);
    vup.append_attribute("Z").set_value(cam->GetViewUp()[2]);

    pugi::xml_node cr = camNode.append_child("ClippingRange");
    cr.append_attribute("Min").set_value(cam->GetClippingRange()[0]);
    cr.append_attribute("Max").set_value(cam->GetClippingRange()[1]);

    camNode.append_attribute("ViewAngle").set_value(cam->GetViewAngle());
    camNode.append_attribute("ParallelScale").set_value(cam->GetParallelScale());
    }
  std::cout << "Exporting camera states...Done" << std::endl;
  std::cout << "Exported " << max - min + 1 << " camera states" << std::endl;

  document.save_file("VolumeBlogAnimation.xml");
  std::cout << "Writing VolumeBlogAnimation.xml..." << std::endl << std::endl;
}

//-----------------------------------------------------------------------------
void vtkAnimationUtils::PrintSelf(ostream &os, vtkIndent indent)
{
  if (this->RenderWindow)
    {
    os << indent << "RenderWindow" << endl;
    this->RenderWindow->PrintSelf(os, indent.GetNextIndent());
    }
}
