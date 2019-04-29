//
// This source code is property of the Computer Graphics and Visualization 
// chair of the TU Dresden. Do not distribute in modified or unmodified form! 
// Copyright (C) 2018 CGV TU Dresden - All Rights Reserved
//

#include <vtkAutoInit.h>
VTK_MODULE_INIT( vtkRenderingOpenGL2 );
VTK_MODULE_INIT( vtkInteractionStyle );
VTK_MODULE_INIT( vtkRenderingFreeType );

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkDEMReader.h>
#include <vtkContourFilter.h>
#include <vtkImageGradientMagnitude.h>
#include <vtkWarpScalar.h>
#include <vtkDataSetMapper.h>
#include <vtkLookupTable.h>

#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRendererCollection.h>

#include <vtkColorTransferFunction.h>
#include <vtkScalarBarActor.h>
#include <vtkTextRenderer.h>


// standard includes
#include <vector>
#include <algorithm>




// ----- utility functions -----
void setGradientBackground( vtkSmartPointer<vtkRenderer> renderer )
{
	renderer->GradientBackgroundOn();
	renderer->SetBackground( 0.5, 0.5, 0.5 );
	renderer->SetBackground2( 0.9, 0.9, 0.9 );
}
// ----- end of utility functions -----


vtkSmartPointer<vtkRenderWindow> createRenderWindowFromMapper( vtkSmartPointer<vtkMapper> mapper )
{
	//create renderer, window and actor
	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> window = vtkSmartPointer<vtkRenderWindow>::New();
	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	
	// let's have a fancy background for the canvas
	setGradientBackground( renderer );
	// fallback color, is used whenever mappers do not utilize scalars for coloring
	actor->GetProperty()->SetColor( 1, 0, 0 );

	// connect everything
	actor->SetMapper( mapper );
	renderer->AddActor( actor );
	window->AddRenderer( renderer );
	
	// set window size and return
	window->SetSize( 800, 800 );
	return window;
}

vtkSmartPointer<vtkRenderWindow> createRenderWindowFromMultipleMappers( std::vector<vtkSmartPointer<vtkMapper>> mappers )
{
	// create renderer and window
	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> window = vtkSmartPointer<vtkRenderWindow>::New();

	// let's have a fancy background for the canvas
	setGradientBackground( renderer );

	// connect window and renderer
	window->AddRenderer( renderer );

	// task 4.2
	// for every mapper we create an actor and add it to the renderer
	// (do not forget to set the fallback color (see createRenderWindowFromMapper(...) )
	
	// student begin...

	// your code here
	for (vtkSmartPointer<vtkMapper> mapper : mappers)
	{
		vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
		actor->SetMapper(mapper);
		//fallback color
		actor->GetProperty()->SetColor(1, 0, 0);
		renderer->AddActor(actor);
	}
	
	// ... student end


	// set window size and return
	window->SetSize( 800, 800 );
	return window;
}

void doRenderingAndInteraction( vtkSmartPointer<vtkRenderWindow> window )
{
    // create interactor and connect a window
	vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	interactor->SetRenderWindow( window );
	// set an interaction style
	interactor->SetInteractorStyle( vtkInteractorStyleTrackballCamera::New() );

    // execute render/interaction loop
    interactor->Initialize();
    interactor->Start();

    // close the window when finished
    window->Finalize();
}


