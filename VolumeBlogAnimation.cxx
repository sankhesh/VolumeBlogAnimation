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
