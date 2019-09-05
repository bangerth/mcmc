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


// Check the SpuriousAutocovariance consumer
// So far, we couldn't use Ranger producer because of insufficiency in spurious_autocovariance.h,
// where code expects input as vallaray, vector, array or etc. However, metropolis_hasting algorithm with some
// modifications in perturb function is totally fine to do this job.


#include <iostream>
#include <fstream>
#include <valarray>
#include <iomanip>

#include <sampleflow/producers/metropolis_hastings.h>
#include <boost/numeric/ublas/matrix.hpp>
#include <sampleflow/consumers/spurious_autocovariance.h>

using SampleType = std::valarray<double>;

double log_likelihood (const SampleType &x)
{
	return 1;
}

SampleType perturb (const SampleType &x)
{
	SampleType y = x;

	for (auto &el : y)
		el += 1;

	return y;
}

int main ()
{

	SampleFlow::Producers::MetropolisHastings<SampleType> mh_sampler;

	const unsigned int AC_length = 10;
	SampleFlow::Consumers::SpuriousAutocovariance<SampleType> autocovariance(AC_length);
	autocovariance.connect_to_producer (mh_sampler);

	mh_sampler.sample ({0,1},
			&log_likelihood,
			&perturb,
			20);

	//Due to division by not very nice numbers, we might expect answers with long decimal numbers,
	//in which we are not that much interested in. So set precision 3 would help us to avoid unnecessary problems.
	std::cout << std::fixed;
	std::cout << std::setprecision(3);

	// At this point, we have sampled two dimensional vectors - (1,2), (2,3), ..., (20,21).
	// After doing calculations in statistical software R, we expect result to be as it is at output file.
	// Whatever we got, we print it out


	std::cout << autocovariance.get()[0] << std::endl;
	std::cout << autocovariance.get()[1] << std::endl;
	std::cout << autocovariance.get()[2] << std::endl;
	std::cout << autocovariance.get()[3] << std::endl;
	std::cout << autocovariance.get()[4] << std::endl;
	std::cout << autocovariance.get()[5] << std::endl;
	std::cout << autocovariance.get()[6] << std::endl;
	std::cout << autocovariance.get()[7] << std::endl;
	std::cout << autocovariance.get()[8] << std::endl;
	std::cout << autocovariance.get()[9] << std::endl;

}
