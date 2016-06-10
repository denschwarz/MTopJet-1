#include <iostream>
#include <memory>

#include <UHH2/core/include/AnalysisModule.h>
#include <UHH2/core/include/Event.h>
#include <UHH2/core/include/Selection.h>

#include <UHH2/common/include/CleaningModules.h>
#include <UHH2/common/include/NSelections.h>
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
#include <UHH2/MTopJet/include/MTopJetHists.h>


#include <UHH2/MTopJet/include/ModuleBASE.h>
#include <UHH2/MTopJet/include/MTopJetSelections.h>
#include <UHH2/MTopJet/include/MTopJetUtils.h>

class MTopJetPostSelectionModule : public ModuleBASE {

 public:
  explicit MTopJetPostSelectionModule(uhh2::Context&);
  virtual bool process(uhh2::Event&) override;

 protected:
  enum lepton { muon, elec };
  lepton channel_;

  // cleaners
  std::unique_ptr<MuonCleaner>     muoSR_cleaner;
  std::unique_ptr<ElectronCleaner> eleSR_cleaner;

  std::unique_ptr<JetCleaner>                      jet_IDcleaner;
  std::unique_ptr<JetCorrector>                    jet_corrector;
  std::unique_ptr<GenericJetResolutionSmearer>     jetER_smearer;
  std::unique_ptr<JetLeptonCleaner_by_KEYmatching> jetlepton_cleaner;
  std::unique_ptr<JetCleaner>                      jet_cleaner1;
  std::unique_ptr<JetCleaner>                      jet_cleaner2;
  std::unique_ptr<JetLeptonCleaner>                jetlep_cleaner;

  std::unique_ptr<JetCleaner>                  topjet_IDcleaner;
  std::unique_ptr<TopJetCorrector>             topjet_corrector;
  std::unique_ptr<SubJetCorrector>             topjet_subjet_corrector;
  std::unique_ptr<GenericJetResolutionSmearer> topjetER_smearer;
  std::unique_ptr<TopJetLeptonDeltaRCleaner>   topjetlepton_cleaner;
  // std::unique_ptr<TopJetLeptonCleaner>         topjetlepton2_cleaner; 
  std::unique_ptr<TopJetCleaner>               topjet_cleaner;

  // selections
 
  std::unique_ptr<uhh2::Selection> topjet_sel;
  std::unique_ptr<uhh2::Selection> topjet1_sel;
  std::unique_ptr<uhh2::Selection> topjet2_sel;
  std::unique_ptr<uhh2::Selection> deltarcut_sel;
  std::unique_ptr<uhh2::Selection> topmass_sel;

  std::unique_ptr<uhh2::AnalysisModule> ttgenprod;


  // store Hist collection as member variables
  std::unique_ptr<Hists> 
    h_topjet_event, h_topjet_elec, h_topjet_muon, h_topjet_jets, h_topjet_topjets, h_topjet_event2,
    h_toplepcleaner_event, h_toplepcleaner_elec, h_toplepcleaner_muon, h_toplepcleaner_jets, h_toplepcleaner_topjets, h_toplepcleaner_event2,
    h_toplepdR_event, h_toplepdR_elec, h_toplepdR_muon, h_toplepdR_jets, h_toplepdR_topjets, h_toplepdR_event2,	
    h_jetlepdR_event, h_jetlepdR_elec, h_jetlepdR_muon, h_jetlepdR_jets, h_jetlepdR_topjets, h_jetlepdR_event2, 
    h_topmass_event, h_topmass_elec, h_topmass_muon, h_topmass_jets, h_topmass_topjets, h_topmass_event2;

  // Event::Handle<float> tt_TMVA_response;// response of TMVA method, dummy value at this step

};

