#include "UHH2/MTopJet/include/JetCluster.h"
#include "fastjet/contrib/HOTVR.hh"
#include "fastjet/contrib/XConePlugin.hh"
// #include "fastjet/contrib/ClusteringVetoPlugin.hh"
#include <math.h>

using namespace std;
using namespace fastjet;
using namespace contrib;



// --------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------ Jet Clustering ----------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

// ------------------------------------ Get clustered Reco Jet from PF Particles ---------------------------------------------------------------------------------
std::vector<fastjet::PseudoJet> JetCluster::get_recojets(std::vector<PFParticle>* pfparts, enum  JetCluster::E_algorithm algorithm, double jet_radius, double ptmin){

  for (unsigned int i=0; i<(pfparts->size()); ++i){
      PFParticle* part = &(pfparts->at(i));
      particle_in_reco.push_back(convert_recoparticle(part));
  }

  if(algorithm==e_ca) jetdef_reco= new fastjet::JetDefinition(fastjet::cambridge_algorithm,jet_radius);
  if(algorithm==e_akt)jetdef_reco= new fastjet::JetDefinition(fastjet::antikt_algorithm,jet_radius);

  clust_seq_reco=new fastjet::ClusterSequence(particle_in_reco, *jetdef_reco);

  new_recojets = sorted_by_pt(clust_seq_reco->inclusive_jets(ptmin));

  // new_recojets = particle_in_reco;

  delete jetdef;
  return new_recojets;

}

// ------------------------------------ Get clustered Gen Jet from Gen Particles ---------------------------------------------------------------------------------
std::vector<fastjet::PseudoJet> JetCluster::get_genjets(std::vector<GenParticle>* genparts, enum  JetCluster::E_algorithm algorithm, double jet_radius, double ptmin){

  fastjet::PseudoJet lepton;
  for (unsigned int i=0; i<(genparts->size()); ++i){
      GenParticle* part = &(genparts->at(i));
      if(IsStable(part)){
	  particle_in.push_back(convert_particle(part));
	  if(IsLepton(part)){
	      lepton = convert_particle(part);
	  }
	}
  }

  if(algorithm==e_ca) jetdef= new fastjet::JetDefinition(fastjet::cambridge_algorithm,jet_radius);
  if(algorithm==e_akt)jetdef= new fastjet::JetDefinition(fastjet::antikt_algorithm,jet_radius);

  clust_seq=new fastjet::ClusterSequence(particle_in, *jetdef);
  new_jets = sorted_by_pt(clust_seq->inclusive_jets(ptmin));

   delete jetdef;
  return new_jets; 
}


// ------------------------------------ Get clustered HOTVR Jets from Gen Particles ---------------------------------------------------------------------------------
std::vector<fastjet::PseudoJet> JetCluster::get_hotvr_jets(std::vector<GenParticle>* genparts,enum  JetCluster::E_algorithm algorithm, double rho, double min_r, double max_r, double mu, double theta, double pt_cut){ 

   for (unsigned int i=0; i<(genparts->size()); ++i){
      GenParticle* part = &(genparts->at(i));
	if(IsStable(part)){
	  particle_in2.push_back(convert_particle(part));
	}
  }

  HOTVR::ClusterType clustertype;
  if(algorithm==e_akt) clustertype=HOTVR::ClusterType::AKTLIKE;
  if(algorithm==e_ca) clustertype=HOTVR::ClusterType::CALIKE;
  if(algorithm==e_kt) clustertype=HOTVR::ClusterType::KTLIKE;
  HOTVR plugin_hotvr(mu, theta, min_r, max_r, rho, pt_cut, clustertype);  //call HOTVR algorithm
  fastjet::JetDefinition jet_def_hotvr(&plugin_hotvr);
  clust_seq_hotvr=new fastjet::ClusterSequence(particle_in2, jet_def_hotvr);

  std::vector<fastjet::PseudoJet> hotvr_jets,rejected_jets,soft_jets ; //vector of hotvr_jets, jets that were rejcted durning the clustering procedure and soft jets

  //get vector from the plugin
  hotvr_jets=plugin_hotvr.get_jets();
  rejected_jets=plugin_hotvr.get_rejected_cluster();
  soft_jets=plugin_hotvr.get_soft_cluster();
  return hotvr_jets;
}


// ------------------------------------ Get clustered HOTVR Jets from PFParticles ---------------------------------------------------------------------------------
std::vector<fastjet::PseudoJet> JetCluster::get_hotvr_recojets(std::vector<PFParticle>* pfparts,enum  JetCluster::E_algorithm algorithm, double rho, double min_r, double max_r, double mu, double theta, double pt_cut){ 

 for (unsigned int i=0; i<(pfparts->size()); ++i){
      PFParticle* part = &(pfparts->at(i));
      particle_in_reco2.push_back(convert_recoparticle(part));
  }

  HOTVR::ClusterType clustertype;
  if(algorithm==e_akt) clustertype=HOTVR::ClusterType::AKTLIKE;
  if(algorithm==e_ca) clustertype=HOTVR::ClusterType::CALIKE;
  if(algorithm==e_kt) clustertype=HOTVR::ClusterType::KTLIKE;
  HOTVR plugin_hotvr(mu, theta, min_r, max_r, rho, pt_cut, clustertype);  //call HOTVR algorithm
  fastjet::JetDefinition jet_def_hotvr(&plugin_hotvr);
  clust_seq_hotvr=new fastjet::ClusterSequence(particle_in_reco2, jet_def_hotvr);

  std::vector<fastjet::PseudoJet> hotvr_recojets,rejected_recojets,soft_recojets ; //vector of hotvr_jets, jets that were rejcted durning the clustering procedure and soft jets
	
  //get vector from the plugin
  hotvr_recojets=plugin_hotvr.get_jets();
  rejected_recojets=plugin_hotvr.get_rejected_cluster();
  soft_recojets=plugin_hotvr.get_soft_cluster();
  return hotvr_recojets;
}

// ------------------------------------ Get clustered XCone Jets from Gen Particles ---------------------------------------------------------------------------------
std::vector<fastjet::PseudoJet> JetCluster::get_xcone_jets(std::vector<GenParticle>* genparts, int N, double R0, double beta, double ptmin){ 

  std::vector<fastjet::PseudoJet> xcone_jets; 
  for (unsigned int i=0; i<(genparts->size()); ++i){
    GenParticle* part = &(genparts->at(i));
    if(IsStable(part)){
      particle_in2.push_back(convert_particle(part));
    }
  }
  XConePlugin plugin_xcone(N, R0, beta);
  fastjet::JetDefinition jet_def_xcone(&plugin_xcone);
  clust_seq_xcone=new fastjet::ClusterSequence(particle_in2, jet_def_xcone);
  xcone_jets = sorted_by_pt(clust_seq_xcone->inclusive_jets(ptmin));



  return xcone_jets;
}


