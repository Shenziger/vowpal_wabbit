#pragma once

#ifndef SWIG
#include "ds_predictor_container.h"
#endif

#include <vector>
#include <stdint.h>
#include <stdexcept>

//#include <cstdint>

#ifndef DISABLE_NAMESPACE
namespace Microsoft {
  namespace DecisionService {
#endif      
      #ifndef SWIG
      struct ActionProbability
      {
          int action;

          float probability;
      };

      class ActionProbabilities : public std::vector<ActionProbability>
      {
      public:
        ActionProbabilities(size_t count);

        ActionProbabilities(size_t count, float initial_probability);

        void safety(float min_prob, bool zeros);

        size_t sample(float draw);

        void swap_first(size_t index);

        void sort_by_probabilities_desc();

        // probabilities ordered by action
        vector<float> probabilities();

        // actions as currently ordered
        vector<int> actions();
      };

      #endif

      class IExplorer
      {
          public:
            // IExplorer();
            virtual ~IExplorer();

#ifndef SWIG
            virtual ActionProbabilities explore(PredictorContainer& container);
#endif
      };

      class Sampling
      {
          uint64_t _seed_from_app_id;

        public:
          Sampling();
          Sampling(const char* app_id);

#ifndef SWIG
          uint32_t choose_action(const char* event_id, float* probability_distribution, uint32_t num_actions);

          void rank(const char* event_id, float* probability_distribution, uint32_t* action_ordering, uint32_t num_actions);

          void rank_by_score(const char* event_id, float* probability_distribution, uint32_t* action_ordering, float* scores, uint32_t num_actions);
#endif
          // python signature
          int choose_action(const char* event_id, std::vector<float> probability_distribution);

          std::vector<int> rank(const char* event_id, std::vector<float> probability_distribution, std::vector<int> action_ordering);

          std::vector<int> rank_by_score(const char* event_id, std::vector<float> probability_distribution, std::vector<float> scores);
      };

      class EpsilonGreedyExplorer : public IExplorer
      {
          float _epsilon;

          public:
            EpsilonGreedyExplorer(float epsilon);

#ifndef SWIG
            virtual ActionProbabilities explore(PredictorContainer& container);
#endif
      };

      class SoftmaxExplorer : public IExplorer
      {
          float _lambda;
          float _min_epsilon;

          public:
            SoftmaxExplorer(float lambda, float min_epsilon = 0);

#ifndef SWIG
            virtual ActionProbabilities explore(PredictorContainer& container);
#endif
      };

      class BagExplorer : public IExplorer
      {
          float _min_epsilon;

          public:
            BagExplorer(float min_epsilon = 0);

#ifndef SWIG
            virtual ActionProbabilities explore(PredictorContainer& container);
#endif
      };

/*
      class CoverExplorer : public IExplorer
      {
          public:
            virtual const std::vector<ActionProbability> explore(PredictorContainer& container);
      };
      */
#ifndef DISABLE_NAMESPACE
  }
}
#endif
