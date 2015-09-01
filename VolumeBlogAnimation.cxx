// Code to produce the animation movie for the volume blog

#include <vtkActor.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkColorTransferFunction.h>
#include <vtkCommand.h>
//#include <vtkCullerCollection.h>
//#include <vtkCuller.h>
//#include <vtkDataArray.h>
//#include <vtkFrustumCoverageCuller.h>
#include <vtkGPUVolumeRayCastMapper.h>
//#include <vtkImageCheckerboard.h>
#include <vtkImageData.h>
//#include <vtkImageGridSource.h>
//#include <vtkImageShiftScale.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNew.h>
#include <vtkPiecewiseFunction.h>
//#include <vtkPlaneCollection.h>
//#include <vtkPlane.h>
//#include <vtkPlaneSource.h>
//#include <vtkPNGWriter.h>
//#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
//#include <vtkSampleFunction.h>
//#include <vtkSLCReader.h>
#include <vtkSmartPointer.h>
//#include <vtkSphere.h>
//#include <vtkTransform.h>
//#include <vtkVersion.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
//#include <vtkWindowToImageFilter.h>
//#include <vtkXMLImageDataReader.h>
#include <vtkCameraWidget.h>
#include <vtkDICOMImageReader.h>
#include <vtkCameraInterpolator.h>

#include <string>

#include "pugixml/pugixml.hpp"

struct CameraInterpolator
{
  vtkSmartPointer<vtkCamera> Camera;
  vtkSmartPointer<vtkCameraInterpolator> Interpolator;
  vtkSmartPointer<vtkRenderWindow> RenderWindow;
  vtkSmartPointer<vtkVolumeProperty> VolumeProperty;

  CameraInterpolator()
    {
    Camera = 0;
    Interpolator = 0;
    RenderWindow = 0;
    VolumeProperty = 0;
    }

  void AddCamera()
    {
    if (!this->Interpolator || !this->Camera)
      {
      return;
      }
    double t = static_cast<double> (this->Interpolator->GetNumberOfCameras());
    this->Interpolator->AddCamera(t, this->Camera);
    }

  void Animate()
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

  void WriteAnimationState()
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
      this->Interpolator->InterpolateCamera(static_cast<double>(i), cam.GetPointer());
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

  void ReadAnimationState()
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
};

void KeypressCallbackFunction( vtkObject* caller,
                               long unsigned int vtkNotUsed(eventId),
                               void* clientData,
                               void* vtkNotUsed(callData))
{
  vtkSmartPointer<vtkRenderWindowInteractor> iren =
    vtkRenderWindowInteractor::SafeDownCast(caller);

  CameraInterpolator* camInterp =
    reinterpret_cast<CameraInterpolator*>(clientData);

  if (!iren)
    {
    return;
    }
  std::string key = iren->GetKeySym();
  if (key == "c")
    {
    // Add current camera parameters to animation
    camInterp->AddCamera();
    }
  else if (key == "a")
    {
    // Animate the current interpolator spline
    camInterp->Animate();
    }
  else if (key == "w")
    {
    // Write the animation state
    camInterp->WriteAnimationState();
    }
  else if (key == "l")
    {
    camInterp->ReadAnimationState();
    }
}

int main(int argc, char * argv[])
{
  if (argc < 2)
    {
    std::cerr << "Usage: " << argv[0] << " <input file> " << std::endl;
    return EXIT_FAILURE;
    }

  vtkNew<vtkDICOMImageReader> reader;
  reader->SetDirectoryName(argv[1]);

  vtkNew<vtkGPUVolumeRayCastMapper> volumeMapper;
//  volumeMapper->SetAutoAdjustSampleDistances(0);
//  volumeMapper->SetSampleDistance(0.1);

  vtkNew<vtkRenderer> ren;

  // Create transfer mapping scalar value to opacity.
  vtkNew<vtkPiecewiseFunction> opacityTransferFunction;
  opacityTransferFunction->AddPoint(-1000, 0.0);
  opacityTransferFunction->AddPoint(132.19, 0.0);
  opacityTransferFunction->AddPoint(258.93, 0.19);
  opacityTransferFunction->AddPoint(952, 0.2);

  // Create transfer mapping scalar value to color.
  vtkNew<vtkColorTransferFunction> colorTransferFunction;
  colorTransferFunction->AddRGBPoint(-1000,76/255.0,76/255.0,1.0);
  colorTransferFunction->AddRGBPoint(-508,76/255.0,1.0,76/255.0);
  colorTransferFunction->AddRGBPoint(443.28,168/255.0,140/255.0,155/255.0);
  colorTransferFunction->AddRGBPoint(639.15,1.0,233/255.0,9/255.0);
  colorTransferFunction->AddRGBPoint(953,1.0,76/255.0,76/255.0);

  vtkNew<vtkVolumeProperty> volumeProperty;
  volumeProperty->SetColor(colorTransferFunction.GetPointer());
  volumeProperty->SetScalarOpacity(opacityTransferFunction.GetPointer());
  volumeProperty->ShadeOn();
  volumeProperty->SetInterpolationTypeToLinear();
  volumeProperty->SetDisableGradientOpacity(1);

  volumeMapper->SetInputConnection(reader->GetOutputPort());

  vtkNew<vtkVolume> volume;
  volume->SetMapper(volumeMapper.GetPointer());
  volume->SetProperty(volumeProperty.GetPointer());
  volumeMapper->SetBlendModeToComposite();
  ren->AddVolume(volume.GetPointer());

  // Setup render window
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(1024, 1024);
  renderWindow->SetAlphaBitPlanes(1);
  renderWindow->AddRenderer(ren.GetPointer());
  ren->ResetCamera();

  // Setup the camera interpolation
  vtkNew<vtkCameraInterpolator> cameraInterp;
  cameraInterp->SetInterpolationTypeToSpline();

  // Setup render window interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow.GetPointer());
  vtkNew<vtkInteractorStyleTrackballCamera> style;
  renderWindowInteractor->SetInteractorStyle(style.GetPointer());

  // Hook up custom events
  CameraInterpolator* camInterp = new CameraInterpolator();
  camInterp->RenderWindow = renderWindow.GetPointer();
  camInterp->Interpolator = cameraInterp.GetPointer();
  camInterp->Camera = ren->GetActiveCamera();
  camInterp->VolumeProperty = volumeProperty.GetPointer();
  vtkNew<vtkCallbackCommand> keypressCallback;
  keypressCallback->SetCallback(KeypressCallbackFunction);
  keypressCallback->SetClientData(camInterp);
  renderWindowInteractor->AddObserver(vtkCommand::KeyPressEvent,
                                      keypressCallback.GetPointer());

  // Render and start interaction
  renderWindow->Render();
  renderWindowInteractor->Initialize();

  // Start the event loop
  renderWindowInteractor->Start();

  delete camInterp;
  return EXIT_SUCCESS;
}
