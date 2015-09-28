/*=============================================================================
Copyright and License information
=============================================================================*/

#ifndef __vtkVolumePropertyCollection_h
#define __vtkVolumePropertyCollection_h

// VTK includes
#include <vtkObject.h>
#include <vtkNew.h>
#include <vtkVolumeProperty.h>

/// Multiple volume properties
class vtkVolumePropertyCollection : public vtkObject
{
public:
  vtkTypeMacro(vtkVolumePropertyCollection, vtkObject);
  void PrintSelf(ostream &os, vtkIndent indent);

  static vtkVolumePropertyCollection* New();

  vtkNew<vtkVolumeProperty> Bones;
  vtkNew<vtkVolumeProperty> Lung;

protected:
  vtkVolumePropertyCollection();
  ~vtkVolumePropertyCollection();

private:
  vtkVolumePropertyCollection(const vtkVolumePropertyCollection&); // Not implemented
  void operator=(const vtkVolumePropertyCollection); // Not implemented
};

#endif //__vtkVolumePropertyCollection_h
