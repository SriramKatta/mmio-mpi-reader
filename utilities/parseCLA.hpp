#ifndef PARSECLA_HPP
#define PARSECLA_HPP
#pragma once

#include <boost/program_options.hpp>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <string>

namespace po = boost::program_options;

void parseCLA(int &argc, char *argv[], std::string &fname) {
  po::options_description desc("Allowed Options", 100);
  // clang-format off
  desc.add_options()
    ("help,h", "produce help message")
    ("file,F", po::value<std::string>(), "matrix file name (positional)");
  // clang-format on

  po::positional_options_description p;
  p.add("file", 1); // map the first positional argument to "file"

  po::variables_map vm;
  po::store(
      po::command_line_parser(argc, argv).options(desc).positional(p).run(),
      vm);
  po::notify(vm);

  if (vm.count("file")) {
    fname = vm["file"].as<std::string>();
  } else {
    fmt::print("Error: No matrix file provided.\n");
    std::cout << desc << "\n";
    exit(1);
  }

  if (vm.count("help")) {
    std::cout << desc << "\n";
    exit(0);
  }
}

#endif
