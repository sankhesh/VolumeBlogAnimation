/*=============================================================================
Copyright and License information
=============================================================================*/

#ifndef __vtkAnimationUtils_h
#define __vtkAnimationUtils_h

#include "vtkVolumePropertyCollection.h"

// VTK includes
#include <vtkCamera.h>
#include <vtkCameraInterpolator.h>
#include <vtkObject.h>
#include <vtkRenderWindow.h>
#include <vtkVolumeProperty.h>
#include <vtkNew.h>

/// This class provides utility functions to animate the view
class vtkAnimationUtils : public vtkObject
{
public:
  vtkTypeMacro(vtkAnimationUtils, vtkObject);
  void PrintSelf(ostream &os, vtkIndent indent);

  static vtkAnimationUtils* New();

  // Set/Get the active camera
  vtkSetObjectMacro(Camera, vtkCamera);
  vtkGetObjectMacro(Camera, vtkCamera);

  // Set/Get the camera interpolator
  vtkSetObjectMacro(Interpolator, vtkCameraInterpolator);
  vtkGetObjectMacro(Interpolator, vtkCameraInterpolator);

  // Set/Get the render window
  vtkSetObjectMacro(RenderWindow, vtkRenderWindow);
  vtkGetObjectMacro(RenderWindow, vtkRenderWindow);

  // Set/Get the volume property
  vtkSetObjectMacro(VolumeProperty, vtkVolumeProperty);
  vtkGetObjectMacro(VolumeProperty, vtkVolumeProperty);

  // Add the current view to the animation stack
  void AddCurrentView();

  // Animate the view
  void Animate();

  // Load animation state
  void LoadState();

  // Save animation state
  void SaveState();

protected:
  vtkAnimationUtils();
  ~vtkAnimationUtils();

  vtkCamera* Camera;
  vtkCameraInterpolator* Interpolator;
  vtkRenderWindow* RenderWindow;
  vtkVolumeProperty* VolumeProperty;

  vtkNew<vtkVolumePropertyCollection> Properties;

private:
  vtkAnimationUtils(const vtkAnimationUtils&); // Not implemented
  void operator=(const vtkAnimationUtils); // Not implemented
};

#endif //__vtkAnimationUtils_h
