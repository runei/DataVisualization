//
// This source code is property of the Computer Graphics and Visualization 
// chair of the TU Dresden. Do not distribute in modified or unmodified form! 
// Copyright (C) 2017, 2018 CGV TU Dresden - All Rights Reserved
//

#include <vtkAutoInit.h>
// Needed for general rendering
VTK_MODULE_INIT(vtkRenderingOpenGL2);
// Needed for interactive controls
VTK_MODULE_INIT(vtkInteractionStyle);
// Guess what that is needed for
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2);
// Needed for the text on the slider
VTK_MODULE_INIT(vtkRenderingFreeType);

#include "vtkhelper.h"

#include <vtkSmartPointer.h>
#include <vtkImageData.h>

#include <vtkXMLImageDataReader.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkVolumeProperty.h>
#include <vtkSmartVolumeMapper.h>

#include <vtkMarchingCubes.h>
#include <vtkDataSetMapper.h>
#include <vtkLookupTable.h>

#include <vtkSliderRepresentation2D.h>
#include <vtkSliderWidget.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>

#include <vtkVersion.h>
#include <vtkSphere.h>
#include <vtkSampleFunction.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkCamera.h>
#include <vtkImageShiftScale.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>

#include <vtkVoxelModeller.h>
#include <vtkSphereSource.h>
#include <vtkDICOMImageReader.h>

#include <vtkActor.h>
#include <vtkPolyDataMapper.h>

class IsoSliderCallback : public vtkCommand {
private:
	vtkSmartPointer<vtkMarchingCubes> isoSurface;
	IsoSliderCallback() {}

public:
	static IsoSliderCallback *New() { return new IsoSliderCallback; }

	void SetData( vtkSmartPointer<vtkMarchingCubes> isoSurface ) { this->isoSurface = isoSurface; }

	virtual void Execute( vtkObject *caller, unsigned long eventId, void *callData ) {
		// Get our slider widget back
		vtkSliderWidget *slider = static_cast<vtkSliderWidget*>(caller);

		// Get the value
		double value = static_cast<vtkSliderRepresentation*>(slider->GetRepresentation())->GetValue();

		// Set new Iso value
		isoSurface->SetValue( 0, value );
		isoSurface->Update();
	}
};