// ------------------------------------ Get clustered XCone 2x3 Jets from Gen Particles ---------------------------------------------------------------------------------
std::vector<fastjet::PseudoJet> JetCluster::get_xcone23_jets(uhh2::Event & event, uhh2::Event::Handle<vector<int>> h_list, std::vector<GenParticle>* genparts, double ptmin, double ptmin_sub, int choose_jet){ 

  std::vector<fastjet::PseudoJet> fatjets, subjets1, subjets2, returnjets; 
  std::vector<GenParticle> genparts2, genparts_sub1, genparts_sub2;
  GenParticle lepton;
  particle_in_noGhost.clear();
  particle_in2.clear();
  for (unsigned int i=0; i<(genparts->size()); ++i){
    GenParticle* part = &(genparts->at(i));
    if(IsStable(part)){
      genparts2.push_back(genparts->at(i));
      particle_in_noGhost.push_back(convert_particle(part));
      if(abs(genparts->at(i).pdgId()) == 11 || abs(genparts->at(i).pdgId()) == 13) lepton = genparts->at(i);
    }
  }
  // add ghost particles
  // cout<<"Number Genparts: "<<genparts2.size()<<endl;
  // cout<<"Number before: "<<particle_in_noGhost.size()<<endl;
  bool ghosts = true;
  if(ghosts) particle_in2 = add_ghosts(particle_in_noGhost);
  else particle_in2 = particle_in_noGhost;
  // cout<<"Number after: "<<particle_in2.size()<<endl;

  XConePlugin plugin_xcone(2, 100, 2.0);
  fastjet::JetDefinition jet_def_xcone(&plugin_xcone);
  clust_seq_xcone=new fastjet::ClusterSequence(particle_in2, jet_def_xcone);

  fatjets = sorted_by_pt(clust_seq_xcone->inclusive_jets(ptmin));

  // get list: if particle i ist clustered in jet j, the i-th entry of the list == j, also write list
  std::vector<int> particle_list;
  particle_list.clear();
  particle_list = clust_seq_xcone->particle_jet_indices(fatjets);
  // get one set of particles for each jet. Also check if lepton is clustered into jet
  bool lep_in_jet1 = false;
  bool lep_in_jet2 = false;
  for (unsigned int ipart=0; ipart<particle_in2.size(); ++ipart){
    if (particle_list[ipart]==0){
      if(ipart < genparts2.size() && (abs(genparts2.at(ipart).pdgId()) == 11 || abs(genparts2.at(ipart).pdgId()) == 13)){
	lep_in_jet1 = true;
      }
      if(ipart < genparts2.size()) genparts_sub1.push_back(genparts2.at(ipart)); // get vector as GenParticles (without Ghosts)
      particle_in_subjet1.push_back(particle_in2.at(ipart)); // get vector as PseudoJet
    }
    if (particle_list[ipart]==1){
      if(ipart < genparts2.size() && (abs(genparts2.at(ipart).pdgId()) == 11 || abs(genparts2.at(ipart).pdgId()) == 13)){
	lep_in_jet2 = true;
      }
      if(ipart < genparts2.size()) genparts_sub2.push_back(genparts2.at(ipart)); // get vector as GenParticles (without Ghosts)
      particle_in_subjet2.push_back(particle_in2.at(ipart)); // get vector as PseudoJet
    }
    // if (particle_list[ipart]==-1 && (abs(genparts2.at(ipart).pdgId()) == 11 || abs(genparts2.at(ipart).pdgId()) == 13)) cout<<"Lepton out of Jet!!!"<< endl;
  }

  // if lepton is not clustered into one of the jets get nearest jet to lepton and tag this jet as "leptonic"
  if(!(lep_in_jet1 || lep_in_jet2)){
    double dR1 = uhh2::deltaR(lepton, fatjets.at(0));
    double dR2 = uhh2::deltaR(lepton, fatjets.at(1));
    if(dR1 < dR2) lep_in_jet1 = true;
    else if(dR2 < dR1) lep_in_jet2 = true;
  }

  // now cluster subjets. the fat jet with (or next to) the lepton gets 2 subjets, the other one gets 3 subjets
  if(lep_in_jet2){
    if(choose_jet == 1){
      XConePlugin plugin_xcone_sub1(3, 0.4, 2.0);
      fastjet::JetDefinition jet_def_sub1(&plugin_xcone_sub1);
      clust_seq_sub1=new fastjet::ClusterSequence(particle_in_subjet1, jet_def_sub1);
      subjets1 = sorted_by_pt(clust_seq_sub1->inclusive_jets(ptmin_sub));
    }
    if(choose_jet == 2){
      XConePlugin plugin_xcone_sub2(2, 0.4, 2.0);
      fastjet::JetDefinition jet_def_sub2(&plugin_xcone_sub2);
      clust_seq_sub2=new fastjet::ClusterSequence(particle_in_subjet2, jet_def_sub2);
      subjets2 = sorted_by_pt(clust_seq_sub2->inclusive_jets(ptmin_sub));
    }
  }
  if(lep_in_jet1){
    if(choose_jet == 1){
      XConePlugin plugin_xcone_sub1(3, 0.4, 2.0);
      fastjet::JetDefinition jet_def_sub1(&plugin_xcone_sub1);
      clust_seq_sub1=new fastjet::ClusterSequence(particle_in_subjet2, jet_def_sub1);
      subjets1 = sorted_by_pt(clust_seq_sub1->inclusive_jets(ptmin_sub));
    }
    if(choose_jet == 2){
      XConePlugin plugin_xcone_sub2(2, 0.4, 2.0);
      fastjet::JetDefinition jet_def_sub2(&plugin_xcone_sub2);
      clust_seq_sub2=new fastjet::ClusterSequence(particle_in_subjet1, jet_def_sub2);
      subjets2 = sorted_by_pt(clust_seq_sub2->inclusive_jets(ptmin_sub));
    }
  }

  // Substract Lepton
  std::vector<int> particle_list1, particle_list2;
  GenParticle* lep;
  int jetnr;
  if(choose_jet == 1){
    particle_list1.clear();
    particle_list1 = clust_seq_sub1->particle_jet_indices(subjets1);
    for(unsigned int i=0; i<genparts_sub1.size(); ++i){
      if(abs(genparts_sub1.at(i).pdgId()) == 11 || abs(genparts_sub1.at(i).pdgId()) == 13){
	lep = &(genparts_sub1.at(i));
	jetnr = particle_list1[i];
	if(jetnr != -1){
	  // cout<<"Jet Px before Cleaner: "<<subjets1.at(jetnr).px()<<endl;
	  // cout<<"Lepton Px before Cleaner: "<<lep->v4().Px()<<endl;
	  subjets1.at(jetnr) = substract_lepton(lep, subjets1.at(jetnr));
	  // cout<<"Jet Px after Cleaner: "<<subjets1.at(jetnr).px()<<endl;
	}
      }
    }
  }
  if(choose_jet == 2){
    particle_list2.clear();
    particle_list2 = clust_seq_sub2->particle_jet_indices(subjets2);
    for(unsigned int j=0; j<genparts_sub2.size(); ++j){
      if(abs(genparts_sub2.at(j).pdgId()) == 11 || abs(genparts_sub2.at(j).pdgId()) == 13){
	lep = &(genparts_sub2.at(j));
	jetnr = particle_list2[j];
	if(jetnr != -1) subjets2.at(jetnr) = substract_lepton(lep, subjets2.at(jetnr));
      }
    }
  }
  if(choose_jet == 0) event.set(h_list, particle_list);
  if(choose_jet == 1) event.set(h_list, particle_list1);
  if(choose_jet == 2) event.set(h_list, particle_list2);
  if(choose_jet == 0) returnjets = fatjets;
  if(choose_jet == 1) returnjets = subjets1;
  if(choose_jet == 2) returnjets = subjets2;

  return returnjets;
}

