#ifndef PARSECLA_HPP
#define PARSECLA_HPP
#pragma once

#include <boost/program_options.hpp>
#include <fmt/format.h>
#include <iostream>
#include <string.h>
#include <fstream>

namespace po = boost::program_options;

void parseCLA(int &argc, const char *argv[], std::ifstream &fin) {

  po::options_description desc("Allowed Options", 100);
  // clang-format off
    desc.add_options()
    ("help,h", "produce help message")
    ("fileName,F", po::value<std::string>(), "matrix file name")
    ;
  // clang-format on
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("fileName")) {
    std::string fname = vm["fileName"].as<std::string>();
    fin.open(fname);
    if (!fin.is_open()) {
      fmt::print("failed to open the file : {}\n", fname);
      exit(0);
    }
  }
  if (vm.count("help")) {
    std::cout << desc;
    exit(0);
  }
}

#endif