int main()
{
	vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
	vtkSmartPointer<vtkXMLImageDataReader> source = vtkSmartPointer<vtkXMLImageDataReader>::New();
	source->SetFileName("../../../data/headsq-half.vti");
	source->Update();
	imageData->ShallowCopy(source->GetOutput());


	// Task 5.2

	// visualize volume directly:
	// * create a vtkSmartVolumeMapper that gets its input from the source
	vtkSmartPointer<vtkSmartVolumeMapper> volumeMapper = vtkSmartPointer<vtkSmartVolumeMapper>::New();
	volumeMapper->SetBlendModeToComposite(); // composite first
	volumeMapper->SetInputData(imageData);

	// * create an opacity transfer function as vtkPiecewiseFunction and add density-opacity pairs
	vtkSmartPointer<vtkPiecewiseFunction> compositeOpacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
	compositeOpacity->AddPoint(0.0,0.0);
	compositeOpacity->AddPoint(127.0,1.0);
	compositeOpacity->AddPoint(255.0,0.0);

	// * create a color transfer function as vtkColorTransferFunction and add density-color pairs
	vtkSmartPointer<vtkColorTransferFunction> color = vtkSmartPointer<vtkColorTransferFunction>::New();
	color->AddRGBPoint(0.0  ,1.0,0.0,0.0);
	color->AddRGBPoint(40.0  ,0.0,0.0,1.0);
	color->AddRGBPoint(255.0,1.0,0.0,0.0);

	// * create a vtkVolumeProperty object, set the opacity and color function and set the 
	//   interpolation type to linear. Turn shading on
	vtkSmartPointer<vtkVolumeProperty> volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
	volumeProperty->SetScalarOpacity(compositeOpacity);
	volumeProperty->SetColor(color);
	volumeProperty->ShadeOn();
	volumeProperty->SetInterpolationType(VTK_LINEAR_INTERPOLATION);

	// * create the actor as a vtkVolume object and assign the previously created volume mapper and property object
	vtkSmartPointer<vtkVolume> volume = vtkSmartPointer<vtkVolume>::New();
	volume->SetMapper(volumeMapper);
	volume->SetProperty(volumeProperty);

	// * create a vtkRenderer and a vtkRenderWindow. (Note that you cannot use the method createRenderWindowFromMapper
	//   since it does not create a vtkVolume actor.)
	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	renderWindow->AddRenderer(renderer);
	vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	iren->SetRenderWindow(renderWindow);
	renderer->AddViewProp(volume);
	// * you can create interactor and display as usual via doRenderingAndInteraction(window) to test your code to this point.
	// doRenderingAndInteraction(renderWindow);
	// (delete the line when you're done)


	// visualize volume via isosurfaces:
	// * generate polygon data from the volume dataset by using a vtkMarchingCubes filter
	vtkSmartPointer<vtkMarchingCubes> surface = vtkSmartPointer<vtkMarchingCubes>::New();
	surface->SetInputData(imageData);
	// * set number of contours to one, set scalar value of that contour to something meaningful
	surface->SetNumberOfContours(1);
	surface->SetValue(0, 300);
	// * manually update the Marching Cubes filter aftwerwards via Update() method to apply the contour value
	surface->Update();
	// * create vtkDataSetMapper and set input connection, don't use scalars for coloring (set scalar visibility to false)
	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(surface->GetOutputPort());
	mapper->ScalarVisibilityOff();
	// * create vtkActor and set mapper as input
	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	// * assign actor to existing renderer
	renderer->AddActor(actor);


	// * create a slider as a slider 2d representation
	vtkSmartPointer<vtkSliderRepresentation2D> sliderRep = vtkSmartPointer<vtkSliderRepresentation2D>::New();
	sliderRep->GetPoint1Coordinate()->SetCoordinateSystemToDisplay();
  sliderRep->GetPoint1Coordinate()->SetValue(40 ,40);
  sliderRep->GetPoint2Coordinate()->SetCoordinateSystemToDisplay();
  sliderRep->GetPoint2Coordinate()->SetValue(100, 40);
	// * set the minimum and maximum values to correspond to the dataset
	sliderRep->SetMinimumValue(0.0);
	sliderRep->SetMaximumValue(500.0);

	// * show a slider title
	sliderRep->SetTitleText("Iso-value");
	// * show the current slider value above the slider with one digit behind the decimal point (setLabelFormat)
	sliderRep->SetValue(300.0);
	// * you need to assign an interactor to the slider in order to use it: 
	// * create a vtkRenderWindowInteractor and assign a rendering window 
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);
	// * create a new vtkSliderWidget and assign the previous interactor and representation to it
	vtkSmartPointer<vtkSliderWidget> sliderWidget = vtkSmartPointer<vtkSliderWidget>::New();
	sliderWidget->SetInteractor(renderWindowInteractor);
	// * use SetAnimationModeToAnimate() and EnabledOn()
	sliderWidget->SetRepresentation(sliderRep);
	sliderWidget->SetAnimationModeToAnimate();
	sliderWidget->EnabledOn();


	// * invoke the callback code:
	// * create an IsoSlider Callback
	vtkSmartPointer<IsoSliderCallback> callback = vtkSmartPointer<IsoSliderCallback>::New();
	// * assign the Marching Cubes data
	callback->SetData(surface);
	// * assign the callback object to the slider via AddObserver(vtkCommand::InteracationEvent, ptrToCallback);
	sliderWidget->AddObserver(vtkCommand::InteractionEvent, callback);

	// * finally you can then use the version of doRenderingAndInteraction that accepts an interactor object.

	renderWindowInteractor->Initialize();
	renderWindow->Render();
	renderWindowInteractor->Start();
	iren->Start();

	return 0;
}