// ------------------------------------ Get clustered XCone Jets from PFParticles ---------------------------------------------------------------------------------
std::vector<fastjet::PseudoJet> JetCluster::get_xcone_recojets(std::vector<PFParticle>* pfparts, int N, double R0, double beta, double ptmin){ 
 
 std::vector<fastjet::PseudoJet> xcone_recojets; 
 for (unsigned int i=0; i<(pfparts->size()); ++i){
      PFParticle* part = &(pfparts->at(i));
      particle_in_reco2.push_back(convert_recoparticle(part));
  }
  XConePlugin plugin_xcone(N, R0, beta);
  fastjet::JetDefinition jet_def_xcone(&plugin_xcone);
  clust_seq_xcone=new fastjet::ClusterSequence(particle_in_reco2, jet_def_xcone);
  xcone_recojets = sorted_by_pt(clust_seq_xcone->inclusive_jets(ptmin));

  return xcone_recojets;
  
}
// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------ additional functions ----------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

// ------------------------------------ Check if Gen particle is stable ---------------------------------------------------------------------------------
bool JetCluster::IsStable(GenParticle* p)
{
	int st = p->status();
	if (st==1) return true;
	else return false;
}

// ------------------------------------ Check if Gen particle is Electron/Muon ---------------------------------------------------------------------------------
bool JetCluster::IsLepton(GenParticle* p)
{
  int id = abs(p->pdgId());
  if (id==11 || id==13) return true;
  else return false;
}

// ------------------------------------ convert Gen Particles to PseudoJet ---------------------------------------------------------------------------------
fastjet::PseudoJet JetCluster::convert_particle(GenParticle* genparticle){
  TLorentzVector particle;
  particle.SetPtEtaPhiE(genparticle->pt(),genparticle->eta(),genparticle->phi(),genparticle->energy());
  fastjet::PseudoJet gen_particle(particle.Px(),particle.Py(),particle.Pz(),particle.E());
  return gen_particle;

}

// ------------------------------------ convert PF Particles to PseudoJet ---------------------------------------------------------------------------------
fastjet::PseudoJet JetCluster::convert_recoparticle(PFParticle* pfparticle){
  TLorentzVector particle;
  particle.SetPtEtaPhiE(pfparticle->pt(),pfparticle->eta(),pfparticle->phi(),pfparticle->energy());
  fastjet::PseudoJet pf_particle(particle.Px(),particle.Py(),particle.Pz(),particle.E());
  return pf_particle;
}

// ------------------------------------ convert Pseudo Jet to Jet ---------------------------------------------------------------------------------
std::vector<Jet> JetCluster::convert_pseudojet_to_jet(std::vector<fastjet::PseudoJet> fjet){
  vector<Jet> new_jets;
  Jet new_jet;
  // TLorentzVector jet;
  for(unsigned i=0; i<fjet.size(); ++i){
    new_jet.set_pt(fjet[i].pt());
    new_jet.set_eta(fjet[i].eta());
    new_jet.set_phi(fjet[i].phi());
    new_jet.set_energy(fjet[i].E());
    new_jets.push_back(new_jet);
  }
  return new_jets;
}

// ------------------------------------ Substract Lepton ---------------------------------------------------------------------------------
fastjet::PseudoJet JetCluster::substract_lepton(GenParticle* lepton2, fastjet::PseudoJet fjet){
  fastjet::PseudoJet new_fjet;
  fastjet::PseudoJet lepton = convert_particle(lepton2);
  double px, py, pz, E;
  px = fjet.px()-lepton.px();
  py = fjet.py()-lepton.py();
  pz = fjet.pz()-lepton.pz();
  E = fjet.E()-lepton.E();
  
  new_fjet.reset(px, py, pz, E);

  return new_fjet;
}

// ------------------------------------ Add Ghost GenParticles to Pseudo Jet of GenParticles ---------------------------------------------
 std::vector<fastjet::PseudoJet> JetCluster::add_ghosts(std::vector<fastjet::PseudoJet> gen_in){
   //fastjet::PseudoJet ghost;
   double pt, y, phi, E, p, px, py, pz;
  for(unsigned int i=0; i < 50; ++i){
   for(unsigned int j=0; j < 50; ++j){
     phi = -M_PI + (i+0.5)*(2*M_PI/50);
     y = -M_PI + (j+0.5)*(2*M_PI/50);
     E = (rand() % 2)*0.0001;
     p = sqrt(E*E);
     pt = p/cosh(y);
     px = pt * cos(phi);
     py = pt * sin(phi);
     pz = pt * sinh(y);
     fastjet::PseudoJet ghost(px, py, pz, E);
     gen_in.push_back(ghost);
   }
  }
  return gen_in;
}
// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------ Jet Producer ------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------


// ------------------------------------ Gen Jets ----------------------------------------------------------------------------------
JetProducer::JetProducer(uhh2::Context & ctx, const std::string & name, float ptmin, float jet_radius):
  h_newgenjets(ctx.declare_event_output<std::vector<Jet>>(name)),
  ptmin_(ptmin),
  jet_radius_(jet_radius) {}

bool JetProducer::process(uhh2::Event & event){
  
  std::vector<GenParticle>* genparts = event.genparticles;
  JetCluster* jetc = new JetCluster();
  std::vector<fastjet::PseudoJet> gen;
  gen = jetc->get_genjets(genparts, JetCluster::e_akt, jet_radius_, ptmin_); 

  event.set(h_newgenjets, jetc->convert_pseudojet_to_jet(gen));

  delete jetc;
  return true;
}

