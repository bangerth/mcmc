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

#ifndef SAMPLEFLOW_CONSUMERS_ACCEPTANCE_RATIO_H
#define SAMPLEFLOW_CONSUMERS_ACCEPTANCE_RATIO_H

#include <sampleflow/consumer.h>
#include <sampleflow/types.h>
#include <mutex>

namespace SampleFlow
{
  namespace Consumers
  {
    /**
     * A Consumer class that implements computing the acceptance ratio
     * over all samples seen so far. The last value so computed can be
     * obtained by calling the get() function.
     *
     * This code works if our sample have the form of vector.
     *
     * The concept of acceptance/rejection of a sample is explained in the documentation of
     * the Producers::MetropolisHastings class. This calculation uses the
     * assumption that every *accepted* sample in the sampling algorithm is *different* from last one.
     *
     * With every new sample, this algorithm checks if it differs from the last one. If it differs, it adds
     * one to the counter that keeps track of the number of accepted samples. The get() function
     * then returns the number of accepted samples divided by the overall number of samples.
     *
     * ### Threading model ###
     *
     * The implementation of this class is thread-safe, i.e., its
     * consume() member function can be called concurrently and from multiple
     * threads.
     *
     * @tparam InputType The C++ type used for the samples $x_k$. So far, implementation let us work
     */
    template <typename InputType>
    class AcceptanceRatio : public Consumer<InputType>
    {
      public:
        /**
         * The type of the information generated by this class, i.e., in which
         * the mean value is computed. This is of course the InputType.
         */
        using value_type = InputType;

        /**
         * Constructor.
         */
        AcceptanceRatio();

        /**
         * Process one sample by updating the previously computed mean value
         * using this one sample.
         *
         * @param[in] sample The sample to process.
         * @param[in] aux_data Auxiliary data about this sample. The current
         *   class does not know what to do with any such data and consequently
         *   simply ignores it.
         */
        virtual
        void
        consume (InputType     sample,
                 AuxiliaryData aux_data) override;

        /**
         * A function that returns the ratio computed from the samples
         * seen so far. If no samples have been processed so far, then a
         * default-constructed object of type InputType will be returned.
         *
         * @return Acceptance ration
         */
        double get () const;

      private:
        /**
         * A mutex used to lock access to all member variables when running
         * on multiple threads.
         */
        mutable std::mutex mutex;

        /**
         * The current value of accepted values as described in the introduction
         * of this class.
         */
        types::sample_index accept;

        /**
         * The number of samples processed so far.
         */
        types::sample_index n_samples;

        /**
         * Previous sample
         */
        InputType past_sample;

    };


/**
 * Constructor
 */
    template <typename InputType>
    AcceptanceRatio<InputType>::
	AcceptanceRatio ()
      :
      n_samples (0),
	  accept (0)
    {}


    template <typename InputType>
    void
	AcceptanceRatio<InputType>::
    consume (InputType sample, AuxiliaryData /*aux_data*/)
    {
      std::lock_guard<std::mutex> lock(mutex);

      // If this is the first sample we see, naturally, this sample is accepted.
      if (n_samples == 0) {
          n_samples=1;
          accept=1;
          past_sample = sample;
        }
      //Check if new sample is not equal to previous sample and if that is true add one to accept.
      else
      	  {
          ++n_samples;
          bool truth=false;
          for(int i = 0; i < sample.size(); i++) {
              if(past_sample[i]!=sample[i]){
            	  truth=true;
            	  break;
              	  }
          	  }
          if(truth) accept++;
          past_sample = sample;
        }
    }

    template <typename InputType>
    double
    AcceptanceRatio<InputType>::
    get () const
    {
      std::lock_guard<std::mutex> lock(mutex);
      double ratio = static_cast<double>(accept) / (double)(n_samples);
      return (ratio);
    }
  }
}

#endif
