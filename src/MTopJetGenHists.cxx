#include "UHH2/MTopJet/include/MTopJetGenHists.h"
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

MTopJetGenHists::MTopJetGenHists(uhh2::Context & ctx, const std::string & dirname): Hists(ctx, dirname){
  // book all histograms here
  // RecoPT_old = book<TH1F>("p_{T} old ak8 Jets", "p_{T} [GeV/c]", 20, 0, 1000);
  // RecoNumber_old = book<TH1F>("Number old ak8 Jets ", "number", 15, 0, 15);
  // RecoEta_old = book<TH1F>("#eta old ak8 Jets", "#eta", 20, -3, 3);
  // RecoJet1Mass_old = book<TH1F>("Jet Mass old ak8 Jet", "M_{jet}", 30, 0, 600);

  // RecoEvNumber = book<TH1F>("Number Reco Parts per Event", "number", 30, 0, 300);

  // RecoPT_ak06 = book<TH1F>("p_{T} AK8 Jets", "p_{T} [GeV/c]", 20, 0, 1000);
  // RecoNumber_ak06 = book<TH1F>("Number AK8 Jets", "number", 15, 0, 15);
  // RecoEta_ak06 = book<TH1F>("#eta AK8 Jets", "#eta", 20, -3, 3);
  // RecoJet1Mass_ak06 = book<TH1F>("Jet Mass AK8", "M_{jet}", 30, 0, 600);

  // GenEvNumber = book<TH1F>("Number Gen Parts per Event", "number", 50, 0, 1000);
  // GenEvNumberStable = book<TH1F>("Number stable Gen Parts per Event", "number", 50, 0, 1000);

  // GenPT_ak06 = book<TH1F>("p_{T} AK8 GenJets", "p_{T} [GeV/c]", 20, 0, 1000);
  // GenNumber_ak06 = book<TH1F>("Number AK8 GenJets", "number", 50, 0, 50);
  // GenEta_ak06 = book<TH1F>("#eta AK8 GenJets", "#eta", 20, -3, 3);

  GenJet1Mass = book<TH1F>("GenJet Mass", "M_{jet}", 30, 0, 600);
  GenJet1Mass_unmatched = book<TH1F>("GenJet Mass unmatched", "M_{jet}", 30, 0, 600);
  GenJet1Mass_matched = book<TH1F>("GenJet Mass matched", "M_{jet}", 30, 0, 600);

  // GenJet1Mass_ak06 = book<TH1F>("GenJet Mass AK6", "M_{jet}", 30, 0, 600);
  // GenJet1Mass_ak08 = book<TH1F>("GenJet Mass AK8", "M_{jet}", 30, 0, 600);
  // GenJet1Mass_ak10 = book<TH1F>("GenJet Mass AK10", "M_{jet}", 30, 0, 600);

  // GenJet1Mass_ak06_unmatched = book<TH1F>("GenJet Mass AK6 unmatched", "M_{jet}", 30, 0, 600);
  // GenJet1Mass_ak08_unmatched = book<TH1F>("GenJet Mass AK8 unmatched", "M_{jet}", 30, 0, 600);
  // GenJet1Mass_ak10_unmatched = book<TH1F>("GenJet Mass AK10 unmatched", "M_{jet}", 30, 0, 600);

  // GenJet1Mass_ak06_matched = book<TH1F>("GenJet Mass AK6 matched", "M_{jet}", 30, 0, 600);
  // GenJet1Mass_ak08_matched = book<TH1F>("GenJet Mass AK8 matched", "M_{jet}", 30, 0, 600);
  // GenJet1Mass_ak10_matched = book<TH1F>("GenJet Mass AK10 matched", "M_{jet}", 30, 0, 600);

  TopHadPT = book<TH1F>("p_{T} of hadronic Top", "p_{T}", 20, 0, 1000);
  TopLepPT = book<TH1F>("p_{T} of leptonic Top", "p_{T}", 20, 0, 1000);

  deltaR_lep1_jet1 = book<TH1F>("deltaR_lep1_jet1", "#Delta R(lep1,1st Jet)", 80, 0, 4.0);
  deltaR_lep2_jet1 = book<TH1F>("deltaR_lep2_jet1", "#Delta R(lep2,1st Jet)", 80, 0, 4.0);
  deltaR_bot_lep_jet1 = book<TH1F>("deltaR_bot_lep_jet1", "#Delta R(bot_lep,1st Jet)", 80, 0, 4.0);
  deltaR_bot_jet1 = book<TH1F>("deltaR_bot_jet1", "#Delta R(bot,1st Jet)", 80, 0, 4.0);
  deltaR_q1_jet1 = book<TH1F>("deltaR_q1_jet1", "#Delta R(q1,1st Jet)", 80, 0, 4.0);
  deltaR_q2_jet1 = book<TH1F>("deltaR_q2_jet1", "#Delta R(q2,1st Jet)", 80, 0, 4.0);
  deltaR_tophad_jet1 = book<TH1F>("deltaR_tophad_jet1", "#Delta R(had Top, 1st Jet)", 80, 0, 4.0);
  deltaR_toplep_jet1 = book<TH1F>("deltaR_toplep_jet1", "#Delta R(lep Top, 1st Jet)", 80, 0, 4.0);

  // handle for PF particles
  h_pfpart=ctx.get_handle<vector<PFParticle>>("PFParticles");
  // handle for TTbarGen class
  h_ttbargen=ctx.get_handle<TTbarGen>("ttbargen");

}



