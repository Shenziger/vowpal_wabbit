/*
Copyright (c) by respective owners including Yahoo!, Microsoft, and
individual contributors. All rights reserved.  Released under a BSD (revised)
license as described in the file LICENSE.
*/

#include "ds_api.h"
#include "ds_internal.h"
#include "ds_vw.h"
#include "ds_predictor_container.h"

using namespace std;
#include "utility.h"

#include <memory>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>

namespace Microsoft {
  namespace DecisionService {

    using namespace std::chrono_literals;
    using namespace MultiWorldTesting;
 
    DecisionServiceClient::DecisionServiceClient(DecisionServiceConfiguration& config)
      : _state(new DecisionServiceClientInternal(config))
    { }

    DecisionServiceClient::~DecisionServiceClient()
    { }

    RankResponse* DecisionServiceClient::rank_struct(const char* features, const char* event_id, const Array<float>& scores)
    {
      return rank_cstyle(features, event_id, scores.data, scores.length);
    }

    RankResponse* DecisionServiceClient::rank_vector(const char* features, const char* event_id, const vector<float>& scores)
    {
      return rank_cstyle(features, event_id, &scores[0], scores.size());
    }

    // the assumption is that the ranking is independent of other options present (e.g. A,B,C and B,C)
    class DecisionServicePredictorsSimple : public DecisionServicePredictors {
        vector<float> _scores;
      public:
        DecisionServicePredictorsSimple(const float* scores, size_t n)
          : _scores(scores, scores+n)
        { }

        virtual void get_prediction(size_t index, const std::vector<int>& previous_decisions, DecisionServicePrediction* output_result) 
        {
          // TODO: not sure on how to behave we've an index here... let's be safe
          output_result->set(_scores);
        }
    };

    RankResponse* DecisionServiceClient::rank_cstyle(const char* features, const char* event_id, const float* scores, size_t scores_size)
    {
      DecisionServicePredictorsSimple scores_as_iterator(scores, scores_size);
      return explore_and_log(features, event_id, &scores_as_iterator);
    }

    RankResponse* DecisionServiceClient::explore_and_log(const char* features, const char* event_id, DecisionServicePredictors* predictors)
    {
      // generate event id if not provided      
      std::string l_event_id;
      if (!event_id || strlen(event_id) == 0)
        l_event_id = boost::uuids::to_string(boost::uuids::random_generator()());
      else
        l_event_id = event_id;

      PredictorContainer container(predictors);

      // generate probabilitiy distribution and ranking
      ActionProbabilities probability_distribution = _state->_config.explorer->explore(container);

      // make sure that random seed is unique per application even for the same event id
      // consider a pipeline with multiple randomization steps, each of them use the same event id for seeding
      // you'd end up with correlated seeds.
      uint64_t seed = _state->_seed_from_app_id + HashUtils::Compute_Id_Hash(l_event_id);
      PRG::prg random_generator(seed);
      float draw = random_generator.Uniform_Unit_Interval();

      // sample from distribution
      size_t choosen_action = probability_distribution.sample(draw);

      // swap top slot
      probability_distribution.swap_first(choosen_action);

      RankResponse* response = new RankResponse(
        probability_distribution.actions(),
        l_event_id.c_str(), 
        "m1", 
        probability_distribution.probabilities(),
        features);

      _state->enqueue_interaction(*response);

      return response;
    }

    void DecisionServiceClient::reward(const char* event_id, const char* reward)
    {
      _state->upload_reward(event_id, reward);
    }

    void DecisionServiceClient::update_model(unsigned char* model, size_t len)
    {
      update_model(model, 0, len);
    }

    void DecisionServiceClient::update_model(unsigned char* model, size_t offset, size_t len)
    {
      DS_LOG(_state->_config, DecisionServiceLogLevel::error, "update_model(len=" << len << ")")

      // TODO: swap out model
    }
  }
}