MTopJetPostSelectionModule::MTopJetPostSelectionModule(uhh2::Context& ctx){

  //// CONFIGURATION
  const bool isMC = (ctx.get("dataset_type") == "MC");
  const std::string& keyword = ctx.get("keyword");

  bool noCleaner = false;
  bool deltaR = false;
  bool TopLep = false;

  if(keyword == "v01") noCleaner = true;
  else if(keyword == "v02") deltaR = true;
  else if(keyword == "v03") TopLep = true;

  //// COMMON MODULES


  ////

  //// OBJ CLEANING
  std::vector<std::string> JEC_AK4, JEC_AK8;
  if(isMC){
    JEC_AK4 = JERFiles::Fall15_25ns_L123_AK4PFchs_MC;
    JEC_AK8 = JERFiles::Fall15_25ns_L123_AK8PFchs_MC;
  }
  else {
    JEC_AK4 = JERFiles::Fall15_25ns_L123_AK4PFchs_DATA;
    JEC_AK8 = JERFiles::Fall15_25ns_L123_AK8PFchs_DATA;
  }

  if(noCleaner){
  // topjetlepton_cleaner.reset();  
  // topjetlepton2_cleaner.reset();  
  }
  else if(deltaR){
  // topjetlepton_cleaner.reset(new TopJetLeptonDeltaRCleaner(.8));  
  // topjetlepton2_cleaner.reset();  
  }
  else if(TopLep){
  // topjetlepton_cleaner.reset();  
  // topjetlepton2_cleaner.reset(new TopJetLeptonCleaner(ctx, JEC_AK8));  
    jetlep_cleaner.reset(new JetLeptonCleaner(ctx, JEC_AK8));
  }
  topjet_cleaner.reset(new TopJetCleaner(ctx, TopJetId(PtEtaCut(200., 2.4))));


  //// EVENT SELECTION



  topjet2_sel.reset(new NTopJetSelection(2, 2, TopJetId(PtEtaCut(200, 2.4))));
  topjet1_sel.reset(new NTopJetSelection(1, 2, TopJetId(PtEtaCut(400, 2.4))));
  
  topmass_sel.reset(new TopJetMassCut());
  
  deltarcut_sel.reset(new deltaRCut(.8));

  ////

  //// set up Hists classes:

  h_topjet_event.reset(new EventHists(ctx, "01_topjet_Event"));
  h_topjet_elec.reset(new ElectronHists(ctx, "01_topjet_Elec"));
  h_topjet_muon.reset(new MuonHists(ctx, "01_topjet_Muon"));
  h_topjet_jets.reset(new JetHists(ctx, "01_topjet_Jets"));
  h_topjet_topjets.reset(new TopJetHists(ctx, "01_topjet_TopJets"));
  h_topjet_event2.reset(new MTopJetHists(ctx, "01_topjet_Event2"));

  h_topmass_event.reset(new EventHists(ctx, "02_topmass_Event"));
  h_topmass_elec.reset(new ElectronHists(ctx, "02_topmass_Elec"));
  h_topmass_muon.reset(new MuonHists(ctx, "02_topmass_Muon"));
  h_topmass_jets.reset(new JetHists(ctx, "02_topmass_Jets"));
  h_topmass_topjets.reset(new TopJetHists(ctx, "02_topmass_TopJets"));
  h_topmass_event2.reset(new MTopJetHists(ctx, "02_topmass_Event2"));

  h_toplepdR_event.reset(new EventHists(ctx, "03_toplepdR_Event"));
  h_toplepdR_elec.reset(new ElectronHists(ctx, "03_toplepdR_Elec"));
  h_toplepdR_muon.reset(new MuonHists(ctx, "03_toplepdR_Muon"));
  h_toplepdR_jets.reset(new JetHists(ctx, "03_toplepdR_Jets"));
  h_toplepdR_topjets.reset(new TopJetHists(ctx, "03_toplepdR_TopJets"));
  h_toplepdR_event2.reset(new MTopJetHists(ctx, "03_toplepdR_Event2"));

////

}

bool MTopJetPostSelectionModule::process(uhh2::Event& event){

  //// COMMON MODULES

  ////

  //// LEPTON selection

  ////

  //// JET selection

  ////

  //// Top Jet Selection

  // topjetlepton_cleaner->process(event);
  // topjetlepton2_cleaner->process(event);
  jetlep_cleaner->process(event);
  topjet_cleaner->process(event);

  /* 2nd AK8 jet selection */
  const bool pass_topjet2 = topjet2_sel->passes(event);
  if(!pass_topjet2) return false;

  /* 1st AK8 jet selection */
  const bool pass_topjet1 = topjet1_sel->passes(event);
  if(!pass_topjet1) return false;

  h_topjet_event->fill(event);
  h_topjet_elec->fill(event);
  h_topjet_muon->fill(event);
  h_topjet_jets->fill(event);
  h_topjet_topjets->fill(event);
  h_topjet_event2->fill(event);

  /* Top Jet Mass Cut M1 > M2 */
  const bool pass_topmass = topmass_sel->passes(event);
  if(!pass_topmass) return false;

  h_topmass_event->fill(event);
  h_topmass_elec->fill(event);
  h_topmass_muon->fill(event);
  h_topmass_jets->fill(event);
  h_topmass_topjets->fill(event);
  h_topmass_event2->fill(event);

  /* delta R (lep, topjet2) < 0.8  */
  const bool pass_deltaR = deltarcut_sel->passes(event);
  if(pass_deltaR){
    h_toplepdR_event->fill(event);
    h_toplepdR_elec->fill(event);
    h_toplepdR_muon->fill(event);
    h_toplepdR_jets->fill(event);
    h_toplepdR_topjets->fill(event);
    h_toplepdR_event2->fill(event);
  }

  ////
return true;
}

UHH2_REGISTER_ANALYSIS_MODULE(MTopJetPostSelectionModule)