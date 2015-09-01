// Code to produce the animation movie for the volume blog

// Custom includes
#include "vtkAnimationUtils.h"
#include "pugixml/pugixml.hpp"

// VTK includes
#include <vtkActor.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkCameraInterpolator.h>
#include <vtkCameraWidget.h>
#include <vtkColorTransferFunction.h>
#include <vtkCommand.h>
#include <vtkDICOMImageReader.h>
#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkImageData.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNew.h>
#include <vtkPiecewiseFunction.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>

// STL includes
#include <string>

void KeypressCallbackFunction( vtkObject* caller,
                               long unsigned int vtkNotUsed(eventId),
                               void* clientData,
                               void* vtkNotUsed(callData))
{
  vtkSmartPointer<vtkRenderWindowInteractor> iren =
    vtkRenderWindowInteractor::SafeDownCast(caller);

  vtkSmartPointer<vtkAnimationUtils> utils =
    reinterpret_cast<vtkAnimationUtils*>(clientData);

  if (!iren)
    {
    return;
    }
  std::string key = iren->GetKeySym();
  if (key == "c")
    {
    // Add current camera parameters to animation
    utils->AddCurrentView();
    }
  else if (key == "a")
    {
    // Animate the current interpolator spline
    utils->Animate();
    }
  else if (key == "w")
    {
    // Write the animation state
    utils->SaveState();
    }
  else if (key == "l")
    {
    utils->LoadState();
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
  vtkNew<vtkAnimationUtils> utils;
  utils->SetRenderWindow(renderWindow.GetPointer());
  utils->SetCamera(ren->GetActiveCamera());
  utils->SetInterpolator(cameraInterp.GetPointer());
  utils->SetVolumeProperty(volumeProperty.GetPointer());
  vtkNew<vtkCallbackCommand> keypressCallback;
  keypressCallback->SetCallback(KeypressCallbackFunction);
  keypressCallback->SetClientData(utils.GetPointer());
  renderWindowInteractor->AddObserver(vtkCommand::KeyPressEvent,
                                      keypressCallback.GetPointer());

  // Render and start interaction
  renderWindow->Render();
  renderWindowInteractor->Initialize();

  // Start the event loop
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