int main(int argc, char * argv[])
{
    // -- begin of basic visualization network definition --
   
	// 1. create source
	vtkSmartPointer<vtkDEMReader> source = vtkSmartPointer<vtkDEMReader>::New();
	source->SetFileName( "../../../data/SainteHelens.dem" );
	
	// 1a) Warped Scalars
	vtkSmartPointer<vtkWarpScalar> warpScalar = vtkSmartPointer<vtkWarpScalar>::New();
	warpScalar->SetInputConnection(source->GetOutputPort());
	warpScalar->SetScaleFactor(2);

	// 1b) mapper for warped scalars
	vtkSmartPointer<vtkDataSetMapper> warpMapper = vtkSmartPointer<vtkDataSetMapper>::New();
	warpMapper->SetInputConnection(warpScalar->GetOutputPort());

	// 1c) actor for warped scalars
	/*vtkSmartPointer<vtkActor> warpActor = vtkSmartPointer<vtkActor>::New();
	warpActor->SetMapper(warpMapper);*/

	// 1d) Visualize
	vtkSmartPointer<vtkRenderer> warpRenderer = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> warpRenderWindow = createRenderWindowFromMapper(warpMapper);

    // 2. create filters
	// a) contour filter
	vtkSmartPointer<vtkContourFilter> contourFilter = vtkSmartPointer<vtkContourFilter>::New();
	// use source as filter input
	contourFilter->SetInputConnection( warpScalar->GetOutputPort() );
	// set the height value at which the contour line shall be drawn
	for (int i = 1; i <= 15; i++)
	{
		contourFilter->SetValue(i, 200*i);
	}
	
	//
	// b) gradient filter
	//vtkSmartPointer<vtkImageGradientMagnitude> gradientFilter = vtkSmartPointer<vtkImageGradientMagnitude>::New();
	//// how many dimensions does the data have
	//gradientFilter->SetDimensionality( 2 );
	//// use source as filter input
	//gradientFilter->SetInputConnection( source->GetOutputPort() );
	
	

	//// 3.  create mappers
	// a) image mapper, show the data as 2D image with standard color transfer function
	vtkSmartPointer<vtkDataSetMapper> imageMapper = vtkSmartPointer<vtkDataSetMapper>::New();
	// connect directly to source (the pipeline is source->imageMapper->...)
	imageMapper->SetInputConnection( warpScalar->GetOutputPort() );
	// set the range of the scalar values in order to correctly map the colors
	imageMapper->SetScalarRange( 680, 3000 );
	
	// b) contour mapper, show the regions where the data has a specific value
	vtkSmartPointer<vtkDataSetMapper> contourMapper = vtkSmartPointer<vtkDataSetMapper>::New();
	// connect to the contour filter output (the pipeline is source->contourFilter->contourMapper->...)
	contourMapper->SetInputConnection( contourFilter->GetOutputPort() );
	// avoid z-buffer fighting with small polygon shift
	contourMapper->SetResolveCoincidentTopologyToPolygonOffset();
	// don't use the scalar value to color the line, see fallback coloring via actor in createRenderWindowFromMapper
	contourMapper->ScalarVisibilityOff();
	//
	//// c) gradient mapper, show the gradient magnitudes as 2D image 
	//vtkSmartPointer<vtkDataSetMapper> gradientMapper = vtkSmartPointer<vtkDataSetMapper>::New();
	//// connect to the gradient filter output (the pipeline is source->gradientFilter->gradientMapper->...)
	//gradientMapper->SetInputConnection( gradientFilter->GetOutputPort() );
	//// set the range of the gradient magnitudes in order to correctly map the colors
	//gradientMapper->SetScalarRange( 0, 2 );
	//
 //   // -- end of basic visualization network definition --


 //   // 4. create actors, renderers, render windows 	
	//vtkSmartPointer<vtkRenderWindow> imageWindow = createRenderWindowFromMapper( imageMapper );
	//vtkSmartPointer<vtkRenderWindow> contourWindow = createRenderWindowFromMapper( contourMapper );
	//vtkSmartPointer<vtkRenderWindow> gradientWindow = createRenderWindowFromMapper( gradientMapper );

	// Create mapper vector to store imageMapper and contourMapper
	std::vector<vtkSmartPointer<vtkMapper>> mappers;
	mappers.push_back(imageMapper);
	mappers.push_back(contourMapper);
	vtkSmartPointer<vtkRenderWindow> finalWindow = createRenderWindowFromMultipleMappers(mappers);

	// Setting up actor and lookup table for legend

	vtkSmartPointer<vtkLookupTable> hueLut = vtkSmartPointer<vtkLookupTable>::New();
	hueLut->SetTableRange ((int) 680, (int)3000);
	hueLut->SetHueRange (0, 0.5);
	hueLut->SetSaturationRange (1, 1);
	hueLut->SetValueRange (1, 1);
	hueLut->Build();

	vtkSmartPointer<vtkScalarBarActor> scalarBarActor = vtkSmartPointer<vtkScalarBarActor>::New();
	finalWindow->GetRenderers()->GetFirstRenderer()->AddActor2D(scalarBarActor);
	scalarBarActor->SetLookupTable(hueLut);
	scalarBarActor->SetTitle("Elevation");
	// scalarBarActor->SetUnconstrainedFontSize(30);
	
 //   // 5. successively show each window and allow user interaction (until it is closed)
	//doRenderingAndInteraction(warpRenderWindow);
	//doRenderingAndInteraction( imageWindow );
	//doRenderingAndInteraction( contourWindow );
	//doRenderingAndInteraction( gradientWindow );
	doRenderingAndInteraction(finalWindow);

    return 0;
}