/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: fft.h
 *
 * Code generated for Simulink model 'fft'.
 *
 * Model version                  : 1.4
 * Simulink Coder version         : 8.12 (R2017a) 16-Feb-2017
 * C/C++ source code generated on : Thu Dec 14 09:36:37 2017
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Intel->x86-64 (Windows64)
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef RTW_HEADER_fft_h_
#define RTW_HEADER_fft_h_
#include <string.h>
#include <stddef.h>
#ifndef fft_COMMON_INCLUDES_
# define fft_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* fft_COMMON_INCLUDES_ */

#include "fft_types.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm)        ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val)   ((rtm)->errorStatus = (val))
#endif

/* Constant parameters (auto storage) */
typedef struct {
  /* Computed Parameter: FFT_TwiddleTable
   * Referenced by: '<Root>/FFT'
   */
  real32_T FFT_TwiddleTable[192];
} ConstP_fft_T;

/* Real-time Model Data Structure */
struct tag_RTM_fft_T {
  const char_T * volatile errorStatus;
};

/* Constant parameters (auto storage) */
extern const ConstP_fft_T fft_ConstP;

/* Model entry point functions */
extern void fft_initialize(void);
extern void fft_terminate(void);

/* Customized model step function */
extern void fft_custom(const real32_T arg_In1[256], creal32_T arg_Out1[256]);

/* Real-time Model object */
extern RT_MODEL_fft_T *const fft_M;

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'fft'
 */
#endif                                 /* RTW_HEADER_fft_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
