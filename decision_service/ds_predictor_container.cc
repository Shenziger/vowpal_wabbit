/*
Copyright (c) by respective owners including Yahoo!, Microsoft, and
individual contributors. All rights reserved.  Released under a BSD (revised)
license as described in the file LICENSE.
*/

#include "ds_predictor_container.h"

namespace Microsoft {
  namespace DecisionService {
    using namespace std;

        PredictorContainer::PredictorContainer(DecisionServicePredictors* predictors)
            : PredictorContainer(predictors, vector<int>())
        { }

        PredictorContainer::PredictorContainer(DecisionServicePredictors* predictors, const std::vector<int>& previous_decisions)
            : _predictors(predictors), _previous_decisions(previous_decisions)
        {
            _predictions = new DecisionServicePrediction[predictors->count()];
        }
        
        // move ctor
        PredictorContainer::PredictorContainer(PredictorContainer&& other)
            : _predictors(other._predictors), _predictions(other._predictions)
        {
            other._predictions = nullptr;
            other._predictors = nullptr;
        }

        PredictorContainer::~PredictorContainer()
        {
            if (_predictions)
            {
                delete[] _predictions;
                _predictions = nullptr;
            }

            _predictors = nullptr;
        }

        PredictorContainer::iterator PredictorContainer::begin()
        {
            return PredictorContainer::iterator(this, 0);
        }

        PredictorContainer::iterator PredictorContainer::end()
        {
            return PredictorContainer::iterator(this, _predictors->count());
        }

        size_t PredictorContainer::count() 
        {
            return _predictors->count();
        }

        DecisionServicePrediction* PredictorContainer::prediction(size_t index)
        {
            // TODO: throw exception?
            if (index < 0 || index >= _predictors->count())
                return nullptr;

            DecisionServicePrediction* prediction = _predictions + index;
            
            if (_predictions[index].scores().size() == 0)
                _predictors->get_prediction(index, _previous_decisions, prediction);

            return prediction;
        }

        PredictorContainer::iterator::iterator(PredictorContainer* container, int index)
            : _container(container), _index(index)
        { }

        DecisionServicePrediction& PredictorContainer::iterator::operator*()
        {
            return *_container->prediction(_index);
        }

        DecisionServicePrediction* PredictorContainer::iterator::operator->() const
        {
            return _container->prediction(_index);
        }

        // pre-increment
        PredictorContainer::iterator& PredictorContainer::iterator::operator++()
        {
            ++_index;
            return *this;
        }

        // post-increment
        PredictorContainer::iterator PredictorContainer::iterator::operator++(int)
        {
            iterator tmp = *this;
            ++_index;
            return tmp; 
        } 
  }
}