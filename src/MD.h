/*
 * Ioannis Nikiteas 13/7/2017
 *
 * BSc Dissertation:
 * Investigating the transition from Molecular Dynamics to
 * Smoothed Particle Hydrodynamics
 *
 * University: Royal Holloway University of London
 *
 * A program meant to simulate a MD fluid with an only
 * repulsive BIP pair-potential. Increasing the parameter A
 * creates a coarse-graining effect for the system allowing it
 * to transition to SPH
 */
#pragma once
#include <chrono>      // CPU run-time
#include <ctime>       // std::chrono
#include <functional>  // funciton pointers
#include <iomanip>     // setprecision
#include <numeric>     // accumulate
#include <random>      // normal_dist
#include <sstream>     // stringstream
#include <vector>      // vectors

#include "md_pair_potentials.h"
#include "stat_file_logger.h"

// Check for Compiler support
// TODO: in future C++ versions, rm fs:: from global scope and mv in constructor
#if __cplusplus <= 201103L
#error This library requires at least C++17 compiler support
// If C++ version C++2a or above use
#elif __cplusplus >= 201709
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

// Load Intel math lib if available
#if defined(__INTEL_COMPILER)
#include <mathimf.h>  // Intel Math library
#define COMPILER "INTEL"
#else
#include <math.h>
#endif

class MD {
 protected:
  std::vector<double> rx, ry, rz;        // Position Arrays
  std::vector<double> vx, vy, vz;        // Velocity Arrays
  std::vector<double> fx, fy, fz;        // Force arrays
  std::vector<double> Cvx, Cvy, Cvz;     // VAF arrays
  std::vector<double> rrx, rry, rrz;     // used in MSD calculation
  std::vector<double> MSDx, MSDy, MSDz;  // MSD arrays
  std::vector<double> sfx, sfy, sfz;     // Structure factor k-arrays

  // Statistical quantity vectors, VAF, MSD, Energies and pressures
  std::vector<double> Cr, msd, u_en, k_en, pc, pk, temperature, density;

  /* Constructor variables */
  size_t __Nx, __Ny, __Nz, __N;  // Particles in x, y, z and total
  size_t __step_idx, __steps;    // step index, maximum steps
  size_t __nhist, __rdf_wait;    // histogram bin number, equilibrium period
  std::string __lattice;         // Parameter defining the initial lattice

  double __dt;       // time step
  double __KE;       // Kinetic Energy
  double __T;        // Temperature
  double __L;        // Length of the box after scaling
  double __cut_off;  // simulation runs only within cutoff
  double __var;      // variance random positions generator
  double scale_v;    // velocity scaling

  /* Visualisation vectors, initialised in constructor */
  std::vector<std::vector<double>> *pos_x;
  std::vector<std::vector<double>> *pos_y;
  std::vector<std::vector<double>> *pos_z;

  /* Compression variables */
  bool __compress;
  size_t c_counter = 0;  // counts the number compression that have occurred

  /* Radial Distribution variables */
  int igr;                 // Index of Hist
  double __rg;             // cut off radius
  double __dr;             // bin increment in terms of radius units
  std::vector<double> gr;  // RDF vector container

 private:
  double PI;
  /* Variables for storing inside the object the file ID */
  stat_file logger;
  std::string __dir;
  std::string __simulation_name;

  /* Variables storing the simulation setup variables */
  double __rho;           // density
  double __T0;            // target/ Thermostat temperature
  double __power;         // pair potential intensity
  double __a_cst;         // generic softening parameter
  std::string __pp_type;  // pair potential type

  /* Visualisation flag */
  bool __visualise;

 public:
  bool fixed_seed;

  MD(size_t step_number, size_t particles_per_axis, std::string lattice);

  ~MD();

  void load_options(std::string out_directory, bool is_compressing,
                    bool track_particles, size_t rdf_bins,
                    size_t collect_rdf_after);

  void simulation(std::string simulation_name, double DENSITY,
                  double TEMPERATURE, double POWER, double A_CST,
                  std::string pp_type);

  void reset_values(bool force_reset = false);

 protected:
  double initialise(std::vector<double> &x, std::vector<double> &y,
                    std::vector<double> &z, std::vector<double> &vx,
                    std::vector<double> &vy, std::vector<double> &vz,
                    double TEMPERATURE);

  void mb_distribution(std::vector<double> &vx, std::vector<double> &vy,
                       std::vector<double> &vz, double TEMPERATURE);

  double verlet_algorithm(std::vector<double> &rx, std::vector<double> &ry,
                          std::vector<double> &rz, std::vector<double> &vx,
                          std::vector<double> &vy, std::vector<double> &vz,
                          bool sample_msd);

  void velocity_autocorrelation_function(std::vector<double> &Cvx,
                                         std::vector<double> &Cvy,
                                         std::vector<double> &Cvz);

  void radial_distribution_function(double &rho, double &cut_off, size_t &bins,
                                    size_t &particles);

  void mean_square_displacement(std::vector<double> &MSDx,
                                std::vector<double> &MSDy,
                                std::vector<double> &MSDz);

  void structure_factor(std::vector<double> &rx, std::vector<double> &ry,
                        std::vector<double> &rz);

  std::string set_simulation_params(double &rho, double &T, double &power,
                                    double &a, std::string &pp_type);
  /* Helper Functions */
 public:
  std::string get_dir();

  std::string get_simulation_name();

  std::string get_lattice_structure();

  bool get_compression_flag();

  void set_compression_flag(bool is_compressing);

  bool get_visualisation_flag();

  void set_visualisation_flag(bool is_visualising);

  size_t get_particle_number();

  size_t get_rdf_accuracy();

  void enable_testing(bool is_testing);

  size_t get_rdf_collect_after();

  void set_rdf_collect_after(size_t rdf_collect_after);

  void set_random_position_variance(double var);
};