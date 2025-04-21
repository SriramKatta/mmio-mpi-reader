#ifndef PARSECLA_HPP
#define PARSECLA_HPP
#pragma once

#include <boost/program_options.hpp>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <string.h>

namespace po = boost::program_options;

void parseCLA(int &argc, char *argv[], std::string &fname) {

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
    fname = vm["fileName"].as<std::string>();
  }
  if (vm.count("help")) {
    std::cout << desc;
    exit(0);
  }
}

#endif