// ------------------------------------ Reco Jets ---------------------------------------------------------------------------------
RecoJetProducer::RecoJetProducer(uhh2::Context & ctx, const std::string & name, float ptmin, float jet_radius):
  h_newrecojets(ctx.declare_event_output<std::vector<Jet>>(name)),
  h_pfpart(ctx.get_handle<vector<PFParticle>>("PFParticles")),
  ptmin_(ptmin),
  jet_radius_(jet_radius) {}

bool RecoJetProducer::process(uhh2::Event & event){

  std::vector<PFParticle> pfparts = event.get(h_pfpart);
  JetCluster* jetc_reco = new JetCluster();
  std::vector<fastjet::PseudoJet> reco;
  reco = jetc_reco->get_recojets(&pfparts, JetCluster::e_akt, jet_radius_, ptmin_); 
  event.set(h_newrecojets, jetc_reco->convert_pseudojet_to_jet(reco));

  delete jetc_reco;
  return true;
}
// ------------------------------------ HOTVR Reco Jets -----------------------------------------------------------------------------
RecoHOTVRJetProducer::RecoHOTVRJetProducer(uhh2::Context & ctx, const std::string & name, double rho):
  h_newrecohotvrjets(ctx.declare_event_output<std::vector<Jet>>(name)),
  h_pfpart(ctx.get_handle<vector<PFParticle>>("PFParticles")),
  rho_(rho) {}

bool RecoHOTVRJetProducer::process(uhh2::Event & event){
  // standard values for HOTVR:
  double mu(30.),     // massjump threshold
    theta(0.7),       // massjump parameter
    max_r(1.5),       // maximum allowed distance R
    min_r(0.1),       // minimum allowed distance R
    // rho(600),         // cone shrinking parameter
    pt_cut(30.);      // minimum pT of subjets

  std::vector<PFParticle> pfparts = event.get(h_pfpart);
  JetCluster* jetc_reco = new JetCluster();
  std::vector<fastjet::PseudoJet> reco;

  reco = jetc_reco->get_hotvr_recojets(&pfparts, JetCluster::e_ca, rho_, min_r, max_r, mu, theta, pt_cut);

  event.set(h_newrecohotvrjets, jetc_reco->convert_pseudojet_to_jet(reco));

  delete jetc_reco;
  return true;
}


// ------------------------------------ HOTVR Gen Jets -----------------------------------------------------------------------------
GenHOTVRJetProducer::GenHOTVRJetProducer(uhh2::Context & ctx, const std::string & name, double rho):
  h_newgenhotvrjets(ctx.declare_event_output<std::vector<Jet>>(name)),
  rho_(rho) {}

bool GenHOTVRJetProducer::process(uhh2::Event & event){
  // standard values for HOTVR:
  double mu(30.),     // massjump threshold
    theta(0.7),       // massjump parameter
    max_r(1.5),       // maximum allowed distance R
    min_r(0.1),       // minimum allowed distance R
    // rho(600),         // cone shrinking parameter
    pt_cut(30.);      // minimum pT of subjets

  std::vector<GenParticle>* genparts = event.genparticles;
  JetCluster* jetc = new JetCluster();
  std::vector<fastjet::PseudoJet> gen;
  gen = jetc->get_hotvr_jets(genparts, JetCluster::e_ca, rho_, min_r, max_r, mu, theta, pt_cut);

  event.set(h_newgenhotvrjets, jetc->convert_pseudojet_to_jet(gen));

  delete jetc;
  return true;
}

// ------------------------------------ XCone Reco Jets ----------------------------------------------------------------------------
RecoXCONEJetProducer::RecoXCONEJetProducer(uhh2::Context & ctx, const std::string & name, int N, double R0, double beta, double ptmin):
  h_newrecoxconejets(ctx.declare_event_output<std::vector<Jet>>(name)),
  h_pfpart(ctx.get_handle<vector<PFParticle>>("PFParticles")),
  N_(N),
  R0_(R0),
  beta_(beta),
  ptmin_(ptmin) {}

bool RecoXCONEJetProducer::process(uhh2::Event & event){


  std::vector<PFParticle> pfparts = event.get(h_pfpart);
  JetCluster* jetc_reco = new JetCluster();
  std::vector<fastjet::PseudoJet> reco;
  reco = jetc_reco->get_xcone_recojets(&pfparts, N_, R0_, beta_, ptmin_);

  event.set(h_newrecoxconejets, jetc_reco->convert_pseudojet_to_jet(reco));

  delete jetc_reco;
  return true;
}

// ------------------------------------ XCone Gen Jets -----------------------------------------------------------------------------
GenXCONEJetProducer::GenXCONEJetProducer(uhh2::Context & ctx, const std::string & name, int N, double R0, double beta, double ptmin):
  h_newgenxconejets(ctx.declare_event_output<std::vector<Jet>>(name)),
  N_(N),
  R0_(R0),
  beta_(beta),
  ptmin_(ptmin) {}

bool GenXCONEJetProducer::process(uhh2::Event & event){


  std::vector<GenParticle>* genparts = event.genparticles;
  JetCluster* jetc = new JetCluster();
  std::vector<fastjet::PseudoJet> gen;
  gen = jetc->get_xcone_jets(genparts, N_, R0_, beta_, ptmin_);

  event.set(h_newgenxconejets, jetc->convert_pseudojet_to_jet(gen));

  delete jetc;
  return true;
}

// ------------------------------------ merging XCone Gen Jets -------------------------------------------------------------------
MergeXConeGen::MergeXConeGen(uhh2::Context & ctx, const std::string & in_jet, const std::string & out_jet):
  h_ttbargen(ctx.get_handle<TTbarGen>("ttbargen")),
  h_injets(ctx.get_handle<std::vector<Jet>>(in_jet)),
  h_newjets(ctx.declare_event_output<std::vector<Jet>>(out_jet)) {}

