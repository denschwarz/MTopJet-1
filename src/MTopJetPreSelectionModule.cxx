#include <iostream>
#include <memory>

#include <UHH2/core/include/AnalysisModule.h>
#include <UHH2/core/include/Event.h>
#include <UHH2/core/include/Selection.h>

#include <UHH2/common/include/CleaningModules.h>
#include <UHH2/common/include/NSelections.h>
#include <UHH2/common/include/LumiSelection.h>
#include <UHH2/common/include/TriggerSelection.h>
#include <UHH2/common/include/JetCorrections.h>
#include <UHH2/common/include/ObjectIdUtils.h>
#include <UHH2/common/include/MuonIds.h>
#include <UHH2/common/include/ElectronIds.h>
#include <UHH2/common/include/JetIds.h>
#include <UHH2/common/include/TopJetIds.h>
#include <UHH2/common/include/TTbarGen.h>
#include <UHH2/common/include/Utils.h>
#include <UHH2/common/include/AdditionalSelections.h>

#include <UHH2/common/include/ElectronHists.h>
#include <UHH2/common/include/EventHists.h>
#include <UHH2/common/include/MuonHists.h>
#include <UHH2/common/include/JetHists.h>
#include <UHH2/common/include/TTbarGenHists.h>

#include <UHH2/MTopJet/include/ModuleBASE.h>
#include <UHH2/MTopJet/include/RecoSelections.h>
#include <UHH2/MTopJet/include/MTopJetUtils.h>

class MTopJetPreSelectionModule : public ModuleBASE {

 public:
  explicit MTopJetPreSelectionModule(uhh2::Context&);
  virtual bool process(uhh2::Event&) override;

 protected:

  // cleaners
  std::unique_ptr<MuonCleaner>     muoSR_cleaner;
  std::unique_ptr<ElectronCleaner> eleSR_cleaner;

  std::unique_ptr<JetCleaner>                      jet_IDcleaner;
  std::unique_ptr<JetCorrector>                    jet_corrector;
  std::unique_ptr<GenericJetResolutionSmearer>     jetER_smearer;
  std::unique_ptr<JetLeptonCleaner_by_KEYmatching> jetlepton_cleaner;
  std::unique_ptr<JetCleaner>                      jet_cleaner1;
  std::unique_ptr<JetCleaner>                      jet_cleaner2;

  std::unique_ptr<JetCleaner>                  topjet_IDcleaner;
  std::unique_ptr<TopJetCorrector>             topjet_corrector;
  std::unique_ptr<SubJetCorrector>             topjet_subjet_corrector;
  std::unique_ptr<GenericJetResolutionSmearer> topjetER_smearer;
  // std::unique_ptr<TopJetLeptonDeltaRCleaner>   topjetlepton_cleaner;
  std::unique_ptr<TopJetCleaner>               topjet_cleaner;

  // selections
  std::unique_ptr<uhh2::Selection> lumi_sel;
  std::unique_ptr<uhh2::AndSelection> metfilters_sel;

  std::unique_ptr<uhh2::Selection> genmttbar_sel;
  std::unique_ptr<uhh2::Selection> genflavor_sel;

  std::unique_ptr<uhh2::Selection> jet2_sel;
  std::unique_ptr<uhh2::Selection> jet1_sel;
  std::unique_ptr<uhh2::Selection> met_sel;
  std::unique_ptr<uhh2::Selection> htlep_sel;
  std::unique_ptr<uhh2::Selection> twodcut_sel;

  std::unique_ptr<uhh2::AnalysisModule> ttgenprod;


  // store Hist collection as member variables
  // std::unique_ptr<Hists> h_jetsel_event, h_jetsel_elec, h_jetsel_muon, h_jetsel_jets, h_metsel_event, h_metsel_elec, h_metsel_muon, h_metsel_jets, h_htsel_event, h_htsel_elec, h_htsel_muon, h_htsel_jets, h_twodsel_event, h_twodsel_elec, h_twodsel_muon, h_twodsel_jets;

