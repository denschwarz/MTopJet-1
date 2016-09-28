#pragma once
#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/TTbarGen.h"
#include <fastjet/PseudoJet.hh> 
#include <fastjet/JetDefinition.hh>
#include "fastjet/AreaDefinition.hh"
#include <fastjet/ClusterSequence.hh>
#include <fastjet/ClusterSequenceArea.hh>
#include <fastjet/GhostedAreaSpec.hh>
#include "UHH2/core/include/PFParticle.h"

#include "UHH2/common/include/Utils.h"
#include "UHH2/core/include/Jet.h"
#include "UHH2/core/include/TopJet.h"
#include "UHH2/core/include/GenJetWithParts.h"
#include "UHH2/core/include/GenParticle.h"
#include "UHH2/core/include/PFParticle.h"
#include "UHH2/core/include/AnalysisModule.h"

#include <vector>
#include <iostream> 
#include <math.h>

using namespace std;
using namespace fastjet;

class JetCluster{

 private:

  std::vector<fastjet::PseudoJet> _hadrons;
  bool IsStable(GenParticle* p);
  bool IsLepton(GenParticle* p);
  fastjet::PseudoJet convert_particle(GenParticle* genparticle);
  fastjet::PseudoJet convert_recoparticle(PFParticle* pfparticle);

 public:

  enum E_algorithm { 
    e_ca, 
    e_akt,
    e_kt,
  };

  std::vector<fastjet::PseudoJet> get_genjets(std::vector<GenParticle>* genparts, enum JetCluster::E_algorithm algorithm, double jet_radius, double ptmin);
  std::vector<fastjet::PseudoJet> get_recojets(std::vector<PFParticle>* pfparts, enum JetCluster::E_algorithm algorithm, double jet_radius, double ptmin);
  std::vector<fastjet::PseudoJet> get_hotvr_jets(std::vector<GenParticle>* genparts, enum  JetCluster::E_algorithm algorithm, double rho, double min_r, double max_r, double mu, double theta, double pt_cut);
  std::vector<fastjet::PseudoJet> get_hotvr_recojets(std::vector<PFParticle>* pfparts, enum  JetCluster::E_algorithm algorithm, double rho, double min_r, double max_r, double mu, double theta, double pt_cut);
  std::vector<fastjet::PseudoJet> get_xcone_jets(std::vector<GenParticle>* genparts, int N, double R0, double beta, double ptmin);
  std::vector<fastjet::PseudoJet> get_xcone23_jets(std::vector<GenParticle>* genparts, double ptmin, double ptmin_sub1, double ptmin_sub2, int choose_jet);
  std::vector<fastjet::PseudoJet> get_xcone_recojets(std::vector<PFParticle>* pfparts, int N, double R0, double beta, double ptmin);


  std::vector<Jet> convert_pseudojet_to_jet(std::vector<fastjet::PseudoJet> fjet);
  std::vector<fastjet::PseudoJet> substract_lepton(GenParticle* genparts, std::vector<fastjet::PseudoJet> fjets, double jet_radius);

  void write_genjets(uhh2::Event & event, enum  JetCluster::E_algorithm algorithm, double jet_radius, double ptmin);

  std::vector<fastjet::PseudoJet> particle_in;
  std::vector<fastjet::PseudoJet> particle_in_subjet1;
  std::vector<fastjet::PseudoJet> particle_in_subjet2;
  std::vector<fastjet::PseudoJet> particle_in2;
  std::vector<fastjet::PseudoJet> particle_in_reco2;
  std::vector<fastjet::PseudoJet> particle_in_reco;
  fastjet::ClusterSequence* clust_seq;
  fastjet::ClusterSequence* clust_seq_hotvr;
  fastjet::ClusterSequence* clust_seq_reco;
  fastjet::ClusterSequence* clust_seq_xcone;
  fastjet::ClusterSequence* clust_seq_sub1;
  fastjet::ClusterSequence* clust_seq_sub2;
  std::vector<fastjet::PseudoJet> new_jets;
  std::vector<fastjet::PseudoJet> new_jets_cleaned;
  std::vector<fastjet::PseudoJet> new_recojets;
  fastjet::JetDefinition *jetdef;
  fastjet::JetDefinition *jetdef_reco;

