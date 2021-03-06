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
#include "TMath.h"
#include "TCanvas.h"
#include "TLorentzVector.h"
#include "TPaveText.h"
#include "TRandom.h"
#include "TGraphAsymmErrors.h"

#include "DesyTauAnalyses/NTupleMaker/interface/Config.h"
#include "DesyTauAnalyses/NTupleMaker/interface/AC1B.h"
#include "DesyTauAnalyses/NTupleMaker/interface/json.h"
#include "DesyTauAnalyses/NTupleMaker/interface/PileUp.h"
#include "DesyTauAnalyses/NTupleMaker/interface/functions.h"


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

  // kinematic cuts on muons
  const float ptMuonLowCut   = cfg.get<float>("ptMuonLowCut");
  const float ptMuonHighCut  = cfg.get<float>("ptMuonHighCut");
  const float etaMuonHighCut = cfg.get<float>("etaMuonHighCut");
  const float etaMuonLowCut = cfg.get<float>("etaMuonLowCut");
  const float dxyMuonCut     = cfg.get<float>("dxyMuonCut");
  const float dzMuonCut      = cfg.get<float>("dzMuonCut");
  const float isoMuonCut     = cfg.get<float>("isoMuonCut");
  const bool  applyTauTauSelection = cfg.get<bool>("ApplyTauTauSelection");
  const bool  selectZToTauTauMuMu = cfg.get<bool>("SelectZToTauTauMuMu");

  // topological cuts
  const float dRleptonsCut   = cfg.get<float>("dRleptonsCut");
  const float dPhileptonsCut = cfg.get<float>("dPhileptonsCut");
  const float DRTrigMatch    = cfg.get<float>("DRTrigMatch"); 
  const bool oppositeSign    = cfg.get<bool>("OppositeSign");

  // trigger
  const bool applyTrigger = cfg.get<bool>("ApplyTrigger");
  const string muonTriggerName  = cfg.get<string>("MuonTriggerName");
  const string muonFilterName   = cfg.get<string>("MuonFilterName");
  const string muon17FilterName = cfg.get<string>("Muon17FilterName"); 
  const string muon8FilterName = cfg.get<string>("Muon8FilterName"); 
  const string singleMuonFilterName = cfg.get<string>("SingleMuonFilterName");
  const float singleMuonTriggerPtCut = cfg.get<float>("SingleMuonTriggerPtCut");
  const float singleMuonTriggerEtaCut = cfg.get<float>("SingleMuonTriggerEtaCut");
  

  TString MuonTriggerName(muonTriggerName);
  TString MuonFilterName(muonFilterName);

  TString Muon17FilterName(muon17FilterName);
  TString Muon8FilterName(muon8FilterName);
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

  // Run range
  const unsigned int RunRangeMin = cfg.get<unsigned int>("RunRangeMin");
  const unsigned int RunRangeMax = cfg.get<unsigned int>("RunRangeMax");

  //
  const string dataBaseDir = cfg.get<string>("DataBaseDir");

  // vertex distributions filenames and histname
  const string vertDataFileName = cfg.get<string>("VertexDataFileName");
  const string vertMcFileName   = cfg.get<string>("VertexMcFileName");
  const string vertHistName     = cfg.get<string>("VertexHistName");

  // lepton scale factors
  const string muonSfDataBarrel = cfg.get<string>("MuonSfDataBarrel");
  const string muonSfDataEndcap = cfg.get<string>("MuonSfDataEndcap");
  const string muonSfMcBarrel = cfg.get<string>("MuonSfMcBarrel");
  const string muonSfMcEndcap = cfg.get<string>("MuonSfMcEndcap");

  const string jsonFile = cfg.get<string>("jsonFile");
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
  char ff[100];

  sprintf(ff,"%s/%s",argv[3],argv[2]);
  // file name and tree name
  std::string rootFileName(argv[2]);
  std::ifstream fileList(ff);
  std::ifstream fileList0(ff);
  std::string ntupleName("makeroottree/AC1B");
  
  TString era=argv[3];
  TString TStrName(rootFileName);
  std::cout <<TStrName <<std::endl;  

  TFile * file = new TFile(era+"/"+TStrName+TString(".root"),"update");
  file->cd("");