  // Event::Handle<float> tt_TMVA_response;// response of TMVA method, dummy value at this step

};

MTopJetPreSelectionModule::MTopJetPreSelectionModule(uhh2::Context& ctx){

  //// CONFIGURATION
  const bool isMC = (ctx.get("dataset_type") == "MC");

  ElectronId eleID;
  float jet1_pt(-1.), jet2_pt(-1.), MET(-1.), HT_lep(-1.);
  bool use_miniiso(false);

  //    eleID  = ElectronID_Spring15_25ns_tight_noIso;
  eleID = ElectronID_MVAnotrig_Spring15_25ns_loose; //TEST 
  use_miniiso = false;

  jet1_pt =  50.;
  jet2_pt =  50.;

  MET     =  20.;
  HT_lep  =  50.;
 
  ////

  //// COMMON MODULES

  if(!isMC) lumi_sel.reset(new LumiSelection(ctx));

  /* MET filters */
  metfilters_sel.reset(new uhh2::AndSelection(ctx, "metfilters"));
  metfilters_sel->add<TriggerSelection>("1-good-vtx", "Flag_goodVertices");
  metfilters_sel->add<TriggerSelection>("HBHENoiseFilter", "Flag_HBHENoiseFilter");
  metfilters_sel->add<TriggerSelection>("HBHENoiseIsoFilter", "Flag_HBHENoiseIsoFilter");
  metfilters_sel->add<TriggerSelection>("CSCTightHalo2015Filter", "Flag_CSCTightHalo2015Filter");
  metfilters_sel->add<TriggerSelection>("EcalDeadCellTriggerPrimitiveFilter", "Flag_EcalDeadCellTriggerPrimitiveFilter");
  metfilters_sel->add<TriggerSelection>("eeBadScFilter", "Flag_eeBadScFilter");
  //  metfilters_sel->add<TriggerSelection>("chargedHadronTrackResolutionFilter", "Flag_chargedHadronTrackResolutionFilter"); 
  // metfilters_sel->add<TriggerSelection>("muonBadTrackFilter", "Flag_muonBadTrackFilter");
  /**********************************/

  /* GEN M-ttbar selection [TTbar MC "0.<M^{gen}_{ttbar}(GeV)<700.] */
  const std::string ttbar_gen_label("ttbargen");

  ttgenprod.reset(new TTbarGenProducer(ctx, ttbar_gen_label, false));

  if(ctx.get("dataset_version") == "TTbar_Mtt0000to0700") genmttbar_sel.reset(new MttbarGenSelection( 0., 700.));
  else                                                    genmttbar_sel.reset(new uhh2::AndSelection(ctx));

  /******************************************************************/

  ////

  //// OBJ CLEANING
  const     MuonId muoSR(AndId<Muon>    (PtEtaCut  (50., 2.1), MuonIDMedium()));
  const ElectronId eleSR(AndId<Electron>(PtEtaSCCut(50., 2.5), eleID));

  if(use_miniiso){

    const     MuonId muoMINIIso(    Muon_MINIIso(0.05, "delta-beta"));
    const ElectronId eleMINIIso(Electron_MINIIso(0.05, "delta-beta"));

    muoSR_cleaner.reset(new     MuonCleaner(AndId<Muon>    (muoSR, muoMINIIso)));
    eleSR_cleaner.reset(new ElectronCleaner(AndId<Electron>(eleSR, eleMINIIso)));
  }
  else{

    muoSR_cleaner.reset(new     MuonCleaner(muoSR));
    eleSR_cleaner.reset(new ElectronCleaner(eleSR));
  }
  //

  const JetId jetID(JetPFID(JetPFID::WP_LOOSE));

  std::vector<std::string> JEC_AK4, JEC_AK8;
  if(isMC){
    JEC_AK4 = JERFiles::Fall15_25ns_L123_AK4PFchs_MC;
    JEC_AK8 = JERFiles::Fall15_25ns_L123_AK8PFchs_MC;
  }
  else {
    JEC_AK4 = JERFiles::Fall15_25ns_L123_AK4PFchs_DATA;
    JEC_AK8 = JERFiles::Fall15_25ns_L123_AK8PFchs_DATA;
  }

  jet_IDcleaner.reset(new JetCleaner(ctx, jetID));
  jet_corrector.reset(new JetCorrector(ctx, JEC_AK4));
  if(isMC) jetER_smearer.reset(new GenericJetResolutionSmearer(ctx));
  jetlepton_cleaner.reset(new JetLeptonCleaner_by_KEYmatching(ctx, JEC_AK4));
  jet_cleaner1.reset(new JetCleaner(ctx, 15., 3.0));
  jet_cleaner2.reset(new JetCleaner(ctx, 30., 2.4));

  topjet_IDcleaner.reset(new JetCleaner(ctx, jetID));
  topjet_corrector.reset(new TopJetCorrector(ctx, JEC_AK8));
  topjet_subjet_corrector.reset(new SubJetCorrector(ctx, JEC_AK4));
  if(isMC){
    ctx.declare_event_input<std::vector<Particle> >(ctx.get("GenTopJetCollection"), "gentopjets");
    topjetER_smearer.reset(new GenericJetResolutionSmearer(ctx, "topjets", "gentopjets", false));
  }
  // topjetlepton_cleaner.reset(new TopJetLeptonDeltaRCleaner(.8));   // TopJetLeptonDeltaRCleaner removes all AK8 Jets from AK8 Jet Collection, where a Lepton is clustered into the AK8 jet -> test different cleaner in further selection
  topjet_cleaner.reset(new TopJetCleaner(ctx, TopJetId(PtEtaCut(200., 2.4)))); // All AK8 Jets have pt>200 (already implied in ntuples)
  ////

  //// EVENT SELECTION
  jet2_sel.reset(new NJetSelection(2, -1, JetId(PtEtaCut(jet2_pt, 2.4))));
  jet1_sel.reset(new NJetSelection(1, -1, JetId(PtEtaCut(jet1_pt, 2.4))));

  met_sel  .reset(new METCut  (MET   , uhh2::infinity));
  htlep_sel.reset(new HTlepCut(HT_lep, uhh2::infinity));

  twodcut_sel.reset(new TwoDCut1(.4, 20.));
  ////

  //// set up Hists classes:
  // h_jetsel_event.reset(new EventHists(ctx, "01_JetSel_Event"));
  // h_jetsel_elec.reset(new ElectronHists(ctx, "01_JetSel_Elec"));
  // h_jetsel_muon.reset(new MuonHists(ctx, "01_JetSel_Muon"));
  // h_jetsel_jets.reset(new JetHists(ctx, "01_JetSel_Jets"));

  // h_metsel_event.reset(new EventHists(ctx, "02_METSel_Event"));
  // h_metsel_elec.reset(new ElectronHists(ctx, "02_METSel_Elec"));
  // h_metsel_muon.reset(new MuonHists(ctx, "02_METSel_Muon"));
  // h_metsel_jets.reset(new JetHists(ctx, "02_METSel_Jets"));

  // h_htsel_event.reset(new EventHists(ctx, "03_HTSel_Event"));
  // h_htsel_elec.reset(new ElectronHists(ctx, "03_HTSel_Elec"));
  // h_htsel_muon.reset(new MuonHists(ctx, "03_HTSel_Muon"));
  // h_htsel_jets.reset(new JetHists(ctx, "03_HTSel_Jets"));

  // h_twodsel_event.reset(new EventHists(ctx, "04_TwoDSel_Event"));
  // h_twodsel_elec.reset(new ElectronHists(ctx, "04_TwoDSel_Elec"));
  // h_twodsel_muon.reset(new MuonHists(ctx, "04_TwoDSel_Muon"));
  // h_twodsel_jets.reset(new JetHists(ctx, "04_TwoDSel_Jets"));

  ////

}