  //double _rho, _mu, _theta, _rmin, _rmax, _ptmin,_radius,_pt_cut;
  string _clustering_algorithmus;

};


class JetProducer: public uhh2::AnalysisModule{
public:

  explicit JetProducer(uhh2::Context & , const std::string & , float, float);
  virtual bool process(uhh2::Event & ) override; 
    
private:
  uhh2::Event::Handle<std::vector<Jet>>h_newgenjets;
  float ptmin_;
  float jet_radius_;
};


class RecoJetProducer: public uhh2::AnalysisModule{
public:

  explicit RecoJetProducer(uhh2::Context&, const std::string &, float, float);
  virtual bool process(uhh2::Event & ) override; 
    
private:
  uhh2::Event::Handle<std::vector<Jet>>h_newrecojets;
  uhh2::Event::Handle<std::vector<PFParticle>> h_pfpart;
  float ptmin_;
  float jet_radius_;
};

class GenHOTVRJetProducer: public uhh2::AnalysisModule{
public:

  explicit GenHOTVRJetProducer(uhh2::Context&, const std::string &, double);
  virtual bool process(uhh2::Event & ) override; 
    
private:
  uhh2::Event::Handle<std::vector<Jet>>h_newgenhotvrjets;
  double rho_;
};


class RecoHOTVRJetProducer: public uhh2::AnalysisModule{
public:

  explicit RecoHOTVRJetProducer(uhh2::Context&, const std::string &, double);
  virtual bool process(uhh2::Event & ) override; 
    
private:
  uhh2::Event::Handle<std::vector<Jet>>h_newrecohotvrjets;
  uhh2::Event::Handle<std::vector<PFParticle>> h_pfpart;
  double rho_;
};

class GenXCONEJetProducer: public uhh2::AnalysisModule{
public:

  explicit GenXCONEJetProducer(uhh2::Context&, const std::string &, int, double, double, double);
  virtual bool process(uhh2::Event & ) override; 
    
private:
  uhh2::Event::Handle<std::vector<Jet>>h_newgenxconejets;
  int N_;
  double R0_;
  double beta_;
  double ptmin_;
};


class RecoXCONEJetProducer: public uhh2::AnalysisModule{
public:

  explicit RecoXCONEJetProducer(uhh2::Context&, const std::string &, int, double, double, double);
  virtual bool process(uhh2::Event & ) override; 
    
private:
  uhh2::Event::Handle<std::vector<Jet>>h_newrecoxconejets;
  uhh2::Event::Handle<std::vector<PFParticle>> h_pfpart;
  int N_;
  double R0_;
  double beta_;
  double ptmin_;
};

class MergeXConeGen: public uhh2::AnalysisModule{
public:

  explicit MergeXConeGen(uhh2::Context&, const std::string &, const std::string &);
  virtual bool process(uhh2::Event & ) override; 
    
private:
  uhh2::Event::Handle<TTbarGen>h_ttbargen;
  uhh2::Event::Handle<std::vector<Jet>>h_injets;
  uhh2::Event::Handle<std::vector<Jet>>h_newjets;
};

class MergeXConeReco: public uhh2::AnalysisModule{
public:

  explicit MergeXConeReco(uhh2::Context&, const std::string &, const std::string &);
  virtual bool process(uhh2::Event & ) override; 
    
private:
  uhh2::Event::Handle<std::vector<Jet>>h_injets;
  uhh2::Event::Handle<std::vector<Jet>>h_newjets;
};

class MergeXConeN6Gen: public uhh2::AnalysisModule{
public:

  explicit MergeXConeN6Gen(uhh2::Context&, const std::string &, const std::string &);
  virtual bool process(uhh2::Event & ) override; 
    
private:
  uhh2::Event::Handle<TTbarGen>h_ttbargen;
  uhh2::Event::Handle<std::vector<Jet>>h_injets;
  uhh2::Event::Handle<std::vector<Jet>>h_newjets;
};

class GenXCONE23JetProducer: public uhh2::AnalysisModule{
public:

  explicit GenXCONE23JetProducer(uhh2::Context&, const std::string &, double, double, double, int);
  virtual bool process(uhh2::Event & ) override; 
    
private:
  uhh2::Event::Handle<std::vector<Jet>>h_newgenxcone23jets;
  double ptmin_;
  double ptmin_sub1_;
  double ptmin_sub2_;
  int choose_jet_;
};
