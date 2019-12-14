#include <iostream>
#include <string>
#include <thread>
#include "MD.h"
#include "helper_functions.h"

#define STEPS 10000

std::string dir_linux = ".";

void MakeDataBase();

int main() { MakeDataBase(); }

void MakeDataBase() {
  /*
   * Builds a relatively complete database mapping the rho,T,n,a space
   * of te MD fluid. Performs Also statistical averaging at the end with
   * quantities such as Pc, U, K, E, Pk.
   * NOTE: It takes a long time to complete and generates multiple files!
   */

  size_t num = 1;
  std::vector<size_t> n = {8, 10, 12};
  std::vector<double> rho = {0.20, 0.3, 0.5, 1.0};
  std::vector<double> T = {0.5, 1.0, 1.5, 2.0};

  std::vector<double> A1 = helper_functions::linspace(0.0, 1.0, 5);
  std::vector<double> A2 = helper_functions::linspace(1.25, 2.25, 5);
  std::vector<double> A3 = helper_functions::linspace(2.50, 4.50, 5);
  std::vector<double> A4 = {0.20, 0.40, 0.60, 0.80, 0.90};

  for (size_t d = 0; d < rho.size(); ++d) {
    for (size_t t = 0; t < T.size(); ++t) {
      for (size_t i = 0; i < n.size(); ++i) {
        for (size_t j = 0; j < A1.size(); ++j) {
          std::cout << " run num: " << num << std::endl;

          MD* run1 = new MD(STEPS, {10, 10, 10}, "SC");
          MD* run2 = new MD(STEPS, {10, 10, 10}, "SC");
          MD* run3 = new MD(STEPS, {10, 10, 10}, "SC");
          MD* run4 = new MD(STEPS, {10, 10, 10}, "SC");

          // ? set temperature manually in default machine
          std::thread th1(&MD::simulation, run1, "", rho[d], T[t], n[i], A1[j],
                          "BIP");
          std::thread th2(&MD::simulation, run2, "", rho[d], T[t], n[i], A2[j],
                          "BIP");
          std::thread th3(&MD::simulation, run3, "", rho[d], T[t], n[i], A3[j],
                          "BIP");
          std::thread th4(&MD::simulation, run4, "", rho[d], T[t], n[i], A4[j],
                          "BIP");

          th1.join();
          th2.join();
          th3.join();
          th4.join();

          delete run1;
          delete run2;
          delete run3;
          delete run4;

          ++num;
        }
      }
    }
  }
}
