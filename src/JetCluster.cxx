#include "UHH2/MTopJet/include/JetCluster.h"
#include <math.h>
#include <vector>
#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/Utils.h"
#include "UHH2/MTopJet/include/MTopJetGenHists.h"
#include "UHH2/common/include/TTbarGen.h"

#include <math.h>
#include <vector>


#include <iostream> 

// Check if Gen particle is stable
bool JetCluster::IsStable(GenParticle* p)
{
	int st = p->status();
	if (st==1) return true;
	else return false;
}

// Check if Gen particle is Electron/Muon
bool JetCluster::IsLepton(GenParticle* p)
{
  int id = abs(p->pdgId());
  if (id==11 || id==13) return true;
  else return false;
}

// convert Gen Particles to PseudoJet
fastjet::PseudoJet JetCluster::convert_particle(GenParticle* genparticle){
  TLorentzVector particle;
  particle.SetPtEtaPhiE(genparticle->pt(),genparticle->eta(),genparticle->phi(),genparticle->energy());
  fastjet::PseudoJet gen_particle(particle.Px(),particle.Py(),particle.Pz(),particle.E());
  return gen_particle;

}

// Get clustered Gen Jet from Gen Particles 
std::vector<fastjet::PseudoJet> JetCluster::get_genjets(std::vector<GenParticle>* genparts, enum  JetCluster::E_algorithm algorithm, double jet_radius, double ptmin){

  for (unsigned int i=0; i<(genparts->size()); ++i){
      GenParticle* part = &(genparts->at(i));
      if(!IsLepton(part)){
	if(IsStable(part)){
	  particle_in.push_back(convert_particle(part));
	}
      }
  }

  if(algorithm==e_ca) jetdef= new fastjet::JetDefinition(fastjet::cambridge_algorithm,jet_radius);
  if(algorithm==e_vrca) jetdef= new fastjet::JetDefinition(fastjet::cambridge_algorithm,jet_radius);
  if(algorithm==e_akt)jetdef= new fastjet::JetDefinition(fastjet::antikt_algorithm,jet_radius);
  if(algorithm==e_vrakt)jetdef= new fastjet::JetDefinition(fastjet::antikt_algorithm,jet_radius);

  clust_seq=new fastjet::ClusterSequence(particle_in, *jetdef);

  new_jets = sorted_by_pt(clust_seq->inclusive_jets(ptmin));

  delete jetdef;
  return new_jets;

}

// Get clustered HOTVR Jets from Gen Particles
std::vector<fastjet::PseudoJet> JetCluster::get_hotvr_jets(std::vector<GenParticle>* genparts,enum  JetCluster::E_algorithm algorithm, double ptmin, double rho, double min_r, double max_r, double mu, double theta, double pt_cut){ 
   for (unsigned int i=0; i<(genparts->size()); ++i){
      GenParticle* part = &(genparts->at(i));
      if(!IsLepton(part)){
	if(IsStable(part)){
	  particle_in.push_back(convert_particle(part));
	}
      }
  }

  HOTVR::ClusterType clustertype;
  if(algorithm==e_akt) clustertype=HOTVR::ClusterType::AKTLIKE;
  if(algorithm==e_ca) clustertype=HOTVR::ClusterType::CALIKE;
  if(algorithm==e_kt) clustertype=HOTVR::ClusterType::KTLIKE;
  HOTVR plugin_hotvr(mu, theta,min_r, max_r,rho,pt_cut, clustertype);//call HOTVR algorithm
  fastjet::JetDefinition jet_def(&plugin_hotvr);
  _clust_seq2=new fastjet::ClusterSequence(particle_ih, jet_def);
    
    
  std::vector<fastjet::PseudoJet> hotvr_jets,rejected_jets,soft_jets ; //vector of hotvr_jets, jets that were rejcted durning the clustering procedure and soft jets
	
  //get vector from the plugin
  hotvr_jets=plugin_hotvr.get_jets();
  rejected_jets=plugin_hotvr.get_rejected_cluster();
  soft_jets=plugin_hotvr.get_soft_cluster();
  return hotvr_jets;
}


// convert PF Particles to PseudoJet
fastjet::PseudoJet JetCluster::convert_recoparticle(PFParticle* pfparticle){
  TLorentzVector particle;
  particle.SetPtEtaPhiE(pfparticle->pt(),pfparticle->eta(),pfparticle->phi(),pfparticle->energy());
  fastjet::PseudoJet pf_particle(particle.Px(),particle.Py(),particle.Pz(),particle.E());
  return pf_particle;
}

// Get clustered Reco Jet from PF Particles 
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