void MTopJetGenHists::fill(const Event & event){
  //---------------------------------------------------------------------------------------
  //--------------------------------- Cluster Gen Jets ------------------------------------
  //---------------------------------------------------------------------------------------
  float pt_min = 150; // only generate Jets with pT > pt_min
  std::vector<GenParticle>* genparts = event.genparticles;
  JetCluster* jetc=new JetCluster();
  std::vector<fastjet::PseudoJet> gen, gen_ak06, gen_ak08, gen_ak10;
  float jetradius = 0.9;
  gen = jetc->get_genjets(genparts, JetCluster::e_ca, jetradius, pt_min);
  // gen_ak06 = jetc->get_genjets(genparts, JetCluster::e_akt, 0.6, pt_min);
  // gen_ak08 = jetc->get_genjets(genparts, JetCluster::e_akt, 0.8, pt_min);
  // gen_ak10 = jetc->get_genjets(genparts, JetCluster::e_akt, 1.0, pt_min);
  //---------------------------------------------------------------------------------------
  //--------------------------------------------------------------------------------------- 


  //---------------------------------------------------------------------------------------
  //--------------------------------- Cluster Reco Jets -----------------------------------
  //---------------------------------------------------------------------------------------
  // std::vector<PFParticle> pfparts = event.get(h_pfpart);
  // JetCluster* jetc_reco=new JetCluster();
  // std::vector<fastjet::PseudoJet> reco_ak06; 
  // reco_ak06 = jetc_reco->get_recojets(&pfparts, JetCluster::e_akt, 0.8, 200);
  //---------------------------------------------------------------------------------------
  //--------------------------------------------------------------------------------------- 
 

  //---------------------------------------------------------------------------------------
  //--------------------------------- compute TTbarGen ------------------------------------
  //---------------------------------------------------------------------------------------
  const auto & ttbargen = event.get(h_ttbargen);
  //---------------------------------------------------------------------------------------
  //---------------------------------------------------------------------------------------


  //---------------------------------------------------------------------------------------
  //--------------------------------- Matching --------------------------------------------
  //---------------------------------------------------------------------------------------
  bool matched = false;
  // get stable particles from ttbar decay and sort them into leptonic and hadronic
  GenParticle bot, q1, q2, bot_lep, lep1, lep2;
  if(gen.size() > 0){
    fastjet::PseudoJet genjet = gen[0];
    //TLorentzVector jet;
    //jet.SetPtEtaPhiE(genjet.pt(),genjet.eta(),genjet.phi(),genjet.E());
    if(ttbargen.IsTopHadronicDecay()){
      bot = ttbargen.bTop();
      q1 = ttbargen.Wdecay1();
      q2 = ttbargen.Wdecay2();
      bot_lep = ttbargen.bAntitop();
      lep1 = ttbargen.WMinusdecay1();
      lep2 = ttbargen.WMinusdecay2();
    }
    else if(ttbargen.IsAntiTopHadronicDecay()){
      bot = ttbargen.bAntitop();
      q1 = ttbargen.WMinusdecay1();
      q2 = ttbargen.WMinusdecay2();
      bot_lep = ttbargen.bTop();
      lep1 = ttbargen.Wdecay1();
      lep2 = ttbargen.Wdecay2();
    }
    else if(!(ttbargen.IsTopHadronicDecay() || ttbargen.IsAntiTopHadronicDecay())){
	throw runtime_error("no hadronic Top found");
    }
    //check if particles from hadronic top are clustered into jet
    if((deltaR(bot, genjet)<=jetradius) && (deltaR(q1, genjet)<=jetradius) && (deltaR(q2, genjet)<=jetradius)) matched = true;
  }
  //---------------------------------------------------------------------------------------
  //---------------------------------------------------------------------------------------



  //---------------------------------------------------------------------------------------
  //--------------------------------- Fill Hists here -------------------------------------
  //---------------------------------------------------------------------------------------
  // get weight
  double weight = event.weight;
  ////

  
  //Mass Gen Jet
  if((gen.size())==2){
    fastjet::PseudoJet genjet = gen[0];
    TLorentzVector jet;
    jet.SetPtEtaPhiE(genjet.pt(),genjet.eta(),genjet.phi(),genjet.E());
    if(genjet.pt()>400){
      GenJet1Mass->Fill(jet.M(),weight);
      if(matched==false)GenJet1Mass_unmatched->Fill(jet.M(),weight);
      if(matched==true)GenJet1Mass_matched->Fill(jet.M(),weight);
    }
  }
  // if((gen_ak06.size())==2){
  //   fastjet::PseudoJet genjet06 = gen_ak06[0];
  //   TLorentzVector jet06;
  //   jet06.SetPtEtaPhiE(genjet06.pt(),genjet06.eta(),genjet06.phi(),genjet06.E());
  //   if(genjet06.pt()>500){
  //     GenJet1Mass_ak06->Fill(jet06.M(),weight);
  //     if(matched==false)GenJet1Mass_ak06_unmatched->Fill(jet06.M(),weight);
  //     if(matched==true)GenJet1Mass_ak06_matched->Fill(jet06.M(),weight);
  //   }
  // } 
  // if((gen_ak08.size())==2){
  //   fastjet::PseudoJet genjet08 = gen_ak08[0];
  //   TLorentzVector jet08;
  //   jet08.SetPtEtaPhiE(genjet08.pt(),genjet08.eta(),genjet08.phi(),genjet08.E());
  //   if(genjet08.pt()>500){
  //     GenJet1Mass_ak08->Fill(jet08.M(),weight);
  //     if(matched==false)GenJet1Mass_ak08_unmatched->Fill(jet08.M(),weight);
  //     if(matched==true)GenJet1Mass_ak08_matched->Fill(jet08.M(),weight);
  //   }
  // } 
  // if((gen_ak10.size())==2){
  //   fastjet::PseudoJet genjet10 = gen_ak10[0];
  //   TLorentzVector jet10;
  //   jet10.SetPtEtaPhiE(genjet10.pt(),genjet10.eta(),genjet10.phi(),genjet10.E());
  //   if(genjet10.pt()>500){
  //     GenJet1Mass_ak10->Fill(jet10.M(),weight);
  //     if(matched==false)GenJet1Mass_ak10_unmatched->Fill(jet10.M(),weight); 
  //     if(matched==true)GenJet1Mass_ak10_matched->Fill(jet10.M(),weight);
  //   }
  // } 
 
  // pT of had. top
  GenParticle tophad = ttbargen.TopHad();
  float tophadpt = tophad.pt();
  TopHadPT->Fill(tophadpt, weight);

  // pT of lep. top
  GenParticle toplep = ttbargen.TopLep();
  float topleppt = toplep.pt();
  TopLepPT->Fill(topleppt, weight);

  if(gen.size() > 0){
    fastjet::PseudoJet genjet = gen[0];
    deltaR_lep1_jet1->Fill(deltaR(genjet, lep1), weight);
    deltaR_lep2_jet1->Fill(deltaR(genjet, lep2), weight);
    deltaR_bot_lep_jet1->Fill(deltaR(genjet, bot_lep), weight);
    deltaR_q1_jet1->Fill(deltaR(genjet, q1), weight);
    deltaR_q2_jet1->Fill(deltaR(genjet, q2), weight);
    deltaR_bot_jet1->Fill(deltaR(genjet, bot), weight);
    deltaR_tophad_jet1->Fill(deltaR(genjet, tophad), weight);
    deltaR_toplep_jet1->Fill(deltaR(genjet, toplep), weight);
  }

  ////

}