bool MergeXConeGen::process(uhh2::Event & event){

  // get Lepton from TTbar
  const auto & ttbargen = event.get(h_ttbargen);
  GenParticle lep1, lep2, lepton;
  if(ttbargen.IsTopHadronicDecay()){
    lep1 = ttbargen.WMinusdecay1();
    lep2 = ttbargen.WMinusdecay2();
  }
  else if(ttbargen.IsAntiTopHadronicDecay()){
    lep1 = ttbargen.Wdecay1();
    lep2 = ttbargen.Wdecay2();
  }
  if(abs(lep1.pdgId()) == 11 || abs(lep1.pdgId()) == 13){
    lepton = lep1;
  }
  else if(abs(lep2.pdgId()) == 11 || abs(lep2.pdgId()) == 13){
    lepton = lep2;
  }

  // get distance to jets
  std::vector<Jet> jets = event.get(h_injets);
  TLorentzVector jet1_v4, jet2_v4, jet3_v4, jet4_v4, combinedjet1_v4, combinedjet2_v4;
  Jet jet1;
  jet1 = jets.at(0);
  float dR1, dR2, dR3, dR4;
  if(jets.size() > 3){
    jet1_v4.SetPxPyPzE(jets.at(0).v4().Px(), jets.at(0).v4().Py(), jets.at(0).v4().Pz(), jets.at(0).v4().E());
    jet2_v4.SetPxPyPzE(jets.at(1).v4().Px(), jets.at(1).v4().Py(), jets.at(1).v4().Pz(), jets.at(1).v4().E()); 
    jet3_v4.SetPxPyPzE(jets.at(2).v4().Px(), jets.at(2).v4().Py(), jets.at(2).v4().Pz(), jets.at(2).v4().E()); 
    jet4_v4.SetPxPyPzE(jets.at(3).v4().Px(), jets.at(3).v4().Py(), jets.at(3).v4().Pz(), jets.at(3).v4().E()); 
  
    // claculate distance to Lepton
    dR1 = uhh2::deltaR(jet1, lepton);
    dR2 = uhh2::deltaR(jets.at(1), lepton);
    dR3 = uhh2::deltaR(jets.at(2), lepton);
    dR4 = uhh2::deltaR(jets.at(3), lepton);
    if(dR1 < dR2 && dR1 < dR3 && dR1 < dR4){
      combinedjet1_v4 = jet2_v4 + jet3_v4 + jet4_v4;
      combinedjet2_v4 = jet1_v4;
    }
    if(dR2 < dR1 && dR2 < dR3 && dR2 < dR4){
      combinedjet1_v4 = jet1_v4 + jet3_v4 + jet4_v4;
      combinedjet2_v4 = jet2_v4;
    }    
    if(dR3 < dR1 && dR3 < dR2 && dR3 < dR4){
      combinedjet1_v4 = jet1_v4 + jet2_v4 + jet4_v4;
      combinedjet2_v4 = jet3_v4;
    }
    if(dR4 < dR1 && dR4 < dR2 && dR4 < dR3){
      combinedjet1_v4 = jet1_v4 + jet2_v4 + jet3_v4;
      combinedjet2_v4 = jet4_v4;
    }
  }

  std::vector<Jet> new_jets;
  Jet new_jet1, new_jet2;
  // TLorentzVector jet;
  new_jet1.set_pt(combinedjet1_v4.Pt());
  new_jet1.set_eta(combinedjet1_v4.Eta());
  new_jet1.set_phi(combinedjet1_v4.Phi());
  new_jet1.set_energy(combinedjet1_v4.E());
  new_jet2.set_pt(combinedjet2_v4.Pt());
  new_jet2.set_eta(combinedjet2_v4.Eta());
  new_jet2.set_phi(combinedjet2_v4.Phi());
  new_jet2.set_energy(combinedjet2_v4.E());

  new_jets.push_back(new_jet1);
  new_jets.push_back(new_jet2);
  event.set(h_newjets, new_jets);

  return true;
}

// ------------------------------------ merging XCone Reco Jets -------------------------------------------------------------------
MergeXConeReco::MergeXConeReco(uhh2::Context & ctx, const std::string & in_jet, const std::string & out_jet):
  h_injets(ctx.get_handle<std::vector<Jet>>(in_jet)),
  h_newjets(ctx.declare_event_output<std::vector<Jet>>(out_jet)) {}

bool MergeXConeReco::process(uhh2::Event & event){

  // get Reco Lepton
  Particle lepton;
  if(event.muons->size() > 0){
    lepton = event.muons->at(0);
  }
  else if(event.electrons->size() > 0){
    lepton = event.electrons->at(0);
  }

  // get distance to jets
  std::vector<Jet> jets = event.get(h_injets);
  TLorentzVector jet1_v4, jet2_v4, jet3_v4, jet4_v4, combinedjet1_v4, combinedjet2_v4;
  Jet jet1;
  jet1 = jets.at(0);
  float dR1, dR2, dR3, dR4;
  if(jets.size() > 3){
    jet1_v4.SetPxPyPzE(jets.at(0).v4().Px(), jets.at(0).v4().Py(), jets.at(0).v4().Pz(), jets.at(0).v4().E());
    jet2_v4.SetPxPyPzE(jets.at(1).v4().Px(), jets.at(1).v4().Py(), jets.at(1).v4().Pz(), jets.at(1).v4().E()); 
    jet3_v4.SetPxPyPzE(jets.at(2).v4().Px(), jets.at(2).v4().Py(), jets.at(2).v4().Pz(), jets.at(2).v4().E()); 
    jet4_v4.SetPxPyPzE(jets.at(3).v4().Px(), jets.at(3).v4().Py(), jets.at(3).v4().Pz(), jets.at(3).v4().E()); 
  
    // claculate distance to Lepton
    dR1 = uhh2::deltaR(jet1, lepton);
    dR2 = uhh2::deltaR(jets.at(1), lepton);
    dR3 = uhh2::deltaR(jets.at(2), lepton);
    dR4 = uhh2::deltaR(jets.at(3), lepton);
    if(dR1 < dR2 && dR1 < dR3 && dR1 < dR4){
      combinedjet1_v4 = jet2_v4 + jet3_v4 + jet4_v4;
      combinedjet2_v4 = jet1_v4;
    }
    if(dR2 < dR1 && dR2 < dR3 && dR2 < dR4){
      combinedjet1_v4 = jet1_v4 + jet3_v4 + jet4_v4;
      combinedjet2_v4 = jet2_v4;
    }    
    if(dR3 < dR1 && dR3 < dR2 && dR3 < dR4){
      combinedjet1_v4 = jet1_v4 + jet2_v4 + jet4_v4;
      combinedjet2_v4 = jet3_v4;
    }
    if(dR4 < dR1 && dR4 < dR2 && dR4 < dR3){
      combinedjet1_v4 = jet1_v4 + jet2_v4 + jet3_v4;
      combinedjet2_v4 = jet4_v4;
    }
  }

  std::vector<Jet> new_jets;
  Jet new_jet1, new_jet2;
  // TLorentzVector jet;
  new_jet1.set_pt(combinedjet1_v4.Pt());
  new_jet1.set_eta(combinedjet1_v4.Eta());
  new_jet1.set_phi(combinedjet1_v4.Phi());
  new_jet1.set_energy(combinedjet1_v4.E());
  new_jet2.set_pt(combinedjet2_v4.Pt());
  new_jet2.set_eta(combinedjet2_v4.Eta());
  new_jet2.set_phi(combinedjet2_v4.Phi());
  new_jet2.set_energy(combinedjet2_v4.E());

  new_jets.push_back(new_jet1);
  new_jets.push_back(new_jet2);
  event.set(h_newjets, new_jets);

  return true;
}

