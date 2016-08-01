#include "UHH2/MTopJet/include/MTopJetRecoGenHists.h"
#include "UHH2/MTopJet/include/JetCluster.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/Utils.h"
#include "UHH2/core/include/PFParticle.h"

#include "UHH2/common/include/TTbarGen.h"

#include <math.h>
#include <vector>

#include "TH1F.h"
#include "TH2F.h"
#include <iostream>

using namespace uhh2;

MTopJetRecoGenHists::MTopJetRecoGenHists(uhh2::Context & ctx, const std::string & dirname, const std::string & rec_label, const std::string & gen_label): Hists(ctx, dirname){
  // book all histograms here
  MassReso = book<TH1F>("MassResolution", "(M^{rec}_{jet1} - M^{gen}_{jet1}) / M^{gen}_{jet1}) ", 90, -1.5, 1.5);
  PtReso = book<TH1F>("PtResolution", "(p^{rec}_{T, jet1} - p^{gen}_{T, jet1}) / p^{gen}_{T, jet1}) ", 90, -1.5, 1.5);
  DeltaR_Rec1_Gen1 = book<TH1F>("DeltaR_Rec1-Gen1", "#Delta R (rec1, gen1) ", 40, 0, 4);
  DeltaR_Rec2_Gen2 = book<TH1F>("DeltaR_Rec2-Gen2", "#Delta R (rec2, gen2) ", 40, 0, 4);


  // handle for clustered jets
  h_recjets=ctx.get_handle<std::vector<Jet>>(rec_label);
  h_genjets=ctx.get_handle<std::vector<Jet>>(gen_label);

}



void MTopJetRecoGenHists::fill(const Event & event){


  //---------------------------------------------------------------------------------------
  //--------------------------------- define needed objects-----------------------------------
  //---------------------------------------------------------------------------------------
 // define all objects needed
  std::vector<Jet> rec_jets = event.get(h_recjets);
  std::vector<Jet> gen_jets = event.get(h_genjets);
  TLorentzVector rec_jet1_v4, rec_jet2_v4, gen_jet1_v4, gen_jet2_v4;
  Jet rec_jet1, gen_jet1, rec_jet2, gen_jet2;
  if(rec_jets.size()>0){
    rec_jet1 = rec_jets.at(0);
    rec_jet1_v4.SetPxPyPzE(rec_jet1.v4().Px(), rec_jet1.v4().Py(), rec_jet1.v4().Pz(), rec_jet1.v4().E());
  }
  if(gen_jets.size()>0){
    gen_jet1 = gen_jets.at(0);
    gen_jet1_v4.SetPxPyPzE(gen_jet1.v4().Px(), gen_jet1.v4().Py(), gen_jet1.v4().Pz(), gen_jet1.v4().E());
  }

  if(rec_jets.size()>1){
    rec_jet2 = rec_jets.at(1);
    rec_jet2_v4.SetPxPyPzE(rec_jet2.v4().Px(), rec_jet2.v4().Py(), rec_jet2.v4().Pz(), rec_jet2.v4().E()); 
  }
  if(gen_jets.size()>1){
    gen_jet2 = gen_jets.at(1);
    gen_jet2_v4.SetPxPyPzE(gen_jet2.v4().Px(), gen_jet2.v4().Py(), gen_jet2.v4().Pz(), gen_jet2.v4().E()); 
  }
  //---------------------------------------------------------------------------------------
  //---------------------------------------------------------------------------------------





  //---------------------------------------------------------------------------------------
  //--------------------------------- Fill Hists here -------------------------------------
  //---------------------------------------------------------------------------------------

  // get weight
  double weight = event.weight;


  if(gen_jets.size() > 0 && rec_jets.size() > 0){
    DeltaR_Rec1_Gen1->Fill(deltaR(rec_jet1, gen_jet1), weight);
  }
  if(gen_jets.size() > 1 && rec_jets.size() > 1){
    DeltaR_Rec2_Gen2->Fill(deltaR(rec_jet2, gen_jet2), weight);
  }
  if(gen_jets.size() > 0 && rec_jets.size() > 0){
    if(deltaR(rec_jet1, gen_jet1) < 1){
      MassReso->Fill( (rec_jet1_v4.M() - gen_jet1_v4.M())/gen_jet1_v4.M(), weight );
      PtReso->Fill( (rec_jet1_v4.Pt() - gen_jet1_v4.Pt())/gen_jet1_v4.Pt(), weight );
    }
  }
  //---------------------------------------------------------------------------------------
  //---------------------------------------------------------------------------------------
 

}


