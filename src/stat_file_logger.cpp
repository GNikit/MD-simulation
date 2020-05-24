#include "stat_file_logger.h"
#include "FileIO.h"

/* Passes the name of the variable to a stream */
#define GET_VAR_NAME(stream, variable) (stream) << #variable

stat_file::stat_file() {}

std::vector<std::ofstream> stat_file::open_files(
    std::vector<std::string> const &file_names) {
  std::vector<std::ofstream> file_streams;
  for (size_t file = 0; file < file_names.size(); ++file) {
    std::ofstream temp;
    temp.open(file_names[file], std::ios::out | std::ios::trunc);
    file_streams.push_back(std::move(temp));
  }

  return file_streams;
}

void stat_file::write_data_file(
    std::ofstream &file_stream, std::string const &header,
    std::vector<std::vector<double>> const &all_output_vectors) {
  /* Write the timestamp and header to the stream */
  /**
   * @brief
   * If this is to be reabled I will have to add padding of deliters equal
   *  to the number of columns-1, since numpy has a problem with jagged CSV rows
   */
  if (false)
    time_stamp(file_stream, header);
  else
    file_stream << header << std::endl;

  file_stream.precision(14);
  file_stream << std::scientific;
  /* Delimiter string */
  std::string del = ",";
  /* Check if the matrix is rectangular */
  size_t rows = all_output_vectors[0].size();
  for (size_t i = 0; i < all_output_vectors.size(); ++i) {
    if (all_output_vectors[i].size() > rows) {
      std::cerr << "Uneven size all_output_vectors with index: " << i
                << std::endl;
      exit(-1);
    }
  }

  /* This accesses the all_output_vectors with a column major ordering,
     not the best for performance */
  for (size_t i = 0; i < rows; ++i) {
    /* The main data file is always the first entry in the vector of streams */
    file_stream << (i + 1);
    for (size_t vec = 0; vec < all_output_vectors.size(); ++vec) {
      try {
        file_stream << del << all_output_vectors[vec][i];
      }
      /* if the array goes out of bounds then just add 0s */
      catch (const std::out_of_range &e) {
        file_stream << del << -666;
      }
    }
    file_stream << std::endl;
  }
}

void stat_file::time_stamp(std::ofstream &file_stream,
                           std::string const &variables) {
  std::chrono::time_point<std::chrono::system_clock> instance;
  instance = std::chrono::system_clock::now();
  std::time_t date_time = std::chrono::system_clock::to_time_t(instance);
  file_stream << "# Created on: " << std::ctime(&date_time);
  file_stream << variables << std::endl;
}

std::string stat_file::file_naming(std::string const &prefix,
                                   size_t const &STEPS, size_t const &N,
                                   double const &DENSITY,
                                   double const &TEMPERATURE,
                                   double const &POWER, double const &A_cst) {
  /* Individual streams handling double to string conversions */
  std::stringstream A_stream, rho_stream, T_stream;

  rho_stream << std::fixed << std::setprecision(4) << DENSITY;    // 4 decimals
  T_stream << std::fixed << std::setprecision(4) << TEMPERATURE;  // 4 decimals

  __step2str = "_step_" + std::to_string(STEPS);
  __particles2str = "_particles_" + std::to_string(N);
  __rho2str = "_rho_" + rho_stream.str();
  __T2str = "_T_" + T_stream.str();

  /* Do not add an A parameter or a potential strength in case they are NAN */
  if (isnan(POWER))
    __n2str = "";
  else
    __n2str = "_n_" + convert_to_string(POWER, 2);

  if (isnan(A_cst))
    __a2str = "";
  else {
    A_stream << std::fixed << std::setprecision(5) << A_cst; /* 5 decimals */
    __a2str = "_A_" + A_stream.str();
  }

  __file_id =
      __step2str + __particles2str + __rho2str + __T2str + __n2str + __a2str;

  /* Path addition */
  return prefix + __file_id + __file_ext;
}

std::string stat_file::convert_to_string(const double &x,
                                         const int &precision) {
  std::ostringstream ss;
  ss.str(std::string()); /* don't forget to empty the stream */
  ss << std::fixed << std::setprecision(precision) << x;

  return ss.str();
}

void stat_file::write_file(std::vector<std::vector<double>> &output_quantities,
                           std::ofstream &fstream, std::string const &header,
                           size_t format) {
  std::string del = ",";
  /* Write the 2D vector in a row major format, each vector is a row in file */
  FileIO::Write2File<double>(output_quantities, fstream, del, header, format);
}

void stat_file::write_file(
    std::vector<std::vector<double> *> &output_quantities,
    std::ofstream &fstream, std::string const &header, size_t format) {
  std::string del = ",";
  /* Write the 2D vector in a row major format, each vector is a row in file */
  FileIO::Write2File<double>(output_quantities, fstream, del, header, format);
}

void stat_file::write_file_line(std::vector<double> const &output_line,
                                std::ofstream &fstream, int index) {
  std::string del = ",";
  /* If the index is no-negative write it in file */
  if (index >= 0) fstream << index << del;

  fstream.precision(14);
  fstream << std::scientific;
  size_t i;
  for (i = 0; i < output_line.size() - 1; ++i) fstream << output_line[i] << del;
  /* Write the last entry without the delimiter just the EOL character */
  fstream << output_line[i] << std::endl;
}
