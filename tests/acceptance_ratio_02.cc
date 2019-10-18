// ---------------------------------------------------------------------
//
// Copyright (C) 2019 by the SampleFlow authors.
//
// This file is part of the SampleFlow library.
//
// The deal.II library is free software; you can use it, redistribute
// it, and/or modify it under the terms of the GNU Lesser General
// Public License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// The full text of the license can be found in the file LICENSE.md at
// the top level directory of deal.II.
//
// ---------------------------------------------------------------------


// Check the AcceptanceRatio consumer


#include <iostream>
#include <fstream>
#include <valarray>

#include <sampleflow/producers/metropolis_hastings.h>
#include <boost/numeric/ublas/matrix.hpp>
#include <sampleflow/consumers/acceptance_ratio.h>

using SampleType = std::valarray<double>;

double log_likelihood (const SampleType &x)
{
  return 1;
}

SampleType perturb (const SampleType &x)
{
  SampleType y = x;

  for (auto &el : y)
    if (el!=1) el += 1;

  return y;
}

int main ()
{

  SampleFlow::Producers::MetropolisHastings<SampleType> mh_sampler;

  SampleFlow::Consumers::AcceptanceRatio<SampleType> acceptance_ratio;
  acceptance_ratio.connect_to_producer(mh_sampler);;

  mh_sampler.sample ({1},
                     &log_likelihood,
                     &perturb,
                     1000);

  // At this point, we have sampled 1,1,1,1,1,1,1,1,1,1,1. All samples 1000 are equal, so ratio should be
  //equal to 0.001.
  //Output whatever we got:
  std::cout << acceptance_ratio.get() << std::endl;
}
