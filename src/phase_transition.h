#include <iostream>
#include "MD.h"

// Load Intel math lib if available
#if defined(__INTEL_COMPILER)
#include <mathimf.h>  // Intel Math library
#define COMPILER "INTEL"
#else
#include <math.h>
#endif

class phase_transition : public MD {
 private:
  /* data */
 public:
  using MD::MD;

  void crystallisation(double DENSITY, double FINAL_DENSITY, double DENSITY_INC,
                       double TEMPERATURE, double POWER, double A_CST,
                       std::string pp_type);

  void run_backwards(double DENSITY, double FINAL_DENSITY, double DENSITY_INC,
                     double TEMPERATURE, double POWER, double A_CST,
                     std::string pp_type);
  // TODO:run a forward crystallisation and go back a

  void detect_transition();
};