/*
  // file name and tree name
  std::string rootFileName(argv[2]);
  std::ifstream fileList(argv[2]);
  std::ifstream fileList0(argv[2]);
  std::string ntupleName("makeroottree/AC1B");

  TString TStrName(rootFileName);
  TString era=argv[3];
  std::cout <<TStrName <<std::endl;  

  TFile * file = new TFile(TStrName+TString(".root"),"recreate");
  file->cd("");
*/
  std::string initNtupleName("initroottree/AC1B");
  TH1F * inputEventsH = new TH1F("inputEventsH","",1,-0.5,0.5);
  TH1F * histWeightsH = new TH1F("histWeightsH","",1,-0.5,0.5);
  TH1F * histWeightsSkimmedH = new TH1F("histWeightsSkimmedH","",1,-0.5,0.5);

  // Histograms after selecting unique dimuon pair
  TH1F * ptLeadingMuSelH = new TH1F("ptLeadingMuSelH","",100,0,200);
  TH1F * ptTrailingMuSelH = new TH1F("ptTrailingMuSelH","",100,0,200);
  TH1F * etaLeadingMuSelH = new TH1F("etaLeadingMuSelH","",50,-2.5,2.5);
  TH1F * etaTrailingMuSelH = new TH1F("etaTrailingMuSelH","",50,-2.5,2.5);
  TH1F * massSelH = new TH1F("massSelH","",200,0,200);
  TH1F * metSelH  = new TH1F("metSelH","",200,0,400);
 
  TH1D * hDiJetmet = new TH1D("hDiJetmet","",200,0,400);
  TH1D * hDiJetmass = new TH1D("hDiJetmass","",500,0,1000);
  TH1D * hHT_ = new TH1D("hHT_","",800,0,1600);
  TH1D * metAll  = new TH1D("metAll","",200,0,400);

  TString scales[21] = {"M10","M9","M8","M7","M6","M5","M4","M3","M2","M1","0",
			"P1","P2","P3","P4","P5","P6","P7","P8","P9","P10"};
  
  TH1F * massSelScaleH[21];
  TH1F * metSelScaleH[21];
  for (int iScale=0; iScale<21; ++iScale) {    
    massSelScaleH[iScale] = new TH1F("massSel"+scales[iScale]+"H","",200,0,200);
    metSelScaleH[iScale] = new TH1F("metSel"+scales[iScale]+"H","",200,0,400);
  }


  TH1F * nJets30SelH    = new TH1F("nJets30SelH","",11,-0.5,10.5);
  TH1F * nJets30etaCutSelH = new TH1F("nJets30etaCutSelH","",11,-0.5,10.5);
  TH1F * nJets20SelH    = new TH1F("nJets20SelH","",11,-0.5,10.5);
  TH1F * nJets20etaCutSelH = new TH1F("nJets20etaCutSelH","",11,-0.5,10.5);

  TH1F * HT30SelH       = new TH1F("HT30SelH","",50,0,500);
  TH1F * HT30etaCutSelH = new TH1F("HT30etaCutSelH","",50,0,500);
  TH1F * HT20SelH       = new TH1F("HT20SelH","",50,0,500);
  TH1F * HT20etaCutSelH = new TH1F("HT20etaCutSelH","",50,0,500);
  
  //Discriminiant histos
  TH1F * h_dimuonEta = new TH1F("dimuonEta","",50,-6,+6);//21Aug
  TH1F * h_dimuonEta_genMuMatch = new TH1F("dimuonEta_genMuMatch","",50,-6,+6);//21Aug
  TH1F * h_ptRatio = new TH1F ("ptRatio","",50,0,1);
  TH1F * h_dxy_muon1 =new TH1F ("dxy_muon1","",50,-0.02,0.02);
  TH1F * h_dxy_muon2 =new TH1F ("dxy_muon2","",50,-0.02,0.02);
  TH1F * h_dz_muon1 = new TH1F ("dz_muon1","",50,-0.1,0.1);
  TH1F * h_dz_muon2 = new TH1F ("dz_muon2","",50,-0.1,0.1);
  TH1F * h_dcaSigdxy_mu1 = new TH1F ("dcaSigdxy_mu1","",50,-4,4);
  TH1F * h_dcaSigdxy_mu2 = new TH1F ("dcaSigdxy_mu2","",50,-4,4);
  TH1F * h_dcaSigdz_mu1 = new TH1F ("dcaSigdz_mu1","",50,-4,4);
  TH1F * h_dcaSigdz_mu2 = new TH1F ("dcaSigdz_mu2","",50,-4,4);
  TH1F * h_dcaSigdxy_muon1 = new TH1F ("dcaSigdxy_muon1","",50,-4,4);
  TH1F * h_dcaSigdxy_muon2 = new TH1F ("dcaSigdxy_muon2","",50,-4,4);
  TH1F * h_dcaSigdz_muon1 = new TH1F ("dcaSigdz_muon1","",50,-4,4);
  TH1F * h_dcaSigdz_muon2 = new TH1F ("dcaSigdz_muon2","",50,-4,4);
  TH1F * h_dcaSigdxy_mu1_genMuMatch = new TH1F ("dcaSigdxy_mu1_genMuMatch","",50,-4,4);
  TH1F * h_dcaSigdxy_mu2_genMuMatch = new TH1F ("dcaSigdxy_mu2_genMuMatch","",50,-4,4);
  TH1F * h_dcaSigdz_mu1_genMuMatch = new TH1F ("dcaSigdz_mu1_genMuMatch","",50,-4,4);
  TH1F * h_dcaSigdz_mu2_genMuMatch = new TH1F ("dcaSigdz_mu2_genMuMatch","",50,-4,4);
  TH1F * h_phi_leadingMu_MET =new TH1F ("phi_leadingMu_MET","",50,0,3);
  TH1F * h_phi_trailingMu_MET =new TH1F ("phi_trailingMu_MET","",50,0,3);
  TH1F * h_dxy_muon1_dcaCut =new TH1F ("dxy_muon1_dcaCut","",50,-0.02,0.02);
  TH1F * h_dxy_muon2_dcaCut =new TH1F ("dxy_muon2_dcaCut","",50,-0.02,0.02);
  TH1F * h_dimuonEta_dcaCut = new TH1F("dimuonEta_dcaCut","",50,-6,+6);
  TH1F * h_ptRatio_dcaCut = new TH1F ("ptRatio_dcaCut","",50,0,1);

  TH1F * NumberOfVerticesH = new TH1F("NumberOfVerticesH","",51,-0.5,50.5);
  
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
  
  int nEtaBins = 3;
  float etaBins[4] = {0,0.9,1.2,2.5}; 
  
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

  TString EtaBins[3] = {"EtaLt0p9",
			"Eta0p9to1p2",
			"EtaGt1p2"};

  TH1F * ZMassEtaPtPass[3][7];
  TH1F * ZMassEtaPtFail[3][7];

  TString JetBins[3] = {"Jet0","Jet1","JetGe2"};

  //*****  create eta histogram with eta ranges associated to their names (eg. endcap, barrel)   ***** //
  TH1F * etaBinsH = new TH1F("etaBinsH", "etaBinsH", nEtaBins, etaBins);
  etaBinsH->SetStats(0);
  etaBinsH->Draw();
  etaBinsH->GetXaxis()->Set(nEtaBins, etaBins);
  for (int i=0; i<nEtaBins; i++){ etaBinsH->GetXaxis()->SetBinLabel(i+1, EtaBins[i]);}
  TPaveText *pavetext = new TPaveText(0.6,0.85,0.98,0.98, "brNDC");
  for (int i=0; i<nEtaBins; i++){    
	pavetext->AddText(etaBinsH->GetXaxis()->GetBinLabel(i+1));
	pavetext->AddText(Form("from %f to %f",etaBinsH->GetXaxis()->GetBinLowEdge(i+1), etaBinsH->GetXaxis()->GetBinLowEdge(i+1)+etaBinsH->GetXaxis()->GetBinWidth(i+1)));
	}
  //TCanvas *cEta = new TCanvas("c1","demo bin labels",10,10,900,500);
  //cEta->cd();
  etaBinsH->Draw();
  //pavetext->Draw();
  file->cd();
  etaBinsH->Write("etaBinsH");
  //cEta->Write("etaBinsCan");

  TH1F * ZMassJetEtaPtPass[3][3][7];
  TH1F * ZMassJetEtaPtFail[3][3][7];

  TH1F * PromptPtPass[3];
  TH1F * PromptPtFail[3];

  TH1F * NonPromptPtPass[3];
  TH1F * NonPromptPtFail[3];

  TH1F * PromptSelPtPass[3];
  TH1F * PromptSelPtFail[3];

  TH1F * NonPromptSelPtPass[3];
  TH1F * NonPromptSelPtFail[3];

  TH1F * PromptSelJetPtPass[3][3];
  TH1F * PromptSelJetPtFail[3][3];

  TH1F * NonPromptSelJetPtPass[3][3];
  TH1F * NonPromptSelJetPtFail[3][3];

  TH1F * ZMassMu23EtaPtPass[3][16];
  TH1F * ZMassMu23EtaPtFail[3][16];

  TH1F * ZMassMu17EtaPtPass[3][16];
  TH1F * ZMassMu17EtaPtFail[3][16];

  TH1F * ZMassMu8EtaPtPass[3][16];
  TH1F * ZMassMu8EtaPtFail[3][16];

  TH1F * ZMassIsoMuEtaPtPass[3][16];
  TH1F * ZMassIsoMuEtaPtFail[3][16];

  for (int iEta=0; iEta<nEtaBins; ++iEta) {
    PromptPtPass[iEta] = new TH1F("PromptPt"+EtaBins[iEta]+"Pass","",nPtBins,ptBins);
    PromptPtFail[iEta] = new TH1F("PromptPt"+EtaBins[iEta]+"Fail","",nPtBins,ptBins);
    NonPromptPtPass[iEta] = new TH1F("NonPromptPt"+EtaBins[iEta]+"Pass","",nPtBins,ptBins);
    NonPromptPtFail[iEta] = new TH1F("NonPromptPt"+EtaBins[iEta]+"Fail","",nPtBins,ptBins);
    PromptSelPtPass[iEta] = new TH1F("PromptSelPt"+EtaBins[iEta]+"Pass","",nPtBins,ptBins);
    PromptSelPtFail[iEta] = new TH1F("PromptSelPt"+EtaBins[iEta]+"Fail","",nPtBins,ptBins);
    NonPromptSelPtPass[iEta] = new TH1F("NonPromptSelPt"+EtaBins[iEta]+"Pass","",nPtBins,ptBins);
    NonPromptSelPtFail[iEta] = new TH1F("NonPromptSelPt"+EtaBins[iEta]+"Fail","",nPtBins,ptBins);
    for (int iJet=0; iJet<3; ++iJet) {
      PromptSelJetPtPass[iEta][iJet] = new TH1F("PromptSelPt"+EtaBins[iEta]+JetBins[iJet]+"Pass","",nPtBins,ptBins);
      PromptSelJetPtFail[iEta][iJet] = new TH1F("PromptSelPt"+EtaBins[iEta]+JetBins[iJet]+"Fail","",nPtBins,ptBins);
      NonPromptSelJetPtPass[iEta][iJet] = new TH1F("NonPromptSelPt"+EtaBins[iEta]+JetBins[iJet]+"Pass","",nPtBins,ptBins);
      NonPromptSelJetPtFail[iEta][iJet] = new TH1F("NonPromptSelPt"+EtaBins[iEta]+JetBins[iJet]+"Fail","",nPtBins,ptBins);
    }
    for (int iPt=0; iPt<nPtBins; ++iPt) {
      ZMassEtaPtPass[iEta][iPt] = new TH1F("ZMass"+EtaBins[iEta]+PtBins[iPt]+"Pass","",60,60,120);
      ZMassEtaPtFail[iEta][iPt] = new TH1F("ZMass"+EtaBins[iEta]+PtBins[iPt]+"Fail","",60,60,120);
      for (int iJet=0; iJet<3; ++iJet) {
	ZMassJetEtaPtPass[iEta][iJet][iPt] = new TH1F("ZMass"+EtaBins[iEta]+JetBins[iJet]+PtBins[iPt]+"Pass","",60,60,120);
	ZMassJetEtaPtFail[iEta][iJet][iPt] = new TH1F("ZMass"+EtaBins[iEta]+JetBins[iJet]+PtBins[iPt]+"Fail","",60,60,120);
      }
    }
    for (int iPt=0; iPt<nPtBinsTrig; ++iPt) {
      ZMassMu23EtaPtPass[iEta][iPt] = new TH1F("ZMassMu23"+EtaBins[iEta]+PtBinsTrig[iPt]+"Pass","",60,60,120);
      ZMassMu23EtaPtFail[iEta][iPt] = new TH1F("ZMassMu23"+EtaBins[iEta]+PtBinsTrig[iPt]+"Fail","",60,60,120);
      ZMassMu17EtaPtPass[iEta][iPt] = new TH1F("ZMassMu17"+EtaBins[iEta]+PtBinsTrig[iPt]+"Pass","",60,60,120);
      ZMassMu17EtaPtFail[iEta][iPt] = new TH1F("ZMassMu17"+EtaBins[iEta]+PtBinsTrig[iPt]+"Fail","",60,60,120);
      ZMassMu8EtaPtPass[iEta][iPt]  = new TH1F("ZMassMu8"+EtaBins[iEta]+PtBinsTrig[iPt]+"Pass","",60,60,120);
      ZMassMu8EtaPtFail[iEta][iPt]  = new TH1F("ZMassMu8"+EtaBins[iEta]+PtBinsTrig[iPt]+"Fail","",60,60,120);
      ZMassIsoMuEtaPtPass[iEta][iPt]  = new TH1F("ZMassIsoMu"+EtaBins[iEta]+PtBinsTrig[iPt]+"Pass","",60,60,120);
      ZMassIsoMuEtaPtFail[iEta][iPt]  = new TH1F("ZMassIsoMu"+EtaBins[iEta]+PtBinsTrig[iPt]+"Fail","",60,60,120);
    }
  }

  TH1F * ZMassMu23BarrelPass = new TH1F("ZMassMu23BarrelPass","",60,60,120);
  TH1F * ZMassMu23BarrelFail = new TH1F("ZMassMu23BarrelFail","",60,60,120);
  TH1F * ZMassMu23EndcapPass = new TH1F("ZMassMu23EndcapPass","",60,60,120);
  TH1F * ZMassMu23EndcapFail = new TH1F("ZMassMu23EndcapFail","",60,60,120);

  TH1F * ZMassMu17BarrelPass = new TH1F("ZMassMu17BarrelPass","",60,60,120);
  TH1F * ZMassMu17BarrelFail = new TH1F("ZMassMu17BarrelFail","",60,60,120);
  TH1F * ZMassMu17EndcapPass = new TH1F("ZMassMu17EndcapPass","",60,60,120);
  TH1F * ZMassMu17EndcapFail = new TH1F("ZMassMu17EndcapFail","",60,60,120);

  TH1F * ZMassIsoMuBarrelPass = new TH1F("ZMassIsoMuBarrelPass","",60,60,120);
  TH1F * ZMassIsoMuBarrelFail = new TH1F("ZMassIsoMuBarrelFail","",60,60,120);
  TH1F * ZMassIsoMuEndcapPass = new TH1F("ZMassIsoMuEndcapPass","",60,60,120);
  TH1F * ZMassIsoMuEndcapFail = new TH1F("ZMassIsoMuEndcapFail","",60,60,120);

  TH1F * ZMassMu8BarrelPass  = new TH1F("ZMassMu8BarrelPass", "",60,60,120);
  TH1F * ZMassMu8BarrelFail  = new TH1F("ZMassMu8BarrelFail", "",60,60,120);
  TH1F * ZMassMu8EndcapPass  = new TH1F("ZMassMu8EndcapPass", "",60,60,120);
  TH1F * ZMassMu8EndcapFail  = new TH1F("ZMassMu8EndcapFail", "",60,60,120);

  TH1D * PUweightsOfficialH = new TH1D("PUweightsOfficialH","PU weights w/ official reweighting",1000, 0, -1);

  int nJetBins = 3;
  int nZPtBins = 5;
  float zPtBins[6] = {0,10,20,30,50,1000};

  TString NJetBins[3] = {"NJet0","NJet1","NJetGe2"};
  TString ZPtBins[5] = {"Pt0to10",
			"Pt10to20",
			"Pt20to30",
			"Pt30to50",
			"PtGt50"};

  TH1F * recoilZParalH[3];
  TH1F * recoilZPerpH[3];

  for (int iBin=0; iBin<nJetBins; ++iBin) {
    recoilZParalH[iBin] = new TH1F("recoilZParal"+NJetBins[iBin]+"H","",100,-200,200);
    recoilZPerpH[iBin] = new TH1F("recoilZPerp"+NJetBins[iBin]+"H","",100,-200,200);
  }
  

 // PILE UP REWEIGHTING - OPTIONS

  if (applyPUreweighting_vertices and applyPUreweighting_official) 
	{std::cout<<"ERROR: Choose only ONE PU reweighting method (vertices or official, not both!) " <<std::endl; exit(-1);}

  // reweighting with vertices

  // reading vertex weights
  TFile * fileDataNVert = new TFile(TString(cmsswBase)+"/src/"+dataBaseDir+"/"+vertDataFileName);
  TFile * fileMcNVert   = new TFile(TString(cmsswBase)+"/src/"+dataBaseDir+"/"+vertMcFileName);

  TH1F * vertexDataH = (TH1F*)fileDataNVert->Get(TString(vertHistName));
  TH1F * vertexMcH   = (TH1F*)fileMcNVert->Get(TString(vertHistName));

  float normVertexData = vertexDataH->GetSumOfWeights();
  float normVertexMc   = vertexMcH->GetSumOfWeights();

  vertexDataH->Scale(1/normVertexData);
  vertexMcH->Scale(1/normVertexMc);


 // reweighting official recipe 
  	// initialize pile up object
  PileUp * PUofficial = new PileUp();
  

  
  if (applyPUreweighting_official) {
    TFile * filePUdistribution_data = new TFile(TString(cmsswBase)+"/src/DesyTauAnalyses/NTupleMaker/data/PileUpDistrib/Data_Pileup_2015D_Nov17.root","read"); 
    TFile * filePUdistribution_MC = new TFile (TString(cmsswBase)+"/src/DesyTauAnalyses/NTupleMaker/data/PileUpDistrib/MC_Spring15_PU25_Startup.root", "read"); 
    TH1D * PU_data = (TH1D *)filePUdistribution_data->Get("pileup");
    TH1D * PU_mc = (TH1D *)filePUdistribution_MC->Get("pileup");
    PUofficial->set_h_data(PU_data); 
    PUofficial->set_h_MC(PU_mc);
  }



  TFile *f10 = new TFile(TString(cmsswBase)+"/src/"+dataBaseDir+"/"+muonSfDataBarrel);  // mu SF barrel data
  TFile *f11 = new TFile(TString(cmsswBase)+"/src/"+dataBaseDir+"/"+muonSfDataEndcap); // mu SF endcap data
  TFile *f12 = new TFile(TString(cmsswBase)+"/src/"+dataBaseDir+"/"+muonSfMcBarrel);  // mu SF barrel MC
  TFile *f13 = new TFile(TString(cmsswBase)+"/src/"+dataBaseDir+"/"+muonSfMcEndcap); // mu SF endcap MC 
  
  TGraphAsymmErrors *hEffBarrelData = (TGraphAsymmErrors*)f10->Get("ZMassBarrel");
  TGraphAsymmErrors *hEffEndcapData = (TGraphAsymmErrors*)f11->Get("ZMassEndcap");
  TGraphAsymmErrors *hEffBarrelMC = (TGraphAsymmErrors*)f12->Get("ZMassBarrel");
  TGraphAsymmErrors *hEffEndcapMC = (TGraphAsymmErrors*)f13->Get("ZMassEndcap");
  
  double * dataEffBarrel = new double[10];
  double * dataEffEndcap = new double[10];
  double * mcEffBarrel = new double[10];
  double * mcEffEndcap = new double[10];
  
  dataEffBarrel = hEffBarrelData->GetY();
  dataEffEndcap = hEffEndcapData->GetY();
  mcEffBarrel = hEffBarrelMC->GetY();
  mcEffEndcap = hEffEndcapMC->GetY();


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
 vector <unsigned int> run_;
 vector <unsigned int> lumi_;
 vector <unsigned int> event_;
 run_.clear();
 lumi_.clear();
 event_.clear();

    std::vector<Event> EventList;
    std::string line;
    std::ifstream EventsFile;
    TString file_events=argv[4]; //eventlist_csc2015.txt
    //EventsFile.open("MET_filters/eventlist_"+file_events+".txt");
    EventsFile.open("MET_filters/"+file_events+".txt");
    cout<<"  limits  int -> "<<std::numeric_limits<int>::max()<<"  long int -> "<<std::numeric_limits<long int>::max()<<"  unsigned int -> "<<std::numeric_limits<unsigned int>::max()<<endl;
    cout<<" The file that will be used will be  MET_filters/"<<file_events<<".txt"<<endl;


    while (getline(EventsFile, line))
    {
        std::vector<std::string> columns = split(line,':');
        run_.push_back(std::stoi(columns[0]));
        lumi_.push_back(std::stoi(columns[1]));
        event_.push_back(std::stoul(columns[2]));
   	/*   Event events_;

        events_.name     = "Test";
        events_.run     = std::stoi(columns[0]);
        events_.lumi   = std::stoi(columns[1]);
        events_.eventrn = std::stof(columns[2]);

        EventList.push_back(events_);
  */
      	}

  cout<<" In total there are "<<run_.size()<< " entries for "<<file_events<<" filter "<<endl;
    EventsFile.close();
  //----Attention----//
  //if(XSec!=1) nTotalFiles=20;
  //nTotalFiles=5;



  for (int iF=0; iF<nTotalFiles; ++iF) {

    std::string filen;
    fileList >> filen;

    std::cout << "file " << iF+1 << " out of " << nTotalFiles << " filename : " << filen << std::endl;
    TFile * file_ = TFile::Open(TString(filen));
    
    TH1D * histoInputEvents = NULL;
    histoInputEvents = (TH1D*)file_->Get("makeroottree/nEvents");
    if (histoInputEvents==NULL) continue;
    int NE = int(histoInputEvents->GetEntries());
    for (int iE=0;iE<NE;++iE)
      inputEventsH->Fill(0.);
    std::cout << "      number of input events         = " << NE << std::endl;

    TTree * _inittree = NULL;
    _inittree = (TTree*)file_->Get(TString(initNtupleName));
    if (_inittree==NULL) continue;
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

      histWeightsSkimmedH->Fill(float(0),weight);

      if (!isData) {
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

	double Ninteractions = analysisTree.numtruepileupinteractions;
	double PUweight = PUofficial->get_PUweight(Ninteractions);
	weight *= PUweight;
	PUweightsOfficialH->Fill(PUweight);

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
      }

      if (isData && applyGoodRunSelection){
	
	
	bool lumi = false;
	int n=analysisTree.event_run;
	int lum = analysisTree.event_luminosityblock;
	int nr = analysisTree.event_nr;
	
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
        bool runbool=false;
	bool lumibool=false;
	bool eventbool=false;
	runbool= std::find(run_.begin(), run_.end(), n) != run_.end();
	lumibool= std::find(lumi_.begin(), lumi_.end(), lum) != lumi_.end();
	eventbool= std::find(event_.begin(), event_.end(), nr) != event_.end();

       if (runbool && lumibool && eventbool) continue;
	//if (lumi ) cout<<"  =============  Found good run"<<"  "<<n<<"  "<<lum<<endl;
	//std::remove("myinputfile");
      }     
      float metall = sqrt(analysisTree.pfmet_ex*analysisTree.pfmet_ex+analysisTree.pfmet_ey*analysisTree.pfmet_ey);
                metAll->Fill(metall,weight);

      if (analysisTree.event_run<RunMin)
	RunMin = analysisTree.event_run;
      
      if (analysisTree.event_run>RunMax)
	RunMax = analysisTree.event_run;
      
      //std::cout << " Run : " << analysisTree.event_run << std::endl;
      
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
	if (HLTFilter==MuonFilterName) {
	  nMuonFilter = i;
	  isMuonFilter = true;
	}
	if (HLTFilter==Muon17FilterName) {
	  nMuon17Filter = i;
	  isMuon17Filter = true;
	}
	if (HLTFilter==Muon8FilterName) {
	  nMuon8Filter = i;
	  isMuon8Filter = true;
	}
	if (HLTFilter==SingleMuonFilterName) {
          nSingleMuonFilter = i;
          isSingleMuonFilter = true;
        }
      }
      if (!isMuonFilter) {
	cout << "Filter " << MuonFilterName << " not found " << endl;
	exit(-1);
      }
      if (!isMuon17Filter) {
	cout << "Filter " << Muon17FilterName << " not found " << endl;
        exit(-1);
      }
      if (!isMuon8Filter) {
	cout << "Filter " << Muon8FilterName << " not found " << endl;
        exit(-1);
      }
      if (!isSingleMuonFilter) {
	cout << "Filter " << SingleMuonFilterName << " not found " << endl;
        exit(-1);
      }

      // vertex cuts
      
      if (fabs(analysisTree.primvertex_z)>zVertexCut) continue;
      if (analysisTree.primvertex_ndof<ndofVertexCut) continue;
      float dVertex = (analysisTree.primvertex_x*analysisTree.primvertex_x+
		       analysisTree.primvertex_y*analysisTree.primvertex_y);
      if (dVertex>dVertexCut) continue;
      
      // muon selection

      vector<unsigned int> allMuons; allMuons.clear();
      vector<unsigned int> idMuons; idMuons.clear();
      vector<unsigned int> isoMuons; isoMuons.clear();
      vector<float> isoMuonsValue; isoMuonsValue.clear();
      vector<bool> isMuonPassedIdIso; isMuonPassedIdIso.clear();
      vector<bool> isMuonMatched23Filter; isMuonMatched23Filter.clear();
      vector<bool> isMuonMatched17Filter; isMuonMatched17Filter.clear();
      vector<bool> isMuonMatched8Filter; isMuonMatched8Filter.clear();
      vector<bool> isMuonMatchedSingleMuFilter; isMuonMatchedSingleMuFilter.clear();

      for (unsigned int im = 0; im<analysisTree.muon_count; ++im) {
	allMuons.push_back(im);
	bool muPassed    = true;
	bool mu23Matched = false;
	bool mu17Matched = false;
	bool mu8Matched  = false;
	bool muSingleMatched = false;
	if (analysisTree.muon_pt[im]<5) muPassed = false;
	if (fabs(analysisTree.muon_eta[im])>etaMuonLowCut) muPassed = false;
	if (fabs(analysisTree.muon_dxy[im])>dxyMuonCut) muPassed = false;
	if (fabs(analysisTree.muon_dz[im])>dzMuonCut) muPassed = false;
	if (!analysisTree.muon_isMedium[im]) muPassed = false;
	if (muPassed) idMuons.push_back(im);
	float absIso = analysisTree.muon_r03_sumChargedHadronPt[im];
	float neutralIso = analysisTree.muon_r03_sumNeutralHadronEt[im] + 
	  analysisTree.muon_r03_sumPhotonEt[im] - 
	  0.5*analysisTree.muon_r03_sumPUPt[im];
	neutralIso = TMath::Max(float(0),neutralIso); 
	absIso += neutralIso;
	float relIso = absIso/analysisTree.muon_pt[im];
	if (relIso>isoMuonCut) muPassed = false;
	if (muPassed && analysisTree.muon_pt[im]>ptMuonLowCut) { 
	  isoMuons.push_back(im);
	  isoMuonsValue.push_back(relIso);
	}
	//	cout << "pt:" << analysisTree.muon_pt[im] << "  passed:" << muPassed << endl;
	isMuonPassedIdIso.push_back(muPassed);
	for (unsigned int iT=0; iT<analysisTree.trigobject_count; ++iT) {
	  float dRtrig = deltaR(analysisTree.muon_eta[im],analysisTree.muon_phi[im],
				analysisTree.trigobject_eta[iT],analysisTree.trigobject_phi[iT]);
	  if (dRtrig>DRTrigMatch) continue;
	  if (analysisTree.trigobject_filters[iT][nMuon17Filter] && analysisTree.trigobject_pt[iT]>23.0) 
	    mu23Matched = true;
	  if (analysisTree.trigobject_filters[iT][nMuon17Filter] && analysisTree.trigobject_pt[iT]>17.0)
	    mu17Matched = true;
	  if (analysisTree.trigobject_filters[iT][nSingleMuonFilter] && analysisTree.trigobject_pt[iT]>singleMuonTriggerPtCut)
	    muSingleMatched = true;
	  if (analysisTree.trigobject_filters[iT][nMuon8Filter] && analysisTree.trigobject_pt[iT]>8.0) 
	    mu8Matched = true;
	  
	}
	isMuonMatched23Filter.push_back(mu23Matched);
	isMuonMatched17Filter.push_back(mu17Matched);
	isMuonMatched8Filter.push_back(mu8Matched);
	isMuonMatchedSingleMuFilter.push_back(muSingleMatched);
      }

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
	      int ptBinTrig = binNumber(ptProbe,nPtBinsTrig,ptBinsTrig);
	      int etaBin = binNumber(absEtaProbe,nEtaBins,etaBins);
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
		
		float energy = analysisTree.pfjet_e[jet];
		float chf = analysisTree.pfjet_chargedhadronicenergy[jet]/energy;
		float nhf = 1 - analysisTree.pfjet_chargedhadronicenergy[jet]/energy;
		float phf = 1 - analysisTree.pfjet_chargedemenergy[jet]/energy;
		float elf = analysisTree.pfjet_chargedemenergy[jet]/energy;
		float chm = analysisTree.pfjet_chargedmulti[jet];
		float npr = analysisTree.pfjet_chargedmulti[jet] + analysisTree.pfjet_neutralmulti[jet];
		bool isPFJetId = (npr>1 && phf<0.99 && nhf<0.99) && (absJetEta>2.4 || (elf<0.99 && chf>0 && chm>0));
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
		if (ptProbe>24&&absEtaProbe<2.4) {
		  if (absEtaProbe<1.48) {
		    if (isMuonMatched23Filter[iMu]) 
		      ZMassMu23BarrelPass->Fill(mass,weight); 
		    else
		      ZMassMu23BarrelFail->Fill(mass,weight);
		  }
		  else {
		    if (isMuonMatched23Filter[iMu])
                      ZMassMu23EndcapPass->Fill(mass,weight);
                    else
                      ZMassMu23EndcapFail->Fill(mass,weight);
		  }
		}
		// muon17 filter
		if (isMuonMatched17Filter[iMu]) 
		  ZMassMu17EtaPtPass[etaBin][ptBinTrig]->Fill(mass,weight); 
		else
		  ZMassMu17EtaPtFail[etaBin][ptBinTrig]->Fill(mass,weight); 
		if (ptProbe>18&&absEtaProbe<2.4) {
		  if (absEtaProbe<1.48) {
		    if (isMuonMatched17Filter[iMu]) 
		      ZMassMu17BarrelPass->Fill(mass,weight); 
		    else
		      ZMassMu17BarrelFail->Fill(mass,weight);
		  }
		  else {
		    if (isMuonMatched17Filter[iMu])
                      ZMassMu17EndcapPass->Fill(mass,weight);
                    else
                      ZMassMu17EndcapFail->Fill(mass,weight);
		  }
		}
		// muon8 filter
		if (isMuonMatched8Filter[iMu]) 
		  ZMassMu8EtaPtPass[etaBin][ptBinTrig]->Fill(mass,weight); 
		else
		  ZMassMu8EtaPtFail[etaBin][ptBinTrig]->Fill(mass,weight); 
		if (ptProbe>10&&absEtaProbe<2.4) {
		  if (absEtaProbe<1.48) {
		    if (isMuonMatched8Filter[iMu]) 
		      ZMassMu8BarrelPass->Fill(mass,weight); 
		    else
		      ZMassMu8BarrelFail->Fill(mass,weight);
		  }
		  else {
		    if (isMuonMatched8Filter[iMu])
                      ZMassMu8EndcapPass->Fill(mass,weight);
                    else
                      ZMassMu8EndcapFail->Fill(mass,weight);
		  }
		}
		// single muon filter
		if (absEtaProbe<singleMuonTriggerEtaCut) { 
		  if (isMuonMatchedSingleMuFilter[iMu]) 
		    ZMassIsoMuEtaPtPass[etaBin][ptBinTrig]->Fill(mass,weight); 
		  else
		    ZMassIsoMuEtaPtFail[etaBin][ptBinTrig]->Fill(mass,weight);
		  float offlinePtCut = singleMuonTriggerPtCut + 1.0;
		  if (ptProbe>offlinePtCut) {
		    if (absEtaProbe<1.48) {
		      if (isMuonMatchedSingleMuFilter[iMu]) 
			ZMassIsoMuBarrelPass->Fill(mass,weight); 
		      else
			ZMassIsoMuBarrelFail->Fill(mass,weight);
		    }
		    else {
		      if (isMuonMatchedSingleMuFilter[iMu])
			ZMassIsoMuEndcapPass->Fill(mass,weight);
		      else
			ZMassIsoMuEndcapFail->Fill(mass,weight);
		    }
		  }
		}
	      }
	      else {
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
	    bool isPairSelected = q1*q2 > 0;
	    if (oppositeSign) isPairSelected = q1*q2 < 0;
	    bool isTriggerMatch = (isMu1matched || isMu2matched);
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

      if (isIsoMuonsPair) {      
	//match to genparticles
	bool genmatch_m1 = false, genmatch_m2 = false;
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

	// selecting good jets --->
	if (analysisTree.pfjet_count>1) { 
			
	  for (unsigned int jet=0; jet<2; ++jet) {
	  float energy = analysisTree.pfjet_e[jet];
	  float absJetEta = fabs(analysisTree.pfjet_eta[jet]);
	  float chf = analysisTree.pfjet_chargedhadronicenergy[jet]/energy;
	  float nhf = analysisTree.pfjet_neutralhadronicenergy[jet]/energy;
	  float phf = analysisTree.pfjet_neutralemenergy[jet]/energy;
	  float elf = analysisTree.pfjet_chargedemenergy[jet]/energy;
	  float chm = analysisTree.pfjet_chargedmulti[jet];
	  float npr = analysisTree.pfjet_chargedmulti[jet] + analysisTree.pfjet_neutralmulti[jet];
	  bool isPFJetId = (npr>1 && phf<0.99 && nhf<0.99) && (absJetEta>2.4 || (elf<0.99 && chf>0 && chm>0));
	  if (!isPFJetId) continue;

	  if ( analysisTree.pfjet_pt[0] > 200. && analysisTree.pfjet_pt[1] > 200.) 
	  { 
          float metSel = sqrt(analysisTree.pfmet_ex*analysisTree.pfmet_ex+analysisTree.pfmet_ey*analysisTree.pfmet_ey);
	  hDiJetmet->Fill(metSel, weight);
	  
	  TLorentzVector jmass1; jmass1.SetXYZM(analysisTree.pfjet_px[0],
					analysisTree.pfjet_py[0],
					analysisTree.pfjet_pz[0],
					0);

	  TLorentzVector jmass2; jmass2.SetXYZM(analysisTree.pfjet_px[1],
					analysisTree.pfjet_py[1],
					analysisTree.pfjet_pz[1],
					0);

	  TLorentzVector dijet = jmass1 + jmass2;
	      hDiJetmass->Fill(dijet.M(),weight);
	  }
         }
	 }	 

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
	float HT_=0;
	for (unsigned int jet=0; jet<analysisTree.pfjet_count; ++jet) {
	  float absJetEta = fabs(analysisTree.pfjet_eta[jet]);
	  if (absJetEta>jetEtaCut) continue;
	  HT_=HT_+analysisTree.pfjet_pt[jet];
	  
	  float dR1 = deltaR(analysisTree.pfjet_eta[jet],analysisTree.pfjet_phi[jet],
			     analysisTree.muon_eta[indx1],analysisTree.muon_phi[indx1]);
	  if (dR1<dRJetLeptonCut) continue;
	  
	  float dR2 = deltaR(analysisTree.pfjet_eta[jet],analysisTree.pfjet_phi[jet],
			     analysisTree.muon_eta[indx2],analysisTree.muon_phi[indx2]);
	  
	  if (dR2<dRJetLeptonCut) continue;
	  
	  // pfJetId

	  float energy = analysisTree.pfjet_e[jet];
	  float chf = analysisTree.pfjet_chargedhadronicenergy[jet]/energy;
	  float nhf = analysisTree.pfjet_neutralhadronicenergy[jet]/energy;
	  float phf = analysisTree.pfjet_neutralemenergy[jet]/energy;
	  float elf = analysisTree.pfjet_chargedemenergy[jet]/energy;
	  float chm = analysisTree.pfjet_chargedmulti[jet];
	  float npr = analysisTree.pfjet_chargedmulti[jet] + analysisTree.pfjet_neutralmulti[jet];
	  bool isPFJetId = (npr>1 && phf<0.99 && nhf<0.99) && (absJetEta>2.4 || (elf<0.99 && chf>0 && chm>0));
	  if (!isPFJetId) continue;
	  
	  if (analysisTree.pfjet_pt[jet]>jetPtHighCut) {
	    nJets30++;
	    HT30 += analysisTree.pfjet_pt[jet];
	    if (fabs(analysisTree.pfjet_eta[jet])<jetEtaTrkCut) {
	      HT30etaCut += analysisTree.pfjet_pt[jet];
	      nJets30etaCut++;
	    }
	  }

	  if (analysisTree.pfjet_pt[jet]>jetPtLowCut) {
	    nJets20++;
	    HT20 += analysisTree.pfjet_pt[jet]; 
	    if (fabs(analysisTree.pfjet_eta[jet])<jetEtaTrkCut) {
              HT20etaCut += analysisTree.pfjet_pt[jet];
              nJets20etaCut++;
            }
	  }	  


	}

	hHT_->Fill(HT_,weight);
	if (!isData && applyLeptonSF) {
	  float ptMu1 = TMath::Min(float(59),analysisTree.muon_pt[indx1]);
	  float ptMu2 = TMath::Min(float(59),analysisTree.muon_pt[indx2]);
	  int ptBinMu1 = binNumber(ptMu1,nPtBins,ptBins);
	  int ptBinMu2 = binNumber(ptMu2,nPtBins,ptBins);
	  float absEtaMu1 = fabs(analysisTree.muon_eta[indx1]);
	  float absEtaMu2 = fabs(analysisTree.muon_eta[indx2]);
	  float dataMu1 = 1;
	  float mcMu1 = 1;
	  float dataMu2 = 1;
	  float mcMu2 = 1;
	  if(absEtaMu1 < 1.48){
	    dataMu1 = dataEffBarrel[ptBinMu1];
	    mcMu1 = mcEffBarrel[ptBinMu1];
	  }
	  else {
	    dataMu1 = dataEffEndcap[ptBinMu1];
	    mcMu1 = mcEffEndcap[ptBinMu1];
	  }
	  if(absEtaMu2 < 1.48){
	    dataMu2 = dataEffBarrel[ptBinMu2];
	    mcMu2 = mcEffBarrel[ptBinMu2];
	  }
	  else {
	    dataMu2 = dataEffEndcap[ptBinMu2];
	    mcMu2 = mcEffEndcap[ptBinMu2];
	  }
	  float wMu1 = dataMu1/mcMu1;
	  float wMu2 = dataMu2/mcMu2;
	  //	  cout << "Muons SF : Mu1 = " << wMu1 << "   Mu2 = " << wMu2 << endl;
	  weight = weight*wMu1*wMu2;
	}

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

	massSelH->Fill(massSel,weight);
	for (int iScale=0; iScale<21; ++ iScale) {
	  float scaleFactor = 0.98 + 0.002*float(iScale);
	  massSelScaleH[iScale]->Fill(massSel*scaleFactor,weight);
	}


	if (massSel>20) {
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

	  // cout << "dxy (mu1) = " << analysisTree.muon_dxy[indx1] << "   error = " << analysisTree.muon_dxyerr[indx1] << std::endl;
	  //cout << "dxy (mu2) = " << analysisTree.muon_dxy[indx2] << "   error = " << analysisTree.muon_dxyerr[indx2] << std::endl;
	  //	 vector<int>indexMuTau; indexMuTau.clear();  
	  //	 if (selectZToTauTauMuMu && indexMuTau.size()!=2) continue;
	  float dimuonEta = dimuon.Eta();
	  float dimuonPt = dimuon.Pt();
	  float sumMuonPt = (mu1.Pt()+mu2.Pt());
	  float ptRatio = 0.0;
	  if (sumMuonPt != 0)
	    ptRatio = (dimuonPt/sumMuonPt);
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
	  // if(genmatch_m1 && genmatch_mu2)
	  //h_dcaSigdxy_mu1_genMuMatch->Fill(analysisTree.muon_dxy[indx1]/analysisTree.muon_dxyerr[indx1]);
	  if (genmatch_m1 && genmatch_m2) h_dcaSigdxy_mu1_genMuMatch->Fill(dcaSigdxy_mu1,weight);
          if (genmatch_m1 && genmatch_m2) h_dcaSigdxy_mu2_genMuMatch->Fill(dcaSigdxy_mu2,weight);
          if (genmatch_m1 && genmatch_m2) h_dcaSigdz_mu1_genMuMatch->Fill(dcaSigdz_mu1,weight);
          if (genmatch_m1 && genmatch_m2) h_dcaSigdz_mu2_genMuMatch->Fill(dcaSigdz_mu2,weight);
	  
	  float pfmet_phi = TMath::ATan2(analysisTree.pfmet_ey,analysisTree.pfmet_ex);
	  h_phi_leadingMu_MET->Fill((analysisTree.muon_phi[indx1]-pfmet_phi),weight);
	  h_phi_trailingMu_MET->Fill((analysisTree.muon_phi[indx2]-pfmet_phi),weight);
	  
	  if(!isData){
	    if (dcaSigdxy_muon1<1.8 && dcaSigdxy_muon1>0.2) h_dxy_muon1_dcaCut->Fill(analysisTree.muon_dxy[indx1],weight);
	    if (dcaSigdxy_muon1<1.8 && dcaSigdxy_muon1>0.2) h_dxy_muon2_dcaCut->Fill(analysisTree.muon_dxy[indx2],weight);
	    if (dcaSigdxy_muon1<1.8 && dcaSigdxy_muon1>0.2) h_dimuonEta_dcaCut->Fill(dimuonEta,weight);
	    if (dcaSigdxy_muon1<1.8 && dcaSigdxy_muon1>0.2) h_ptRatio_dcaCut->Fill(ptRatio,weight);
	  }
	  
	  NumberOfVerticesH->Fill(float(analysisTree.primvertex_count),weight);
	  float metSel = sqrt(analysisTree.pfmet_ex*analysisTree.pfmet_ex+analysisTree.pfmet_ey*analysisTree.pfmet_ey);
	  metSelH->Fill(metSel,weight);
	  for (int iScale=0; iScale<21; ++ iScale) {
	    float scaleFactor = 0.7 + 0.03*float(iScale);
	    metSelScaleH[iScale]->Fill(metSel*scaleFactor,weight);
	  }


	  if (massSel>70&&massSel<110) {
	    float unitX = dimuon.Px()/dimuon.Pt();
	    float unitY = dimuon.Py()/dimuon.Pt();
	    float phiUnit = TMath::ATan2(unitY,unitX);
	    float recoilParal = analysisTree.pfmet_ex*unitX + analysisTree.pfmet_ey*unitY;
	    float perpUnitX = TMath::Cos(phiUnit+0.5*TMath::Pi());
	    float perpUnitY = TMath::Sin(phiUnit+0.5*TMath::Pi());
	    float recoilPerp = analysisTree.pfmet_ex*perpUnitX + analysisTree.pfmet_ey*perpUnitY;
	    int jetBin = 0;
	    if (nJets30==1)
	      jetBin = 1;
	    else if (nJets30>1)
	      jetBin = 2;
	    recoilZParalH[jetBin]->Fill(recoilParal,weight);
	    recoilZPerpH[jetBin]->Fill(recoilPerp,weight);

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
  std::cout << "Total number of input events                     = " << allEvents << std::endl;
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