bool MTopJetPreSelectionModule::process(uhh2::Event& event){

  //// COMMON MODULES

  if(!event.isRealData){

    /* GEN M-ttbar selection */
    ttgenprod->process(event);
    if(!genmttbar_sel->passes(event)) return false;

  }

  /* CMS-certified luminosity sections */
  if(event.isRealData){

    if(!lumi_sel->passes(event)) return false;
  }

  /* MET filters */
  if(!metfilters_sel->passes(event)) return false;

  ////

  //// LEPTON selection
  muoSR_cleaner->process(event);
  sort_by_pt<Muon>(*event.muons);

  eleSR_cleaner->process(event);
  sort_by_pt<Electron>(*event.electrons);

  const bool pass_lep1 = ((event.muons->size() >= 1) || (event.electrons->size() >= 1));
  if(!pass_lep1) return false;
  ////

  //// JET selection

  jet_IDcleaner->process(event);
  jet_corrector->process(event);
  if(jetER_smearer.get()) jetER_smearer->process(event);
  jetlepton_cleaner->process(event);
  jet_cleaner1->process(event);
  sort_by_pt<Jet>(*event.jets);

  /* lepton-2Dcut variables */
  const bool pass_twodcut = twodcut_sel->passes(event); {

    for(auto& muo : *event.muons){

      float    dRmin, pTrel;
      std::tie(dRmin, pTrel) = drmin_pTrel(muo, *event.jets);

      muo.set_tag(Muon::twodcut_dRmin, dRmin);
      muo.set_tag(Muon::twodcut_pTrel, pTrel);
    }

    for(auto& ele : *event.electrons){

      float    dRmin, pTrel;
      std::tie(dRmin, pTrel) = drmin_pTrel(ele, *event.jets);

      ele.set_tag(Electron::twodcut_dRmin, dRmin);
      ele.set_tag(Electron::twodcut_pTrel, pTrel);
    }
  }


  jet_cleaner2->process(event);
  sort_by_pt<Jet>(*event.jets);

  topjet_IDcleaner->process(event);
  topjet_corrector->process(event);
  topjet_subjet_corrector->process(event);
  if(topjetER_smearer.get()) topjetER_smearer->process(event);
  // topjetlepton_cleaner->process(event);
  topjet_cleaner->process(event);
  sort_by_pt<TopJet>(*event.topjets);

  /* 2nd AK4 jet selection */
  const bool pass_jet2 = jet2_sel->passes(event);
  if(!pass_jet2) return false;

  /* 1st AK4 jet selection */
  const bool pass_jet1 = jet1_sel->passes(event);
  if(!pass_jet1) return false;

  // h_jetsel_event->fill(event);
  // h_jetsel_elec->fill(event);
  // h_jetsel_muon->fill(event);
  // h_jetsel_jets->fill(event);
  ////

  //// MET selection
  const bool pass_met = met_sel->passes(event);
  if(!pass_met) return false;
  // h_metsel_event->fill(event);
  // h_metsel_elec->fill(event);
  // h_metsel_muon->fill(event);
  // h_metsel_jets->fill(event);
  ////

  //// HT_lep selection
  const bool pass_htlep = htlep_sel->passes(event);
  if(!pass_htlep) return false;
  // h_htsel_event->fill(event);
  // h_htsel_elec->fill(event);
  // h_htsel_muon->fill(event);
  // h_htsel_jets->fill(event);
  

  
  //// LEPTON-2Dcut selection
  if(!pass_twodcut) return false;
  // h_twodsel_event->fill(event);
  // h_twodsel_elec->fill(event);
  // h_twodsel_muon->fill(event);
  // h_twodsel_jets->fill(event);
  ////

  return true;
}

UHH2_REGISTER_ANALYSIS_MODULE(MTopJetPreSelectionModule)
