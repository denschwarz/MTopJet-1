#pragma once
#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/TTbarGen.h"
#include "UHH2/common/include/Utils.h"
#include "UHH2/core/include/Jet.h"
#include "UHH2/core/include/TopJet.h"
#include "UHH2/core/include/GenJetWithParts.h"
#include "UHH2/core/include/GenParticle.h"
#include "UHH2/core/include/AnalysisModule.h"

#include <vector>
#include <iostream> 
#include <math.h>

using namespace std;

class WriteOutput: public uhh2::AnalysisModule{
public:

  explicit WriteOutput(uhh2::Context&);
  virtual bool process(uhh2::Event & ) override; 
    
private:
  uhh2::Event::Handle<double>h_weight;
  uhh2::Event::Handle<double>h_top1_mass;
  uhh2::Event::Handle<int>h_top_number;

};

