/*
Copyright (c) by respective owners including Yahoo!, Microsoft, and
individual contributors. All rights reserved.  Released under a BSD (revised)
license as described in the file LICENSE.
*/

#pragma once

#include <vector>
#include <cstddef>

#ifndef DISABLE_NAMESPACE
namespace Microsoft {
  namespace DecisionService {
#endif

    class DecisionServicePrediction {

        std::vector<float> _scores;

      public:
        void set(const std::vector<float>& scores);

        size_t top_action() const;

        float max_score() const;

        size_t num_actions() const;

        std::vector<float>& scores();

        // TODO: for C#
        // void set(const Array<float>& default_ranking);
        // void set(const float* default_ranking, size_t default_ranking_size);
    };
  
    // doesn't work with feature modifying reductions
    // this bears some threading/timing issues:
    // the closure will have to get a "version" lock 
    // to make sure the same version is used through out the calls.
    class DecisionServicePredictors {
      size_t _count;
      
    public:
    // TODO: change swig.i files (from Iterator)
      DecisionServicePredictors();

      DecisionServicePredictors(size_t count);
      
      virtual ~DecisionServicePredictors();

      virtual void get_prediction(size_t index, const std::vector<int>& previous_decisions, DecisionServicePrediction* output_result);

      size_t count() const;
    };

#ifndef DISABLE_NAMESPACE
  }
}
#endif