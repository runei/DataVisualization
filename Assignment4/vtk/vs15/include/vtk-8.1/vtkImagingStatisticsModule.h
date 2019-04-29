
#ifndef VTKIMAGINGSTATISTICS_EXPORT_H
#define VTKIMAGINGSTATISTICS_EXPORT_H

#ifdef VTKIMAGINGSTATISTICS_STATIC_DEFINE
#  define VTKIMAGINGSTATISTICS_EXPORT
#  define VTKIMAGINGSTATISTICS_NO_EXPORT
#else
#  ifndef VTKIMAGINGSTATISTICS_EXPORT
#    ifdef vtkImagingStatistics_EXPORTS
        /* We are building this library */
#      define VTKIMAGINGSTATISTICS_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define VTKIMAGINGSTATISTICS_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef VTKIMAGINGSTATISTICS_NO_EXPORT
#    define VTKIMAGINGSTATISTICS_NO_EXPORT 
#  endif
#endif

#ifndef VTKIMAGINGSTATISTICS_DEPRECATED
#  define VTKIMAGINGSTATISTICS_DEPRECATED __declspec(deprecated)
#  define VTKIMAGINGSTATISTICS_DEPRECATED_EXPORT VTKIMAGINGSTATISTICS_EXPORT __declspec(deprecated)
#  define VTKIMAGINGSTATISTICS_DEPRECATED_NO_EXPORT VTKIMAGINGSTATISTICS_NO_EXPORT __declspec(deprecated)
#endif

#define DEFINE_NO_DEPRECATED 0
#if DEFINE_NO_DEPRECATED
# define VTKIMAGINGSTATISTICS_NO_DEPRECATED
#endif



#endif