// ------------------------------------ merging XCone Gen Jets N=6  -------------------------------------------------------------------
MergeXConeN6Gen::MergeXConeN6Gen(uhh2::Context & ctx, const std::string & in_jet, const std::string & out_jet):
  h_ttbargen(ctx.get_handle<TTbarGen>("ttbargen")),
  h_injets(ctx.get_handle<std::vector<Jet>>(in_jet)),
  h_newjets(ctx.declare_event_output<std::vector<Jet>>(out_jet)) {}

bool MergeXConeN6Gen::process(uhh2::Event & event){

  // get Lepton from TTbar
  const auto & ttbargen = event.get(h_ttbargen);
  GenParticle lep1, lep2, lepton;
  if(ttbargen.IsTopHadronicDecay()){
    lep1 = ttbargen.WMinusdecay1();
    lep2 = ttbargen.WMinusdecay2();
  }
  else if(ttbargen.IsAntiTopHadronicDecay()){
    lep1 = ttbargen.Wdecay1();
    lep2 = ttbargen.Wdecay2();
  }
  if(abs(lep1.pdgId()) == 11 || abs(lep1.pdgId()) == 13){
    lepton = lep1;
  }
  else if(abs(lep2.pdgId()) == 11 || abs(lep2.pdgId()) == 13){
    lepton = lep2;
  }

  // get distance to jets
  std::vector<Jet> jets = event.get(h_injets);
  TLorentzVector jet1_v4, jet2_v4, jet3_v4, jet4_v4, jet5_v4, combinedjet1_v4, combinedjet2_v4, combinedjet3_v4;
  Jet jet1;
  jet1 = jets.at(0);
  float dR1, dR2, dR3, dR4, dR5;
  if(jets.size() > 4){
    jet1_v4.SetPxPyPzE(jets.at(0).v4().Px(), jets.at(0).v4().Py(), jets.at(0).v4().Pz(), jets.at(0).v4().E());
    jet2_v4.SetPxPyPzE(jets.at(1).v4().Px(), jets.at(1).v4().Py(), jets.at(1).v4().Pz(), jets.at(1).v4().E()); 
    jet3_v4.SetPxPyPzE(jets.at(2).v4().Px(), jets.at(2).v4().Py(), jets.at(2).v4().Pz(), jets.at(2).v4().E()); 
    jet4_v4.SetPxPyPzE(jets.at(3).v4().Px(), jets.at(3).v4().Py(), jets.at(3).v4().Pz(), jets.at(3).v4().E()); 
    jet5_v4.SetPxPyPzE(jets.at(4).v4().Px(), jets.at(4).v4().Py(), jets.at(4).v4().Pz(), jets.at(4).v4().E()); 

    // claculate distance to Lepton
    dR1 = uhh2::deltaR(jet1, lepton);
    dR2 = uhh2::deltaR(jets.at(1), lepton);
    dR3 = uhh2::deltaR(jets.at(2), lepton);
    dR4 = uhh2::deltaR(jets.at(3), lepton);
    dR5 = uhh2::deltaR(jets.at(4), lepton);

    // calculate distance between jets;
    float dR12, dR13, dR14, dR15, dR23, dR24, dR25, dR34, dR35, dR45;
    dR12 = uhh2::deltaR(jets.at(0), jets.at(1));
    dR13 = uhh2::deltaR(jets.at(0), jets.at(2));
    dR14 = uhh2::deltaR(jets.at(0), jets.at(3));
    dR15 = uhh2::deltaR(jets.at(0), jets.at(4));
    dR23 = uhh2::deltaR(jets.at(1), jets.at(2));
    dR24 = uhh2::deltaR(jets.at(1), jets.at(3));
    dR25 = uhh2::deltaR(jets.at(1), jets.at(4));
    dR34 = uhh2::deltaR(jets.at(2), jets.at(3));
    dR35 = uhh2::deltaR(jets.at(2), jets.at(4));
    dR45 = uhh2::deltaR(jets.at(3), jets.at(4));


    // first get nearest jet to lep -> combined jet 2 / if one jet has big distance to all other 3 jets (3 dRs of this jet > other dRs) -> combine other 3 jets / else combine 4 jets
    if(dR1 < dR2 && dR1 < dR3 && dR1 < dR4 && dR1 < dR5){
      combinedjet2_v4 = jet1_v4;
      if((dR23 > dR34 && dR23 > dR35 && dR23 > dR45) && (dR24 > dR34 && dR24 > dR35 && dR24 > dR45) && (dR25 > dR34 && dR25 > dR35 && dR25 > dR45)){
	combinedjet1_v4 = jet3_v4 + jet4_v4 + jet5_v4;
	combinedjet3_v4 = jet2_v4;
      }
      else if((dR23 > dR24 && dR23 > dR25 && dR23 > dR45) && (dR34 > dR24 && dR34 > dR25 && dR34 > dR45) && (dR35 > dR24 && dR35 > dR25 && dR35 > dR45)){
	combinedjet1_v4 = jet2_v4 + jet4_v4 + jet5_v4;
	combinedjet3_v4 = jet3_v4;
      }	
      else if((dR24 > dR23 && dR24 > dR25 && dR24 > dR35) && (dR34 > dR23 && dR34 > dR25 && dR34 > dR35) && (dR45 > dR23 && dR45 > dR25 && dR45 > dR35)){
	combinedjet1_v4 = jet2_v4 + jet3_v4 + jet5_v4;
	combinedjet3_v4 = jet4_v4;
      }
      else if((dR25 > dR23 && dR25 > dR24 && dR25 > dR34) && (dR35 > dR23 && dR35 > dR24 && dR35 > dR34) && (dR45 > dR23 && dR45 > dR24 && dR45 > dR34)){
	combinedjet1_v4 = jet2_v4 + jet3_v4 + jet4_v4;
	combinedjet3_v4 = jet5_v4;
      }
      else{
	combinedjet1_v4 = jet2_v4 + jet3_v4 + jet4_v4 + jet5_v4;
      }
    }

    if(dR2 < dR1 && dR2 < dR3 && dR2 < dR4 && dR2 < dR5){
      combinedjet2_v4 = jet2_v4;
      if((dR13 > dR34 && dR13 > dR35 && dR13 > dR45) && (dR14 > dR34 && dR14 > dR35 && dR14 > dR45) && (dR15 > dR34 && dR15 > dR35 && dR15 > dR45)){
	combinedjet1_v4 = jet3_v4 + jet4_v4 + jet5_v4;
	combinedjet3_v4 = jet1_v4;
      }
      else if((dR13 > dR14 && dR13 > dR15 && dR13 > dR45) && (dR34 > dR14 && dR34 > dR15 && dR34 > dR45) && (dR35 > dR14 && dR35 > dR15 && dR35 > dR45)){
	combinedjet1_v4 = jet1_v4 + jet4_v4 + jet5_v4;
	combinedjet3_v4 = jet3_v4;
      }	
      else if((dR14 > dR13 && dR14 > dR15 && dR14 > dR35) && (dR34 > dR13 && dR34 > dR15 && dR34 > dR35) && (dR45 > dR13 && dR45 > dR15 && dR45 > dR35)){
	combinedjet1_v4 = jet1_v4 + jet3_v4 + jet5_v4;
	combinedjet3_v4 = jet4_v4;
      }
      else if((dR15 > dR13 && dR15 > dR14 && dR15 > dR34) && (dR35 > dR13 && dR35 > dR14 && dR35 > dR34) && (dR45 > dR13 && dR45 > dR14 && dR45 > dR34)){
	combinedjet1_v4 = jet1_v4 + jet3_v4 + jet4_v4;
	combinedjet3_v4 = jet5_v4;
      }
      else{
	combinedjet1_v4 = jet1_v4 + jet3_v4 + jet4_v4 + jet5_v4;
      }
    }    
    if(dR3 < dR1 && dR3 < dR2 && dR3 < dR4 && dR3 < dR5){
      combinedjet2_v4 = jet3_v4;
      if((dR12 > dR24 && dR12 > dR25 && dR12 > dR45) && (dR14 > dR24 && dR14 > dR25 && dR14 > dR45) && (dR15 > dR24 && dR15 > dR25 && dR15 > dR45)){
	combinedjet1_v4 = jet2_v4 + jet4_v4 + jet5_v4;
	combinedjet3_v4 = jet1_v4;
      }
      else if((dR12 > dR14 && dR12 > dR15 && dR12 > dR45) && (dR24 > dR14 && dR24 > dR15 && dR24 > dR45) && (dR25 > dR14 && dR25 > dR15 && dR25 > dR45)){
	combinedjet1_v4 = jet1_v4 + jet4_v4 + jet5_v4;
	combinedjet3_v4 = jet2_v4;
      }	
      else if((dR14 > dR12 && dR14 > dR15 && dR14 > dR25) && (dR24 > dR12 && dR24 > dR15 && dR24 > dR25) && (dR45 > dR12 && dR45 > dR15 && dR45 > dR25)){
	combinedjet1_v4 = jet1_v4 + jet2_v4 + jet5_v4;
	combinedjet3_v4 = jet4_v4;
      }
      else if((dR15 > dR12 && dR15 > dR14 && dR15 > dR24) && (dR25 > dR12 && dR25 > dR14 && dR25 > dR24) && (dR45 > dR12 && dR45 > dR14 && dR45 > dR24)){
	combinedjet1_v4 = jet1_v4 + jet2_v4 + jet4_v4;
	combinedjet3_v4 = jet5_v4;
      }
      else{
	combinedjet1_v4 = jet1_v4 + jet2_v4 + jet4_v4 + jet5_v4;
      }
    }
    if(dR4 < dR1 && dR4 < dR2 && dR4 < dR3 && dR4 < dR5){
      combinedjet2_v4 = jet4_v4;
      if((dR12 > dR23 && dR12 > dR25 && dR12 > dR35) && (dR13 > dR23 && dR13 > dR25 && dR13 > dR35) && (dR15 > dR23 && dR15 > dR25 && dR15 > dR35)){
	combinedjet1_v4 = jet2_v4 + jet3_v4 + jet5_v4;
	combinedjet3_v4 = jet1_v4;
      }
      else if((dR12 > dR13 && dR12 > dR15 && dR12 > dR35) && (dR23 > dR13 && dR23 > dR15 && dR23 > dR35) && (dR25 > dR13 && dR25 > dR15 && dR25 > dR35)){
	combinedjet1_v4 = jet1_v4 + jet3_v4 + jet5_v4;
	combinedjet3_v4 = jet2_v4;
      }	
      else if((dR13 > dR12 && dR13 > dR15 && dR13 > dR25) && (dR23 > dR12 && dR23 > dR15 && dR23 > dR25) && (dR35 > dR12 && dR35 > dR15 && dR35 > dR25)){
	combinedjet1_v4 = jet1_v4 + jet2_v4 + jet5_v4;
	combinedjet3_v4 = jet3_v4;
      }
      else if((dR15 > dR12 && dR15 > dR13 && dR15 > dR23) && (dR25 > dR12 && dR25 > dR13 && dR25 > dR23) && (dR35 > dR12 && dR35 > dR13 && dR35 > dR23)){
	combinedjet1_v4 = jet1_v4 + jet2_v4 + jet3_v4;
	combinedjet3_v4 = jet5_v4;
      }
      else{
	combinedjet1_v4 = jet1_v4 + jet2_v4 + jet3_v4 + jet5_v4;
      }
    }
    if(dR5 < dR1 && dR5 < dR2 && dR5 < dR3 && dR5 < dR4){
      combinedjet2_v4 = jet5_v4;
      if((dR12 > dR23 && dR12 > dR24 && dR12 > dR34) && (dR13 > dR23 && dR13 > dR24 && dR13 > dR34) && (dR14 > dR23 && dR14 > dR24 && dR14 > dR34)){
	combinedjet1_v4 = jet2_v4 + jet3_v4 + jet4_v4;
	combinedjet3_v4 = jet1_v4;
      }
      else if((dR12 > dR13 && dR12 > dR14 && dR12 > dR34) && (dR23 > dR13 && dR23 > dR14 && dR23 > dR34) && (dR24 > dR13 && dR24 > dR14 && dR24 > dR34)){
	combinedjet1_v4 = jet1_v4 + jet3_v4 + jet4_v4;
	combinedjet3_v4 = jet2_v4;
      }	
      else if((dR13 > dR12 && dR13 > dR14 && dR13 > dR24) && (dR23 > dR12 && dR23 > dR14 && dR23 > dR24) && (dR34 > dR12 && dR34 > dR14 && dR34 > dR24)){
	combinedjet1_v4 = jet1_v4 + jet2_v4 + jet4_v4;
	combinedjet3_v4 = jet3_v4;
      }
      else if((dR14 > dR12 && dR14 > dR13 && dR14 > dR23) && (dR24 > dR12 && dR24 > dR13 && dR24 > dR23) && (dR34 > dR12 && dR34 > dR13 && dR34 > dR23)){
	combinedjet1_v4 = jet1_v4 + jet2_v4 + jet3_v4;
	combinedjet3_v4 = jet4_v4;
      }
     else{
	combinedjet1_v4 = jet1_v4 + jet2_v4 + jet3_v4 + jet4_v4;
      }
    }
  }

  std::vector<Jet> new_jets;
  Jet new_jet1, new_jet2, new_jet3;
  // TLorentzVector jet;
  new_jet1.set_pt(combinedjet1_v4.Pt());
  new_jet1.set_eta(combinedjet1_v4.Eta());
  new_jet1.set_phi(combinedjet1_v4.Phi());
  new_jet1.set_energy(combinedjet1_v4.E());
  new_jet2.set_pt(combinedjet2_v4.Pt());
  new_jet2.set_eta(combinedjet2_v4.Eta());
  new_jet2.set_phi(combinedjet2_v4.Phi());
  new_jet2.set_energy(combinedjet2_v4.E());
  new_jet3.set_pt(combinedjet3_v4.Pt());
  new_jet3.set_eta(combinedjet3_v4.Eta());
  new_jet3.set_phi(combinedjet3_v4.Phi());
  new_jet3.set_energy(combinedjet3_v4.E());
  new_jets.push_back(new_jet1);
  new_jets.push_back(new_jet2);
  new_jets.push_back(new_jet3);
  event.set(h_newjets, new_jets);

  return true;
}


