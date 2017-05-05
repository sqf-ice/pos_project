
#ifndef _TYPEDEFS_VX_H_
#define _TYPEDEFS_VX_H_

/* suppported architectures */


#define SECOND 100           //!< \brief second definition (100)
#define MINUTE (60L*SECOND)  //!< \brief minute definition
#define HOUR (60L*MINUTE)    //!< \brief hour definition

											

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#define castptr(cast_type,cast_var) ((cast_type)(void*)(cast_var))
// extern MagData *crd;



#endif	// _TYPEDEFS_VX_H_
