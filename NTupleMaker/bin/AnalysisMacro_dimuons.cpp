#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <sstream>

#include "TFile.h" 
#include "TH1.h" 
#include "TH2.h"
#include "TGraph.h"
#include "TTree.h"
#include "TROOT.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TRFIOFile.h"
#include "TH1D.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TLorentzVector.h"
#include "TPaveText.h"
#include "TRandom.h"
#include "TRandom3.h"
#include "TGraphAsymmErrors.h"

#include "DesyTauAnalyses/NTupleMaker/interface/Config.h"
#include "DesyTauAnalyses/NTupleMaker/interface/AC1B.h"
#include "DesyTauAnalyses/NTupleMaker/interface/json.h"
#include "DesyTauAnalyses/NTupleMaker/interface/PileUp.h"
#include "DesyTauAnalyses/NTupleMaker/interface/Jets.h"
#include "DesyTauAnalyses/NTupleMaker/interface/functions.h"
#include "HTT-utilities/LepEffInterface/interface/ScaleFactor.h"
#include "HTT-utilities/RecoilCorrections/interface/RecoilCorrector.h"
#include "HTT-utilities/RecoilCorrections/interface/MEtSys.h"

#include "CondFormats/BTauObjects/interface/BTagCalibration.h"
#include "CondTools/BTau/interface/BTagCalibrationReader.h"

float topPtWeight(float pt1,
		  float pt2) {

  float a = 0.156;    // Run1 a parameter
  float b = -0.00137;  // Run1 b parameter

  if (pt1>400) pt1 = 400;
  if (pt2>400) pt2 = 400;

  float w1 = TMath::Exp(a+b*pt1);
  float w2 = TMath::Exp(a+b*pt2);

  return TMath::Sqrt(w1*w2);

}

/*

float topPtWeight(float pt1,
		  float pt2) {
    
  if (pt1>400) pt1 = 400;
  if (pt2>400) pt2 = 400;
    
  float a = 0.0615;    // Run2 a parameter
  float b = -0.0005;  // Run2 b parameter
    
  float w1 = TMath::Exp(a+b*pt1);
  float w2 = TMath::Exp(a+b*pt2);
    
  return TMath::Sqrt(w1*w2);  
}

*/

float nJetsWeight(int nJets) {

  float weight = 1;
  if (nJets==0)
    weight = 1.02;
  else if (nJets==1)
    weight = 0.95;
  else 
    weight = 0.93;

  return weight;

}

void computeRecoil(float metx, float mety,
		   float unitX,float unitY,
		   float perpUnitX, float perpUnitY,
		   float dimuonPt,
		   float & recoilParal,
		   float & recoilPerp,
		   float & responseHad) {

  recoilParal = metx*unitX + mety*unitY;
  recoilPerp = metx*perpUnitX + mety*perpUnitY;
  responseHad = 1 + recoilParal/dimuonPt;
}

bool isICHEPmed(unsigned int Index, AC1B analysisTree) {
        bool goodGlob = 
	  analysisTree.muon_isGlobal[Index] && 
	  analysisTree.muon_normChi2[Index] < 3 && 
	  analysisTree.muon_combQ_chi2LocalPosition[Index] < 12 && 
	  analysisTree.muon_combQ_trkKink[Index] < 20;
        bool isICHEPmedium  = 
	  analysisTree.muon_isLoose[Index] &&
	  analysisTree.muon_validFraction[Index] >0.49 &&
	  analysisTree.muon_segmentComp[Index] > (goodGlob ? 0.303 : 0.451);
        return isICHEPmedium;
}