// ------------------------------------ XCone Gen Jets -----------------------------------------------------------------------------
GenXCONE23JetProducer::GenXCONE23JetProducer(uhh2::Context & ctx, const std::string & name_fat, const std::string & name_sub, double ptmin, double ptmin_sub1, double ptmin_sub2):
  h_xcone23seedjets(ctx.declare_event_output<std::vector<Jet>>("xcone23_gen_seedjet")),
  h_xcone23fatjets(ctx.declare_event_output<std::vector<Jet>>(name_fat)),
  h_xcone23subjets(ctx.declare_event_output<std::vector<Jet>>(name_sub)),
  h_particle_fatjet(ctx.declare_event_output<vector<int>>("particle_fatjet_list")),
  h_particle_subjets1(ctx.declare_event_output<vector<int>>("particle_subjets1_list")),
  h_particle_subjets2(ctx.declare_event_output<vector<int>>("particle_subjets2_list")),
  ptmin_(ptmin),
  ptmin_sub1_(ptmin_sub1),
  ptmin_sub2_(ptmin_sub2){}

bool GenXCONE23JetProducer::process(uhh2::Event & event){


  std::vector<GenParticle>* genparts = event.genparticles;
  JetCluster* jetc0 = new JetCluster();
  JetCluster* jetc1 = new JetCluster();
  JetCluster* jetc2 = new JetCluster();
  std::vector<fastjet::PseudoJet> gen1, gen2, gen0;
  gen0 = jetc0->get_xcone23_jets(event, h_particle_fatjet, genparts, ptmin_, ptmin_sub1_, 0);
  gen1 = jetc1->get_xcone23_jets(event, h_particle_subjets1, genparts, ptmin_, ptmin_sub1_, 1);
  gen2 = jetc2->get_xcone23_jets(event, h_particle_subjets2, genparts, ptmin_, ptmin_sub2_, 2);
  std::vector<Jet> subjets1, subjets2, combined_jets, subjets, seedjets;
  seedjets = jetc0->convert_pseudojet_to_jet(gen0);
  subjets1 = jetc1->convert_pseudojet_to_jet(gen1);
  subjets2 = jetc2->convert_pseudojet_to_jet(gen2);
  TLorentzVector jet1_v4, jet2_v4, jet3_v4, jet4_v4, jet5_v4, combinedjet1_v4, combinedjet2_v4;
  if(subjets1.size() > 2){
    jet1_v4.SetPxPyPzE(subjets1.at(0).v4().Px(), subjets1.at(0).v4().Py(), subjets1.at(0).v4().Pz(), subjets1.at(0).v4().E());
    jet2_v4.SetPxPyPzE(subjets1.at(1).v4().Px(), subjets1.at(1).v4().Py(), subjets1.at(1).v4().Pz(), subjets1.at(1).v4().E()); 
    jet3_v4.SetPxPyPzE(subjets1.at(2).v4().Px(), subjets1.at(2).v4().Py(), subjets1.at(2).v4().Pz(), subjets1.at(2).v4().E()); 
  }
  if(subjets2.size() > 1){
    jet4_v4.SetPxPyPzE(subjets2.at(0).v4().Px(), subjets2.at(0).v4().Py(), subjets2.at(0).v4().Pz(), subjets2.at(0).v4().E()); 
    jet5_v4.SetPxPyPzE(subjets2.at(1).v4().Px(), subjets2.at(1).v4().Py(), subjets2.at(1).v4().Pz(), subjets2.at(1).v4().E()); 
  }
  Jet seed1, seed2, new_jet1, new_jet2, subjet1, subjet2, subjet3, subjet4, subjet5;
  combinedjet1_v4 = jet1_v4 + jet2_v4 + jet3_v4;
  combinedjet2_v4 = jet4_v4 + jet5_v4;


  new_jet1.set_pt(combinedjet1_v4.Pt());
  new_jet1.set_eta(combinedjet1_v4.Eta());
  new_jet1.set_phi(combinedjet1_v4.Phi());
  new_jet1.set_energy(combinedjet1_v4.E());

  new_jet2.set_pt(combinedjet2_v4.Pt());
  new_jet2.set_eta(combinedjet2_v4.Eta());
  new_jet2.set_phi(combinedjet2_v4.Phi());
  new_jet2.set_energy(combinedjet2_v4.E());

  subjet1.set_pt(jet1_v4.Pt());
  subjet1.set_eta(jet1_v4.Eta());
  subjet1.set_phi(jet1_v4.Phi());
  subjet1.set_energy(jet1_v4.E());

  subjet2.set_pt(jet2_v4.Pt());
  subjet2.set_eta(jet2_v4.Eta());
  subjet2.set_phi(jet2_v4.Phi());
  subjet2.set_energy(jet2_v4.E());

  subjet3.set_pt(jet3_v4.Pt());
  subjet3.set_eta(jet3_v4.Eta());
  subjet3.set_phi(jet3_v4.Phi());
  subjet3.set_energy(jet3_v4.E());

  subjet4.set_pt(jet4_v4.Pt());
  subjet4.set_eta(jet4_v4.Eta());
  subjet4.set_phi(jet4_v4.Phi());
  subjet4.set_energy(jet4_v4.E());

  subjet5.set_pt(jet5_v4.Pt());
  subjet5.set_eta(jet5_v4.Eta());
  subjet5.set_phi(jet5_v4.Phi());
  subjet5.set_energy(jet5_v4.E());

  combined_jets.push_back(new_jet1);
  combined_jets.push_back(new_jet2);
  subjets.push_back(subjet1);
  subjets.push_back(subjet2);
  subjets.push_back(subjet3);
  subjets.push_back(subjet4);
  subjets.push_back(subjet5);
  event.set(h_xcone23fatjets, combined_jets);
  event.set(h_xcone23subjets, subjets);
  event.set(h_xcone23seedjets, seedjets);
  delete jetc0;
  delete jetc1;
  delete jetc2;
  return true;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------
