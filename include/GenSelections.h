#pragma once

#include <UHH2/core/include/Event.h>
#include <UHH2/core/include/AnalysisModule.h>
#include <UHH2/core/include/Selection.h>
#include <UHH2/core/include/Utils.h>

#include <UHH2/common/include/NSelections.h>
#include <UHH2/common/include/ReconstructionHypothesis.h>
#include <UHH2/common/include/ObjectIdUtils.h>
#include <UHH2/common/include/TopJetIds.h>
#include <UHH2/common/include/TTbarGen.h>
#include <UHH2/common/include/Utils.h>

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>


namespace uhh2 {

  class TTbarSemilep : public Selection { 

  public: 
    explicit TTbarSemilep(Context&);
    virtual bool passes(const Event&) override;

  protected:
    uhh2::Event::Handle<TTbarGen> h_ttbargen;

  };

  class TopHadPT : public Selection { 

  public: 
    explicit TopHadPT(Context&, float ptmin=300);
    virtual bool passes(const Event&) override;

  private:
    uhh2::Event::Handle<TTbarGen> h_ttbargen;
    float ptmin_;

  };

  class Matching : public Selection { 

  public: 
    explicit Matching(Context&, const std::string &, float);
    virtual bool passes(const Event&) override;

  private:
    uhh2::Event::Handle<std::vector<Jet>> h_jets;
    uhh2::Event::Handle<TTbarGen> h_ttbargen;
    float jetradius_;
  };

   class Matching_HOTVR : public Selection { 

  public: 
    explicit Matching_HOTVR(Context&, const std::string &, double);
    virtual bool passes(const Event&) override;

  private:
    uhh2::Event::Handle<std::vector<Jet>> h_jets;
    uhh2::Event::Handle<TTbarGen> h_ttbargen;
    float rho_;
  };

  class Matching_XCone : public Selection { 

  public: 
    explicit Matching_XCone(Context&, const std::string &);
    virtual bool passes(const Event&) override;

  private:
    uhh2::Event::Handle<std::vector<Jet>> h_jets;
    uhh2::Event::Handle<TTbarGen> h_ttbargen;
  };

  class Matching_XCone23 : public Selection { 

  public: 
    explicit Matching_XCone23(Context&, const std::string &);
    virtual bool passes(const Event&) override;

  private:
    uhh2::Event::Handle<std::vector<Jet>> h_jets;
    uhh2::Event::Handle<TTbarGen> h_ttbargen;
  };

  class Matching_XCone_botlep_lep : public Selection { 

  public: 
    explicit Matching_XCone_botlep_lep(Context&, const std::string &);
    virtual bool passes(const Event&) override;

  private:
    uhh2::Event::Handle<std::vector<Jet>> h_jets;
    uhh2::Event::Handle<TTbarGen> h_ttbargen;
  };

   class Matching_top : public Selection { 

  public: 
    explicit Matching_top(Context&, float);
    virtual bool passes(const Event&) override;

  private:
    uhh2::Event::Handle<std::vector<GenTopJet>> h_gentopjets;
    uhh2::Event::Handle<TTbarGen> h_ttbargen;
    float jetradius_;
  };

   
 class LeadingJetPT : public Selection { 

  public: 
    explicit LeadingJetPT(Context&, const std::string &, float);
    virtual bool passes(const Event&) override;

  private:
    uhh2::Event::Handle<std::vector<Jet>> h_jets;
    float ptcut_;
  };

 class LeadingTopJetPT : public Selection { 

  public: 
    explicit LeadingTopJetPT(Context&, float);
    virtual bool passes(const Event&) override;

  private:
    uhh2::Event::Handle<std::vector<GenTopJet>> h_gentopjets;
    float ptcut_;
  };
 
 class MassCut : public Selection { 

  public: 
    explicit MassCut(Context&, const std::string &);
    virtual bool passes(const Event&) override;

  private:
    uhh2::Event::Handle<std::vector<Jet>> h_jets;
    uhh2::Event::Handle<TTbarGen> h_ttbargen;
  };

  class MassCut_top : public Selection { 

  public: 
    explicit MassCut_top(Context&);
    virtual bool passes(const Event&) override;

  private:
    uhh2::Event::Handle<std::vector<GenTopJet>> h_gentopjets;
    uhh2::Event::Handle<TTbarGen> h_ttbargen;
  };
  
 class DeltaRCut : public Selection { 

  public: 
    explicit DeltaRCut(Context&, const std::string &, float);
    virtual bool passes(const Event&) override;

  private:
    uhh2::Event::Handle<std::vector<Jet>> h_jets;
    uhh2::Event::Handle<TTbarGen> h_ttbargen;
    float jetradius_;
  };

 class DeltaRCut_HOTVR : public Selection { 

  public: 
    explicit DeltaRCut_HOTVR(Context&, const std::string &, double);
    virtual bool passes(const Event&) override;

  private:
    uhh2::Event::Handle<std::vector<Jet>> h_jets;
    uhh2::Event::Handle<TTbarGen> h_ttbargen;
    float rho_;
  };


 class DeltaRCut_top : public Selection { 

  public: 
    explicit DeltaRCut_top(Context&, float);
    virtual bool passes(const Event&) override;

  private:
    uhh2::Event::Handle<std::vector<GenTopJet>> h_gentopjets;
    uhh2::Event::Handle<TTbarGen> h_ttbargen;
    float jetradius_;
  };
 
 class NGenJets : public Selection { 

  public: 
    explicit NGenJets(Context&, const std::string &, float min_pt = 0, float min = 0, float max = 9999);
    virtual bool passes(const Event&) override;

  private:
    uhh2::Event::Handle<std::vector<Jet>> h_jets;
    float min_pt_;
    float min_;
    float max_;
  };

  class NGenTopJets : public Selection { 

  public: 
    explicit NGenTopJets(Context&, float min_pt = 0, float min = 0, float max = 9999);
    virtual bool passes(const Event&) override;

  private:
    uhh2::Event::Handle<std::vector<GenTopJet>> h_gentopjets;
    float min_pt_;
    float min_;
    float max_;
  };
  
 class GenJetLeptonCleaner : public uhh2::AnalysisModule { 

  public: 
    explicit GenJetLeptonCleaner(Context&, const std::string &, float);
    virtual bool process(Event& ) override;

  private:
    uhh2::Event::Handle<std::vector<Jet>> h_jets;
    uhh2::Event::Handle<TTbarGen> h_ttbargen;
    float jetradius_; 
  };

 class GenTopJetLeptonCleaner : public uhh2::AnalysisModule { 

  public: 
    explicit GenTopJetLeptonCleaner(Context&, float);
    virtual bool process(Event& ) override;

  private:
     uhh2::Event::Handle<std::vector<GenTopJet>> h_gentopjets;
     uhh2::Event::Handle<TTbarGen> h_ttbargen;
     float jetradius_; 
  };

 
 class MassCutGen1 : public Selection{ 

  public: 
   explicit MassCutGen1(Context&, const std::string &, float, float);
    virtual bool passes(const Event& ) override;

  private:
    uhh2::Event::Handle<std::vector<Jet>> h_jets;
    float M_min_;
    float M_max_; 
  };

  class MassCutGen1_top : public Selection{ 

  public: 
    explicit MassCutGen1_top(Context&, float, float);
    virtual bool passes(const Event& ) override;

  private:
    uhh2::Event::Handle<std::vector<GenTopJet>> h_gentopjets;
    float M_min_;
    float M_max_; 
  };

}