int main(int argc, char * argv[]) {

  // first argument - config file 
  // second argument - filelist

  using namespace std;

  // **** configuration
  Config cfg(argv[1]);

  const bool isData = cfg.get<bool>("IsData");
  const bool applyGoodRunSelection = cfg.get<bool>("ApplyGoodRunSelection");

  // pile up reweighting
  const bool applyPUreweighting_vertices = cfg.get<bool>("ApplyPUreweighting_vertices");
  const bool applyPUreweighting_official = cfg.get<bool>("ApplyPUreweighting_official");

  //const bool applyPUreweighting = cfg.get<bool>("ApplyPUreweighting");
  const bool applyLeptonSF = cfg.get<bool>("ApplyLeptonSF");
  const bool applyRecoilCorrections = cfg.get<bool>("ApplyRecoilCorrections");
  const bool applyRecoilOnGenerator = cfg.get<bool>("ApplyRecoilOnGenerator");
  const bool applySimpleRecoilCorrections = cfg.get<bool>("ApplySimpleRecoilCorrections");
  const bool applyTopPtReweighting = cfg.get<bool>("ApplyTopPtReweighting");
  const bool applyNJetReweighting = cfg.get<bool>("ApplyNJetReweighting");
  const bool applyZMassPtReweighting = cfg.get<bool>("ApplyZMassPtReweighting");
  const bool interpolateZMassPtWeight = cfg.get<bool>("InterpolateZMassPtWeight");

  // kinematic cuts on muons
  const float ptMuonLowCut   = cfg.get<float>("ptMuonLowCut");
  const float ptMuonHighCut  = cfg.get<float>("ptMuonHighCut");
  const float etaMuonHighCut = cfg.get<float>("etaMuonHighCut");
  const float etaMuonLowCut = cfg.get<float>("etaMuonLowCut");
  const float dxyMuonCut     = cfg.get<float>("dxyMuonCut");
  const float dzMuonCut      = cfg.get<float>("dzMuonCut");
  const float dxyMuonLooseCut     = cfg.get<float>("dxyMuonLooseCut");
  const float dzMuonLooseCut      = cfg.get<float>("dzMuonLooseCut");
  const float isoMuonCut     = cfg.get<float>("isoMuonCut");
  const float isoMuonProbeCut = cfg.get<float>("isoMuonProbeCut");
  const bool  applyTauTauSelection = cfg.get<bool>("ApplyTauTauSelection");
  const bool  selectZToTauTauMuMu = cfg.get<bool>("SelectZToTauTauMuMu");

  // topological cuts
  const float dRleptonsCut   = cfg.get<float>("dRleptonsCut");
  const float dPhileptonsCut = cfg.get<float>("dPhileptonsCut");
  const float DRTrigMatch    = cfg.get<float>("DRTrigMatch"); 
  const bool oppositeSign    = cfg.get<bool>("OppositeSign");
  const float dimuonMassCut = cfg.get<float>("DimuonMassCut");
  const bool isoDR03         = cfg.get<bool>("IsoDR03");

  // trigger
  const bool applyTrigger = cfg.get<bool>("ApplyTrigger");
  const string muonTriggerName  = cfg.get<string>("MuonTriggerName");
  const string muonFilterName   = cfg.get<string>("MuonFilterName");
  const string muon17FilterName = cfg.get<string>("Muon17FilterName"); 
  const string muon8FilterName1 = cfg.get<string>("Muon8FilterName1"); 
  const string muon8FilterName2 = cfg.get<string>("Muon8FilterName2"); 
  const string singleMuonFilterName = cfg.get<string>("SingleMuonFilterName");
  const float singleMuonTriggerPtCut = cfg.get<float>("SingleMuonTriggerPtCut");
  const float singleMuonTriggerEtaCut = cfg.get<float>("SingleMuonTriggerEtaCut");
  

  TString MuonTriggerName(muonTriggerName);
  TString MuonFilterName(muonFilterName);

  TString Muon17FilterName(muon17FilterName);
  TString Muon8FilterName1(muon8FilterName1);
  TString Muon8FilterName2(muon8FilterName2);
  TString SingleMuonFilterName(singleMuonFilterName);

  // vertex cuts
  const float ndofVertexCut  = cfg.get<float>("NdofVertexCut");   
  const float zVertexCut     = cfg.get<float>("ZVertexCut");
  const float dVertexCut     = cfg.get<float>("DVertexCut");

  // jet related cuts
  const float jetEtaCut      = cfg.get<float>("JetEtaCut");
  const float jetEtaTrkCut   = cfg.get<float>("JetEtaTrkCut");
  const float jetPtHighCut   = cfg.get<float>("JetPtHighCut");
  const float jetPtLowCut    = cfg.get<float>("JetPtLowCut");
  const float dRJetLeptonCut = cfg.get<float>("dRJetLeptonCut");

  // vertex distributions filenames and histname
  const string vertDataFileName = cfg.get<string>("VertexDataFileName");
  const string vertMcFileName   = cfg.get<string>("VertexMcFileName");
  const string vertHistName     = cfg.get<string>("VertexHistName");

  const string MuonIdIsoFile = cfg.get<string>("MuonIdIsoEff");
  const string MuonTrigFile  = cfg.get<string>("MuonTrigEff"); 
  const string trackingSFFile = cfg.get<string>("TrackingSFFile");

  const string recoilFileName   = cfg.get<string>("RecoilFileName");
  TString RecoilFileName(recoilFileName);

  const string recoilPuppiFileName   = cfg.get<string>("RecoilPuppiFileName");
  TString RecoilPuppiFileName(recoilPuppiFileName);

  const string recoilMvaFileName   = cfg.get<string>("RecoilMvaFileName");
  TString RecoilMvaFileName(recoilMvaFileName);

  // systematics
  const string metsysFileName   = cfg.get<string>("MetSysFileName");
  TString MetSysFileName(metsysFileName);

  const string metsysPuppiFileName   = cfg.get<string>("MetSysPuppiFileName");
  TString MetSysPuppiFileName(metsysPuppiFileName);

  const string metsysMvaFileName   = cfg.get<string>("MetSysMvaFileName");
  TString MetSysMvaFileName(metsysMvaFileName);

  const string zMassPtWeightsFileName   = cfg.get<string>("ZMassPtWeightsFileName");
  TString ZMassPtWeightsFileName(zMassPtWeightsFileName);

  const string zMassPtWeightsHistName   = cfg.get<string>("ZMassPtWeightsHistName");
  TString ZMassPtWeightsHistName(zMassPtWeightsHistName);

  const string jsonFile = cfg.get<string>("jsonFile");
  const int applyJES = cfg.get<int>("applyJES");
  const float metScale = cfg.get<float>("MetScale");
  const float metResolution = cfg.get<float>("MetResolution");
  const float muonMomScale = cfg.get<float>("MuonMomentumScale");
  const int bkgdType =  cfg.get<float>("BkgdType");

  const string pileUpDataFile = cfg.get<string>("PileUpDataFileName");
  const string pileUpMCFile = cfg.get<string>("PileUpMCFileName");

  TString PileUpDataFile(pileUpDataFile);
  TString PileUpMCFile(pileUpMCFile);

  // **** end of configuration

  string cmsswBase = (getenv ("CMSSW_BASE"));
  string fullPathToJsonFile = cmsswBase + "/src/DesyTauAnalyses/NTupleMaker/test/json/" + jsonFile;

  // Run-lumi selector
  std::vector<Period> periods;  
  if (isData) { // read the good runs 
    std::fstream inputFileStream(fullPathToJsonFile.c_str(), std::ios::in);
    if (inputFileStream.fail() ) {
      std::cout << "Error: cannot find json file " << fullPathToJsonFile << std::endl;
      std::cout << "please check" << std::endl;
      std::cout << "quitting program" << std::endl;
      exit(-1);
    }
    
    for(std::string s; std::getline(inputFileStream, s); ) {
      periods.push_back(Period());
      std::stringstream ss(s);
      ss >> periods.back();
    }
  }


  // file name and tree name
  std::string rootFileName(argv[2]);
  std::ifstream fileList(argv[2]);
  std::ifstream fileList0(argv[2]);
  std::string ntupleName("makeroottree/AC1B");
  std::string initNtupleName("initroottree/AC1B");

  TString TStrName(rootFileName);
  std::cout <<TStrName <<std::endl;  

  TH1::SetDefaultSumw2(true);

  TFile * file = new TFile(TStrName+TString(".root"),"recreate");
  file->cd("");

  int eventNumber;
  int runNumber;
  bool IsoMu17;

  float ptTag;
  float etaTag;
  float phiTag;
  float dxyTag;
  float dzTag;
  float chargeTag;
  float isoTag;
  bool isMediumTag;

  float ptProbe;
  float etaProbe;
  float phiProbe;
  float dxyProbe;
  float dzProbe;
  float chargeProbe;
  float isoProbe;
  bool isMediumProbe;

  float massTagProbe;

  TTree * events = new TTree("events","events");
  events->Branch("eventNumber",&eventNumber,"eventNumber/I");
  events->Branch("runNumber",&runNumber,"runNumber/I");
  events->Branch("IsoMu17",&IsoMu17,"IsoMu17/O");
  events->Branch("ptTag",&ptTag,"ptTag/F");
  events->Branch("etaTag",&etaTag,"etaTag/F");
  events->Branch("phiTag",&phiTag,"phiTag/F");
  events->Branch("dxyTag",&dxyTag,"dxyTag/F");
  events->Branch("dzTag",&dzTag,"dzTag/F");
  events->Branch("chargeTag",&chargeTag,"chargeTag/F");
  events->Branch("isoTag",&isoTag,"isoTag/F");
  events->Branch("isMediumTag",&isMediumTag,"isMediumTag/O");
  events->Branch("ptProbe",&ptProbe,"ptProbe/F");
  events->Branch("etaProbe",&etaProbe,"etaProbe/F");
  events->Branch("phiProbe",&phiProbe,"phiProbe/F");
  events->Branch("dxyProbe",&dxyProbe,"dxyProbe/F");
  events->Branch("dzProbe",&dzProbe,"dzProbe/F");
  events->Branch("chargeProbe",&chargeProbe,"chargeProbe/F");
  events->Branch("isoProbe",&isoProbe,"isoProbe/F");
  events->Branch("isMediumProbe",&isMediumProbe,"isMediumProbe/O");
  events->Branch("massTagProbe",&massTagProbe,"massTagProbe/F");


  TH1D * inputEventsH = new TH1D("inputEventsH","",1,-0.5,0.5);
  TH1D * histWeightsH = new TH1D("histWeightsH","",1,-0.5,0.5);
  TH1D * histWeightsSkimmedH = new TH1D("histWeightsSkimmedH","",1,-0.5,0.5);

  TH1D * massZH = new TH1D("massZH","",1000,0,1000);
  TH1D * ptZH = new TH1D("ptZH","",1000,0,1000);
  TH2D * massPtZH = new TH2D("massPtZH","",100,0,1000,100,0,1000);

  // Histograms after final selection
  TH1D * ptLeadingMuSelH = new TH1D("ptLeadingMuSelH","",100,0,200);
  TH1D * ptTrailingMuSelH = new TH1D("ptTrailingMuSelH","",100,0,200);
  TH1D * etaLeadingMuSelH = new TH1D("etaLeadingMuSelH","",50,-2.5,2.5);
  TH1D * etaTrailingMuSelH = new TH1D("etaTrailingMuSelH","",50,-2.5,2.5);
  TH1D * massSelH = new TH1D("massSelH","",200,0,200);
  TH1D * massExtendedSelH =  new TH1D("massExtendedSelH","",500,0,5000);
  

  TH2D * dimuonMassPtH = new TH2D("dimuonMassPtH","",100,0,1000,100,0,1000);

  TH1D * dimuonPtSelH = new TH1D("dimuonPtSelH","",100,0,1000);
  TH1D * dimuonEtaSelH = new TH1D("dimuonEtaSelH","",120,-6,6);
  TH1D * metSelH  = new TH1D("metSelH","",200,0,400);
  TH1D * puppimetSelH = new TH1D("puppimetSelH","",200,0,400);
  TH1D * mvametSelH = new TH1D("mvametSelH","",200,0,400);

  TH1D * metZSelH  = new TH1D("metZSelH","",200,0,400);
  TH1D * puppimetZSelH = new TH1D("puppimetZSelH","",200,0,400);
  TH1D * mvametZSelH = new TH1D("mvametZSelH","",200,0,400);

  TH1D * metTopSelH  = new TH1D("metTopSelH","",200,0,400);
  TH1D * puppimetTopSelH = new TH1D("puppimetTopSelH","",200,0,400);
  TH1D * mvametTopSelH = new TH1D("mvametTopSelH","",200,0,400);

  TH1D * numberOfVerticesH = new TH1D("numberOfVerticesH","",100,-0.5,99.5);

  // Event categories
  TH1D * mass0jetH = new TH1D("mass0jetH","",200,0,200);
  TH1D * mass0jetBvetoH = new TH1D("mass0jetBvetoH","",200,0,200);

  TH1D * massBoostedH = new TH1D("massBoostedH","",200,0,200);
  TH1D * massBoostedBvetoH = new TH1D("massBoostedBvetoH","",200,0,200);

  TH1D * massVBFH = new TH1D("massVBFH","",200,0,200);
  TH1D * massVBFBvetoH = new TH1D("massVBFBvetoH","",200,0,200);

  // Applying selection and mass cut

  TH1D * nJets20BTagMedium0jetH = new TH1D("nJets20BTagMedium0jetH","",11,-0.5,10.5);
  TH1D * nJets20BTagMediumBoostedH = new TH1D("nJets20BTagMediumBoostedH","",11,-0.5,10.5);
  TH1D * nJets20BTagMediumVBFH = new TH1D("nJets20BTagMediumVBFH","",11,-0.5,10.5);

  TH1D * mjjVBFH = new TH1D("mjjVBFH","",30,0,3000);
  TH1D * mjjVBFBvetoH = new TH1D("mjjVBFBvetoH","",30,0,3000);

  TH1D * dimuonPtBoostedH = new TH1D("dimuonPtBoostedH","",100,0,1000);
  TH1D * dimuonPtBoostedBvetoH = new TH1D("dimuonPtBoostedBvetoH","",100,0,1000);

  TH1D * leadingJetPtH  = new TH1D("leadingJetPtH","",50,0,500);
  TH1D * leadingJetEtaH = new TH1D("leadingJetEtaH","",100,-5,5);
  TH1D * leadingJetPhiH = new TH1D("leadingJetPhiH","",100,-TMath::Pi(),TMath::Pi());
  
  TString scales[21] = {"M10","M9","M8","M7","M6","M5","M4","M3","M2","M1","0",
			"P1","P2","P3","P4","P5","P6","P7","P8","P9","P10"};
  
  TH1D * massSelScaleH[21];
  TH1D * metSelScaleH[21];
  TH1D * puppimetSelScaleH[21];
  TH1D * mvametSelScaleH[21];
  for (int iScale=0; iScale<21; ++iScale) {    
    massSelScaleH[iScale] = new TH1D("massSel"+scales[iScale]+"H","",200,0,200);
    metSelScaleH[iScale] = new TH1D("metSel"+scales[iScale]+"H","",200,0,400);
    puppimetSelScaleH[iScale] = new TH1D("puppimetSel"+scales[iScale]+"H","",200,0,400);
    mvametSelScaleH[iScale] = new TH1D("mvametSel"+scales[iScale]+"H","",200,0,400);
  }


  TH1D * nJets30SelH    = new TH1D("nJets30SelH","",11,-0.5,10.5);
  TH1D * nJets30etaCutSelH = new TH1D("nJets30etaCutSelH","",11,-0.5,10.5);
  TH1D * nJets20SelH    = new TH1D("nJets20SelH","",11,-0.5,10.5);
  TH1D * nJets20etaCutSelH = new TH1D("nJets20etaCutSelH","",11,-0.5,10.5);

  TH1D * nJets20BTagLooseSelH    = new TH1D("nJets20BTagLooseSelH","",11,-0.5,10.5);
  TH1D * nJets20BTagMediumSelH   = new TH1D("nJets20BTagMediumSelH","",11,-0.5,10.5); 

  TH1D * nJets30ZSelH    = new TH1D("nJets30ZSelH","",11,-0.5,10.5);
  TH1D * nJets30etaCutZSelH = new TH1D("nJets30etaCutZSelH","",11,-0.5,10.5);
  TH1D * nJets20ZSelH    = new TH1D("nJets20ZSelH","",11,-0.5,10.5);
  TH1D * nJets20etaCutZSelH = new TH1D("nJets20etaCutZSelH","",11,-0.5,10.5);

  TH1D * nJets20BTagLooseZSelH    = new TH1D("nJets20BTagLooseZSelH","",11,-0.5,10.5);
  TH1D * nJets20BTagMediumZSelH   = new TH1D("nJets20BTagMediumZSelH","",11,-0.5,10.5); 

  TH1D * HT30SelH       = new TH1D("HT30SelH","",50,0,500);
  TH1D * HT30etaCutSelH = new TH1D("HT30etaCutSelH","",50,0,500);
  TH1D * HT20SelH       = new TH1D("HT20SelH","",50,0,500);
  TH1D * HT20etaCutSelH = new TH1D("HT20etaCutSelH","",50,0,500);
  
  //Discriminiant histos
  TH1D * h_dimuonEta = new TH1D("dimuonEta","",120,-6,+6);//21Aug
  TH1D * h_dimuonEta_genMuMatch = new TH1D("dimuonEta_genMuMatch","",50,-6,+6);//21Aug
  TH1D * h_ptRatio = new TH1D ("ptRatio","",50,0,1);
  TH1D * h_dxy_muon1 =new TH1D ("dxy_muon1","",50,-0.02,0.02);
  TH1D * h_dxy_muon2 =new TH1D ("dxy_muon2","",50,-0.02,0.02);
  TH1D * h_dz_muon1 = new TH1D ("dz_muon1","",50,-0.1,0.1);
  TH1D * h_dz_muon2 = new TH1D ("dz_muon2","",50,-0.1,0.1);
  TH1D * h_dcaSigdxy_mu1 = new TH1D ("dcaSigdxy_mu1","",50,-4,4);
  TH1D * h_dcaSigdxy_mu2 = new TH1D ("dcaSigdxy_mu2","",50,-4,4);
  TH1D * h_dcaSigdz_mu1 = new TH1D ("dcaSigdz_mu1","",50,-4,4);
  TH1D * h_dcaSigdz_mu2 = new TH1D ("dcaSigdz_mu2","",50,-4,4);
  TH1D * h_dcaSigdxy_muon1 = new TH1D ("dcaSigdxy_muon1","",50,-4,4);
  TH1D * h_dcaSigdxy_muon2 = new TH1D ("dcaSigdxy_muon2","",50,-4,4);
  TH1D * h_dcaSigdz_muon1 = new TH1D ("dcaSigdz_muon1","",50,-4,4);
  TH1D * h_dcaSigdz_muon2 = new TH1D ("dcaSigdz_muon2","",50,-4,4);
  TH1D * h_dcaSigdxy_mu1_genMuMatch = new TH1D ("dcaSigdxy_mu1_genMuMatch","",50,-4,4);
  TH1D * h_dcaSigdxy_mu2_genMuMatch = new TH1D ("dcaSigdxy_mu2_genMuMatch","",50,-4,4);
  TH1D * h_dcaSigdz_mu1_genMuMatch = new TH1D ("dcaSigdz_mu1_genMuMatch","",50,-4,4);
  TH1D * h_dcaSigdz_mu2_genMuMatch = new TH1D ("dcaSigdz_mu2_genMuMatch","",50,-4,4);
  TH1D * h_phi_leadingMu_MET =new TH1D ("phi_leadingMu_MET","",50,0,3);
  TH1D * h_phi_trailingMu_MET =new TH1D ("phi_trailingMu_MET","",50,0,3);
  TH1D * h_dxy_muon1_dcaCut =new TH1D ("dxy_muon1_dcaCut","",50,-0.02,0.02);
  TH1D * h_dxy_muon2_dcaCut =new TH1D ("dxy_muon2_dcaCut","",50,-0.02,0.02);
  TH1D * h_dimuonEta_dcaCut = new TH1D("dimuonEta_dcaCut","",50,-6,+6);
  TH1D * h_ptRatio_dcaCut = new TH1D ("ptRatio_dcaCut","",50,0,1);

  TH1D * NumberOfVerticesH = new TH1D("NumberOfVerticesH","",51,-0.5,50.5);

  TH1D * MuSF_IdIso_Mu1H = new TH1D("MuIdIsoSF_Mu1H", "MuIdIsoSF_Mu1", 100, 0.5,1.5);
  TH1D * MuSF_IdIso_Mu2H = new TH1D("MuIdIsoSF_Mu2H", "MuIdIsoSF_Mu2", 100, 0.5,1.5);

  int nPtBins = 7;
  float ptBins[8] = {10,15,20,25,30,40,60,1000};

  int nPtBinsTrig = 16;
  float ptBinsTrig[17] = {10,
			  13,
			  16,
			  19,
			  22,
			  25,
			  28,
			  31,
			  34,
			  37,
			  40,
			  45,
			  50,
			  60,
			  70,
			  100,
			  1000};  
  
  int nEtaBins = 4;
  float etaBins[5] = {0,0.9,1.2,2.1,2.4}; 
  
  TString PtBins[7] = {"Pt10to15",
		       "Pt15to20",
		       "Pt20to25",
		       "Pt25to30",
		       "Pt30to40",
		       "Pt40to60",
		       "PtGt60"};
  
  TString PtBinsTrig[16] = {"Pt10to13",
			    "Pt13to16",
			    "Pt16to19",
			    "Pt19to22",
			    "Pt22to25",
			    "Pt25to28",
			    "Pt28to31",
			    "Pt31to34",
			    "Pt34to37",
			    "Pt37to40",
			    "Pt40to45",
			    "Pt45to50",
			    "Pt50to60",
			    "Pt60to70",
			    "Pt70to100",
			    "PtGt100"};

  TString EtaBins[4] = {"EtaLt0p9",
			"Eta0p9to1p2",
			"Eta1p2to2p1",
			"EtaGt2p1"};

  TString JetBins[3] = {"Jet0","Jet1","JetGe2"};

  //*****  create eta histogram with eta ranges associated to their names (eg. endcap, barrel)   ***** //

  TH1D * etaBinsH = new TH1D("etaBinsH", "etaBinsH", nEtaBins, etaBins);
  etaBinsH->Draw();
  etaBinsH->GetXaxis()->Set(nEtaBins, etaBins);
  for (int i=0; i<nEtaBins; i++){ etaBinsH->GetXaxis()->SetBinLabel(i+1, EtaBins[i]);}
  etaBinsH->Draw();
  file->cd();
  etaBinsH->Write("etaBinsH");


  //*****  create pt histogram_s with pt ranges associated to their names (eg. Pt10to13, ..)   ***** //
  //*****  two different pT binning, one for IdIso and one for trigger   ***** //

  TH1D * ptBinsH =  new TH1D("ptBinsH", "ptBinsH", nPtBins, ptBins);
  ptBinsH->Draw();
  ptBinsH->GetXaxis()->Set(nPtBins, ptBins);
  for (int i=0; i<nPtBins; i++){ ptBinsH->GetXaxis()->SetBinLabel(i+1, PtBins[i]);}
  ptBinsH->Draw();
  file->cd();
  ptBinsH->Write("ptBinsH");

  TH1D * ptBinsTrigH =  new TH1D("ptBinsTrigH", "ptBinsTrigH", nPtBinsTrig, ptBinsTrig);
  ptBinsTrigH->Draw();
  ptBinsTrigH->GetXaxis()->Set(nPtBinsTrig, ptBinsTrig);
  for (int i=0; i<nPtBinsTrig; i++){ ptBinsTrigH->GetXaxis()->SetBinLabel(i+1, PtBinsTrig[i]);}
  ptBinsTrigH->Draw();
  file->cd();
  ptBinsTrigH->Write("ptBinsTrigH");

  TH1D * ZMassEtaPtPass[4][7];
  TH1D * ZMassEtaPtFail[4][7];

  TH1D * ZMassJetEtaPtPass[4][3][7];
  TH1D * ZMassJetEtaPtFail[4][3][7];

  TH1D * PromptPtPass[4];
  TH1D * PromptPtFail[4];

  TH1D * NonPromptPtPass[4];
  TH1D * NonPromptPtFail[4];

  TH1D * PromptSelPtPass[4];
  TH1D * PromptSelPtFail[4];

  TH1D * NonPromptSelPtPass[4];
  TH1D * NonPromptSelPtFail[4];

  TH1D * PromptSelJetPtPass[4][3];
  TH1D * PromptSelJetPtFail[4][3];

  TH1D * NonPromptSelJetPtPass[4][3];
  TH1D * NonPromptSelJetPtFail[4][3];

  TH1D * ZMassMu23EtaPtPass[4][16];
  TH1D * ZMassMu23EtaPtFail[4][16];

  TH1D * ZMassMu17EtaPtPass[4][16];
  TH1D * ZMassMu17EtaPtFail[4][16];

  TH1D * ZMassMu8EtaPtPass[4][16];
  TH1D * ZMassMu8EtaPtFail[4][16];

  TH1D * ZMassIsoMuEtaPtPass[4][16];
  TH1D * ZMassIsoMuEtaPtFail[4][16];

  TH1D * ZMassPass = new TH1D("ZMassPass","",80,50,130);
  TH1D * ZMassFail = new TH1D("ZMassFail","",80,50,130);

  for (int iEta=0; iEta<nEtaBins; ++iEta) {
    PromptPtPass[iEta] = new TH1D("PromptPt"+EtaBins[iEta]+"Pass","",nPtBins,ptBins);
    PromptPtFail[iEta] = new TH1D("PromptPt"+EtaBins[iEta]+"Fail","",nPtBins,ptBins);
    NonPromptPtPass[iEta] = new TH1D("NonPromptPt"+EtaBins[iEta]+"Pass","",nPtBins,ptBins);
    NonPromptPtFail[iEta] = new TH1D("NonPromptPt"+EtaBins[iEta]+"Fail","",nPtBins,ptBins);
    PromptSelPtPass[iEta] = new TH1D("PromptSelPt"+EtaBins[iEta]+"Pass","",nPtBins,ptBins);
    PromptSelPtFail[iEta] = new TH1D("PromptSelPt"+EtaBins[iEta]+"Fail","",nPtBins,ptBins);
    NonPromptSelPtPass[iEta] = new TH1D("NonPromptSelPt"+EtaBins[iEta]+"Pass","",nPtBins,ptBins);
    NonPromptSelPtFail[iEta] = new TH1D("NonPromptSelPt"+EtaBins[iEta]+"Fail","",nPtBins,ptBins);
    for (int iJet=0; iJet<3; ++iJet) {
      PromptSelJetPtPass[iEta][iJet] = new TH1D("PromptSelPt"+EtaBins[iEta]+JetBins[iJet]+"Pass","",nPtBins,ptBins);
      PromptSelJetPtFail[iEta][iJet] = new TH1D("PromptSelPt"+EtaBins[iEta]+JetBins[iJet]+"Fail","",nPtBins,ptBins);
      NonPromptSelJetPtPass[iEta][iJet] = new TH1D("NonPromptSelPt"+EtaBins[iEta]+JetBins[iJet]+"Pass","",nPtBins,ptBins);
      NonPromptSelJetPtFail[iEta][iJet] = new TH1D("NonPromptSelPt"+EtaBins[iEta]+JetBins[iJet]+"Fail","",nPtBins,ptBins);
    }
    for (int iPt=0; iPt<nPtBins; ++iPt) {
      ZMassEtaPtPass[iEta][iPt] = new TH1D("ZMass"+EtaBins[iEta]+PtBins[iPt]+"Pass","",80,50,130);
      ZMassEtaPtFail[iEta][iPt] = new TH1D("ZMass"+EtaBins[iEta]+PtBins[iPt]+"Fail","",80,50,130);
      for (int iJet=0; iJet<3; ++iJet) {
	ZMassJetEtaPtPass[iEta][iJet][iPt] = new TH1D("ZMass"+EtaBins[iEta]+JetBins[iJet]+PtBins[iPt]+"Pass","",80,50,130);
	ZMassJetEtaPtFail[iEta][iJet][iPt] = new TH1D("ZMass"+EtaBins[iEta]+JetBins[iJet]+PtBins[iPt]+"Fail","",80,50,130);
      }
    }
    for (int iPt=0; iPt<nPtBinsTrig; ++iPt) {
      ZMassMu23EtaPtPass[iEta][iPt] = new TH1D("ZMassMu23"+EtaBins[iEta]+PtBinsTrig[iPt]+"Pass","",80,50,130);
      ZMassMu23EtaPtFail[iEta][iPt] = new TH1D("ZMassMu23"+EtaBins[iEta]+PtBinsTrig[iPt]+"Fail","",80,50,130);
      ZMassMu17EtaPtPass[iEta][iPt] = new TH1D("ZMassMu17"+EtaBins[iEta]+PtBinsTrig[iPt]+"Pass","",80,50,130);
      ZMassMu17EtaPtFail[iEta][iPt] = new TH1D("ZMassMu17"+EtaBins[iEta]+PtBinsTrig[iPt]+"Fail","",80,50,130);
      ZMassMu8EtaPtPass[iEta][iPt]  = new TH1D("ZMassMu8"+EtaBins[iEta]+PtBinsTrig[iPt]+"Pass","",80,50,130);
      ZMassMu8EtaPtFail[iEta][iPt]  = new TH1D("ZMassMu8"+EtaBins[iEta]+PtBinsTrig[iPt]+"Fail","",80,50,130);
      ZMassIsoMuEtaPtPass[iEta][iPt]  = new TH1D("ZMassIsoMu"+EtaBins[iEta]+PtBinsTrig[iPt]+"Pass","",80,50,130);
      ZMassIsoMuEtaPtFail[iEta][iPt]  = new TH1D("ZMassIsoMu"+EtaBins[iEta]+PtBinsTrig[iPt]+"Fail","",80,50,130);
    }
  }

  TH1D * PUweightsOfficialH = new TH1D("PUweightsOfficialH","PU weights w/ official reweighting",1000, 0, 10);
  TH1D * nTruePUInteractionsH = new TH1D("nTruePUInteractionsH","",50,-0.5,49.5);

  int nJetBins = 3;
  int nZPtBins = 5;
  float zPtBins[6] = {0,10,20,30,50,1000};
  float jetBins[4] = {-0.5,0.5,1.5,2.5};

  TString NJetBins[3] = {"NJet0","NJet1","NJetGe2"};
  TString ZPtBins[5] = {"Pt0to10",
			"Pt10to20",
			"Pt20to30",
			"Pt30to50",
			"PtGt50"};

  TString RecoilZParal("recoilZParal_");
  TString RecoilZPerp("recoilZPerp_");
  TString RecoilPuppiZParal("recoilPuppiZParal_");
  TString RecoilPuppiZPerp("recoilPuppiZPerp_");
  TString RecoilMvaZParal("recoilMvaZParal_");
  TString RecoilMvaZPerp("recoilMvaZPerp_");

  TString RecoilTopParal("recoilTopParal_");
  TString RecoilTopPerp("recoilTopPerp_");
  TString RecoilPuppiTopParal("recoilPuppiTopParal_");
  TString RecoilPuppiTopPerp("recoilPuppiTopPerp_");
  TString RecoilMvaTopParal("recoilMvaTopParal_");
  TString RecoilMvaTopPerp("recoilMvaTopPerp_");

  // Saving Z pt bins
  TH1D * ZPtBinsH = new TH1D("ZPtBinsH","ZPtBinsH",nZPtBins,zPtBins);
  for (int iB=0; iB<nZPtBins; ++iB) 
    ZPtBinsH->GetXaxis()->SetBinLabel(iB+1,ZPtBins[iB]);
  
  // Saving jet bins
  TH1D * JetBinsH = new TH1D("JetBinsH","JetBinsH",nJetBins,jetBins);
  for (int iB=0; iB<nJetBins; ++iB)
    JetBinsH->GetXaxis()->SetBinLabel(iB+1,NJetBins[iB]);

  TH1D * metSelNJetsH[3];
  TH1D * puppimetSelNJetsH[3];
  TH1D * mvametSelNJetsH[3];

  TH1D * metZSelNJetsH[3];
  TH1D * puppimetZSelNJetsH[3];
  TH1D * mvametZSelNJetsH[3];

  TH1D * metTopSelNJetsH[3];
  TH1D * puppimetTopSelNJetsH[3];
  TH1D * mvametTopSelNJetsH[3];

  TH1D * recoilZParalH[3];
  TH1D * recoilZPerpH[3];
  TH1D * recoilPuppiZParalH[3];
  TH1D * recoilPuppiZPerpH[3];
  TH1D * recoilMvaZParalH[3];
  TH1D * recoilMvaZPerpH[3];

  TH1D * recoilTopParalH[3];
  TH1D * recoilTopPerpH[3];
  TH1D * recoilPuppiTopParalH[3];
  TH1D * recoilPuppiTopPerpH[3];
  TH1D * recoilMvaTopParalH[3];
  TH1D * recoilMvaTopPerpH[3];

  TH1D * recoilZParal_Ptbins_nJetsH[3][5];
  TH1D * recoilZPerp_Ptbins_nJetsH[3][5];
  TH1D * recoilPuppiZParal_Ptbins_nJetsH[3][5];
  TH1D * recoilPuppiZPerp_Ptbins_nJetsH[3][5];
  TH1D * recoilMvaZParal_Ptbins_nJetsH[3][5];
  TH1D * recoilMvaZPerp_Ptbins_nJetsH[3][5];

  TH1D * recoilResponse_nJets[3];
  TH1D * recoilResponse_Ptbins_nJets[3][5];

  TH1D * recoilPuppiResponse_nJets[3];
  TH1D * recoilPuppiResponse_Ptbins_nJets[3][5];

  TH1D * recoilMvaResponse_nJets[3];
  TH1D * recoilMvaResponse_Ptbins_nJets[3][5];

  TH1D * recoilResponseMC_Ptbins[5];
  TH1D * recoilResponseMC_nJets[3];
  TH1D * recoilResponseMC_Ptbins_nJets[3][5];

  TH1D * recoilPuppiResponseMC_Ptbins[5];
  TH1D * recoilPuppiResponseMC_nJets[3];
  TH1D * recoilPuppiResponseMC_Ptbins_nJets[3][5];

  TH1D * recoilMvaResponseMC_Ptbins[5];
  TH1D * recoilMvaResponseMC_nJets[3];
  TH1D * recoilMvaResponseMC_Ptbins_nJets[3][5];
  
  // systematics ===>

  TH1D * recoilMvaZParalSysH[3][4][2];
  TH1D * recoilMvaZPerpSysH[3][4][2];

  TH1D * recoilMvaTopParalSysH[3][4][2];
  TH1D * recoilMvaTopPerpSysH[3][4][2];

  TH1D * mvametZSelNJetsSysH[3][4][2];
  TH1D * mvametTopSelNJetsSysH[3][4][2];

  TString sysNames[4] = {"JES","MEtScale","MEtReso","MuonMom"};
  TString UpDown[2] = {"Up","Down"};

  for (int iBin=0; iBin<nJetBins; ++iBin) {
    recoilZParalH[iBin] = new TH1D(RecoilZParal+NJetBins[iBin]+"H","",200,-400,400);
    recoilZPerpH[iBin] = new TH1D(RecoilZPerp+NJetBins[iBin]+"H","",200,-400,400);

    recoilPuppiZParalH[iBin] = new TH1D(RecoilPuppiZParal+NJetBins[iBin]+"H","",200,-400,400);
    recoilPuppiZPerpH[iBin] = new TH1D(RecoilPuppiZPerp+NJetBins[iBin]+"H","",200,-400,400);

    recoilMvaZParalH[iBin] = new TH1D(RecoilMvaZParal+NJetBins[iBin]+"H","",200,-400,400);
    recoilMvaZPerpH[iBin] = new TH1D(RecoilMvaZPerp+NJetBins[iBin]+"H","",200,-400,400);

    recoilTopParalH[iBin] = new TH1D(RecoilTopParal+NJetBins[iBin]+"H","",200,-400,400);
    recoilTopPerpH[iBin] = new TH1D(RecoilTopPerp+NJetBins[iBin]+"H","",200,-400,400);

    recoilPuppiTopParalH[iBin] = new TH1D(RecoilPuppiTopParal+NJetBins[iBin]+"H","",200,-400,400);
    recoilPuppiTopPerpH[iBin] = new TH1D(RecoilPuppiTopPerp+NJetBins[iBin]+"H","",200,-400,400);

    recoilMvaTopParalH[iBin] = new TH1D(RecoilMvaTopParal+NJetBins[iBin]+"H","",200,-400,400);
    recoilMvaTopPerpH[iBin] = new TH1D(RecoilMvaTopPerp+NJetBins[iBin]+"H","",200,-400,400);

    metSelNJetsH[iBin] = new TH1D("metSel"+NJetBins[iBin]+"H","",200,0.,400.);
    puppimetSelNJetsH[iBin] = new TH1D("puppimetSel"+NJetBins[iBin]+"H","",200,0.,400.);
    mvametSelNJetsH[iBin] = new TH1D("mvametSel"+NJetBins[iBin]+"H","",200,0.,400.);

    metZSelNJetsH[iBin] = new TH1D("metZSel"+NJetBins[iBin]+"H","",200,0.,400.);
    puppimetZSelNJetsH[iBin] = new TH1D("puppimetZSel"+NJetBins[iBin]+"H","",200,0.,400.);
    mvametZSelNJetsH[iBin] = new TH1D("mvametZSel"+NJetBins[iBin]+"H","",200,0.,400.);

    metTopSelNJetsH[iBin] = new TH1D("metTopSel"+NJetBins[iBin]+"H","",200,0.,400.);
    puppimetTopSelNJetsH[iBin] = new TH1D("puppimetTopSel"+NJetBins[iBin]+"H","",200,0.,400.);
    mvametTopSelNJetsH[iBin] = new TH1D("mvametTopSel"+NJetBins[iBin]+"H","",200,0.,400.);

    recoilResponse_nJets[iBin] = new TH1D("recoilResponse"+NJetBins[iBin],"",400,-20,20);
    recoilPuppiResponse_nJets[iBin] = new TH1D("recoilPuppiResponse"+NJetBins[iBin],"",400,-20,20);
    recoilMvaResponse_nJets[iBin] = new TH1D("recoilMvaResponse"+NJetBins[iBin],"",400,-20,20);

    recoilResponseMC_nJets[iBin] = new TH1D("recoilResponseMC"+NJetBins[iBin],"",400,-20,20);
    recoilPuppiResponseMC_nJets[iBin] = new TH1D("recoilPuppiResponseMC"+NJetBins[iBin],"",400,-20,20);
    recoilMvaResponseMC_nJets[iBin] = new TH1D("recoilMvaResponseMC"+NJetBins[iBin],"",400,-20,20);

    for (int iSys=0; iSys<4; ++iSys) {
      for (int iUD=0; iUD<2; ++iUD) {

	mvametZSelNJetsSysH[iBin][iSys][iUD] = new TH1D("mvametZSel"+NJetBins[iBin]+sysNames[iSys]+UpDown[iUD]+"H","",200,0.,400.);
	mvametTopSelNJetsSysH[iBin][iSys][iUD] = new TH1D("mvametTopSel"+NJetBins[iBin]+sysNames[iSys]+UpDown[iUD]+"H","",200,0.,400.);
	
	recoilMvaZParalSysH[iBin][iSys][iUD] = new TH1D(RecoilMvaZParal+NJetBins[iBin]+sysNames[iSys]+UpDown[iUD]+"H","",200,-400,400);
	recoilMvaZPerpSysH[iBin][iSys][iUD] = new TH1D(RecoilMvaZPerp+NJetBins[iBin]+sysNames[iSys]+UpDown[iUD]+"H","",200,-400,400);

	recoilMvaTopParalSysH[iBin][iSys][iUD] = new TH1D(RecoilMvaTopParal+NJetBins[iBin]+sysNames[iSys]+UpDown[iUD]+"H","",200,-400,400);
	recoilMvaTopPerpSysH[iBin][iSys][iUD] = new TH1D(RecoilMvaTopPerp+NJetBins[iBin]+sysNames[iSys]+UpDown[iUD]+"H","",200,-400,400);

      }
    }

  }

  for (int iPtBins=0; iPtBins<nZPtBins; ++iPtBins){
    recoilResponseMC_Ptbins[iPtBins] = new TH1D("recoilResponseMC"+ZPtBins[iPtBins],"",400,-20,20);
    recoilPuppiResponseMC_Ptbins[iPtBins] = new TH1D("recoilPuppiResponseMC"+ZPtBins[iPtBins],"",400,-20,20);
    recoilMvaResponseMC_Ptbins[iPtBins] = new TH1D("recoilMvaResponseMC"+ZPtBins[iPtBins],"",400,-20,20);
  }

  for (int iJets=0; iJets<nJetBins; ++iJets) {
    for (int iPtBins=0; iPtBins<nZPtBins; ++iPtBins){

      recoilZParal_Ptbins_nJetsH[iJets][iPtBins] = new TH1D(RecoilZParal+NJetBins[iJets]+ZPtBins[iPtBins],"",100,-200,200);
      recoilZPerp_Ptbins_nJetsH[iJets][iPtBins] = new TH1D(RecoilZPerp+NJetBins[iJets]+ZPtBins[iPtBins],"",100,-200,200);

      recoilPuppiZParal_Ptbins_nJetsH[iJets][iPtBins] = new TH1D(RecoilPuppiZParal+NJetBins[iJets]+ZPtBins[iPtBins],"",100,-200,200);
      recoilPuppiZPerp_Ptbins_nJetsH[iJets][iPtBins] = new TH1D(RecoilPuppiZPerp+NJetBins[iJets]+ZPtBins[iPtBins],"",100,-200,200);

      recoilMvaZParal_Ptbins_nJetsH[iJets][iPtBins] = new TH1D(RecoilMvaZParal+NJetBins[iJets]+ZPtBins[iPtBins],"",100,-200,200);
      recoilMvaZPerp_Ptbins_nJetsH[iJets][iPtBins] = new TH1D(RecoilMvaZPerp+NJetBins[iJets]+ZPtBins[iPtBins],"",100,-200,200);

      recoilResponse_Ptbins_nJets[iJets][iPtBins] = new TH1D("recoilResponse"+NJetBins[iJets]+ZPtBins[iPtBins],"",400,-20,20);
      recoilPuppiResponse_Ptbins_nJets[iJets][iPtBins] = new TH1D("recoilPuppiResponse"+NJetBins[iJets]+ZPtBins[iPtBins],"",400,-20,20);
      recoilMvaResponse_Ptbins_nJets[iJets][iPtBins] = new TH1D("recoilMvaResponse"+NJetBins[iJets]+ZPtBins[iPtBins],"",400,-20,20);

      recoilResponseMC_Ptbins_nJets[iJets][iPtBins] = new TH1D("recoilResponseMC"+NJetBins[iJets]+ZPtBins[iPtBins],"",400,-20,20);
      recoilPuppiResponseMC_Ptbins_nJets[iJets][iPtBins] = new TH1D("recoilPuppiResponseMC"+NJetBins[iJets]+ZPtBins[iPtBins],"",400,-20,20);
      recoilMvaResponseMC_Ptbins_nJets[iJets][iPtBins] = new TH1D("recoilMvaResponseMC"+NJetBins[iJets]+ZPtBins[iPtBins],"",400,-20,20);

    }
  }

  ULong64_t evt;
  float pt_1;
  float pt_2;
  float eta_1;
  float eta_2;
  float phi_1;
  float phi_2;

  float dilepton_pt;
  float dilepton_eta;
  float dilepton_phi;

  float mvametX;
  float metX;
  float metuncorrX;
  float puppimetX;

  float u1_mvametX;
  float u1_metX;
  float u1_metuncorrX;

  float u2_mvametX;
  float u2_metX;
  float u2_metuncorrX;

  int njets;
  bool match_1;
  bool match_2;

  TTree * synch_tree = new TTree("TauCheck","TauChek");

  synch_tree->Branch("evt", &evt, "evt/I");

  synch_tree->Branch("dilepton_pt",&dilepton_pt,"dilepton_pt/F");
  synch_tree->Branch("dilepton_eta",&dilepton_eta,"dilepton_eta/F");
  synch_tree->Branch("dilepton_phi",&dilepton_phi,"dilepton_phi/F");

  synch_tree->Branch("pt_1",&pt_1,"pt_1/F");
  synch_tree->Branch("pt_2",&pt_2,"pt_2/F");

  synch_tree->Branch("eta_1",&eta_1,"eta_1/F");
  synch_tree->Branch("eta_2",&eta_2,"eta_2/F");

  synch_tree->Branch("phi_1",&phi_1,"phi_1/F");
  synch_tree->Branch("phi_2",&phi_2,"phi_2/F");
  
  synch_tree->Branch("match_1",&match_1,"match_1/O");
  synch_tree->Branch("match_2",&match_2,"match_2/O");

  synch_tree->Branch("mvamet",&mvametX,"mvamet/F");
  synch_tree->Branch("met",&metX,"met/F");
  synch_tree->Branch("metuncorr",&metuncorrX,"metuncorr/F");
  synch_tree->Branch("puppimet",&puppimetX,"puppimet/F");

  synch_tree->Branch("u1_mvamet",&u1_mvametX,"u1_mvamet/F");
  synch_tree->Branch("u1_met",&u1_metX,"u1_met/F");
  synch_tree->Branch("u1_metuncorr",&u1_metuncorrX,"u1_metuncorr/F");

  synch_tree->Branch("u2_mvamet",&u2_mvametX,"u2_mvamet/F");
  synch_tree->Branch("u2_met",&u2_metX,"u2_met/F");
  synch_tree->Branch("u2_metuncorr",&u2_metuncorrX,"u2_metuncorr/F");

  synch_tree->Branch("njets", &njets, "njets/I");

  // PILE UP REWEIGHTING - OPTIONS

  if (applyPUreweighting_vertices and applyPUreweighting_official) 
	{std::cout<<"ERROR: Choose only ONE PU reweighting method (vertices or official, not both!) " <<std::endl; exit(-1);}

  // reweighting with vertices

  // reading vertex weights
  TFile * fileDataNVert = new TFile(TString(cmsswBase)+"/src/"+vertDataFileName);
  TFile * fileMcNVert   = new TFile(TString(cmsswBase)+"/src/"+vertMcFileName);

  TH1D * vertexDataH = (TH1D*)fileDataNVert->Get(TString(vertHistName));
  TH1D * vertexMcH   = (TH1D*)fileMcNVert->Get(TString(vertHistName));

  float normVertexData = vertexDataH->GetSumOfWeights();
  float normVertexMc   = vertexMcH->GetSumOfWeights();

  vertexDataH->Scale(1/normVertexData);
  vertexMcH->Scale(1/normVertexMc);


  // reweighting official recipe 
  // initialize pile up object
  PileUp * PUofficial = new PileUp();
  
  if (applyPUreweighting_official) {
    TFile * filePUdistribution_data = new TFile(TString(cmsswBase)+"/src/DesyTauAnalyses/NTupleMaker/data/PileUpDistrib/"+PileUpDataFile,"read"); 
    TFile * filePUdistribution_MC = new TFile (TString(cmsswBase)+"/src/DesyTauAnalyses/NTupleMaker/data/PileUpDistrib/"+PileUpMCFile, "read"); 
    TH1D * PU_data = (TH1D *)filePUdistribution_data->Get("pileup");
    TH1D * PU_mc = (TH1D *)filePUdistribution_MC->Get("pileup");
    PUofficial->set_h_data(PU_data); 
    PUofficial->set_h_MC(PU_mc);
  }

  // HTT Met recoil corrections
  //  RecoilCorrector recoilPFMetCorrector("HTT-utilities/RecoilCorrections/data/recoilPFMet.root");
  //  RecoilCorrector recoilMvaMetCorrector("HTT-utilities/RecoilCorrections/data/recoilMvaMet.root");
  //  RecoilCorrector recoilPuppiMetCorrector("HTT-utilities/RecoilCorrections/data/recoilPuppiMet.root");

  RecoilCorrector recoilPFMetCorrector(RecoilFileName);
  RecoilCorrector recoilMvaMetCorrector(RecoilMvaFileName);
  RecoilCorrector recoilPuppiMetCorrector(RecoilPuppiFileName);

  // MetResponse
  MEtSys metSys(MetSysFileName);
  MEtSys metSysPuppi(MetSysPuppiFileName);
  MEtSys metSysMva(MetSysMvaFileName);


  // Lepton Scale Factors 

  ScaleFactor * SF_muonIdIso; 
  ScaleFactor * SF_muonTrig;
  if (applyLeptonSF) {
    SF_muonIdIso = new ScaleFactor();
    SF_muonIdIso->init_ScaleFactor(TString(cmsswBase)+"/src/"+TString(MuonIdIsoFile));
    SF_muonTrig = new ScaleFactor();
    SF_muonTrig->init_ScaleFactor(TString(cmsswBase)+"/src/"+TString(MuonTrigFile));
  }

  // tracking efficiency SF
  TFile * fileTrackingSF = new TFile(TString(cmsswBase)+"/src/"+TString(trackingSFFile));
  TH1D * trackEffMuonH = (TH1D*)fileTrackingSF->Get("effTrackingMu");

  // Z mass pt weights
  TFile * fileZMassPtWeights = new TFile(TString(cmsswBase)+"/src/"+ZMassPtWeightsFileName); 
  if (fileZMassPtWeights->IsZombie()) {
    std::cout << "File " << TString(cmsswBase) << "/src/" << ZMassPtWeightsFileName << "  does not exist!" << std::endl;
    exit(-1);
  }
  TH2D * histZMassPtWeights = (TH2D*)fileZMassPtWeights->Get(ZMassPtWeightsHistName); 
  if (histZMassPtWeights==NULL) {
    std::cout << "histogram " << ZMassPtWeightsHistName << " is not found in file " << TString(cmsswBase) << "/src/DesyTauAnalyses/NTupleMaker/data/" << ZMassPtWeightsFileName << std::endl;
    exit(-1);
  }

  // BTag scale factors                                                                                                                                                                                                                    
  BTagCalibration calib("csvv2", cmsswBase+"/src/DesyTauAnalyses/NTupleMaker/data/CSVv2_ichep.csv");
  BTagCalibrationReader reader_B(BTagEntry::OP_MEDIUM,"central",{"up","down"});
  BTagCalibrationReader reader_C(BTagEntry::OP_MEDIUM,"central",{"up","down"});
  BTagCalibrationReader reader_Light(BTagEntry::OP_MEDIUM,"central",{"up","down"});
  reader_B.load(calib,BTagEntry::FLAV_B,"comb");
  reader_C.load(calib,BTagEntry::FLAV_C,"comb");
  reader_Light.load(calib,BTagEntry::FLAV_UDSG,"incl");

  float etaBTAG[2] = {0.5,2.1};
  float ptBTAG[5] = {25.,35.,50.,100.,200.};

  for (int iEta=0; iEta<2; ++iEta) {
    for (int iPt=0; iPt<5; ++iPt) {
      float sfB = reader_B.eval_auto_bounds("central",BTagEntry::FLAV_B, etaBTAG[iEta], ptBTAG[iPt]);
      float sfC = reader_C.eval_auto_bounds("central",BTagEntry::FLAV_C, etaBTAG[iEta], ptBTAG[iPt]);
      float sfLight = reader_Light.eval_auto_bounds("central",BTagEntry::FLAV_UDSG, etaBTAG[iEta], ptBTAG[iPt]);
      
      float sfB_Up = reader_B.eval_auto_bounds("up",BTagEntry::FLAV_B, etaBTAG[iEta], ptBTAG[iPt]);
      float sfC_Up = reader_C.eval_auto_bounds("up",BTagEntry::FLAV_C, etaBTAG[iEta], ptBTAG[iPt]);
      float sfLight_Up = reader_Light.eval_auto_bounds("up",BTagEntry::FLAV_UDSG, etaBTAG[iEta], ptBTAG[iPt]);

      float sfB_Down = reader_B.eval_auto_bounds("down",BTagEntry::FLAV_B, etaBTAG[iEta], ptBTAG[iPt]);
      float sfC_Down = reader_C.eval_auto_bounds("down",BTagEntry::FLAV_C, etaBTAG[iEta], ptBTAG[iPt]);
      float sfLight_Down = reader_Light.eval_auto_bounds("down",BTagEntry::FLAV_UDSG, etaBTAG[iEta], ptBTAG[iPt]);
      
      printf("pT = %3.0f   eta = %3.1f  ->  SFb = %5.3f + %5.3f - %5.3f ; SFc = %5.3f + %5.3f - %5.3f ; SFl = %5.3f + %5.3f - %5.3f\n",
	     ptBTAG[iPt],etaBTAG[iEta],
	     sfB,sfB_Up,sfB_Down,
	     sfC,sfC_Up,sfC_Down,
	     sfLight,sfLight_Up,sfLight_Down);
      
        }
  }

  //  exit(0);

  TFile * fileTagging = new TFile(TString(cmsswBase)+TString("/src/DesyTauAnalyses/NTupleMaker/data/tagging_efficiencies_ichep2016.root"));
  TH2F * tagEff_B = (TH2F*)fileTagging->Get("btag_eff_b");
  TH2F * tagEff_C = (TH2F*)fileTagging->Get("btag_eff_c");
  TH2F * tagEff_Light = (TH2F*)fileTagging->Get("btag_eff_oth");

  float MaxBJetPt = 1000;
  float MinBJetPt = 20;

  TRandom3 rand;

  int nFiles = 0;
  int nEvents = 0;
  int selEventsAllMuons = 0;
  int selEventsIdMuons = 0;
  int selEventsIsoMuons = 0;


  int nTotalFiles = 0;
  std::string dummy;
  // count number of files --->
  while (fileList0 >> dummy) nTotalFiles++;

  unsigned int RunMin = 9999999;
  unsigned int RunMax = 0;

  std::vector<unsigned int> allRuns; allRuns.clear();
 
  //----Attention----//
  //if(XSec!=1) nTotalFiles=20;
  //nTotalFiles=5;



  for (int iF=0; iF<nTotalFiles; ++iF) {
  
    std::string filen;
    fileList >> filen;

    std::cout << "file " << iF+1 << " out of " << nTotalFiles << " filename : " << filen << std::endl;
    TFile * file_ = TFile::Open(TString(filen));
    
    /*
    TH1D * histoInputEvents = NULL;
    histoInputEvents = (TH1D*)file_->Get("makeroottree/nEvents");
    if (histoInputEvents==NULL) continue;
    int NE = int(histoInputEvents->GetEntries());
    for (int iE=0;iE<NE;++iE)
      inputEventsH->Fill(0.);
    std::cout << "      number of input events         = " << NE << std::endl;


    TTree * _inittree = NULL;
    _inittree = (TTree*)file_->Get(TString(initNtupleName));
    if (_inittree!=NULL) {
      Float_t genweight;
      if (!isData)
	_inittree->SetBranchAddress("genweight",&genweight);
      Long64_t numberOfEntriesInitTree = _inittree->GetEntries();
      std::cout << "      number of entries in Init Tree = " << numberOfEntriesInitTree << std::endl;
      for (Long64_t iEntry=0; iEntry<numberOfEntriesInitTree; iEntry++) {
	_inittree->GetEntry(iEntry);
	if (isData)
	  histWeightsH->Fill(0.,1.);
	else
	  histWeightsH->Fill(0.,genweight);
      }
    }
    */
    TTree * _tree = NULL;
    _tree = (TTree*)file_->Get(TString(ntupleName));
    if (_tree==NULL) continue;
    Long64_t numberOfEntries = _tree->GetEntries();
    std::cout << "      number of entries in Tree      = " << numberOfEntries << std::endl;
    AC1B analysisTree(_tree);


    // EVENT LOOP //
    for (Long64_t iEntry=0; iEntry<numberOfEntries; iEntry++) { 
    
      analysisTree.GetEntry(iEntry);
      nEvents++;
      
      if (nEvents%10000==0) 
	cout << "      processed " << nEvents << " events" << endl; 

      float weight = 1;

      //------------------------------------------------

      if (!isData) 
	weight *=analysisTree.genweight;

      histWeightsH->Fill(float(0),weight);

      TLorentzVector genZ; genZ.SetXYZT(0,0,0,0); 
      TLorentzVector genV; genV.SetXYZT(0,0,0,0);
      TLorentzVector genL; genL.SetXYZT(0,0,0,0);
      TLorentzVector genMuonsLV; genMuonsLV.SetXYZT(0,0,0,0);
      TLorentzVector genElectronsLV; genElectronsLV.SetXYZT(0,0,0,0);
      TLorentzVector genTausLV; genTausLV.SetXYZT(0,0,0,0);
      TLorentzVector genVisTausLV; genVisTausLV.SetXYZT(0,0,0,0);
      std::vector<unsigned int> genMuons; genMuons.clear();
      std::vector<unsigned int> genElectrons; genElectrons.clear();
      std::vector<unsigned int> genTaus; genTaus.clear();
      std::vector<unsigned int> genVisTaus; genVisTaus.clear();
      if (!isData) {
	for (unsigned int igentau=0; igentau < analysisTree.gentau_count; ++igentau) {
	  TLorentzVector tauLV; tauLV.SetXYZT(analysisTree.gentau_px[igentau],
					      analysisTree.gentau_py[igentau],
					      analysisTree.gentau_pz[igentau],
					      analysisTree.gentau_e[igentau]);
	  TLorentzVector tauVisLV; tauVisLV.SetXYZT(analysisTree.gentau_visible_px[igentau],
						    analysisTree.gentau_visible_py[igentau],
						    analysisTree.gentau_visible_pz[igentau],
						    analysisTree.gentau_visible_e[igentau]);
	  if (analysisTree.gentau_isPrompt[igentau]&&analysisTree.gentau_isFirstCopy[igentau]) {
	    genTaus.push_back(igentau);
	    genTausLV += tauLV;
	  }
	  if (analysisTree.gentau_isPrompt[igentau]&&analysisTree.gentau_isLastCopy[igentau]) {	
	    genVisTaus.push_back(igentau);
	    genVisTausLV += tauVisLV;
	  }
	  
	}

	for (unsigned int igen=0; igen<analysisTree.genparticles_count; ++igen) {
	  //	  cout << igen << "   pdgId = " << analysisTree.genparticles_pdgid[igen] << endl;
	  TLorentzVector genPart; genPart.SetXYZT(analysisTree.genparticles_px[igen],
						  analysisTree.genparticles_py[igen],
						  analysisTree.genparticles_pz[igen],
						  analysisTree.genparticles_e[igen]);
	  if (analysisTree.genparticles_pdgid[igen]==23) {
	    if (analysisTree.genparticles_fromHardProcess[igen])
	      genZ.SetXYZT(analysisTree.genparticles_px[igen],
			   analysisTree.genparticles_py[igen],
			   analysisTree.genparticles_pz[igen],
			   analysisTree.genparticles_e[igen]);
	  }
	  bool isMuon = fabs(analysisTree.genparticles_pdgid[igen])==13;
	  bool isElectron = fabs(analysisTree.genparticles_pdgid[igen])==11;
	  bool isLepton = isMuon || isElectron;
	  bool isNeutrino = fabs(analysisTree.genparticles_pdgid[igen])==12||
	    fabs(analysisTree.genparticles_pdgid[igen])==14||
	    fabs(analysisTree.genparticles_pdgid[igen])==16;
	  bool isPrompt = analysisTree.genparticles_isPrompt[igen]||
	    analysisTree.genparticles_isPromptTauDecayProduct[igen];

	  if (fabs(analysisTree.genparticles_pdgid[igen])==11) { 
	    if (analysisTree.genparticles_fromHardProcess[igen]&&analysisTree.genparticles_status[igen]==1) {
	      genElectrons.push_back(igen);
	      genElectronsLV += genPart;
	    }
	  }
	  
	  if (fabs(analysisTree.genparticles_pdgid[igen])==13) { 
	    if (analysisTree.genparticles_fromHardProcess[igen]&&analysisTree.genparticles_status[igen]==1) {
	      genMuons.push_back(igen);
	      genMuonsLV += genPart;
	    }
	  }

	  
	  if (analysisTree.genparticles_status[igen]==1&&isPrompt) {
	    if (isLepton&&
		fabs(genPart.Eta())<2.4&&
		genPart.Pt()>10) {
	      genV += genPart;
	      genL += genPart;
	    }
	    if (isNeutrino) 
	      genV += genPart;
	  }
	}
	if (genV.Pt()<0.1) genV.SetXYZM(0.1,0.1,0.,0.);

	/*	
	std::cout << "Prompt electrons = " << genElectrons.size() << std::endl;
	std::cout << "Prompt muons     = " << genMuons.size() << std::endl;
	std::cout << "Prompt taus      = " << genTaus.size() << std::endl;
	std::cout << "Prompt vis taus  = " << genVisTaus.size() << std::endl;
	printf("gen Z Boson   -> px = %7.1f  py = %7.1f  pz = %7.1f\n",genZ.Px(),genZ.Py(),genZ.Pz());
	printf("gen muons     -> px = %7.1f  py = %7.1f  pz = %7.1f\n",genMuonsLV.Px(),genMuonsLV.Py(),genMuonsLV.Pz());
	printf("gen electrons -> px = %7.1f  py = %7.1f  pz = %7.1f\n",genElectronsLV.Px(),genElectronsLV.Py(),genElectronsLV.Pz());
	printf("gen taus      -> px = %7.1f  py = %7.1f  pz = %7.1f\n",genTausLV.Px(),genTausLV.Py(),genTausLV.Pz());
	printf("gen vis taus  -> px = %7.1f  py = %7.1f  pz = %7.1f\n",genVisTausLV.Px(),genVisTausLV.Py(),genVisTausLV.Pz());
	std::cout << std::endl;
	*/
	float genZPt = -1.0;
	float genZMass = -1.0;

	if (genElectrons.size()==2) {
	  genZPt   = genElectronsLV.Pt();
	  genZMass = genElectronsLV.M();
	}
	else if (genMuons.size()==2) {
	  genZPt   = genMuonsLV.Pt();
          genZMass = genMuonsLV.M();
	}
	else if (genTaus.size()==2) {
          genZPt   = genTausLV.Pt();
          genZMass = genTausLV.M();
        }

	massZH->Fill(genZMass,weight);
	ptZH->Fill(genZPt,weight);
	massPtZH->Fill(genZMass,genZPt,weight);

	if (applyZMassPtReweighting) {
	  if (genZMass>1000) genZMass=999;
	  if (genZPt>1000) genZPt=999;
	  if (genZMass>50.0&&genZPt>0.0) {
	    float dyWeight = 1;
	    if (interpolateZMassPtWeight) 
	      dyWeight = histZMassPtWeights->Interpolate(genZMass,genZPt);
	    else 
	      dyWeight = histZMassPtWeights->GetBinContent(histZMassPtWeights->FindBin(genZMass,genZPt));

	    //	    std::cout << "Z mass = " << genZMass << "   Z Pt = " << genZPt << "   weight = " << dyWeight << std::endl;
	    weight *= dyWeight;
	  }

	}
	
	//	cout << "PUinteractions = " << analysisTree.numtruepileupinteractions << endl;

	if (applyPUreweighting_vertices) {
	  int binNvert = vertexDataH->FindBin(analysisTree.primvertex_count);
	  float_t dataNvert = vertexDataH->GetBinContent(binNvert);
	  float_t mcNvert = vertexMcH->GetBinContent(binNvert);
	  if (mcNvert < 1e-10){mcNvert=1e-10;}
	  float_t vertWeight = dataNvert/mcNvert;
	  weight *= vertWeight;
	  //	  cout << "NVert = " << analysisTree.primvertex_count << "   weight = " << vertWeight << endl;
	}

        if (applyPUreweighting_official) {
	  nTruePUInteractionsH->Fill(analysisTree.numtruepileupinteractions,weight);
	  double Ninteractions = analysisTree.numtruepileupinteractions;
	  double PUweight = PUofficial->get_PUweight(Ninteractions);
	  weight *= float(PUweight);
	  PUweightsOfficialH->Fill(PUweight);
	  //	  cout << PUweight << endl;
        }

	if (applyTauTauSelection) {
	  unsigned int nTaus = 0;
	  if (analysisTree.gentau_count>0) {
	    //	  cout << "Generated taus present" << endl;
	    for (unsigned int itau = 0; itau < analysisTree.gentau_count; ++itau) {
	      //	    cout << itau << "  : pt = " 
	      //		 << analysisTree.gentau_visible_pt[itau] 
	      //		 << "   eta = " <<  analysisTree.gentau_visible_eta[itau]
	      //		 << "   mother = " << int(analysisTree.gentau_mother[itau]) << endl;
	      if (int(analysisTree.gentau_mother[itau])==3) nTaus++;
	      
	    }
	  }
	  bool notTauTau = nTaus < 2;
	  //	  std::cout << "nTaus = " << nTaus << std::endl;
	  
	  if (selectZToTauTauMuMu&&notTauTau) { 
	    //	    std::cout << "Skipping event..." << std::endl;
	    //	    cout << endl;
	    continue;
	  }
	  if (!selectZToTauTauMuMu&&!notTauTau) { 
	    //	    std::cout << "Skipping event..." << std::endl;
	    //	    cout << endl;
	    continue;
	  }
	  //	  cout << endl;
	}
	
	if (applyTopPtReweighting) {
	  float topPt = -1;
	  float antitopPt = -1;
	  for (unsigned int igen=0; igen < analysisTree.genparticles_count; ++igen) {

	    if (analysisTree.genparticles_pdgid[igen]==6)
	      topPt = TMath::Sqrt(analysisTree.genparticles_px[igen]*analysisTree.genparticles_px[igen]+
				  analysisTree.genparticles_py[igen]*analysisTree.genparticles_py[igen]);
	    
	    if (analysisTree.genparticles_pdgid[igen]==-6)
	      antitopPt = TMath::Sqrt(analysisTree.genparticles_px[igen]*analysisTree.genparticles_px[igen]+
				      analysisTree.genparticles_py[igen]*analysisTree.genparticles_py[igen]);
	    
	    
	  }
	  if (topPt>0&&antitopPt>0) {
	    float topptweight = topPtWeight(topPt,antitopPt);
	    //	    cout << "toppt = " << topPt << "   antitoppt = " << antitopPt << "   weight = " << topptweight << endl;
	    weight *= topptweight;
	  }
	}
      }

      
      if (analysisTree.event_run<RunMin)
	RunMin = analysisTree.event_run;
      
      if (analysisTree.event_run>RunMax)
	RunMax = analysisTree.event_run;
      
      //      continue;
      //      std::cout << "Event " << analysisTree.event_nr << std::endl;

      if (isData && applyGoodRunSelection){
	
	
	bool lumi = false;
	int n=analysisTree.event_run;
	int lum = analysisTree.event_luminosityblock;
	
	std::string num = std::to_string(n);
	std::string lnum = std::to_string(lum);
	for(const auto& a : periods)
	  {
	    
	    if ( num.c_str() ==  a.name ) {
	      //std::cout<< " Eureka "<<num<<"  "<<a.name<<" ";
	      //     std::cout <<"min "<< last->lower << "- max last " << last->bigger << std::endl;
	      
	      for(auto b = a.ranges.begin(); b != std::prev(a.ranges.end()); ++b) {
		
		//	cout<<b->lower<<"  "<<b->bigger<<endl;
		if (lum  >= b->lower && lum <= b->bigger ) lumi = true;
	      }
	      auto last = std::prev(a.ranges.end());
	      //    std::cout <<"min "<< last->lower << "- max last " << last->bigger << std::endl;
	      if (  (lum >=last->lower && lum <= last->bigger )) lumi=true;
	      
	      
	    }
	    
	  }
    
	if (!lumi) continue;
	//if (lumi ) cout<<"  =============  Found good run"<<"  "<<n<<"  "<<lum<<endl;
	//std::remove("myinputfile");
      }     

      //      std::cout << "Event passed good run selection " << std::endl;

      bool isNewRun = true;
      if (allRuns.size()>0) {
	for (unsigned int iR=0; iR<allRuns.size(); ++iR) {
	  if (analysisTree.event_run==allRuns.at(iR)) {
	    isNewRun = false;
	    break;
	  }
	}
      }
	
      if (isNewRun) 
	allRuns.push_back(analysisTree.event_run);
      
      bool isTriggerMuon = false;
      for (std::map<string,int>::iterator it=analysisTree.hltriggerresults->begin(); it!=analysisTree.hltriggerresults->end(); ++it) {
	TString trigName(it->first);
	if (trigName.Contains(MuonTriggerName)) {
	  //	  std::cout << it->first << " : " << it->second << std::endl;
	  if (it->second==1)
	    isTriggerMuon = true;
	}
      }

      if (applyTrigger && !isTriggerMuon) continue;
      //      std::cout << "Event passed trigger" << std::endl;


      unsigned int nMuonFilter = 0;
      bool isMuonFilter = false;

      unsigned int nMuon17Filter = 0;
      bool isMuon17Filter = false;

      unsigned int nMuon8Filter = 0;
      bool isMuon8Filter = false;

      unsigned int nSingleMuonFilter = 0;
      bool isSingleMuonFilter = false;

      unsigned int nfilters = analysisTree.run_hltfilters->size();
      for (unsigned int i=0; i<nfilters; ++i) {
	TString HLTFilter(analysisTree.run_hltfilters->at(i));
	//	std::cout << HLTFilter << std::endl;
	if (HLTFilter==MuonFilterName) {
	  nMuonFilter = i;
	  isMuonFilter = true;
	}
	if (HLTFilter==Muon17FilterName) {
	  nMuon17Filter = i;
	  isMuon17Filter = true;
	}
	if (HLTFilter==Muon8FilterName1||HLTFilter==Muon8FilterName2) {
	  nMuon8Filter = i;
	  isMuon8Filter = true;
	}
	if (HLTFilter==SingleMuonFilterName) {
          nSingleMuonFilter = i;
          isSingleMuonFilter = true;
        }
      }
      if (applyTrigger) {
	if (!isMuonFilter) {
	  cout << "Filter " << MuonFilterName << " not found " << endl;
	  continue;
	}
	if (!isMuon17Filter) {
	  cout << "Filter " << Muon17FilterName << " not found " << endl;
	  //	  continue;
	}
	if (!isMuon8Filter) {
	  cout << "Filter " << Muon8FilterName1 << " or " << Muon8FilterName2 << " not found " << endl;
	  //	  continue;
	}
	if (!isSingleMuonFilter) {
	  cout << "Filter " << SingleMuonFilterName << " not found " << endl;
	  continue;
	}
      }
      //      std::cout << "Filters found " << std::endl;

      float pfmet_ex = analysisTree.pfmetcorr_ex;
      float pfmet_ey = analysisTree.pfmetcorr_ey;
      float pfmet_phi = analysisTree.pfmetcorr_phi;
      if (!isData) {
	pfmet_ex = analysisTree.pfmet_ex;
	pfmet_ey = analysisTree.pfmet_ey;
	pfmet_phi = analysisTree.pfmet_phi;
      }

      float pfmet = TMath::Sqrt(pfmet_ex*pfmet_ex+pfmet_ey*pfmet_ey);
      //      TLorentzVector MetLV; MetLV.SetPx(pfmet_ex); MetLV.SetPy(pfmet_ey);
      //      std::cout << "pfmet = " << pfmet << " : " << analysisTree.pfmet_pt << " : " << MetLV.Pt() << std::endl;

      float puppimet_ex = analysisTree.puppimet_ex;
      float puppimet_ey = analysisTree.puppimet_ey;
      float puppimet_phi = analysisTree.puppimet_phi;
      float puppimet = TMath::Sqrt(puppimet_ex*puppimet_ex+puppimet_ey*puppimet_ey);
      
      // vertex cuts
      //      if (fabs(analysisTree.primvertex_z)>zVertexCut) continue;
      //      if (analysisTree.primvertex_ndof<ndofVertexCut) continue;
      //      float dVertex = (analysisTree.primvertex_x*analysisTree.primvertex_x+
      //		       analysisTree.primvertex_y*analysisTree.primvertex_y);
      //      if (dVertex>dVertexCut) continue;
      
      // muon selection

      vector<unsigned int> allMuons; allMuons.clear();
      vector<unsigned int> idMuons; idMuons.clear();
      vector<unsigned int> isoMuons; isoMuons.clear();
      vector<float> isoMuonsValue; isoMuonsValue.clear();
      vector<float> allMuonsIso; allMuonsIso.clear();
      vector<bool> isMuonPassedIdIso; isMuonPassedIdIso.clear();
      vector<bool> isMuonMatched23Filter; isMuonMatched23Filter.clear();
      vector<bool> isMuonMatched17Filter; isMuonMatched17Filter.clear();
      vector<bool> isMuonMatched8Filter; isMuonMatched8Filter.clear();
      vector<bool> isMuonMatchedSingleMuFilter; isMuonMatchedSingleMuFilter.clear();

      //      std::cout << "number of muons = " << analysisTree.muon_count << std::endl;

      for (unsigned int im = 0; im<analysisTree.muon_count; ++im) {
	if (!isData&&fabs(muonMomScale-1.0)>0.001) {
	  analysisTree.muon_px[im] *= muonMomScale;
	  analysisTree.muon_py[im] *= muonMomScale;
	  analysisTree.muon_pz[im] *= muonMomScale;
	  analysisTree.muon_pt[im] *= muonMomScale;
	} 
	bool muPassed    = true;
	bool mu23Matched = false;
	bool mu17Matched = false;
	bool mu8Matched  = false;
	bool muSingleMatched = false;
	if (analysisTree.muon_pt[im]<ptBins[0]) continue;
	if (fabs(analysisTree.muon_eta[im])>etaBins[nEtaBins]) continue;
	if (fabs(analysisTree.muon_dxy[im])>dxyMuonLooseCut) continue;
	if (fabs(analysisTree.muon_dz[im])>dzMuonLooseCut) continue;
	allMuons.push_back(im);
	if (fabs(analysisTree.muon_dxy[im])>dxyMuonCut) muPassed = false;
	if (fabs(analysisTree.muon_dz[im])>dzMuonCut) muPassed = false;
	//	bool goodGlob =
	//          analysisTree.muon_isGlobal[im] &&
	//          analysisTree.muon_normChi2[im] < 3 &&
	//          analysisTree.muon_combQ_chi2LocalPosition[im] < 12 &&
	//          analysisTree.muon_combQ_trkKink[im] < 20;
	//	bool ichepMed = 
	//	  analysisTree.muon_isLoose[im] &&
	//          analysisTree.muon_validFraction[im] >0.49 &&
	//          analysisTree.muon_segmentComp[im] > (goodGlob ? 0.303 : 0.451);
	//	bool ichepMed = analysisTree.muon_isICHEP[im];
	bool ichepMed = analysisTree.muon_isMedium[im];
	if (!ichepMed) muPassed = false;
	//	if (!analysisTree.muon_isMedium[im]) muPassed = false;
	if (muPassed) idMuons.push_back(im);
	float absIso = 0;
	if (isoDR03) { 
	  absIso = analysisTree.muon_r03_sumChargedHadronPt[im];
	  float neutralIso = analysisTree.muon_r03_sumNeutralHadronEt[im] + 
	    analysisTree.muon_r03_sumPhotonEt[im] - 
	    0.5*analysisTree.muon_r03_sumPUPt[im];
	  neutralIso = TMath::Max(float(0),neutralIso); 
	  absIso += neutralIso;
	}
	else {
	  absIso = analysisTree.muon_chargedHadIso[im];
          float neutralIso = analysisTree.muon_neutralHadIso[im] +
            analysisTree.muon_photonIso[im] -
            0.5*analysisTree.muon_puIso[im];
          neutralIso = TMath::Max(float(0),neutralIso);
          absIso += neutralIso;
	}
	float relIso = absIso/analysisTree.muon_pt[im];
	allMuonsIso.push_back(relIso);
	if (muPassed && relIso<isoMuonCut) { 
	  isoMuons.push_back(im);
	  isoMuonsValue.push_back(relIso);
	}
	if (relIso>isoMuonProbeCut) muPassed = false;
	isMuonPassedIdIso.push_back(muPassed);
	for (unsigned int iT=0; iT<analysisTree.trigobject_count; ++iT) {
	  float dRtrig = deltaR(analysisTree.muon_eta[im],analysisTree.muon_phi[im],
				analysisTree.trigobject_eta[iT],analysisTree.trigobject_phi[iT]);
	  if (dRtrig>DRTrigMatch) continue;
	  if (analysisTree.trigobject_filters[iT][nMuon17Filter]&&analysisTree.trigobject_pt[iT]>23) 
	    mu23Matched = true;
	  if (analysisTree.trigobject_filters[iT][nMuon17Filter])
	    mu17Matched = true;
	  if (analysisTree.trigobject_filters[iT][nSingleMuonFilter] 
	      && analysisTree.trigobject_pt[iT]>singleMuonTriggerPtCut
	      && fabs(analysisTree.trigobject_eta[iT])<singleMuonTriggerEtaCut)
	    muSingleMatched = true;
	  if (analysisTree.trigobject_filters[iT][nMuon8Filter]) 
	    mu8Matched = true;
	  if (!applyTrigger) {
	    mu23Matched = true;
	    mu17Matched = true;
	    mu8Matched = true;
	    muSingleMatched = true;
	  }

	}
	isMuonMatched23Filter.push_back(mu23Matched);
	isMuonMatched17Filter.push_back(mu17Matched);
	isMuonMatched8Filter.push_back(mu8Matched);
	isMuonMatchedSingleMuFilter.push_back(muSingleMatched);
      }
      //      std::cout << "muons processed" << std::endl;

      // MC study
      vector<unsigned int> promptMuons; promptMuons.clear();
      vector<unsigned int> nonpromptMuons; nonpromptMuons.clear();
      //      std::cout << "GenParticles = " << analysisTree.genparticles_count << std::endl;
      if (!isData) {
	for (unsigned int igen=0; igen<analysisTree.genparticles_count; ++igen) {
	  if (fabs(analysisTree.genparticles_pdgid[igen])==13&&analysisTree.genparticles_status[igen]==1) {
	    //	  float pxGen = analysisTree.genparticles_px[igen];
	    //	  float pyGen = analysisTree.genparticles_py[igen];
	    //	  float pzGen = analysisTree.genparticles_pz[igen];
	    //	  float etaGen = PtoEta(pxGen,pyGen,pzGen);
	    //	  float ptGen  = PtoPt(pxGen,pyGen);
	    //	  std::cout << analysisTree.genparticles_pdgid[igen] << " : "
	    //		    << "  pt = " << ptGen
	    //		    << "  eta = " << etaGen
	    //		    << "  info = " << analysisTree.genparticles_info[igen] << std::endl;
	    if (analysisTree.genparticles_info[igen]==1)
	      promptMuons.push_back(igen);
	    if (analysisTree.genparticles_info[igen]==5)
	      nonpromptMuons.push_back(igen);
	  }
	}
      }	

      vector<bool> isMuonPrompt; isMuonPrompt.clear();
      for (unsigned int iRecoMuons=0; iRecoMuons<allMuons.size(); iRecoMuons++) {
	unsigned int irec = allMuons[iRecoMuons];
	bool isPrompt = false;
	TLorentzVector recMuon; recMuon.SetXYZM(analysisTree.muon_px[irec],
						analysisTree.muon_py[irec],
						analysisTree.muon_pz[irec],
						muonMass);

	for (unsigned int iMuons=0; iMuons<promptMuons.size(); ++iMuons) {
	  unsigned int igen = promptMuons[iMuons];
	  TLorentzVector genMuon; genMuon.SetXYZM(analysisTree.genparticles_px[igen],
						  analysisTree.genparticles_py[igen],
						  analysisTree.genparticles_pz[igen],
						  muonMass);
	  
	  
	  float relativeDifference = (genMuon-recMuon).P()/genMuon.P();
	  if (relativeDifference<0.05) {
	    isPrompt = true;
	    float absEtaProbe = TMath::Abs(genMuon.Eta());
	    if (absEtaProbe>=etaMuonLowCut) absEtaProbe = fabs(etaBins[nEtaBins]) - 0.01;
	    int iEta = binNumber(absEtaProbe,nEtaBins,etaBins);
	    if (isMuonPassedIdIso[iRecoMuons]) 
	      PromptPtPass[iEta]->Fill(genMuon.Pt(),weight);
	    else
	      PromptPtFail[iEta]->Fill(genMuon.Pt(),weight);
	  }
	}
	isMuonPrompt.push_back(isPrompt);
      }

      vector<bool> isMuonNonPrompt; isMuonNonPrompt.clear();
      for (unsigned int iRecoMuons=0; iRecoMuons<allMuons.size(); iRecoMuons++) {
	unsigned int irec = allMuons[iRecoMuons];
	bool isNonPrompt = false;
	TLorentzVector recMuon; recMuon.SetXYZM(analysisTree.muon_px[irec],
						analysisTree.muon_py[irec],
						analysisTree.muon_pz[irec],
						muonMass);

	for (unsigned int iMuons=0; iMuons<nonpromptMuons.size(); ++iMuons) {
	  unsigned int igen = nonpromptMuons[iMuons];
	  TLorentzVector genMuon; genMuon.SetXYZM(analysisTree.genparticles_px[igen],
						  analysisTree.genparticles_py[igen],
						  analysisTree.genparticles_pz[igen],
						  muonMass);
	  
	  
	  float relativeDifference = (genMuon-recMuon).P()/genMuon.P();
	  if (relativeDifference<0.05) {
	    isNonPrompt = true;
	    float absEtaProbe = TMath::Abs(genMuon.Eta());
	    if (absEtaProbe>=etaMuonLowCut) absEtaProbe = fabs(etaBins[nEtaBins]) - 0.01;
            int iEta = binNumber(absEtaProbe,nEtaBins,etaBins);
 	    if (isMuonPassedIdIso[iRecoMuons]) 
	      NonPromptPtPass[iEta]->Fill(genMuon.Pt(),weight);
	    else
	      NonPromptPtFail[iEta]->Fill(genMuon.Pt(),weight);
	  }
	}
	isMuonNonPrompt.push_back(isNonPrompt);
      }


      unsigned int indx1 = 0;
      unsigned int indx2 = 0;
      bool isIsoMuonsPair = false;
      bool firstTrigger = true;
      float isoMin = 9999;
      if (isoMuons.size()>0) {
	for (unsigned int im1=0; im1<isoMuons.size(); ++im1) {
	  unsigned int index1 = isoMuons[im1];
	  bool isMu1matched = false;
	  for (unsigned int iT=0; iT<analysisTree.trigobject_count; ++iT) {
	    float dRtrig = deltaR(analysisTree.muon_eta[index1],analysisTree.muon_phi[index1],
				  analysisTree.trigobject_eta[iT],analysisTree.trigobject_phi[iT]);
	    if (dRtrig>DRTrigMatch) continue;
	    if (analysisTree.trigobject_filters[iT][nMuonFilter] && 
		analysisTree.muon_pt[index1] > ptMuonHighCut &&
		fabs(analysisTree.muon_eta[index1]) < etaMuonHighCut) 
	      isMu1matched = true;
	  }
	  if (!applyTrigger && 
	      analysisTree.muon_pt[index1] > ptMuonHighCut && 
	      fabs(analysisTree.muon_eta[index1]) < etaMuonHighCut) isMu1matched = true;
	  if (isMu1matched) {
	    for (unsigned int iMu=0; iMu<allMuons.size(); ++iMu) {
	      unsigned int indexProbe = allMuons[iMu];
	      if (index1==indexProbe) continue;
	      float q1 = analysisTree.muon_charge[index1];
	      float q2 = analysisTree.muon_charge[indexProbe];
	      if (q1*q2>0) continue;
	      float dR = deltaR(analysisTree.muon_eta[index1],analysisTree.muon_phi[index1],
				analysisTree.muon_eta[indexProbe],analysisTree.muon_phi[indexProbe]);
	      if (dR<dRleptonsCut) continue; 
	      float dPhi = dPhiFrom2P(analysisTree.muon_px[index1],analysisTree.muon_py[index1],
				      analysisTree.muon_px[indexProbe],analysisTree.muon_py[indexProbe]);
	      if (dPhi>dPhileptonsCut) continue;
	      float ptProbe = TMath::Min(float(analysisTree.muon_pt[indexProbe]),float(ptBins[nPtBins]-0.1));
	      float absEtaProbe = fabs(analysisTree.muon_eta[indexProbe]);
	      int ptBin = binNumber(ptProbe,nPtBins,ptBins);
	      if (ptBin<0) continue;
	      int ptBinTrig = binNumber(ptProbe,nPtBinsTrig,ptBinsTrig);
	      if (ptBinTrig<0) continue;
	      int etaBin = binNumber(absEtaProbe,nEtaBins,etaBins);
	      if (etaBin<0) continue;

	      TLorentzVector muon1; muon1.SetXYZM(analysisTree.muon_px[index1],
						  analysisTree.muon_py[index1],
						  analysisTree.muon_pz[index1],
						  muonMass);
	      TLorentzVector muon2; muon2.SetXYZM(analysisTree.muon_px[indexProbe],
						  analysisTree.muon_py[indexProbe],
						  analysisTree.muon_pz[indexProbe],
						  muonMass);
	      // number of jets
	      int nJets30 = 0;
	      int nJets30etaCut = 0;

	      for (unsigned int jet=0; jet<analysisTree.pfjet_count; ++jet) {
		float absJetEta = fabs(analysisTree.pfjet_eta[jet]);
		if (absJetEta>jetEtaCut) continue;
	  
		float dR1 = deltaR(analysisTree.pfjet_eta[jet],analysisTree.pfjet_phi[jet],
				   analysisTree.muon_eta[index1],analysisTree.muon_phi[index1]);
		if (dR1<dRJetLeptonCut) continue;
		
		float dR2 = deltaR(analysisTree.pfjet_eta[jet],analysisTree.pfjet_phi[jet],
				   analysisTree.muon_eta[indexProbe],analysisTree.muon_phi[indexProbe]);
		
		if (dR2<dRJetLeptonCut) continue;
	  
		// pfJetId
		bool isPFJetId = looseJetiD(analysisTree,int(jet));
		if (!isPFJetId) continue;
		
		if (analysisTree.pfjet_pt[jet]>jetPtHighCut) {
		  nJets30++;
		  if (fabs(analysisTree.pfjet_eta[jet])<jetEtaTrkCut) {
		    nJets30etaCut++;
		  }
		}

	      }	 

	      int JetBin = nJets30etaCut;
	      if (JetBin>2) JetBin = 2;
 
	      float mass = (muon1+muon2).M();
	      if (isMuonPassedIdIso[iMu]) { 
		ZMassPass->Fill(mass,weight);
		ZMassEtaPtPass[etaBin][ptBin]->Fill(mass,weight);
		ZMassJetEtaPtPass[etaBin][JetBin][ptBin]->Fill(mass,weight);
		if (isMuonPrompt[iMu]) {
		  PromptSelPtPass[etaBin]->Fill(ptProbe,weight); 
		  PromptSelJetPtPass[etaBin][JetBin]->Fill(ptProbe,weight);
		}
		if (isMuonNonPrompt[iMu]) {
		  NonPromptSelPtPass[etaBin]->Fill(ptProbe,weight);
		  NonPromptSelJetPtPass[etaBin][JetBin]->Fill(ptProbe,weight);
		}
		// muon23 filter
		if (isMuonMatched23Filter[iMu]) 
		  ZMassMu23EtaPtPass[etaBin][ptBinTrig]->Fill(mass,weight); 
		else
		  ZMassMu23EtaPtFail[etaBin][ptBinTrig]->Fill(mass,weight); 
		// muon17 filter
		if (isMuonMatched17Filter[iMu]) 
		  ZMassMu17EtaPtPass[etaBin][ptBinTrig]->Fill(mass,weight); 
		else
		  ZMassMu17EtaPtFail[etaBin][ptBinTrig]->Fill(mass,weight); 
		// muon8 filter
		if (isMuonMatched8Filter[iMu]) 
		  ZMassMu8EtaPtPass[etaBin][ptBinTrig]->Fill(mass,weight); 
		else
		  ZMassMu8EtaPtFail[etaBin][ptBinTrig]->Fill(mass,weight); 
		// single muon filter
		if (absEtaProbe<singleMuonTriggerEtaCut) { 
		  if (isMuonMatchedSingleMuFilter[iMu]) 
		    ZMassIsoMuEtaPtPass[etaBin][ptBinTrig]->Fill(mass,weight); 
		  else
		    ZMassIsoMuEtaPtFail[etaBin][ptBinTrig]->Fill(mass,weight);
		}
	      }
	      else {
		ZMassFail->Fill(mass,weight);
		ZMassEtaPtFail[etaBin][ptBin]->Fill(mass,weight);
		ZMassJetEtaPtFail[etaBin][JetBin][ptBin]->Fill(mass,weight);
		if (isMuonPrompt[iMu]) {
                  PromptSelPtFail[etaBin]->Fill(ptProbe,weight);
                  PromptSelJetPtFail[etaBin][JetBin]->Fill(ptProbe,weight);
                }
                if (isMuonNonPrompt[iMu]) {
                  NonPromptSelPtFail[etaBin]->Fill(ptProbe,weight);
                  NonPromptSelJetPtFail[etaBin][JetBin]->Fill(ptProbe,weight);
                }
		// filling events tree
		if (!isData&&etaBin==0&&ptBin==6) {

		  eventNumber = analysisTree.event_nr;
		  runNumber = analysisTree.event_run;
		  IsoMu17 = isTriggerMuon;

		  ptTag = analysisTree.muon_pt[index1];
		  etaTag = analysisTree.muon_eta[index1];
		  phiTag = analysisTree.muon_phi[index1];
		  dxyTag = analysisTree.muon_dxy[index1];
		  dzTag = analysisTree.muon_dz[index1];
		  chargeTag = analysisTree.muon_charge[index1];
		  isoTag = isoMuonsValue[im1];
		  //		  isMediumTag = isICHEPmed(index1,analysisTree);
		  //		  isMediumTag = analysisTree.muon_isMedium[index1];
		  bool goodGlobTag =
		    analysisTree.muon_isGlobal[index1] &&
		    analysisTree.muon_normChi2[index1] < 3 &&
		    analysisTree.muon_combQ_chi2LocalPosition[index1] < 12 &&
		    analysisTree.muon_combQ_trkKink[index1] < 20;
		  isMediumTag =
		    analysisTree.muon_isLoose[index1] &&
		    analysisTree.muon_validFraction[index1] >0.49 &&
		    analysisTree.muon_segmentComp[index1] > (goodGlobTag ? 0.303 : 0.451);
		  
		  ptProbe = analysisTree.muon_pt[indexProbe];
		  etaProbe = analysisTree.muon_eta[indexProbe];
		  phiProbe = analysisTree.muon_phi[indexProbe];
		  dxyProbe = analysisTree.muon_dxy[indexProbe];
		  dzProbe = analysisTree.muon_dz[indexProbe];
		  chargeProbe = analysisTree.muon_charge[indexProbe];
		  isoProbe = allMuonsIso[iMu];
		  //		  isMediumProbe = isICHEPmed(indexProbe,analysisTree);
		  //		  isMediumProbe = analysisTree.muon_isMedium[indexProbe];;
		  bool goodGlobProbe =
                    analysisTree.muon_isGlobal[indexProbe] &&
                    analysisTree.muon_normChi2[indexProbe] < 3 &&
                    analysisTree.muon_combQ_chi2LocalPosition[indexProbe] < 12 &&
                    analysisTree.muon_combQ_trkKink[indexProbe] < 20;
                  isMediumProbe =
                    analysisTree.muon_isLoose[indexProbe] &&
                    analysisTree.muon_validFraction[indexProbe] >0.49 &&
                    analysisTree.muon_segmentComp[indexProbe] > (goodGlobProbe ? 0.303 : 0.451);
		  
		  massTagProbe = mass;
		  //		  events->Fill();
		}
	      }
	    }
	  }
	  for (unsigned int im2=im1+1; im2<isoMuons.size(); ++im2) {
	    unsigned int index2 = isoMuons[im2];
	    float q1 = analysisTree.muon_charge[index1];
	    float q2 = analysisTree.muon_charge[index2];
	    bool isMu2matched = false;
	    for (unsigned int iT=0; iT<analysisTree.trigobject_count; ++iT) {
	      float dRtrig = deltaR(analysisTree.muon_eta[index2],analysisTree.muon_phi[index2],
				    analysisTree.trigobject_eta[iT],analysisTree.trigobject_phi[iT]);
	      if (dRtrig>DRTrigMatch) continue;
	      if (analysisTree.trigobject_filters[iT][nMuonFilter] && 
		  analysisTree.muon_pt[index2] > ptMuonHighCut &&
		  fabs(analysisTree.muon_eta[index2]) < etaMuonHighCut) 
		isMu2matched = true;
	    }
	    if (!applyTrigger && 
		analysisTree.muon_pt[index2] > ptMuonHighCut &&
		fabs(analysisTree.muon_eta[index2]) < etaMuonHighCut) isMu2matched = true;
	    bool isPairSelected = q1*q2 > 0;
	    if (oppositeSign) isPairSelected = q1*q2 < 0;
	    //	    bool isTriggerMatch = (isMu1matched || isMu2matched);
	    bool isTriggerMatch = false;
	    if (analysisTree.muon_pt[index1]>analysisTree.muon_pt[index2]) {
	      isTriggerMatch = isMu1matched;
	      firstTrigger = true;
	    }
	    else {
	      isTriggerMatch = isMu2matched;
	      firstTrigger = false;
	    }
	    float dRmumu = deltaR(analysisTree.muon_eta[index1],analysisTree.muon_phi[index1],
				  analysisTree.muon_eta[index2],analysisTree.muon_phi[index2]);
	    if (isTriggerMatch && isPairSelected && dRmumu>dRleptonsCut) {
	      bool sumIso = isoMuonsValue[im1]+isoMuonsValue[im2];
	      if (sumIso<isoMin) {
		isIsoMuonsPair = true;
		isoMin = sumIso;
		if (analysisTree.muon_pt[index1]>analysisTree.muon_pt[index2]) {
		  indx1 = index1;
		  indx2 = index2;
		}
		else {
		  indx2 = index1;
		  indx1 = index2;
		}
	      }
	    }
	  }
	}
      }
      //      std::cout << "isolated pair " << isIsoMuonsPair << std::endl;


      if (isIsoMuonsPair) {      
	//match to genparticles
	bool genmatch_m1 = false, genmatch_m2 = false;

	TLorentzVector mu1; mu1.SetXYZM(analysisTree.muon_px[indx1],
					analysisTree.muon_py[indx1],
					analysisTree.muon_pz[indx1],
					muonMass);

	TLorentzVector mu2; mu2.SetXYZM(analysisTree.muon_px[indx2],
					analysisTree.muon_py[indx2],
					analysisTree.muon_pz[indx2],
					muonMass);

	TLorentzVector dimuon = mu1 + mu2;

	float massSel = dimuon.M();
	//	cout << "massSel = " << massSel << endl;
	if (!isData) {
	
	  for (unsigned int igen=0; igen<analysisTree.genparticles_count; ++igen) {
	    if (fabs(analysisTree.genparticles_pdgid[igen])==13 &&
		analysisTree.genparticles_status[igen]==1) {
	      
	      TLorentzVector gen_mu1; gen_mu1.SetPxPyPzE(analysisTree.genparticles_px[igen],
							 analysisTree.genparticles_py[igen],
							 analysisTree.genparticles_pz[igen],
							 analysisTree.genparticles_e[igen]);
	      TLorentzVector gen_mu2; gen_mu2.SetPxPyPzE(analysisTree.genparticles_px[igen],
							 analysisTree.genparticles_py[igen],
							 analysisTree.genparticles_pz[igen],
							 analysisTree.genparticles_e[igen]);
	      double deltaR_mu1 = deltaR(gen_mu1.Eta(), gen_mu1.Phi(),
					 analysisTree.muon_eta[indx1],analysisTree.muon_phi[indx1]);
	      if(deltaR_mu1 < 0.3)genmatch_m1 = true;
	      
	      double deltaR_mu2 = deltaR(gen_mu2.Eta(), gen_mu2.Phi(),
					 analysisTree.muon_eta[indx2],analysisTree.muon_phi[indx2]);
	      if(deltaR_mu2 < 0.3)genmatch_m2 = true;
	      
	    }
	  }
	}

	// accessing Mva Met
	/*
	bool mvaMetFound = false;
	unsigned int metMuMu = 0; 
	for (unsigned int iMet=0; iMet<analysisTree.mvamet_count; ++iMet) {
	  if (analysisTree.mvamet_channel[iMet]==5) {
	    if (analysisTree.mvamet_lep1[iMet]==indx1&&
		analysisTree.mvamet_lep2[iMet]==indx2) {
	      metMuMu = iMet;
	      mvaMetFound = true;
	    }
	  }
	}
	if (!mvaMetFound)
	  std::cout << "MVA MEt not found" << std::endl;
	//	else
	//	  std::cout << "MVA MEt found" << std::endl;
	float mvamet = 0;
	float mvamet_phi = 0;
	float mvamet_ex = 0;
	float mvamet_ey = 0;
	if (analysisTree.mvamet_count>0) {
	  mvamet_ex = analysisTree.mvamet_ex[metMuMu];
	  mvamet_ey = analysisTree.mvamet_ey[metMuMu];
	  float mvamet_ex2 = mvamet_ex * mvamet_ex;
	  float mvamet_ey2 = mvamet_ey * mvamet_ey;

	  mvamet = TMath::Sqrt(mvamet_ex2+mvamet_ey2);
	  mvamet_phi = TMath::ATan2(mvamet_ey,mvamet_ex);
	}
	*/
	float mvamet     = analysisTree.pfmet_pt;
	float mvamet_ex  = analysisTree.pfmet_ex;
	float mvamet_ey  = analysisTree.pfmet_ey;
	float mvamet_phi = TMath::ATan2(analysisTree.pfmet_ey,analysisTree.pfmet_ex);

	// selecting good jets --->

	// HT variables
	float HT30 = 0;
	float HT20 = 0;
	float HT30etaCut = 0;
	float HT20etaCut = 0;

	// number of jets
	int nJets30 = 0;
	int nJets20 = 0;
	int nJets30etaCut = 0;
	int nJets20etaCut = 0;

	int nJets30Up = 0;
	int nJets30Down = 0;
	
	// number of btag jets
	int nJets20BTagLoose = 0;
	int nJets20BTagMedium = 0;

	int indexLeadingJet = -1;
	float ptLeadingJet = -1;

	int indexSubLeadingJet = -1;
	float ptSubLeadingJet = -1;

	for (unsigned int jet=0; jet<analysisTree.pfjet_count; ++jet) {
	  //	  std::cout << analysisTree.pfjet_jecUncertainty[jet] << std::endl;
	  float scale = 1;
	  if (applyJES>0) scale = 1 + analysisTree.pfjet_jecUncertainty[jet];
	  if (applyJES<0) scale = 1 - analysisTree.pfjet_jecUncertainty[jet];
	  if (applyJES!=0&&!isData) {
	    analysisTree.pfjet_pt[jet] *= scale;
	    analysisTree.pfjet_px[jet] *= scale;
	    analysisTree.pfjet_py[jet] *= scale;
	    analysisTree.pfjet_pz[jet] *= scale;
	    analysisTree.pfjet_e[jet] *= scale;
	  }

	  float jetEta = analysisTree.pfjet_eta[jet];
	  float absJetEta = fabs(analysisTree.pfjet_eta[jet]);
	  float jetPt = analysisTree.pfjet_pt[jet];
	  if (absJetEta>jetEtaCut) continue;
	  
	  float dR1 = deltaR(analysisTree.pfjet_eta[jet],analysisTree.pfjet_phi[jet],
			     analysisTree.muon_eta[indx1],analysisTree.muon_phi[indx1]);
	  if (dR1<dRJetLeptonCut) continue;
	  
	  float dR2 = deltaR(analysisTree.pfjet_eta[jet],analysisTree.pfjet_phi[jet],
			     analysisTree.muon_eta[indx2],analysisTree.muon_phi[indx2]);
	  if (dR2<dRJetLeptonCut) continue;
	  
	  // pfJetId
	  bool isPFJetId = looseJetiD(analysisTree,int(jet));
	  if (!isPFJetId) continue;
	  
	  if (analysisTree.pfjet_pt[jet]>jetPtHighCut) {
	    nJets30++;
	    HT30 += analysisTree.pfjet_pt[jet];

	    if (indexLeadingJet>=0) {
	      if (jetPt<ptLeadingJet&&jetPt>ptSubLeadingJet) {
		indexSubLeadingJet = jet;
		ptSubLeadingJet = jetPt;
	      }
	    }

	    if (jetPt>ptLeadingJet) {
	      indexSubLeadingJet = indexLeadingJet;
	      ptSubLeadingJet = ptLeadingJet;
	      indexLeadingJet = jet;
	      ptLeadingJet = jetPt; 
	    }



	    if (fabs(analysisTree.pfjet_eta[jet])<jetEtaTrkCut) {
	      HT30etaCut += analysisTree.pfjet_pt[jet];
	      nJets30etaCut++;
	    }
	  }
	  float ptJetUp = analysisTree.pfjet_pt[jet]*(1 + analysisTree.pfjet_jecUncertainty[jet]);
	  float ptJetDown = analysisTree.pfjet_pt[jet]*(1 - analysisTree.pfjet_jecUncertainty[jet]);
	  if (ptJetUp>jetPtHighCut)
	    nJets30Up++;
	  if (ptJetDown>jetPtHighCut)
	    nJets30Down++;

	  if (analysisTree.pfjet_pt[jet]>jetPtLowCut) {
	    nJets20++;
	    HT20 += analysisTree.pfjet_pt[jet]; 

	    if (fabs(analysisTree.pfjet_eta[jet])<jetEtaTrkCut) {

              HT20etaCut += analysisTree.pfjet_pt[jet];
              nJets20etaCut++;

	      bool tagged = analysisTree.pfjet_btag[jet][0]>0.80;

	      float JetPtForBTag = analysisTree.pfjet_pt[jet];
	      float absJetEta = fabs(analysisTree.pfjet_eta[jet]);
	      if (JetPtForBTag>MaxBJetPt) JetPtForBTag = MaxBJetPt - 0.1;
	      if (JetPtForBTag<MinBJetPt) JetPtForBTag = MinBJetPt + 0.1;
	      float jet_scalefactor = 1;
	      float tageff = 1;
	      if (!isData) {
		int flavor = abs(analysisTree.pfjet_flavour[jet]);
		if (flavor==5) { // b-quark
		  jet_scalefactor = reader_B.eval_auto_bounds("central",BTagEntry::FLAV_B, absJetEta, JetPtForBTag);
		  tageff = tagEff_B->GetBinContent(tagEff_B->FindBin(JetPtForBTag,absJetEta));
		}
		else if (flavor==4) { // c-quark
		  jet_scalefactor = reader_C.eval_auto_bounds("central",BTagEntry::FLAV_C, absJetEta, JetPtForBTag);
                  tageff = tagEff_C->GetBinContent(tagEff_C->FindBin(JetPtForBTag,absJetEta));
		}
		else {
		  jet_scalefactor = reader_Light.eval_auto_bounds("central",BTagEntry::FLAV_UDSG, absJetEta, JetPtForBTag);
		  tageff = tagEff_Light->GetBinContent(tagEff_Light->FindBin(JetPtForBTag,absJetEta));
		}

		//		std::cout << "Flavor = " << flavor << " pt = " <<  JetPtForBTag << "  eta = " << jetEta 
		//			  << "  SF = " << jet_scalefactor << " tageff = " << tageff << "  tagged = " << tagged << std::endl;
		//
		if (tageff<1e-5)      tageff = 1e-5;
		if (tageff>0.99999)   tageff = 0.99999;
		rand.SetSeed((int)((jetEta+5)*100000));
		double rannum = rand.Rndm();
		if (jet_scalefactor<1 && tagged) { // downgrading
		  double fraction = 1-jet_scalefactor;
		  if (rannum<fraction) {
		    tagged = false;
		  }
		}
		if (jet_scalefactor>1 && !tagged) { // upgrading
		  double fraction = (jet_scalefactor-1.0)/(1.0/tageff-1.0);
		  if (rannum<fraction) {
		    tagged = true;
		  }
		}

		//		std::cout << "Updated taggged = " << tagged << std::endl;
	      }


	      if (analysisTree.pfjet_btag[jet][0]>0.46) 
		nJets20BTagLoose++;
	      if (tagged) 
		nJets20BTagMedium++;

            }
	  }	  


	}

	float mjj = -1;
	float etaLeadingJet = -999;
	float phiLeadingJet = -999;

	if (indexLeadingJet>=0) {
	  ptLeadingJet = analysisTree.pfjet_pt[indexLeadingJet];
	  etaLeadingJet = analysisTree.pfjet_eta[indexLeadingJet];
	  phiLeadingJet = analysisTree.pfjet_phi[indexLeadingJet];
	}

	if (indexLeadingJet>=0 && indexSubLeadingJet>=0) {


	  TLorentzVector jet1; jet1.SetPxPyPzE(analysisTree.pfjet_px[indexLeadingJet],
					       analysisTree.pfjet_py[indexLeadingJet],
					       analysisTree.pfjet_pz[indexLeadingJet],
					       analysisTree.pfjet_e[indexLeadingJet]);
	  
	  TLorentzVector jet2; jet2.SetPxPyPzE(analysisTree.pfjet_px[indexSubLeadingJet],
                                               analysisTree.pfjet_py[indexSubLeadingJet],
                                               analysisTree.pfjet_pz[indexSubLeadingJet],
                                               analysisTree.pfjet_e[indexSubLeadingJet]);
	  mjj = (jet1+jet2).M();

	}

	bool is0Jet    = nJets30==0;
	bool isBoosted = nJets30==1 || (nJets30==2 && mjj<300) || nJets30>2;
	bool isVBF     = nJets30==2 && mjj>300;

	//	std::cout << "Jet processed" << std::endl;

	if (!isData && applyLeptonSF) {

	//leptonSFweight = SF_yourScaleFactor->get_ScaleFactor(pt, eta)	
	  double ptMu1 = (double)analysisTree.muon_pt[indx1];
	  double ptMu2 = (double)analysisTree.muon_pt[indx2];
	  double etaMu1 = (double)analysisTree.muon_eta[indx1];
	  double etaMu2 = (double)analysisTree.muon_eta[indx2];
	  double IdIsoSF_mu1 = SF_muonIdIso->get_ScaleFactor(ptMu1, etaMu1);
	  double IdIsoSF_mu2 = SF_muonIdIso->get_ScaleFactor(ptMu2, etaMu2);

	  double trackSF_mu1 = trackEffMuonH->GetBinContent(trackEffMuonH->FindBin(etaMu1));
	  double trackSF_mu2 = trackEffMuonH->GetBinContent(trackEffMuonH->FindBin(etaMu2));

	  MuSF_IdIso_Mu1H->Fill(IdIsoSF_mu1);
	  MuSF_IdIso_Mu2H->Fill(IdIsoSF_mu2);
	  /*	  if (ptMu1<20||ptMu2<20) {
	  	  std::cout << "mu 1 ->  pt = " << ptMu1 << "   eta = " << etaMu1 << std::endl;
	  	  std::cout << "eff data mu 1 = " << SF_muonIdIso->get_EfficiencyData(ptMu1, etaMu1)<< " |  eff mc mu 1 = " << SF_muonIdIso->get_EfficiencyMC(ptMu1, etaMu1)<<std::endl;
	  	  std::cout << "mu 2 ->  pt = " << ptMu2 << "   eta = " << etaMu2 << std::endl;
		  std::cout << "eff data mu 2 = " << SF_muonIdIso->get_EfficiencyData(ptMu2, etaMu2)<< " |  eff mc mu 2 = " << SF_muonIdIso->get_EfficiencyMC(ptMu2, etaMu2)<<std::endl;
		  std::cout << "SF mu1 = " << IdIsoSF_mu1 << std::endl;
		  std::cout << "SF mu2 = " << IdIsoSF_mu2 << std::endl;
		  
		  std::cout << " mass = " << massSel << std::endl;
		  std::cout << std::endl;
		  } 
	  */
	  //	  weight = weight*IdIsoSF_mu1*IdIsoSF_mu2*trackSF_mu1*trackSF_mu2;
	  weight = weight*IdIsoSF_mu1*IdIsoSF_mu2;

	  double effDataTrig1 = SF_muonTrig->get_EfficiencyData(ptMu1, etaMu1);  
	  double effDataTrig2 = SF_muonTrig->get_EfficiencyData(ptMu2, etaMu2);  
	  //	  double effTrigData = 1.0 - (1-effDataTrig1)*(1-effDataTrig2);
	  double effTrigData = 1.0;
	  if (firstTrigger) 
	    effTrigData = effDataTrig1;
	  else
	    effTrigData = effDataTrig2;

	  double weightTrig = 1;
	  if (applyTrigger) {
	    double effMcTrig1 = SF_muonTrig->get_EfficiencyMC(ptMu1, etaMu1);
	    double effMcTrig2 = SF_muonTrig->get_EfficiencyMC(ptMu2, etaMu2);
	    //	    double effMcTrig = 1 - (1-effMcTrig1)*(1-effMcTrig2);
	    double effMcTrig = 1;
	    if (firstTrigger)
	      effMcTrig = effMcTrig1;
	    else
	      effMcTrig = effMcTrig2;
	    if (effTrigData>0&&effMcTrig>0) {
	      weightTrig = effTrigData/effMcTrig;
	      weight = weight*weightTrig;
	    }
	  }
	  else {
	    weightTrig = effTrigData;
	    weight = weight*weightTrig;
	  }
	  /*
	  std::cout << "mu 1 ->  pt = " << ptMu1 << "   eta = " << etaMu1 << std::endl;                                                                                      
	  std::cout << "mu 2 ->  pt = " << ptMu2 << "   eta = " << etaMu2 << std::endl;                                                                                      
	  std::cout << "WeightTrig = " << weightTrig << std::endl;
	  */
	}

	if (!isData&&applyNJetReweighting) {
	  float njetWeight = nJetsWeight(nJets30);
	  weight *= njetWeight;
	}

	float visiblePx = dimuon.Px();
	float visiblePy = dimuon.Py();
	if (applyRecoilOnGenerator) {
	  visiblePx = genL.Px();
	  visiblePy = genL.Py();
	}

	if (!isData && applyRecoilCorrections) {

	  float pfmetcorr_ex = pfmet_ex;
	  float pfmetcorr_ey = pfmet_ey;

	  // float pfmetcorr_ex_1 = pfmet_ex;
	  // float pfmetcorr_ey_1 = pfmet_ey;
	  if (applySimpleRecoilCorrections)
	    //	    recoilPFMetCorrector.CorrectByMeanResolution(pfmet_ex,pfmet_ey,genV.Px(),genV.Py(),visiblePx,visiblePy,nJets30,pfmetcorr_ex_1,pfmetcorr_ey_1);
	    recoilPFMetCorrector.CorrectByMeanResolution(pfmet_ex,pfmet_ey,genV.Px(),genV.Py(),visiblePx,visiblePy,nJets30,pfmetcorr_ex,pfmetcorr_ey);
	  else 
	    recoilPFMetCorrector.Correct(pfmet_ex,pfmet_ey,genV.Px(),genV.Py(),visiblePx,visiblePy,nJets30,pfmetcorr_ex,pfmetcorr_ey);
	  //	  std::cout << "PFMet : (" << pfmet_ex << "," << pfmet_ey << ")  "
	  //		    << "  Quantile : (" << pfmetcorr_ex << "," << pfmetcorr_ey << ")" 
	  //		    << "  MeanReso : (" << pfmetcorr_ex_1 << "," << pfmetcorr_ey_1 << ")" 
	  //		    << std::endl; 
 	  pfmet_phi = TMath::ATan2(pfmetcorr_ey,pfmetcorr_ex);
          pfmet = TMath::Sqrt(pfmetcorr_ex*pfmetcorr_ex+pfmetcorr_ey*pfmetcorr_ey);
	  pfmet_ex = pfmetcorr_ex;
	  pfmet_ey = pfmetcorr_ey;
	  
	  float puppimetcorr_ex = puppimet_ex;
          float puppimetcorr_ey = puppimet_ey;
	  //	  float puppimetcorr_ex_1 = puppimet_ex;
	  //      float puppimetcorr_ey_1 = puppimet_ey;
	  if (applySimpleRecoilCorrections)
	    //	    recoilPuppiMetCorrector.CorrectByMeanResolution(puppimet_ex,puppimet_ey,genV.Px(),genV.Py(),visiblePx,visiblePy,nJets30,puppimetcorr_ex_1,puppimetcorr_ey_1);
	    recoilPuppiMetCorrector.CorrectByMeanResolution(puppimet_ex,puppimet_ey,genV.Px(),genV.Py(),visiblePx,visiblePy,nJets30,puppimetcorr_ex,puppimetcorr_ey);
	  else 
	    recoilPuppiMetCorrector.Correct(puppimet_ex,puppimet_ey,genV.Px(),genV.Py(),visiblePx,visiblePy,nJets30,puppimetcorr_ex,puppimetcorr_ey);
	  //	  std::cout << "PuppiMet : (" << puppimet_ex << "," << puppimet_ey << ")  "
	  //		    << "  Quantile = (" << puppimetcorr_ex << "," << puppimetcorr_ey << ")" 
	  //		    << "  MeanReso = (" << puppimetcorr_ex_1 << "," << puppimetcorr_ey_1 << ")"  
	  //		    << std::endl; 
	  puppimet_phi = TMath::ATan2(puppimetcorr_ey,puppimetcorr_ex);
	  puppimet = TMath::Sqrt(puppimetcorr_ex*puppimetcorr_ex+puppimetcorr_ey*puppimetcorr_ey);
	  
	  puppimet_ex = puppimetcorr_ex;
	  puppimet_ey = puppimetcorr_ey;

          float mvametcorr_ex = mvamet_ex;
          float mvametcorr_ey = mvamet_ey;
	  //          float mvametcorr_ex_1 = mvamet_ex;
	  //          float mvametcorr_ey_1 = mvamet_ey;
	  if (applySimpleRecoilCorrections)
	    //	    recoilMvaMetCorrector.CorrectByMeanResolution(mvamet_ex,mvamet_ey,genV.Px(),genV.Py(),visiblePx,visiblePy,nJets30,mvametcorr_ex_1,mvametcorr_ey_1);
	    recoilMvaMetCorrector.CorrectByMeanResolution(mvamet_ex,mvamet_ey,genV.Px(),genV.Py(),visiblePx,visiblePy,nJets30,mvametcorr_ex,mvametcorr_ey);
	  else 
	    recoilMvaMetCorrector.Correct(mvamet_ex,mvamet_ey,genV.Px(),genV.Py(),visiblePx,visiblePy,nJets30,mvametcorr_ex,mvametcorr_ey);
	  //	  std::cout << "MvaMet : (" << mvamet_ex << "," << mvamet_ey << ")  "
	  //		    << "  Quantile = (" << mvametcorr_ex << "," << mvametcorr_ey << ")" 
	  //		    << "  MeanReso = (" << mvametcorr_ex_1 << "," << mvametcorr_ey_1 << ")"
	  //		    << std::endl;
	  //	  std::cout << std::endl;
          mvamet_phi = TMath::ATan2(mvametcorr_ey,mvametcorr_ex);
          mvamet = TMath::Sqrt(mvametcorr_ex*mvametcorr_ex+mvametcorr_ey*mvametcorr_ey);

	  mvamet_ex = mvametcorr_ex;
	  mvamet_ey = mvametcorr_ey; 

	}

	if (!isData&&fabs(metScale-1)>0.001) { // applying met scale systematics

	  float mvametcorr_ex = mvamet_ex;
	  float mvametcorr_ey = mvamet_ey;
	  metSysMva.ShiftMEt(mvamet_ex,mvamet_ey,
			     genV.Px(),genV.Py(),
			     visiblePx,visiblePy,
			     nJets30,
			     bkgdType,
			     MEtSys::SysType::Response,
			     metScale,
			     mvametcorr_ex,mvametcorr_ey);
	  mvamet_phi = TMath::ATan2(mvametcorr_ey,mvametcorr_ex);
          mvamet = TMath::Sqrt(mvametcorr_ex*mvametcorr_ex+mvametcorr_ey*mvametcorr_ey);
	  mvamet_ex = mvametcorr_ex;
	  mvamet_ey = mvametcorr_ey;
	  
	  float pfmetcorr_ex = pfmet_ex;
	  float pfmetcorr_ey = pfmet_ey;
	  metSys.ShiftMEt(pfmet_ex,pfmet_ey,
			  genV.Px(),genV.Py(),
			  visiblePx,visiblePy,
			  nJets30,
			  bkgdType,
			  MEtSys::SysType::Response,
			  metScale,
			  pfmetcorr_ex,pfmetcorr_ey);
	  pfmet_phi = TMath::ATan2(pfmetcorr_ey,pfmetcorr_ex);
          pfmet = TMath::Sqrt(pfmetcorr_ex*pfmetcorr_ex+pfmetcorr_ey*pfmetcorr_ey);
	  pfmet_ex = pfmetcorr_ex;
	  pfmet_ey = pfmetcorr_ey;
	  
	  float puppimetcorr_ex = puppimet_ex;
	  float puppimetcorr_ey = puppimet_ey;
	  metSysPuppi.ShiftMEt(puppimet_ex,puppimet_ey,
			       genV.Px(),genV.Py(),
			       visiblePx,visiblePy,
			       nJets30,
			       bkgdType,
			       MEtSys::SysType::Response,
			       metScale,
			       puppimetcorr_ex,puppimetcorr_ey);
	  puppimet_phi = TMath::ATan2(puppimetcorr_ey,puppimetcorr_ex);
          puppimet = TMath::Sqrt(puppimetcorr_ex*puppimetcorr_ex+puppimetcorr_ey*puppimetcorr_ey);
	  puppimet_ex = puppimetcorr_ex;
	  puppimet_ey = puppimetcorr_ey;
	  

	}

	if (!isData&&fabs(metResolution-1)>0.001) { // applying met resolution systematics
	  float mvametcorr_ex = mvamet_ex;
	  float mvametcorr_ey = mvamet_ey;
	  metSysMva.ShiftMEt(mvamet_ex,mvamet_ey,
			     genV.Px(),genV.Py(),
			     visiblePx,visiblePy,
			     nJets30,
			     bkgdType,
			     MEtSys::SysType::Resolution,
			     metResolution,
			     mvametcorr_ex,mvametcorr_ey);
	  mvamet_phi = TMath::ATan2(mvametcorr_ey,mvametcorr_ex);
          mvamet = TMath::Sqrt(mvametcorr_ex*mvametcorr_ex+mvametcorr_ey*mvametcorr_ey);
	  mvamet_ex = mvametcorr_ex;
	  mvamet_ey = mvametcorr_ey;
	  
	  float pfmetcorr_ex = pfmet_ex;
	  float pfmetcorr_ey = pfmet_ey;
	  metSys.ShiftMEt(pfmet_ex,pfmet_ey,
			  genV.Px(),genV.Py(),
			  visiblePx,visiblePy,
			  nJets30,
			  bkgdType,
			  MEtSys::SysType::Resolution,
			  metResolution,
			  pfmetcorr_ex,pfmetcorr_ey);
	  pfmet_phi = TMath::ATan2(pfmetcorr_ey,pfmetcorr_ex);
          pfmet = TMath::Sqrt(pfmetcorr_ex*pfmetcorr_ex+pfmetcorr_ey*pfmetcorr_ey);
	  pfmet_ex = pfmetcorr_ex;
	  pfmet_ey = pfmetcorr_ey;
	  
	  float puppimetcorr_ex = puppimet_ex;
	  float puppimetcorr_ey = puppimet_ey;
	  metSysPuppi.ShiftMEt(puppimet_ex,puppimet_ey,
			       genV.Px(),genV.Py(),
			       visiblePx,visiblePy,
			       nJets30,
			       bkgdType,
			       MEtSys::SysType::Resolution,
			       metResolution,
			       puppimetcorr_ex,puppimetcorr_ey);
	  puppimet_phi = TMath::ATan2(puppimetcorr_ey,puppimetcorr_ex);
          puppimet = TMath::Sqrt(puppimetcorr_ex*puppimetcorr_ex+puppimetcorr_ey*puppimetcorr_ey);
	  puppimet_ex = puppimetcorr_ex;
	  puppimet_ey = puppimetcorr_ey;
	}

	// systematic shifts in mva met --->

	float mvamet_ex_ScaleUp   = mvamet_ex;
	float mvamet_ex_ScaleDown = mvamet_ex;
	float mvamet_ex_ResoUp    = mvamet_ex;
	float mvamet_ex_ResoDown  = mvamet_ex;
	float mvamet_ex_MuonUp    = mvamet_ex - 0.01*dimuon.Px();
	float mvamet_ex_MuonDown  = mvamet_ex + 0.01*dimuon.Px();
	
	float mvamet_ey_ScaleUp   = mvamet_ey;
	float mvamet_ey_ScaleDown = mvamet_ey;
	float mvamet_ey_ResoUp    = mvamet_ey;
	float mvamet_ey_ResoDown  = mvamet_ey;
	float mvamet_ey_MuonUp    = mvamet_ey - 0.01*dimuon.Py();
	float mvamet_ey_MuonDown  = mvamet_ey + 0.01*dimuon.Py();

	float dimuonPtUp   = 1.01*dimuon.Pt();
	float dimuonPtDown = 0.99*dimuon.Pt();
	

	metSysMva.ShiftMEt(mvamet_ex,mvamet_ey,
			   genV.Px(),genV.Py(),
			   visiblePx,visiblePy,
			   nJets30,
			   bkgdType,
			   MEtSys::SysType::Response,
			   1.10,
			   mvamet_ex_ScaleUp,mvamet_ey_ScaleUp);

	metSysMva.ShiftMEt(mvamet_ex,mvamet_ey,
			   genV.Px(),genV.Py(),
			   visiblePx,visiblePy,
			   nJets30,
			   bkgdType,
			   MEtSys::SysType::Response,
			   0.90,
			   mvamet_ex_ScaleDown,mvamet_ey_ScaleDown);
	
	metSysMva.ShiftMEt(mvamet_ex,mvamet_ey,
			   genV.Px(),genV.Py(),
			   visiblePx,visiblePy,
			   nJets30,
			   bkgdType,
			   MEtSys::SysType::Resolution,
			   1.10,
			   mvamet_ex_ResoUp,mvamet_ey_ResoUp);

	metSysMva.ShiftMEt(mvamet_ex,mvamet_ey,
			   genV.Px(),genV.Py(),
			   visiblePx,visiblePy,
			   nJets30,
			   bkgdType,
			   MEtSys::SysType::Resolution,
			   0.90,
			   mvamet_ex_ResoDown,mvamet_ey_ResoDown);
	
	float mvamet_ScaleUp = TMath::Sqrt(mvamet_ex_ScaleUp*mvamet_ex_ScaleUp+
					   mvamet_ey_ScaleUp*mvamet_ey_ScaleUp);

	float mvamet_ScaleDown = TMath::Sqrt(mvamet_ex_ScaleDown*mvamet_ex_ScaleDown+
					     mvamet_ey_ScaleDown*mvamet_ey_ScaleDown);

	float mvamet_ResoUp = TMath::Sqrt(mvamet_ex_ResoUp*mvamet_ex_ResoUp+
					  mvamet_ey_ResoUp*mvamet_ey_ResoUp);

	float mvamet_ResoDown = TMath::Sqrt(mvamet_ex_ResoDown*mvamet_ex_ResoDown+
					    mvamet_ey_ResoDown*mvamet_ey_ResoDown);

	float mvamet_MuonUp = TMath::Sqrt(mvamet_ex_MuonUp*mvamet_ex_MuonUp+
					    mvamet_ey_MuonUp*mvamet_ey_MuonUp);

	float mvamet_MuonDown = TMath::Sqrt(mvamet_ex_MuonDown*mvamet_ex_MuonDown+
					    mvamet_ey_MuonDown*mvamet_ey_MuonDown);


	//	std::cout << "Mass = " << massSel << std::endl;

	// selection on mass
	if (massSel>dimuonMassCut) {

	  numberOfVerticesH->Fill(double(analysisTree.primvertex_count),weight);

	  massSelH->Fill(massSel,weight);
	  massExtendedSelH->Fill(massSel,weight);
	  for (int iScale=0; iScale<21; ++ iScale) {
	    float scaleFactor = 0.98 + 0.002*float(iScale);
	    massSelScaleH[iScale]->Fill(massSel*scaleFactor,weight);
	  }

	  if (is0Jet) {
	    mass0jetH->Fill(double(massSel),weight);
	    if (nJets20BTagMedium==0) 
	      mass0jetBvetoH->Fill(double(massSel),weight);
	  }

	  if (isBoosted) {
	    massBoostedH->Fill(double(massSel),weight);
	    if (nJets20BTagMedium==0)
	      massBoostedBvetoH->Fill(double(massSel),weight);
	  }

	  if (isVBF) {
	    massVBFH->Fill(double(massSel),weight);
	    if (nJets20BTagMedium==0)
	      massVBFBvetoH->Fill(double(massSel),weight);
	  }

	  ptLeadingMuSelH->Fill(analysisTree.muon_pt[indx1],weight);
	  ptTrailingMuSelH->Fill(analysisTree.muon_pt[indx2],weight);
	  etaLeadingMuSelH->Fill(analysisTree.muon_eta[indx1],weight);
	  etaTrailingMuSelH->Fill(analysisTree.muon_eta[indx2],weight);
	  
	  nJets30SelH->Fill(double(nJets30),weight);
	  nJets20SelH->Fill(double(nJets20),weight);
	  nJets30etaCutSelH->Fill(double(nJets30etaCut),weight);
	  nJets20etaCutSelH->Fill(double(nJets20etaCut),weight);

	  HT30SelH->Fill(double(HT30),weight);
	  HT20SelH->Fill(double(HT20),weight);
	  HT30etaCutSelH->Fill(double(HT30etaCut),weight);
	  HT20etaCutSelH->Fill(double(HT20etaCut),weight);

	  nJets20BTagLooseSelH->Fill(double(nJets20BTagLoose),weight);
	  nJets20BTagMediumSelH->Fill(double(nJets20BTagMedium),weight);

	  // cout << "dxy (mu1) = " << analysisTree.muon_dxy[indx1] << "   error = " << analysisTree.muon_dxyerr[indx1] << std::endl;
	  //cout << "dxy (mu2) = " << analysisTree.muon_dxy[indx2] << "   error = " << analysisTree.muon_dxyerr[indx2] << std::endl;
	  //	 vector<int>indexMuTau; indexMuTau.clear();  
	  //	 if (selectZToTauTauMuMu && indexMuTau.size()!=2) continue;
	  float dimuonEta = dimuon.Eta();
	  float dimuonPt  = dimuon.Pt();
	  float dimuonPhi = dimuon.Phi();
	  float sumMuonPt = (mu1.Pt()+mu2.Pt());
	  float ptRatio = 0.0;
	  if (sumMuonPt != 0)
	    ptRatio = (dimuonPt/sumMuonPt);

	  dimuonMassPtH->Fill(massSel,dimuonPt,weight);

	  float dcaSigdxy_muon1 = 0.0;
	  float dcaSigdxy_muon2 = 0.0;
	  float dcaSigdz_muon1  = 0.0;
	  float dcaSigdz_muon2  =0.0;
	  float dcaSigdxy_mu1 = 0.0;
	  float dcaSigdxy_mu2 = 0.0;
          float dcaSigdz_mu1  = 0.0;
          float dcaSigdz_mu2  =0.0;
	  float dcaSigdxy_mu1_genMuMatch = 0.0;
          float dcaSigdxy_mu2_genMuMatch = 0.0;
          float dcaSigdz_mu1_genMuMatch  = 0.0;
          float dcaSigdz_mu2_genMuMatch  =0.0;
	  if (analysisTree.muon_dxyerr[indx1] != 0){
	    dcaSigdxy_mu1=(analysisTree.muon_dxy[indx1]/analysisTree.muon_dxyerr[indx1]);
	    dcaSigdxy_muon1= log10(analysisTree.muon_dxy[indx1]/analysisTree.muon_dxyerr[indx1]);
	  }
		      
	  if (analysisTree.muon_dxyerr[indx2] != 0){
	    dcaSigdxy_mu2=(analysisTree.muon_dxy[indx2]/analysisTree.muon_dxyerr[indx2]);
	    dcaSigdxy_muon2= log10(analysisTree.muon_dxy[indx2]/analysisTree.muon_dxyerr[indx2]);
	  }

	  if (analysisTree.muon_dzerr[indx1] != 0){
	    dcaSigdz_mu1 =(analysisTree.muon_dz[indx1]/analysisTree.muon_dzerr[indx1]);
	    dcaSigdz_muon1 = log10(analysisTree.muon_dz[indx1]/analysisTree.muon_dzerr[indx1]);
	  }
	  
	  if (analysisTree.muon_dzerr[indx2] != 0){
	    dcaSigdz_mu2 = (analysisTree.muon_dz[indx2]/analysisTree.muon_dzerr[indx2]);
	    dcaSigdz_muon2 = log10(analysisTree.muon_dz[indx2]/analysisTree.muon_dzerr[indx2]);
	  }
	  	
	  dimuonPtSelH->Fill(dimuon.Pt(),weight);
	  dimuonEtaSelH->Fill(dimuon.Eta(),weight);

	  //filling the histograms for discriminators
	  h_dimuonEta->Fill(dimuonEta,weight);
          if (genmatch_m1 && genmatch_m2) h_dimuonEta_genMuMatch->Fill(dimuonEta,weight);
	  h_ptRatio->Fill(ptRatio,weight);
	  h_dxy_muon1->Fill(analysisTree.muon_dxy[indx1],weight);
	  h_dxy_muon2->Fill(analysisTree.muon_dxy[indx2],weight);
	  h_dz_muon1->Fill(analysisTree.muon_dz[indx1],weight);
	  h_dz_muon2->Fill(analysisTree.muon_dz[indx2],weight);
	  h_dcaSigdxy_muon1->Fill(dcaSigdxy_muon1,weight);
	  h_dcaSigdxy_muon2->Fill(dcaSigdxy_muon2,weight);
	  h_dcaSigdz_muon1->Fill(dcaSigdz_muon1,weight);
	  h_dcaSigdz_muon2->Fill(dcaSigdz_muon2,weight);
	  h_dcaSigdxy_mu1->Fill(dcaSigdxy_mu1,weight);
          h_dcaSigdxy_mu2->Fill(dcaSigdxy_mu2,weight);
          h_dcaSigdz_mu1->Fill(dcaSigdz_mu1,weight);
          h_dcaSigdz_mu2->Fill(dcaSigdz_mu2,weight);

	  if (genmatch_m1 && genmatch_m2) h_dcaSigdxy_mu1_genMuMatch->Fill(dcaSigdxy_mu1,weight);
          if (genmatch_m1 && genmatch_m2) h_dcaSigdxy_mu2_genMuMatch->Fill(dcaSigdxy_mu2,weight);
          if (genmatch_m1 && genmatch_m2) h_dcaSigdz_mu1_genMuMatch->Fill(dcaSigdz_mu1,weight);
          if (genmatch_m1 && genmatch_m2) h_dcaSigdz_mu2_genMuMatch->Fill(dcaSigdz_mu2,weight);
	  
	  h_phi_leadingMu_MET->Fill((analysisTree.muon_phi[indx1]-pfmet_phi),weight);
	  h_phi_trailingMu_MET->Fill((analysisTree.muon_phi[indx2]-pfmet_phi),weight);
	  
	  if(!isData){
	    if (dcaSigdxy_muon1<1.8 && dcaSigdxy_muon1>0.2) h_dxy_muon1_dcaCut->Fill(analysisTree.muon_dxy[indx1],weight);
	    if (dcaSigdxy_muon1<1.8 && dcaSigdxy_muon1>0.2) h_dxy_muon2_dcaCut->Fill(analysisTree.muon_dxy[indx2],weight);
	    if (dcaSigdxy_muon1<1.8 && dcaSigdxy_muon1>0.2) h_dimuonEta_dcaCut->Fill(dimuonEta,weight);
	    if (dcaSigdxy_muon1<1.8 && dcaSigdxy_muon1>0.2) h_ptRatio_dcaCut->Fill(ptRatio,weight);
	  }
	  
	  NumberOfVerticesH->Fill(float(analysisTree.primvertex_count),weight);
	  metSelH->Fill(pfmet,weight);
	  puppimetSelH->Fill(puppimet,weight);
	  mvametSelH->Fill(mvamet,weight);

	  // ************************
	  // ** Filling MET NTuple **
	  // ************************
	  
	  evt = analysisTree.event_nr;
	  pt_1 = analysisTree.muon_pt[indx1];
	  eta_1 = analysisTree.muon_eta[indx1];
	  phi_1 = analysisTree.muon_phi[indx1];
	  match_1 = false;
	  pt_2 = analysisTree.muon_pt[indx2];
	  eta_2 = analysisTree.muon_eta[indx2];
	  phi_2 = analysisTree.muon_phi[indx2];
	  match_2 = false;
	  njets = nJets30;
	  mvametX = mvamet;
	  metX = pfmet;
	  metuncorrX = TMath::Sqrt(analysisTree.pfmet_ex*analysisTree.pfmet_ex+analysisTree.pfmet_ey*analysisTree.pfmet_ey);
	  puppimetX = puppimet;
	  if (genMuons.size()==2) {
	    for (unsigned int iGen=0; iGen<genMuons.size(); ++iGen) {
	      unsigned int igen = genMuons[iGen];
	      TLorentzVector genLV; genLV.SetXYZT(analysisTree.genparticles_px[igen],
						  analysisTree.genparticles_py[igen],
						  analysisTree.genparticles_pz[igen],
						  analysisTree.genparticles_e[igen]);
	      float deltaRLeading = deltaR(eta_1,phi_1,
					   genLV.Eta(),genLV.Phi());
	      float deltaRTrailing = deltaR(eta_2,phi_2,
					    genLV.Eta(),genLV.Phi());
	      if (deltaRLeading<0.2)
		match_1 = true;
	      if (deltaRTrailing<0.2)
		match_2 = true;
	      
	    }
	  }
	  
	  dilepton_pt = dimuonPt;
	  dilepton_eta = dimuonEta;
	  dilepton_phi = dimuonPhi;
	  
	  float unitX = dimuon.Px()/dimuon.Pt();
	  float unitY = dimuon.Py()/dimuon.Pt();
	  float phiUnit = TMath::ATan2(unitY,unitX);
	  float perpUnitX = TMath::Cos(phiUnit+0.5*TMath::Pi());
	  float perpUnitY = TMath::Sin(phiUnit+0.5*TMath::Pi());
	  
	  //	  float pfmetcorr_ex = analysisTree.pfmetcorr_ex;
	  //	  float pfmetcorr_ey = analysisTree.pfmetcorr_ey;
	  
	  float u1 = 0;
	  float u2  = 0;
	  float rH = 0;

	  computeRecoil(pfmet_ex,pfmet_ey,unitX,unitY,perpUnitX,perpUnitY,dimuonPt,u1,u2,rH);
	  u1_metuncorrX = u1;
	  u2_metuncorrX = u2;
	  
	  computeRecoil(pfmet_ex,pfmet_ey,unitX,unitY,perpUnitX,perpUnitY,dimuonPt,u1,u2,rH);
	  u1_metX = u1;
	  u2_metX = u2;
	  
	  computeRecoil(mvamet_ex,mvamet_ey,unitX,unitY,perpUnitX,perpUnitY,dimuonPt,u1,u2,rH);
	  u1_mvametX = u1;
	  u2_mvametX = u2;
	  
	  synch_tree->Fill();
	  
	  for (int iScale=0; iScale<21; ++ iScale) {
	    float scaleFactor = 0.7 + 0.03*float(iScale);
	    metSelScaleH[iScale]->Fill(pfmet*scaleFactor,weight);
	    puppimetSelScaleH[iScale]->Fill(puppimet*scaleFactor,weight);
	    mvametSelScaleH[iScale]->Fill(mvamet*scaleFactor,weight);
	  }

	  // jet pt bin
	  int jetBin = 0;
	  if (nJets30==1)
	    jetBin = 1;
	  else if (nJets30>1)
	    jetBin = 2;

	  int jetBinUp = 0;
	  if (nJets30Up==1)
	    jetBinUp = 1;
	  if (nJets30Up>1)
	    jetBinUp = 2;

	  int jetBinDown = 0;
	  if (nJets30Down==1)
	    jetBinDown = 1;
	  if (nJets30Down>1)
	    jetBinDown = 2;


	  int ptBin = binNumber(TMath::Min(float(dimuonPt),float(999)),nZPtBins,zPtBins);

	  metSelNJetsH[jetBin]->Fill(pfmet,weight);
	  puppimetSelNJetsH[jetBin]->Fill(puppimet,weight);
	  mvametSelNJetsH[jetBin]->Fill(mvamet,weight);

	  if (!isData) {
	    // std::cout << "GenV = (" 
	    // 	      << genV.Px() << "," 
	    // 	      << genV.Py() << "," 
	    // 	      << genV.Pz() << ")   mass = "
	    // 	      << genV.M()
	    // 	      << std::endl;
	    // std::cout << "GenL = (" 
	    // 	      << genL.Px() << "," 
	    // 	      << genL.Py() << "," 
	    // 	      << genL.Pz() << ")   mass = "
	    // 	      << genL.M() 
	    //  	      << std::endl;
	    // std::cout << "GenZ = (" 
	    // 	      << genZ.Px() << "," 
	    // 	      << genZ.Py() << "," 
	    // 	      << genZ.Pz() << ")   mass = "
	    // 	      << genZ.M() 
	    // 	      << std::endl;

	    int ptBinV = binNumber(TMath::Min(float(genV.Pt()),float(999)),nZPtBins,zPtBins);
	    float Hparal = 0;
	    float Hperp  = 0;

	    // pfmet ->
	    ComputeHadRecoilFromMet(pfmet_ex,pfmet_ey,genV.Px(),genV.Py(),genL.Px(),genL.Py(),Hparal,Hperp);
	    float response = - Hparal/genV.Pt();
	    recoilResponseMC_Ptbins[ptBinV]->Fill(response,weight);
	    recoilResponseMC_nJets[jetBin]->Fill(response,weight);
	    recoilResponseMC_Ptbins_nJets[jetBin][ptBinV]->Fill(response,weight);
	    //	    std::cout << "PFMet : Had(paral) = " << Hparal << "  Had(perp) = " << Hperp << std::endl;

	    // mvamet ->
	    ComputeHadRecoilFromMet(mvamet_ex,mvamet_ey,genV.Px(),genV.Py(),genL.Px(),genL.Py(),Hparal,Hperp);
	    response = - Hparal/genV.Pt();
	    recoilMvaResponseMC_Ptbins[ptBinV]->Fill(response,weight);
	    recoilMvaResponseMC_nJets[jetBin]->Fill(response,weight);
	    recoilMvaResponseMC_Ptbins_nJets[jetBin][ptBinV]->Fill(response,weight);
	    //	    std::cout << "MVAMet : Had(paral) = " << Hparal << "  Had(perp) = " << Hperp << std::endl;

	    // puppi met ->
	    ComputeHadRecoilFromMet(puppimet_ex,puppimet_ey,genV.Px(),genV.Py(),genL.Px(),genL.Py(),Hparal,Hperp);
	    response = - Hparal/genV.Pt();
	    recoilPuppiResponseMC_Ptbins[ptBinV]->Fill(response,weight);
	    recoilPuppiResponseMC_nJets[jetBin]->Fill(response,weight);
	    recoilPuppiResponseMC_Ptbins_nJets[jetBin][ptBinV]->Fill(response,weight);
	    //	    std::cout << "PuppiMet : Had(paral) = " << Hparal << "  Had(perp) = " << Hperp << std::endl;

	  }

	  float recoilParal = 0;
	  float recoilPerp  = 0;
	  float responseHad = 0;
	  computeRecoil(pfmet_ex,pfmet_ey,unitX,unitY,perpUnitX,perpUnitY,dimuonPt,recoilParal,recoilPerp,responseHad);

	  float recoilPuppiParal = 0;
	  float recoilPuppiPerp  = 0;
	  float responsePuppiHad = 0;
	  computeRecoil(puppimet_ex,puppimet_ey,unitX,unitY,perpUnitX,perpUnitY,dimuonPt,recoilPuppiParal,recoilPuppiPerp,responsePuppiHad);

	  float recoilMvaParal = 0;
	  float recoilMvaPerp  = 0;
	  float responseMvaHad = 0;
	  computeRecoil(mvamet_ex,mvamet_ey,unitX,unitY,perpUnitX,perpUnitY,dimuonPt,recoilMvaParal,recoilMvaPerp,responseMvaHad);

	  float recoilMvaScaleUpParal = 0;
	  float recoilMvaScaleUpPerp  = 0;
	  float responseMvaScaleUpHad = 0;
	  computeRecoil(mvamet_ex_ScaleUp,mvamet_ey_ScaleUp,unitX,unitY,perpUnitX,perpUnitY,dimuonPt,recoilMvaScaleUpParal,recoilMvaScaleUpPerp,responseMvaScaleUpHad);

	  float recoilMvaScaleDownParal = 0;
	  float recoilMvaScaleDownPerp  = 0;
	  float responseMvaScaleDownHad = 0;
	  computeRecoil(mvamet_ex_ScaleDown,mvamet_ey_ScaleDown,unitX,unitY,perpUnitX,perpUnitY,dimuonPt,recoilMvaScaleDownParal,recoilMvaScaleDownPerp,responseMvaScaleDownHad);

	  float recoilMvaResoUpParal = 0;
	  float recoilMvaResoUpPerp  = 0;
	  float responseMvaResoUpHad = 0;
	  computeRecoil(mvamet_ex_ResoUp,mvamet_ey_ResoUp,unitX,unitY,perpUnitX,perpUnitY,dimuonPt,recoilMvaResoUpParal,recoilMvaResoUpPerp,responseMvaResoUpHad);

	  float recoilMvaResoDownParal = 0;
	  float recoilMvaResoDownPerp  = 0;
	  float responseMvaResoDownHad = 0;
	  computeRecoil(mvamet_ex_ResoDown,mvamet_ey_ResoDown,unitX,unitY,perpUnitX,perpUnitY,dimuonPt,recoilMvaResoDownParal,recoilMvaResoDownPerp,responseMvaResoDownHad);

	  float recoilMvaMuonUpParal = 0;
	  float recoilMvaMuonUpPerp  = 0;
	  float responseMvaMuonUpHad = 0;
	  computeRecoil(mvamet_ex_MuonUp,mvamet_ey_MuonUp,unitX,unitY,perpUnitX,perpUnitY,dimuonPtUp,recoilMvaMuonUpParal,recoilMvaMuonUpPerp,responseMvaMuonUpHad);

	  float recoilMvaMuonDownParal = 0;
	  float recoilMvaMuonDownPerp  = 0;
	  float responseMvaMuonDownHad = 0;
	  computeRecoil(mvamet_ex_MuonDown,mvamet_ey_MuonDown,unitX,unitY,perpUnitX,perpUnitY,dimuonPtDown,recoilMvaMuonDownParal,recoilMvaMuonDownPerp,responseMvaMuonDownHad);

	  if (massSel>70&&massSel<110) { // Z Region

	    metZSelH->Fill(pfmet,weight);
	    puppimetZSelH->Fill(puppimet,weight);
	    mvametZSelH->Fill(mvamet,weight);

	    metZSelNJetsH[jetBin]->Fill(pfmet,weight);
	    puppimetZSelNJetsH[jetBin]->Fill(puppimet,weight);
	    mvametZSelNJetsH[jetBin]->Fill(mvamet,weight);

	    nJets20BTagLooseZSelH->Fill(double(nJets20BTagLoose),weight);
	    nJets20BTagMediumZSelH->Fill(double(nJets20BTagMedium),weight);

	    if (nJets30>0) {
	      leadingJetPtH->Fill(ptLeadingJet,weight);
	      leadingJetEtaH->Fill(etaLeadingJet,weight);
	      leadingJetPhiH->Fill(phiLeadingJet,weight);
	    }
	    
	    if (is0Jet) {
	      nJets20BTagMedium0jetH->Fill(double(nJets20BTagMedium),weight);
	    }
	    if (isBoosted) {
	      nJets20BTagMediumBoostedH->Fill(double(nJets20BTagMedium),weight);
	      dimuonPtBoostedH->Fill(dimuonPt,weight);
	      if (nJets20BTagMedium==0) 
		dimuonPtBoostedBvetoH->Fill(dimuonPt,weight);
	    }
	    if (isVBF) {
	      nJets20BTagMediumVBFH->Fill(double(nJets20BTagMedium),weight);
	      mjjVBFH->Fill(mjj,weight);
	      if ( nJets20BTagMedium==0 )
		mjjVBFBvetoH->Fill(mjj,weight);
	    }

	    // pfmet
	    recoilZParalH[jetBin]->Fill(recoilParal,weight);
	    recoilZPerpH[jetBin]->Fill(recoilPerp,weight);
	    recoilZParal_Ptbins_nJetsH[jetBin][ptBin]->Fill(recoilParal,weight);
	    recoilZPerp_Ptbins_nJetsH[jetBin][ptBin]->Fill(recoilPerp,weight);
	    recoilResponse_nJets[jetBin]->Fill(responseHad,weight);
	    recoilResponse_Ptbins_nJets[jetBin][ptBin]->Fill(responseHad,weight);

	    // puppimet
	    recoilPuppiZParalH[jetBin]->Fill(recoilPuppiParal,weight);
            recoilPuppiZPerpH[jetBin]->Fill(recoilPuppiPerp,weight);
            recoilPuppiZParal_Ptbins_nJetsH[jetBin][ptBin]->Fill(recoilPuppiParal,weight);
            recoilPuppiZPerp_Ptbins_nJetsH[jetBin][ptBin]->Fill(recoilPuppiPerp,weight);
	    recoilPuppiResponse_nJets[jetBin]->Fill(responsePuppiHad,weight);
	    recoilPuppiResponse_Ptbins_nJets[jetBin][ptBin]->Fill(responsePuppiHad,weight);

	    // mvamet
            recoilMvaZParalH[jetBin]->Fill(recoilMvaParal,weight);
            recoilMvaZPerpH[jetBin]->Fill(recoilMvaPerp,weight);
            recoilMvaZParal_Ptbins_nJetsH[jetBin][ptBin]->Fill(recoilMvaParal,weight);
            recoilMvaZPerp_Ptbins_nJetsH[jetBin][ptBin]->Fill(recoilMvaPerp,weight);
            recoilMvaResponse_nJets[jetBin]->Fill(responseMvaHad,weight);
            recoilMvaResponse_Ptbins_nJets[jetBin][ptBin]->Fill(responseMvaHad,weight);

	    // systematic changes

	    // JES
	    mvametZSelNJetsSysH[jetBinUp][0][0]->Fill(mvamet,weight);
	    mvametZSelNJetsSysH[jetBinDown][0][1]->Fill(mvamet,weight);

            recoilMvaZParalSysH[jetBinUp][0][0]->Fill(recoilMvaParal,weight);
            recoilMvaZParalSysH[jetBinDown][0][1]->Fill(recoilMvaParal,weight);

            recoilMvaZPerpSysH[jetBinUp][0][0]->Fill(recoilMvaPerp,weight);
            recoilMvaZPerpSysH[jetBinDown][0][1]->Fill(recoilMvaPerp,weight);

	    // MEt scale 
	    mvametZSelNJetsSysH[jetBin][1][0]->Fill(mvamet_ScaleUp,weight);
	    mvametZSelNJetsSysH[jetBin][1][1]->Fill(mvamet_ScaleDown,weight);

            recoilMvaZParalSysH[jetBin][1][0]->Fill(recoilMvaScaleUpParal,weight);
            recoilMvaZParalSysH[jetBin][1][1]->Fill(recoilMvaScaleDownParal,weight);

            recoilMvaZPerpSysH[jetBin][1][0]->Fill(recoilMvaScaleUpPerp,weight);
            recoilMvaZPerpSysH[jetBin][1][1]->Fill(recoilMvaScaleDownPerp,weight);

	    // MEt resolution 
	    mvametZSelNJetsSysH[jetBin][2][0]->Fill(mvamet_ResoUp,weight);
	    mvametZSelNJetsSysH[jetBin][2][1]->Fill(mvamet_ResoDown,weight);

            recoilMvaZParalSysH[jetBin][2][0]->Fill(recoilMvaResoUpParal,weight);
            recoilMvaZParalSysH[jetBin][2][1]->Fill(recoilMvaResoDownParal,weight);

            recoilMvaZPerpSysH[jetBin][2][0]->Fill(recoilMvaResoUpPerp,weight);
            recoilMvaZPerpSysH[jetBin][2][1]->Fill(recoilMvaResoDownPerp,weight);

	    // Muon momentum
	    mvametZSelNJetsSysH[jetBin][3][0]->Fill(mvamet_MuonUp,weight);
	    mvametZSelNJetsSysH[jetBin][3][1]->Fill(mvamet_MuonDown,weight);

            recoilMvaZParalSysH[jetBin][3][0]->Fill(recoilMvaMuonUpParal,weight);
            recoilMvaZParalSysH[jetBin][3][1]->Fill(recoilMvaMuonDownParal,weight);

            recoilMvaZPerpSysH[jetBin][3][0]->Fill(recoilMvaMuonUpPerp,weight);
            recoilMvaZPerpSysH[jetBin][3][1]->Fill(recoilMvaMuonDownPerp,weight);

	  }

	  if (massSel<70||massSel>110) { // Z-depleted region

	    metTopSelH->Fill(pfmet,weight);
	    puppimetTopSelH->Fill(puppimet,weight);
	    mvametTopSelH->Fill(mvamet,weight);

	    metTopSelNJetsH[jetBin]->Fill(pfmet,weight);
	    puppimetTopSelNJetsH[jetBin]->Fill(puppimet,weight);
	    mvametTopSelNJetsH[jetBin]->Fill(mvamet,weight);

	    // pfmet
	    recoilTopParalH[jetBin]->Fill(recoilParal,weight);
	    recoilTopPerpH[jetBin]->Fill(recoilPerp,weight);

	    // puppimet
	    recoilPuppiTopParalH[jetBin]->Fill(recoilPuppiParal,weight);
	    recoilPuppiTopPerpH[jetBin]->Fill(recoilPuppiPerp,weight);

	    // mvamet
            recoilMvaTopParalH[jetBin]->Fill(recoilMvaParal,weight);
            recoilMvaTopPerpH[jetBin]->Fill(recoilMvaPerp,weight);


	    // systematic changes

	    // JES
	    mvametTopSelNJetsSysH[jetBinUp][0][0]->Fill(mvamet,weight);
	    mvametTopSelNJetsSysH[jetBinDown][0][1]->Fill(mvamet,weight);

            recoilMvaTopParalSysH[jetBinUp][0][0]->Fill(recoilMvaParal,weight);
            recoilMvaTopParalSysH[jetBinDown][0][1]->Fill(recoilMvaParal,weight);

            recoilMvaTopPerpSysH[jetBinUp][0][0]->Fill(recoilMvaPerp,weight);
            recoilMvaTopPerpSysH[jetBinDown][0][1]->Fill(recoilMvaPerp,weight);

	    // MEt scale 
	    mvametTopSelNJetsSysH[jetBin][1][0]->Fill(mvamet_ScaleUp,weight);
	    mvametTopSelNJetsSysH[jetBin][1][1]->Fill(mvamet_ScaleDown,weight);

            recoilMvaTopParalSysH[jetBin][1][0]->Fill(recoilMvaScaleUpParal,weight);
            recoilMvaTopParalSysH[jetBin][1][1]->Fill(recoilMvaScaleDownParal,weight);

            recoilMvaTopPerpSysH[jetBin][1][0]->Fill(recoilMvaScaleUpPerp,weight);
            recoilMvaTopPerpSysH[jetBin][1][1]->Fill(recoilMvaScaleDownPerp,weight);

	    // MEt resolution 
	    mvametTopSelNJetsSysH[jetBin][2][0]->Fill(mvamet_ResoUp,weight);
	    mvametTopSelNJetsSysH[jetBin][2][1]->Fill(mvamet_ResoDown,weight);

            recoilMvaTopParalSysH[jetBin][2][0]->Fill(recoilMvaResoUpParal,weight);
            recoilMvaTopParalSysH[jetBin][2][1]->Fill(recoilMvaResoDownParal,weight);

            recoilMvaTopPerpSysH[jetBin][2][0]->Fill(recoilMvaResoUpPerp,weight);
            recoilMvaTopPerpSysH[jetBin][2][1]->Fill(recoilMvaResoDownPerp,weight);

	    // Muon momentum
	    mvametTopSelNJetsSysH[jetBin][3][0]->Fill(mvamet_MuonUp,weight);
	    mvametTopSelNJetsSysH[jetBin][3][1]->Fill(mvamet_MuonDown,weight);

            recoilMvaTopParalSysH[jetBin][3][0]->Fill(recoilMvaMuonUpParal,weight);
            recoilMvaTopParalSysH[jetBin][3][1]->Fill(recoilMvaMuonDownParal,weight);

            recoilMvaTopPerpSysH[jetBin][3][0]->Fill(recoilMvaMuonUpPerp,weight);
            recoilMvaTopPerpSysH[jetBin][3][1]->Fill(recoilMvaMuonDownPerp,weight);

	  }

	}

      }

      if (isIsoMuonsPair) selEventsIsoMuons++;
      
    } // end of file processing (loop over events in one file)
    nFiles++;
    delete _tree;
    file_->Close();
    delete file_;
  }
  std::cout << std::endl;
  int allEvents = int(inputEventsH->GetEntries());
  std::cout << "Sum of weights                                   = " << histWeightsSkimmedH->GetSumOfWeights() << std::endl;
  std::cout << "Total number of events in Tree                   = " << nEvents << std::endl;
  std::cout << "Total number of selected events (iso muon pairs) = " << selEventsIsoMuons << std::endl;
  std::cout << std::endl;
  std::cout << "RunMin = " << RunMin << std::endl;
  std::cout << "RunMax = " << RunMax << std::endl;

  //cout << "weight used:" << weight << std::endl;

  // using object as comp
  std::sort (allRuns.begin(), allRuns.end(), myobject);
  std::cout << "Runs   :";
  for (unsigned int iR=0; iR<allRuns.size(); ++iR)
    std::cout << " " << allRuns.at(iR);
  std::cout << std::endl;
  
  file->Write();
  file->Close();
  delete file;
  
  }



