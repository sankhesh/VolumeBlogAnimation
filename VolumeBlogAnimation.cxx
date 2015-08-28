// Code to produce the animation movie for the volume blog

#include <vtkActor.h>
//#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkColorTransferFunction.h>
//#include <vtkCommand.h>
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
#include <vtkCameraRepresentation.h>
#include <vtkCameraWidget.h>
#include <vtkDICOMImageReader.h>
#include <vtkCameraInterpolator.h>
#include <vtkCameraWidget.h>

#include <string>

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

  // Setup the camera representation and interpolation
  vtkNew<vtkCameraRepresentation> cameraRep;
  cameraRep->SetNumberOfFrames(500);
  vtkNew<vtkCameraInterpolator> cameraInterp;
  cameraInterp->SetInterpolationTypeToSpline();
  cameraRep->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
  cameraRep->GetPositionCoordinate()->SetValue(0.08, 0.07);
  cameraRep->GetPosition2Coordinate()->SetCoordinateSystemToNormalizedDisplay();
  cameraRep->GetPosition2Coordinate()->SetValue(0.16, 0.14);

  // Setup render window interactor
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  vtkNew<vtkInteractorStyleTrackballCamera> style;
  renderWindowInteractor->SetInteractorStyle(style.GetPointer());

  // Setup the camera widget
  vtkNew<vtkCameraWidget> cameraWidget;
  cameraWidget->SetInteractor(renderWindowInteractor.GetPointer());
  cameraWidget->SetRepresentation(cameraRep.GetPointer());
  cameraWidget->KeyPressActivationOff();

  cameraRep->SetCamera(ren->GetActiveCamera());

  // Render and start interaction
  renderWindowInteractor->SetRenderWindow(renderWindow.GetPointer());
  renderWindow->Render();
  renderWindowInteractor->Initialize();

  // Enable the widget
  cameraWidget->On();

  // Start the event loop
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
