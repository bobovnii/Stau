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
#include "TPaveText.h"
#include "TRandom.h"
#include "TGraphAsymmErrors.h"
#include <stdlib.h>

#include "DesyTauAnalyses/NTupleMaker/interface/json.h"
#include "DesyTauAnalyses/NTupleMaker/interface/PileUp.h"
#include "DesyTauAnalyses/NTupleMaker/interface/ScaleFactor.h"
#include "DesyTauAnalyses/NTupleMaker/interface/Jets.h"
#include "DesyTauAnalyses/NTupleMaker/interface/AnalysisMacro.h"

int main(int argc, char * argv[]) {

  // first argument - config file 
  // second argument - filelist



  // **** configuration
  Config cfg(argv[1]);
  string Channel="muel";

  // kinematic cuts on electrons
  bool fillplots= false;
  const bool isData = cfg.get<bool>("IsData");
  const bool applyPUreweighting = cfg.get<bool>("ApplyPUreweighting");
  
  const string pileUpInDataFile = cfg.get<string>("pileUpInDataFile");
  const string pileUpInMCFile = cfg.get<string>("pileUpInMCFile");
  
  const bool applyLeptonSF = cfg.get<bool>("ApplyLeptonSF");
  const bool InvertTauIso = cfg.get<bool>("InvertTauIso");
  const bool InvertLeptonIso = cfg.get<bool>("InvertLeptonIso");
  const bool InvertMET = cfg.get<bool>("InvertMET");
  const double ptElectronLowCut   = cfg.get<double>("ptElectronLowCut");
  const double ptElectronHighCut  = cfg.get<double>("ptElectronHighCut");
  const double etaElectronCut     = cfg.get<double>("etaElectronCut");
  const double dxyElectronCut     = cfg.get<double>("dxyElectronCut");
  const double dzElectronCut      = cfg.get<double>("dzElectronCut");
  const double isoElectronLowCut  = cfg.get<double>("isoElectronLowCut");
  const double isoElectronHighCut = cfg.get<double>("isoElectronHighCut");
  const bool applyElectronId     = cfg.get<bool>("ApplyElectronId");

  // vertex cuts
  const double ndofVertexCut  = cfg.get<double>("NdofVertexCut");   
  const double zVertexCut     = cfg.get<double>("ZVertexCut");
  const double dVertexCut     = cfg.get<double>("DVertexCut");
  // kinematic cuts on muons
  const double ptMuonLowCut   = cfg.get<double>("ptMuonLowCut");
  const double ptMuonHighCut  = cfg.get<double>("ptMuonHighCut");
  const double etaMuonCut     = cfg.get<double>("etaMuonCut");
  const double dxyMuonCut     = cfg.get<double>("dxyMuonCut");
  const double dzMuonCut      = cfg.get<double>("dzMuonCut");
  const double isoMuonLowCut  = cfg.get<double>("isoMuonLowCut");
  const double isoMuonHighCut = cfg.get<double>("isoMuonHighCut");
  const double isoMuonHighCutQCD = cfg.get<double>("isoMuonHighCutQCD");
  const bool applyMuonId     = cfg.get<bool>("ApplyMuonId");

  const double ptTauLowCut = cfg.get<double>("ptTauLowCut"); 
  const double etaTauCut = cfg.get<double>("etaTauCut"); 

  const string dataBaseDir = cfg.get<string>("DataBaseDir");

  string TrigLeg  ;
  if (!isData) TrigLeg  = cfg.get<string>("Mu17LegMC");
  if (isData) TrigLeg  = cfg.get<string>("Mu18LegData");
  const string SingleMuonTriggerFile  = cfg.get<string>("SingMuonTriggEff");
  const float singleMuonTriggerPtCut = cfg.get<float>("SingleMuonTriggerPtCut");
  const float singleMuonTriggerEtaCut = cfg.get<float>("SingleMuonTriggerEtaCut");



  const string idIsoEffFile = cfg.get<string>("idIsoEffFile");
  const string trigEffFile = cfg.get<string>("trigEffFile");


  const string Region  = cfg.get<string>("Region");
  const string Sign  = cfg.get<string>("Sign");


  const double leadchargedhadrcand_dz = cfg.get<double>("leadchargedhadrcand_dz");
  const double leadchargedhadrcand_dxy = cfg.get<double>("leadchargedhadrcand_dxy");



  // kinematic cuts on Jets
  const double etaJetCut   = cfg.get<double>("etaJetCut");
  const double ptJetCut   = cfg.get<double>("ptJetCut");
  // topSingleMuonTriggerFile
  const double dRleptonsCutmutau   = cfg.get<double>("dRleptonsCutmutau");
  const double dZetaCut       = cfg.get<double>("dZetaCut");
  const double deltaRTrigMatch = cfg.get<double>("DRTrigMatch");
  const bool oppositeSign    = cfg.get<bool>("oppositeSign");
  const bool isIsoR03 = cfg.get<bool>("IsIsoR03");

  // tau
  const double taupt    = cfg.get<double>("taupt");
  const double taueta    = cfg.get<double>("taueta");
  const double decayModeFinding    = cfg.get<double>("decayModeFinding");
  const double   decayModeFindingNewDMs  = cfg.get<double>("decayModeFindingNewDMs");
  const double   againstElectronVLooseMVA6  = cfg.get<double>("againstElectronVLooseMVA6");
  const double   againstElectronVLooseMVA5  = cfg.get<double>("againstElectronVLooseMVA5");
  const double   againstMuonTight3  = cfg.get<double>("againstMuonTight3");
  const double   vertexz =  cfg.get<double>("vertexz");
  const double   byCombinedIsolationDeltaBetaCorrRaw3Hits = cfg.get<double>("byCombinedIsolationDeltaBetaCorrRaw3Hits");


  const unsigned int RunRangeMin = cfg.get<unsigned int>("RunRangeMin");
  const unsigned int RunRangeMax = cfg.get<unsigned int>("RunRangeMax");

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

  string cmsswBase = (getenv ("CMSSW_BASE"));
  string fullPathToJsonFile = cmsswBase + "/src/DesyTauAnalyses/NTupleMaker/test/json/" + jsonFile;

  const string MuonIdIsoFile = cfg.get<string>("MuonIdIsoEff");
  const string TauFakeRateFile = cfg.get<string>("TauFakeRateEff");

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

  TString MainTrigger(TrigLeg);

  Bool_t          os;
  Bool_t          dilepton_veto;
  Bool_t          extraelec_veto;
  Bool_t          extramuon_veto;


  TString LowPtLegElectron(lowPtLegElectron);
  TString HighPtLegElectron(highPtLegElectron);
  
  TString LowPtLegMuon(lowPtLegMuon);
  TString HighPtLegMuon(highPtLegMuon);

  const double Lumi   = cfg.get<double>("Lumi");
  const double bTag   = cfg.get<double>("bTag");
  const double metcut = cfg.get<double>("metcut");

  CutList.clear();
  CutList.push_back("No cut");
  CutList.push_back("No cut after PU");
  CutList.push_back("mu");
  CutList.push_back("tau");
  CutList.push_back("Trigger");
  CutList.push_back("2nd lepV");
  CutList.push_back("3rd lepV");
  CutList.push_back("Lepton SF");
  CutList.push_back("TauFakeRate");
  CutList.push_back("topPtRwgt");
  CutList.push_back("Cleaned jets");
  CutList.push_back("b-Veto");
  CutList.push_back("MET gt 50");
  CutList.push_back("MET gt 100");
  CutList.push_back("MET gt 150");
  CutList.push_back("MET gt 200");
  CutList.push_back("InvMass gt 100");
  CutList.push_back("dR gt 3");
  CutList.push_back("Jets lt 3");


  int CutNumb = (int)CutList.size();
  xs=1;fact=1;fact2=1;

  unsigned int RunMin = 9999999;
  unsigned int RunMax = 0;

  ifstream ifs("xsecs");
  string line;

  while(std::getline(ifs, line)) // read one line from ifs
    {

      fact=fact2=1;
      istringstream iss(line); // access line as a stream

      // we only need the first two columns
      string dt;
      iss >> dt >> xs >> fact >> fact2;
      //datasetName = dt.c_str();
      //ifs >> dt >> xs; // no need to read further
      //cout<< " "<<dt<<"  "<<endl;
      //cout<< "For sample ========================"<<dt<<" xsecs is "<<xs<<" XSec "<<XSec<<"  "<<fact<<"  "<<fact2<<endl;
      if (  dt == argv[2]) {
	XSec= xs*fact*fact2;
	cout<<" Found the correct cross section "<<xs<<" for Dataset "<<dt<<" XSec "<<XSec<<" number of expected events for Lumi "<<Lumi <<" /pb  = " <<XSec*Lumi<<endl;
      }
      /*
	if ( argv[2] == st1) {ChiMass=100;mIntermediate=200;}
	else if (argv[2] == st2) {ChiMass=200;mIntermediate=500;}
      */
      if (isData) XSec=1.;
      ChiMass=0.0;
    }

  if (XSec<0&& !isData) {cout<<" Something probably wrong with the xsecs...please check  - the input was "<<argv[2]<<endl;return 0;}
	
  xsecs=XSec;

  std::vector<unsigned int> allRuns; allRuns.clear();

  cout<<" ChiMass is "<<ChiMass<<"  "<<mIntermediate<<endl;
  bool doThirdLeptVeto=true;
  bool doMuVeto=true;

  //CutList[CutNumb]=CutListt[CutNumb];
  char ff[100];


  sprintf(ff,"%s/%s",argv[3],argv[2]);

  // reweighting with vertices

  // reading vertex weights
  TFile * fileDataNVert = new TFile(TString(cmsswBase)+"/src/"+dataBaseDir+"/"+vertDataFileName);
  TFile * fileMcNVert   = new TFile(TString(cmsswBase)+"/src/"+dataBaseDir+"/"+vertMcFileName);

  TH1D * vertexDataH = (TH1D*)fileDataNVert->Get(TString(vertHistName));
  TH1D * vertexMcH   = (TH1D*)fileMcNVert->Get(TString(vertHistName));

  float normVertexData = vertexDataH->GetSumOfWeights();
  float normVertexMc   = vertexMcH->GetSumOfWeights();


  vertexDataH->Scale(1/normVertexData);
  vertexMcH->Scale(1/normVertexMc);
/*
  PileUp * PUofficial = new PileUp();

  TFile * filePUdistribution_data = new TFile(TString(cmsswBase)+"/src/DesyTauAnalyses/NTupleMaker/data/PileUpDistrib/Data_Pileup_2015D_Nov17.root","read");
  TFile * filePUdistribution_MC = new TFile (TString(cmsswBase)+"/src/DesyTauAnalyses/NTupleMaker/data/PileUpDistrib/MC_Spring15_PU25_Startup.root", "read");
  TH1D * PU_data = (TH1D *)filePUdistribution_data->Get("pileup");
  TH1D * PU_mc = (TH1D *)filePUdistribution_MC->Get("pileup");
  PUofficial->set_h_data(PU_data);
  PUofficial->set_h_MC(PU_mc);
*/
  PileUp * PUofficial = new PileUp();
  TFile * filePUdistribution_data = new TFile(TString(cmsswBase)+"/src/"+TString(pileUpInDataFile),"read");
  TFile * filePUdistribution_MC = new TFile (TString(cmsswBase)+"/src/"+TString(pileUpInMCFile), "read");
  TH1D * PU_data = (TH1D *)filePUdistribution_data->Get("pileup");
  TH1D * PU_mc = (TH1D *)filePUdistribution_MC->Get("pileup");
  PUofficial->set_h_data(PU_data);
  PUofficial->set_h_MC(PU_mc);




  TFile *f10= new TFile(TString(cmsswBase)+"/src/DesyTauAnalyses/NTupleMaker/data/"+muonSfDataBarrel);  // mu SF barrel data
  TFile *f11 = new TFile(TString(cmsswBase)+"/src/DesyTauAnalyses/NTupleMaker/data/"+muonSfDataEndcap); // mu SF endcap data
  TFile *f12= new TFile(TString(cmsswBase)+"/src/DesyTauAnalyses/NTupleMaker/data/"+muonSfMcBarrel);  // mu SF barrel MC
  TFile *f13 = new TFile(TString(cmsswBase)+"/src/DesyTauAnalyses/NTupleMaker/data/"+muonSfMcEndcap); // mu SF endcap MC 

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


  // Lepton Scale Factors 

  TH1D * MuSF_IdIso_Mu1H = new TH1D("MuIdIsoSF_Mu1H", "MuIdIsoSF_Mu1", 100, 0.5,1.5);


   // Lepton Scale Factors
  ScaleFactor * SF_muonIdIso = new ScaleFactor();
  SF_muonIdIso->init_ScaleFactor(TString(cmsswBase)+"/src/"+TString(MuonIdIsoFile));
  ScaleFactor * SF_muon17 = new ScaleFactor();
  SF_muon17->init_ScaleFactor(TString(cmsswBase)+"/src/"+TString(Muon17TriggerFile));
  ScaleFactor * SF_muon8 = new ScaleFactor();
  SF_muon8->init_ScaleFactor(TString(cmsswBase)+"/src/"+TString(Muon8TriggerFile));
  ScaleFactor * SF_electronIdIso = new ScaleFactor();
  SF_electronIdIso->init_ScaleFactor(TString(cmsswBase)+"/src/"+TString(ElectronIdIsoFile));
  ScaleFactor * SF_electron17 = new ScaleFactor();
  SF_electron17->init_ScaleFactor(TString(cmsswBase)+"/src/"+TString(Electron17TriggerFile));
  ScaleFactor * SF_electron12 = new ScaleFactor();
  SF_electron12->init_ScaleFactor(TString(cmsswBase)+"/src/"+TString(Electron12TriggerFile));


  double Weight=0;
  int nTotalFiles = 0;
  int iCut=0;
  double CFCounter[CutNumb];
  double statUnc[CutNumb];
  int iCFCounter[CutNumb];
  for (int i=0;i < CutNumb; i++){
    CFCounter[i] = 0;
    CFCounter_[i] = 0;
    iCFCounter[i] = 0;
    statUnc[i] =0;
  }
  // file name and tree name
  std::string rootFileName(argv[2]);
  //std::ifstream fileList(argv[2]);
  std::ifstream fileList(ff);
  //std::ifstream fileList0(argv[2]);
  std::ifstream fileList0(ff);
  std::string ntupleName("makeroottree/AC1B");
  std::string initNtupleName("initroottree/AC1B");

  TString era=argv[3];
  TString invMuStr,invTauStr,invMETStr;
  if(InvertLeptonIso) invMuStr = "_InvMuIso_";
  if(InvertTauIso) invTauStr = "_InvTauIso_";
  if(InvertMET) invMETStr = "_InvMET_";

  TString TStrName(rootFileName+invMuStr+invTauStr+invMETStr+"_"+Region+"_"+Sign);
  datasetName = rootFileName.c_str();
  regionName = invMuStr+invTauStr+invMETStr+"_"+Region.c_str()+"_"+Sign.c_str();
  std::cout <<" The filename will be "<<TStrName <<"  "<<datasetName<<std::endl;  
  // output fileName with histograms
  TFile * file;
  if (isData) file = new TFile(era+"/"+TStrName+TString("_DataDriven.root"),"update");
  if (!isData) file = new TFile(era+"/"+TStrName+TString(".root"),"update");
  file->mkdir(Channel.c_str());
  file->cd(Channel.c_str());

  int nFiles = 0;
  int nEvents = 0;
  int selEvents = 0;

  int selEventsAllMuons = 0;
  int selEventsIdMuons = 0;
  int selEventsIsoMuons = 0;
  bool lumi=false;
  bool isLowIsoMu=false;
  bool isHighIsoMu = false;
  bool isLowIsoTau=false;
  bool isHighIsoTau = false;


  std::string dummy;
  // count number of files --->
  while (fileList0 >> dummy) nTotalFiles++;
  //string treename = rootFileName+"_tree.root";
  
  SetupTree(); 
  SetupHists(CutNumb); 
  if (argv[4] != NULL  && atoi(argv[4])< nTotalFiles) nTotalFiles=atoi(argv[4]);
  //if (nTotalFiles>50) nTotalFiles=50;
  //nTotalFiles = 10;
  for (int iF=0; iF<nTotalFiles; ++iF) {

    std::string filen;
    fileList >> filen;

    std::cout << "file " << iF+1 << " out of " << nTotalFiles << " filename : " << filen << std::endl;
    TFile * file_ = TFile::Open(TString(filen));

    TH1D * histoInputEvents = NULL;
    histoInputEvents = (TH1D*)file_->Get("makeroottree/nEvents");
    if (histoInputEvents==NULL) continue;
    int NE = (int)histoInputEvents->GetEntries();
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
      //else
      //histWeightsH->Fill(0.,genweight);
    }

    TTree * _tree = NULL;
    _tree = (TTree*)file_->Get(TString(ntupleName));
    if (_tree==NULL) continue;
    Long64_t numberOfEntries = _tree->GetEntries();
    std::cout << "      number of entries in Tree      = " << numberOfEntries << std::endl;
    AC1B analysisTree(_tree);

    float genweights=1;
    float topPt = -1;
    float antitopPt = -1;
    if(!isData) 
      {

	TTree *genweightsTree = (TTree*)file_->Get("initroottree/AC1B");
	     
	genweightsTree->SetBranchAddress("genweight",&genweights);
	Long64_t numberOfEntriesInit = genweightsTree->GetEntries();
	for (Long64_t iEntryInit=0; iEntryInit<numberOfEntriesInit; ++iEntryInit) { 
	  genweightsTree->GetEntry(iEntryInit);
	  histWeightsH->Fill(0.,genweights);
	}
    
      }



    for (Long64_t iEntry=0; iEntry<numberOfEntries; ++iEntry) { 

      Float_t weight = 1;
      Float_t puweight = 1;
      Float_t topptweight = 1;
      analysisTree.GetEntry(iEntry);
      nEvents++;

      iCut = 0;

      //std::cout << "      number of entries in Tree = " << numberOfEntries <<" starting weight "<<weight<< std::endl;

      if (nEvents%50000==0) 
	cout << "      processed " << nEvents << " events" << endl; 

      if (fabs(analysisTree.primvertex_z)>zVertexCut) continue;
      if (analysisTree.primvertex_ndof<ndofVertexCut) continue;
      double dVertex = (analysisTree.primvertex_x*analysisTree.primvertex_x+
			analysisTree.primvertex_y*analysisTree.primvertex_y);
      if (dVertex>dVertexCut) continue;
      if (analysisTree.primvertex_count<2) continue;  

      //isData= false;
      bool lumi=false;
      isLowIsoMu=false;
      isHighIsoMu = false;
      isLowIsoTau=false;
      isHighIsoTau = false;

      Float_t genweights;
      float topPt = 0;
      float antitopPt = 0;
      bool isZTT = false;
      LSF_weight = 1.;
      TFR_weight = 1.;
      top_weight = 1.;
      all_weight = 1.;
      pu_weight = 1.;
      gen_weight = 1.;
      trig_weight = 1.;
      if(!isData) 
	{
	  for (unsigned int igen=0; igen<analysisTree.genparticles_count; ++igen) {
	    // 		cout<< "  info = " <<  int(analysisTree.genparticles_count) <<"  "<<int(analysisTree.genparticles_pdgid[igen])<<endl;

	    if (analysisTree.genparticles_pdgid[igen]==6)
	      topPt = TMath::Sqrt(analysisTree.genparticles_px[igen]*analysisTree.genparticles_px[igen]+
				  analysisTree.genparticles_py[igen]*analysisTree.genparticles_py[igen]);
	    if (analysisTree.genparticles_pdgid[igen]==-6)
	      antitopPt = TMath::Sqrt(analysisTree.genparticles_px[igen]*analysisTree.genparticles_px[igen]+
				      analysisTree.genparticles_py[igen]*analysisTree.genparticles_py[igen]);

	  }    
				
	  if (!isData ) {
	    weight *= analysisTree.genweight;
	    gen_weight *=analysisTree.genweight;
	  }
					
				
	  lumi=true;
	  //cout<<"  weight from init "<<genweights<< "  "<<analysisTree.genweight<<"  "<<weight<<endl;
	}



      if (isData)  {
	XSec = 1.;
	histRuns->Fill(analysisTree.event_run);
	///////////////according to dimuons
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
	if (analysisTree.event_run == 254833) lumi = false;
	if (!lumi) continue;
	//if (lumi ) cout<<"  =============  Found good run"<<"  "<<n<<"  "<<lum<<endl;
      }


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




      if (!lumi) continue;

	std::vector<TString> metFlags; metFlags.clear();

     //////////////MET filters flag
      if (isData){

	 metFlags.push_back("Flag_HBHENoiseFilter");
	 metFlags.push_back("Flag_HBHENoiseIsoFilter");
	 metFlags.push_back("Flag_CSCTightHalo2015Filter");
	 metFlags.push_back("Flag_EcalDeadCellTriggerPrimitiveFilter");
	 metFlags.push_back("Flag_goodVertices");
	 metFlags.push_back("Flag_eeBadScFilter");

      }


	bool METflag = metFiltersPasses(analysisTree, metFlags);
	if (!METflag) continue;



      JetsMV.clear();
      ElMV.clear();
      TauMV.clear();
      MuMV.clear();
      LeptMV.clear();
      mu_index=-1;
      tau_index=-1;
      el_index=-1;

      if(fillplots)
	FillMainHists(iCut, weight, ElMV, MuMV, TauMV,JetsMV,METV, ChiMass,mIntermediate,analysisTree, Channel, mu_index,el_index,tau_index);
      CFCounter[iCut]+= weight;
      CFCounter_[iCut]+= weight;
      iCFCounter[iCut]++;
      iCut++;




      double MET = sqrt ( analysisTree.pfmet_ex*analysisTree.pfmet_ex + analysisTree.pfmet_ey*analysisTree.pfmet_ey);

      METV.SetPx(analysisTree.pfmet_ex);	      
      METV.SetPy(analysisTree.pfmet_ey);


      for (unsigned int ijj = 0; ijj<analysisTree.pfjet_count; ++ijj) {
	JetsV.SetPxPyPzE(analysisTree.pfjet_px[ijj], analysisTree.pfjet_py[ijj], analysisTree.pfjet_pz[ijj], analysisTree.pfjet_e[ijj]);
	JetsMV.push_back(JetsV);
      } 


      for (unsigned int imm = 0; imm<analysisTree.muon_count; ++imm) {
	MuV.SetPtEtaPhiM(analysisTree.muon_pt[imm], analysisTree.muon_eta[imm], analysisTree.muon_phi[imm], muonMass);
	MuMV.push_back(MuV);
	//	mu_index=0;
      }

      for (unsigned int ie = 0; ie<analysisTree.electron_count; ++ie) {
	ElV.SetPtEtaPhiM(analysisTree.electron_pt[ie], analysisTree.electron_eta[ie], analysisTree.electron_phi[ie], electronMass);
	ElMV.push_back(ElV);
	//	el_index=0;
      }

      for (unsigned int itt = 0; itt<analysisTree.tau_count; ++itt) {
	TauV.SetPtEtaPhiM(analysisTree.tau_pt[itt], analysisTree.tau_eta[itt], analysisTree.tau_phi[itt], tauMass);
	TauMV.push_back(TauV);
	//	tau_index=0;
      }



      if (!isData) 
	{
	  if (applyPUreweighting)	 {
	    puweight = float(PUofficial->get_PUweight(double(analysisTree.numtruepileupinteractions)));
	    weight *=puweight; 
	    pu_weight = puweight;
	  }
	}

      if(fillplots)
	FillMainHists(iCut, weight, ElMV, MuMV, TauMV,JetsMV,METV, ChiMass,mIntermediate,analysisTree, Channel, mu_index,el_index,tau_index);
      CFCounter[iCut]+= weight;
      CFCounter_[iCut]+= weight;
      iCFCounter[iCut]++;
      iCut++;

      bool trigAccept = false;

      unsigned int nLowPtLegElectron = 0;
      bool isLowPtLegElectron = false;
      
      unsigned int nHighPtLegElectron = 0;
      bool isHighPtLegElectron = false;

      unsigned int nLowPtLegMuon = 0;
      bool isLowPtLegMuon = false;
      
      unsigned int nHighPtLegMuon = 0;
      bool isHighPtLegMuon = false;
      bool isMainTrigger  = false;
      if (!isData && (   string::npos != filen.find("stau") || string::npos != filen.find("C1")) ) isMainTrigger = true;

      unsigned int nfilters = analysisTree.run_hltfilters->size();
      //      std::cout << "nfiltres = " << nfilters << std::endl;
      
      if ( string::npos == filen.find("stau") && string::npos == filen.find("C1")) {
      for (unsigned int i=0; i<nfilters; ++i) {
	//	std::cout << "HLT Filter : " << i << " = " << analysisTree.run_hltfilters->at(i) << std::endl;
	TString HLTFilter(analysisTree.run_hltfilters->at(i));
	if (HLTFilter==LowPtLegElectron) {
	  nLowPtLegElectron = i;
	  isLowPtLegElectron = true;
	}
	if (HLTFilter==HighPtLegElectron) {
	  nHighPtLegElectron = i;
	  isHighPtLegElectron = true;
	}
	if (HLTFilter==LowPtLegMuon) {
	  nLowPtLegMuon = i;
	  isLowPtLegMuon = true;
	}
	if (HLTFilter==HighPtLegMuon) {
	  nHighPtLegMuon = i;
	  isHighPtLegMuon = true;
	}
      }
      if (!isLowPtLegElectron) {
	std::cout << "HLT filter " << LowPtLegElectron << " not found" << std::endl;
	return(-1);
      }
      if (!isHighPtLegElectron) {
	std::cout << "HLT filter " << HighPtLegElectron << " not found" << std::endl;
	return(-1);
      }
      if (!isLowPtLegMuon) {
	std::cout << "HLT filter " << LowPtLegMuon << " not found" << std::endl;
	return(-1);
      }
      if (!isHighPtLegMuon) {
	std::cout << "HLT filter " << HighPtLegMuon << " not found" << std::endl;
	return(-1);
      }
      }



      /////now clear the Mu.El.Jets again to fill them again after cleaning
      MuMV.clear();
      ElMV.clear();
      TauMV.clear();
      LeptMV.clear();

      Float_t isoElMin = 9999;
      bool el_iso=false;
      vector<int> electrons; electrons.clear();
      for (unsigned int ie = 0; ie<analysisTree.electron_count; ++ie) {
	electronPtAllH->Fill(analysisTree.electron_pt[ie],weight);
	if (analysisTree.electron_pt[ie]<ptElectronLowCut) continue;
	if (fabs(analysisTree.electron_eta[ie])>etaElectronCut) continue;
	if (fabs(analysisTree.electron_dxy[ie])>dxyElectronCut) continue;
	if (fabs(analysisTree.electron_dz[ie])>dzElectronCut) continue;
	float neutralIso = 
	  analysisTree.electron_neutralHadIso[ie] + 
	  analysisTree.electron_photonIso[ie] - 
	  0.5*analysisTree.electron_puIso[ie];
	neutralIso = TMath::Max(float(0),neutralIso); 
	float absIso = analysisTree.electron_chargedHadIso[ie] + neutralIso;
	float relIso = absIso/analysisTree.electron_pt[ie];
	if (relIso>isoElectronHighCut) continue;
	if (relIso<isoElectronLowCut) continue;
	bool electronMvaId = electronMvaIdTight(analysisTree.electron_superclusterEta[ie],
						analysisTree.electron_mva_id_nontrigPhys14[ie]);
	if (!electronMvaId&&applyElectronId) continue;

	    if (double(relIso)<double(isoElMin)) {
	      isoElMin  = relIso;
	      el_index = ie;
	      el_iso=true;
	      electrons.push_back(ie);
        //      ElMV.push_back(ElV);
	    }

	    if (relIso!=0 && relIso==isoElMin && ie != el_index) {
          if(   analysisTree.electron_pt[ie] > analysisTree.electron_pt[el_index]) > el_index =(int) ie;
	    cout<<" found a pair  " <<relIso <<"  "<<el_index<<"  "<<ie<<endl;
	  }

      }
      if (electrons.size()==0 || !el_iso) continue;

	      ElV.SetPtEtaPhiM(analysisTree.electron_pt[el_index], analysisTree.electron_eta[el_index], analysisTree.muon_phi[el_index], electronMass);
	      LeptMV.push_back(ElV);


      int countmuons=0;
      double isoMuMin = 9999;
      bool mu_iso=false;
      vector<int> muons; muons.clear();
      for (unsigned int im = 0; im<analysisTree.muon_count; ++im) {
	if (analysisTree.muon_pt[im]<ptMuonLowCut) continue;
	if (fabs(analysisTree.muon_eta[im])>etaMuonCut) continue;
	if (fabs(analysisTree.muon_dxy[im])>dxyMuonCut) continue;
	if (fabs(analysisTree.muon_dz[im])>dzMuonCut) continue;


	double absIso= analysisTree.muon_r03_sumChargedHadronPt[im]
	  + max(analysisTree.muon_r03_sumNeutralHadronEt[im] + analysisTree.muon_r03_sumPhotonEt[im]
		- 0.5 * analysisTree.muon_r03_sumPUPt[im],0.0);


	double relIso = absIso/analysisTree.muon_pt[im];
	if (relIso<isoMuonLowCut) continue;

	if (applyMuonId && !analysisTree.muon_isMedium[im]) continue;

	if (double(relIso)<double(isoMuMin)) {
	  isoMuMin  = relIso;
	  mu_index = (int)im;
	  mu_iso=true;
	  muons.push_back(im);
	  //cout<<" after muIso index "<<int(mu_index)<<" pT "<<analysisTree.muon_pt[im]<<" relIso "<<relIso<<" isoMuMin "<<isoMuMin<<" muon_count "<<analysisTree.muon_count<<" im "<<im<<" event "<<iEntry<<endl;
	}
	//cout<<" Indexes here  "<<im<<"   "<<mu_index<<endl;

	if (relIso == isoMuMin && im != mu_index) {
	  //cout<<" found a pair  for muons " <<relIso <<" mu_index  "<<mu_index<<"  pT "<<analysisTree.muon_pt[int(mu_index)]<<" new  index "<<im<<"  pT  "<<analysisTree.muon_pt[int(im)]<<" event "<<iEntry<<endl;
	  if (analysisTree.muon_pt[im] > analysisTree.muon_pt[mu_index] ) mu_index = (int)im ;
	}

      }
      if (!mu_iso || muons.size()==0) continue;
      countmuons++;

      MuV.SetPtEtaPhiM(analysisTree.muon_pt[mu_index], analysisTree.muon_eta[mu_index], analysisTree.muon_phi[mu_index], muonMass);
      MuMV.push_back(MuV);
      LeptMV.push_back(MuV);

      double absIso= analysisTree.muon_r03_sumChargedHadronPt[mu_index]
	+ max(analysisTree.muon_r03_sumNeutralHadronEt[mu_index] + analysisTree.muon_r03_sumPhotonEt[mu_index]
	      - 0.5 * analysisTree.muon_r03_sumPUPt[mu_index],0.0);


      double relIso = absIso/analysisTree.muon_pt[mu_index];

      mu_relIso[0]=relIso;

	if (!isData && (   string::npos != filen.find("stau") || string::npos != filen.find("C1")) ) trigweight = Mu17EffData;
      ////////Control the RelIso cut
      sort(LeptMV.begin(), LeptMV.end(),ComparePt); 
      if (LeptMV.size() == 0 ) continue; 


	weight *= trigweight;
	trig_weight = trigweight;
	//	cout<<" Trigger weight "<<trigweight<<endl;
      }

      if(fillplots)
	FillMainHists(iCut, weight, ElMV, MuMV, TauMV,JetsMV,METV, ChiMass,mIntermediate,analysisTree, Channel, mu_index,el_index,tau_index);
      CFCounter[iCut]+= weight;
      CFCounter_[iCut]+= weight;
      iCFCounter[iCut]++;
      iCut++;

	bool isMu17 = false;
	bool isMu8 = false;
	bool isEle17 = false;
	bool isEle12 = false;
      bool isMuon17matched = false;
      bool isMuon8matched  = false;
      bool isElectron17matched = false;
      bool isElectron12matched = false;

      for (unsigned int im=0; im<muons.size(); ++im) {
	isMu17 = false;
	isMu8 = false;
	      unsigned int mIndex  = muons.at(im);
	
	float neutralHadIsoMu = analysisTree.muon_neutralHadIso[mIndex];
	float photonIsoMu = analysisTree.muon_photonIso[mIndex];
	float chargedHadIsoMu = analysisTree.muon_chargedHadIso[mIndex];
	float puIsoMu = analysisTree.muon_puIso[mIndex];
	if (isIsoR03) {
	  neutralHadIsoMu = analysisTree.muon_r03_sumNeutralHadronEt[mIndex];
	  photonIsoMu = analysisTree.muon_r03_sumPhotonEt[mIndex];
	  chargedHadIsoMu = analysisTree.muon_r03_sumChargedHadronPt[mIndex];
	  puIsoMu = analysisTree.muon_r03_sumPUPt[mIndex];
	}
	float neutralIsoMu = neutralHadIsoMu + photonIsoMu - 0.5*puIsoMu;
	neutralIsoMu = TMath::Max(float(0),neutralIsoMu); 
	float absIsoMu = chargedHadIsoMu + neutralIsoMu;
	float relIsoMu = absIsoMu/analysisTree.muon_pt[mIndex];
	for (unsigned int iT=0; iT<analysisTree.trigobject_count; ++iT) {
	  if (analysisTree.trigobject_filters[iT][nHighPtLegMuon]) { // Mu17 Leg
	    float dRtrig = deltaR(analysisTree.muon_eta[mIndex],analysisTree.muon_phi[mIndex],
				  analysisTree.trigobject_eta[iT],analysisTree.trigobject_phi[iT]);
	  if (dRtrig<deltaRTrigMatch) {
	    if (analysisTree.trigobject_filters[iT][nHighPtLegMuon]&&
		analysisTree.muon_pt[mIndex]>ptMuonHighCut) { // Mu17 Leg
	      isMu17 = true;
	    }
	    if (analysisTree.trigobject_filters[iT][nLowPtLegMuon]&&
		analysisTree.muon_pt[mIndex]>ptMuonLowCut) { // Mu8 Leg
	      isMu8 = true;
	    }
	  }
	}

	if (applyTriggerMatch && (!isMu17) && (!isMu8)) continue;

	/////////////////electrons
	for (unsigned int ie=0; ie<electrons.size(); ++ie) {

	  unsigned int eIndex = electrons.at(ie);

	  float dR = deltaR(analysisTree.electron_eta[eIndex],analysisTree.electron_phi[eIndex],
			    analysisTree.muon_eta[mIndex],analysisTree.muon_phi[mIndex]);

	  if (dR<dRleptonsCut) continue;

	  bool isEle17 = false;
	  bool isEle12 = false;

	  for (unsigned int iT=0; iT<analysisTree.trigobject_count; ++iT) {
	    float dRtrig = deltaR(analysisTree.electron_eta[eIndex],analysisTree.electron_phi[eIndex],
				  analysisTree.trigobject_eta[iT],analysisTree.trigobject_phi[iT]);
	    if (dRtrig<deltaRTrigMatch) {
	      if (analysisTree.trigobject_filters[iT][nHighPtLegElectron]&&
		  analysisTree.electron_pt[eIndex]>ptElectronHighCut) { // Ele17 Leg
		isEle17 = true;
	      }
	      if (analysisTree.trigobject_filters[iT][nLowPtLegElectron]&&
		  analysisTree.electron_pt[eIndex]>ptElectronLowCut) { // Ele12 Leg
		isEle12 = true;
	      }
	    }
	  }
	  
	  bool trigMatch = (isMu17&&isEle12) || (isMu8&&isEle17);
	  //	  std::cout << "Trigger match = " << trigMatch << std::endl;

	  if (applyTriggerMatch && !trigMatch) continue;
	  
	  //	  bool isKinematicMatch = false;
	  //	  if (isMu17&&isEle12) {
	  //	    if (analysisTree.muon_pt[mIndex]>ptMuonHighCut&&analysisTree.electron_pt[eIndex]>ptElectronLowCut)
	  //	      isKinematicMatch = true;
	  //	  }
	  //	  if (isMu8&&isEle17) {
	  //            if (analysisTree.muon_pt[mIndex]>ptMuonLowCut&&analysisTree.electron_pt[eIndex]>ptElectronHighCut)
	  //              isKinematicMatch = true;
	  //          }
	  //	  if (!isKinematicMatch) continue;

	  float neutralHadIsoEle = analysisTree.electron_neutralHadIso[eIndex];
	  float photonIsoEle = analysisTree.electron_photonIso[eIndex];
	  float chargedHadIsoEle = analysisTree.electron_chargedHadIso[eIndex];
	  float puIsoEle = analysisTree.electron_puIso[eIndex];
	  if (isIsoR03) {
	    neutralHadIsoEle = analysisTree.electron_r03_sumNeutralHadronEt[eIndex];
	    photonIsoEle = analysisTree.electron_r03_sumPhotonEt[eIndex];
	    chargedHadIsoEle = analysisTree.electron_r03_sumChargedHadronPt[eIndex];
	    puIsoEle = analysisTree.electron_r03_sumPUPt[eIndex];
	  }
	  float neutralIsoEle = neutralHadIsoEle + photonIsoEle - 0.5*puIsoEle;
	  neutralIsoEle = TMath::Max(float(0),neutralIsoEle); 
	  float absIsoEle =  chargedHadIsoEle + neutralIsoEle;
	  float relIsoEle = absIsoEle/analysisTree.electron_pt[eIndex];

	  if (int(mIndex)!=muonIndex) {
	    if (relIsoMu==isoMuMin) {
	      if (analysisTree.muon_pt[mIndex]>analysisTree.muon_pt[muonIndex]) {
		isoMuMin  = relIsoMu;
		muonIndex = int(mIndex);
		isoEleMin = relIsoEle;
		electronIndex = int(eIndex);
		isMuon17matched = isMu17;
		isMuon8matched = isMu8;
		isElectron17matched = isEle17;
		isElectron12matched = isEle12;
	      }
	    }
	    else if (relIsoMu<isoMuMin) {
	      isoMuMin  = relIsoMu;
	      muonIndex = int(mIndex);
	      isoEleMin = relIsoEle;
	      electronIndex = int(eIndex);
	      isMuon17matched = isMu17;
	      isMuon8matched = isMu8;
	      isElectron17matched = isEle17;
	      isElectron12matched = isEle12;
	    }
	  }
	  else {
	    if (relIsoEle==isoEleMin) {
	      if (analysisTree.electron_pt[eIndex]>analysisTree.electron_pt[electronIndex]) {
		isoEleMin = relIsoEle;
		electronIndex = int(eIndex);
		isElectron17matched = isEle17;
		isElectron12matched = isEle12;
	      }
	    }
	    else if (relIsoEle<isoEleMin) {
	      isoEleMin = relIsoEle;
	      electronIndex = int(eIndex);
	      isElectron17matched = isEle17;
	      isElectron12matched = isEle12;
	    }
	  }
	  
	}
      }


      //      cout << "mIndex = " << muonIndex << "   eIndex = " << electronIndex << std::endl;

      if (electronIndex<0) continue;
      if (muonIndex<0) continue;
      //      std::cout << "Post synch selection " << std::endl;
      //      std::cout << std::endl;

      

      os = (analysisTree.muon_charge[muonIndex]*analysisTree.electron_charge[electronIndex]) < 0;

      // looking for extra electron
      bool foundExtraElectron = false;
      for (unsigned int ie = 0; ie<analysisTree.electron_count; ++ie) {
	if (int(ie)==electronIndex) continue;
	if (analysisTree.electron_pt[ie]<ptVetoElectronCut) continue;
	if (fabs(analysisTree.electron_eta[ie])>etaVetoElectronCut) continue;
	if (fabs(analysisTree.electron_dxy[ie])>dxyVetoElectronCut) continue;
	if (fabs(analysisTree.electron_dz[ie])>dzVetoElectronCut) continue;
	//	bool electronMvaId = electronMvaIdWP90(analysisTree.electron_pt[ie],
	//					       analysisTree.electron_superclusterEta[ie],
	//					       analysisTree.electron_mva_id_nontrigPhys14[ie]);
	bool electronMvaId = analysisTree.electron_mva_wp90_nontrig_Spring15_v1[ie];
	if (!electronMvaId&&applyVetoElectronId) continue;
	if (!analysisTree.electron_pass_conversion[ie]&&applyVetoElectronId&&applyVetoElectronId) continue;
	if (analysisTree.electron_nmissinginnerhits[ie]>1&&applyVetoElectronId&&applyVetoElectronId) continue;
	float neutralHadIsoEle = analysisTree.electron_neutralHadIso[ie];
	float photonIsoEle = analysisTree.electron_photonIso[ie];
	float chargedHadIsoEle = analysisTree.electron_chargedHadIso[ie];
	float puIsoEle = analysisTree.electron_puIso[ie];
	if (isIsoR03) {
	  neutralHadIsoEle = analysisTree.electron_r03_sumNeutralHadronEt[ie];
	  photonIsoEle = analysisTree.electron_r03_sumPhotonEt[ie];
	  chargedHadIsoEle = analysisTree.electron_r03_sumChargedHadronPt[ie];
	  puIsoEle = analysisTree.electron_r03_sumPUPt[ie];
	}
	float neutralIsoEle = neutralHadIsoEle + photonIsoEle - 0.5*puIsoEle;
	neutralIsoEle = TMath::Max(float(0),neutralIsoEle); 
	float absIsoEle =  chargedHadIsoEle + neutralIsoEle;
	float relIsoEle = absIsoEle/analysisTree.electron_pt[ie];	
	if (relIsoEle>isoVetoElectronCut) continue;
	foundExtraElectron = true;
      }

      // looking for extra muon
      bool foundExtraMuon = false;
      for (unsigned int im = 0; im<analysisTree.muon_count; ++im) {
	if (int(im)==muonIndex) continue;
	if (analysisTree.muon_pt[im]<ptVetoMuonCut) continue;
	if (fabs(analysisTree.muon_eta[im])>etaVetoMuonCut) continue;
	if (fabs(analysisTree.muon_dxy[im])>dxyVetoMuonCut) continue;
	if (fabs(analysisTree.muon_dz[im])>dzVetoMuonCut) continue;
	if (applyVetoMuonId && !analysisTree.muon_isMedium[im]) continue;
	float neutralHadIsoMu = analysisTree.muon_neutralHadIso[im];
	float photonIsoMu = analysisTree.muon_photonIso[im];
	float chargedHadIsoMu = analysisTree.muon_chargedHadIso[im];
	float puIsoMu = analysisTree.muon_puIso[im];
	if (isIsoR03) {
	  neutralHadIsoMu = analysisTree.muon_r03_sumNeutralHadronEt[im];
	  photonIsoMu = analysisTree.muon_r03_sumPhotonEt[im];
	  chargedHadIsoMu = analysisTree.muon_r03_sumChargedHadronPt[im];
	  puIsoMu = analysisTree.muon_r03_sumPUPt[im];
	}
	float neutralIsoMu = neutralHadIsoMu + photonIsoMu - 0.5*puIsoMu;
	neutralIsoMu = TMath::Max(float(0),neutralIsoMu); 
	float absIsoMu = chargedHadIsoMu + neutralIsoMu;
	float relIsoMu = absIsoMu/analysisTree.muon_pt[im];
	if (relIsoMu>isoVetoMuonCut) continue;
	foundExtraMuon = true;
      }

      dilepton_veto = false;
      extraelec_veto = foundExtraElectron;
      extramuon_veto = foundExtraMuon;

      if (applyInclusiveSelection) {
	if (extraelec_veto) continue;
	if (extramuon_veto) continue;
	if (isoMuMin>isoMuonHighCut) continue;
	if (isoEleMin>isoElectronHighCut) continue;
//	if (!os) continue;
      }


      double q = analysisTree.tau_charge[tau_index] * analysisTree.muon_charge[mu_index];

      event_sign  = q;


      os = (analysisTree.muon_charge[mu_index]*analysisTree.electron_charge[el_index]) < 0;


      // looking for extra electron
      bool foundExtraElectron = false;
      for (unsigned int ie = 0; ie<analysisTree.electron_count; ++ie) {
	if (int(ie)==electronIndex) continue;
	if (analysisTree.electron_pt[ie]<ptVetoElectronCut) continue;
	if (fabs(analysisTree.electron_eta[ie])>etaVetoElectronCut) continue;
	if (fabs(analysisTree.electron_dxy[ie])>dxyVetoElectronCut) continue;
	if (fabs(analysisTree.electron_dz[ie])>dzVetoElectronCut) continue;
	//	bool electronMvaId = electronMvaIdWP90(analysisTree.electron_pt[ie],
	//					       analysisTree.electron_superclusterEta[ie],
	//					       analysisTree.electron_mva_id_nontrigPhys14[ie]);
	bool electronMvaId = analysisTree.electron_mva_wp90_nontrig_Spring15_v1[ie];
	if (!electronMvaId&&applyVetoElectronId) continue;
	if (!analysisTree.electron_pass_conversion[ie]&&applyVetoElectronId&&applyVetoElectronId) continue;
	if (analysisTree.electron_nmissinginnerhits[ie]>1&&applyVetoElectronId&&applyVetoElectronId) continue;
	float neutralHadIsoEle = analysisTree.electron_neutralHadIso[ie];
	float photonIsoEle = analysisTree.electron_photonIso[ie];
	float chargedHadIsoEle = analysisTree.electron_chargedHadIso[ie];
	float puIsoEle = analysisTree.electron_puIso[ie];
	if (isIsoR03) {
	  neutralHadIsoEle = analysisTree.electron_r03_sumNeutralHadronEt[ie];
	  photonIsoEle = analysisTree.electron_r03_sumPhotonEt[ie];
	  chargedHadIsoEle = analysisTree.electron_r03_sumChargedHadronPt[ie];
	  puIsoEle = analysisTree.electron_r03_sumPUPt[ie];
	}
	float neutralIsoEle = neutralHadIsoEle + photonIsoEle - 0.5*puIsoEle;
	neutralIsoEle = TMath::Max(float(0),neutralIsoEle); 
	float absIsoEle =  chargedHadIsoEle + neutralIsoEle;
	float relIsoEle = absIsoEle/analysisTree.electron_pt[ie];	
	if (relIsoEle>isoVetoElectronCut) continue;
	foundExtraElectron = true;
      }

      // looking for extra muon
      bool foundExtraMuon = false;
      for (unsigned int im = 0; im<analysisTree.muon_count; ++im) {
	if (int(im)==muonIndex) continue;
	if (analysisTree.muon_pt[im]<ptVetoMuonCut) continue;
	if (fabs(analysisTree.muon_eta[im])>etaVetoMuonCut) continue;
	if (fabs(analysisTree.muon_dxy[im])>dxyVetoMuonCut) continue;
	if (fabs(analysisTree.muon_dz[im])>dzVetoMuonCut) continue;
	if (applyVetoMuonId && !analysisTree.muon_isMedium[im]) continue;
	float neutralHadIsoMu = analysisTree.muon_neutralHadIso[im];
	float photonIsoMu = analysisTree.muon_photonIso[im];
	float chargedHadIsoMu = analysisTree.muon_chargedHadIso[im];
	float puIsoMu = analysisTree.muon_puIso[im];
	if (isIsoR03) {
	  neutralHadIsoMu = analysisTree.muon_r03_sumNeutralHadronEt[im];
	  photonIsoMu = analysisTree.muon_r03_sumPhotonEt[im];
	  chargedHadIsoMu = analysisTree.muon_r03_sumChargedHadronPt[im];
	  puIsoMu = analysisTree.muon_r03_sumPUPt[im];
	}
	float neutralIsoMu = neutralHadIsoMu + photonIsoMu - 0.5*puIsoMu;
	neutralIsoMu = TMath::Max(float(0),neutralIsoMu); 
	float absIsoMu = chargedHadIsoMu + neutralIsoMu;
	float relIsoMu = absIsoMu/analysisTree.muon_pt[im];
	if (relIsoMu>isoVetoMuonCut) continue;
	foundExtraMuon = true;
      }

      dilepton_veto = false;
      extraelec_veto = foundExtraElectron;
      extramuon_veto = foundExtraMuon;

      if (applyInclusiveSelection) {
	if (extraelec_veto) continue;
	if (extramuon_veto) continue;
	if (isoMuMin>isoMuonHighCut) continue;
	if (isoEleMin>isoElectronHighCut) continue;
	if (!os) continue;
      }



      if (!isData && applyLeptonSF) {

     
      // scale factors
    double  isoweight_1 = (float)SF_electronIdIso->get_ScaleFactor(double(pt_1),double(eta_1));
    double  isoweight_2 = (float)SF_muonIdIso->get_ScaleFactor(double(pt_2),double(eta_2));

      //      cout << "isoweight_1 = " << isoweight_1
      //           << "isoweight_2 = " << isoweight_2 << endl;

      float Ele17EffData = (float)SF_electron17->get_EfficiencyData(double(pt_1),double(eta_1));
      float Ele17EffMC   = (float)SF_electron17->get_EfficiencyMC(double(pt_1),double(eta_1));

      float Ele12EffData = (float)SF_electron12->get_EfficiencyData(double(pt_1),double(eta_1));
      float Ele12EffMC   = (float)SF_electron12->get_EfficiencyMC(double(pt_1),double(eta_1));

      float Mu17EffData = (float)SF_muon17->get_EfficiencyData(double(pt_2),double(eta_2));
      float Mu17EffMC   = (float)SF_muon17->get_EfficiencyMC(double(pt_2),double(eta_2));

      float Mu8EffData = (float)SF_muon8->get_EfficiencyData(double(pt_2),double(eta_2));
      float Mu8EffMC   = (float)SF_muon8->get_EfficiencyMC(double(pt_2),double(eta_2));

      float trigWeightData = Mu17EffData*Ele12EffData + Mu8EffData*Ele17EffData - Mu17EffData*Ele17EffData;
      float trigWeightMC   = Mu17EffMC*Ele12EffMC     + Mu8EffMC*Ele17EffMC     - Mu17EffMC*Ele17EffMC;

      if (isMuon17matched && isElectron12matched) {
        trigweight_1 = (float)SF_electron12->get_ScaleFactor(double(pt_1),double(eta_1));
        trigweight_2 = (float)SF_muon17->get_ScaleFactor(double(pt_2),double(eta_2));
      }
      else if (isMuon8matched && isElectron17matched) {
        trigweight_1 = (float)SF_electron17->get_ScaleFactor(double(pt_1),double(eta_1));
        trigweight_2 = (float)SF_muon8->get_ScaleFactor(double(pt_2),double(eta_2));
      }

      if (trigWeightMC>1e-6)
        trigweight = trigWeightData / trigWeightMC;

      effweight = isoweight_1*isoweight_2*trigweight;



	//leptonSFweight = SF_yourScaleFactor->get_ScaleFactor(pt, eta)	
	double ptMu1 = (double)analysisTree.muon_pt[mu_index];
	double etaMu1 = (double)analysisTree.muon_eta[mu_index];
	double IdIsoSF_mu1 = SF_muonIdIso->get_ScaleFactor(ptMu1, etaMu1);


	MuSF_IdIso_Mu1H->Fill(IdIsoSF_mu1);
	//	  if (ptMu1<20||ptMu2<20) {
	//	    std::cout << "mu 1 ->  pt = " << ptMu1 << "   eta = " << etaMu1 << std::endl;
	//	    std::cout << "eff data mu 1 = " << SF_muonIdIso->get_EfficiencyData(ptMu1, etaMu1)<< " |  eff mc mu 1 = " << SF_muonIdIso->get_EfficiencyMC(ptMu1, etaMu1)<<std::endl;
	//	    std::cout << "mu 2 ->  pt = " << ptMu2 << "   eta = " << etaMu2 << std::endl;
	//	    std::cout << "eff data mu 2 = " << SF_muonIdIso->get_EfficiencyData(ptMu2, etaMu2)<< " |  eff mc mu 2 = " << SF_muonIdIso->get_EfficiencyMC(ptMu2, etaMu2)<<std::endl;
	//	    std::cout << "SF mu1 = " << IdIsoSF_mu1 << std::endl;
	//	    std::cout << "SF mu2 = " << IdIsoSF_mu2 << std::endl;
	//	    
	//	    std::cout << " mass = " << massSel << std::endl;
	//	    std::cout << std::endl;
	//	  }
	weight *= IdIsoSF_mu1;
	LSF_weight = IdIsoSF_mu1;
      }
      if(fillplots)
	FillMainHists(iCut, weight, ElMV, MuMV, TauMV,JetsMV,METV, ChiMass,mIntermediate,analysisTree, Channel, mu_index,el_index,tau_index);
      CFCounter[iCut]+= weight;
      CFCounter_[iCut]+= weight;
      iCFCounter[iCut]++;
      iCut++;
    
    
      TFR_weight  = 1;

      if (!isData && ( string::npos != filen.find("TTJets")  || string::npos != filen.find("TTPowHeg")) ) 
	{

	  if (topPt>0.&&antitopPt>0.) {
	    float topptweight = topPtWeight(topPt,antitopPt);
	    //  cout<<"  "<<topPt<<"  "<<antitopPt<<endl;
	    weight *= topptweight;
	    top_weight = topptweight;
	  }
	}

      if(fillplots)
	FillMainHists(iCut, weight, ElMV, MuMV, TauMV,JetsMV,METV, ChiMass,mIntermediate,analysisTree, Channel, mu_index,el_index,tau_index);
      CFCounter[iCut]+= weight;
      CFCounter_[iCut]+= weight;
      iCFCounter[iCut]++;
      iCut++;

      //////////////////////////////////////////////
      muon_index = (int)mu_index;
      electron_index = (int)el_index;
      taus_index = (int)tau_index;

      mu_count= (int)analysisTree.muon_count;
      //cout<<" here ============================> "<<iEntry<<"  "<<mu_count<<"  "<<(int)analysisTree.muon_count<<"  "<<analysisTree.muon_count<<endl;
      for (unsigned int im=0;im<analysisTree.muon_count;im++){
	mu_px[im]=analysisTree.muon_px[im];
	mu_py[im]=analysisTree.muon_py[im];
	mu_pz[im]=analysisTree.muon_pz[im];
	mu_eta[im]=analysisTree.muon_eta[im];
	mu_pt[im]=analysisTree.muon_pt[im];
	mu_phi[im]=analysisTree.muon_phi[im];
	mu_charge[im]=analysisTree.muon_charge[im];
	mu_dxy[im]=analysisTree.muon_dxy[im];
	mu_dz[im]=analysisTree.muon_dz[im];
      }
				

      el_count=(int)analysisTree.electron_count;
      for (unsigned int ie=0;ie<analysisTree.electron_count;ie++){
	el_px[ie]=analysisTree.electron_px[ie];
	el_py[ie]=analysisTree.electron_py[ie];
	el_pz[ie]=analysisTree.electron_pz[ie];
	el_eta[ie]=analysisTree.electron_eta[ie];
	el_pt[ie]=analysisTree.electron_pt[ie];
	el_phi[ie]=analysisTree.electron_phi[ie];
	el_charge[ie]=analysisTree.electron_charge[ie];
	el_dxy[ie]=analysisTree.electron_dxy[ie];
	el_dz[ie]=analysisTree.electron_dz[ie];
      }

				
      ta_count=(int)analysisTree.tau_count;
      for (unsigned int it=0;it<analysisTree.tau_count;it++){
	ta_px[it]=analysisTree.tau_px[it];
	ta_py[it]=analysisTree.tau_py[it];
	ta_pz[it]=analysisTree.tau_pz[it];
	ta_eta[it]=analysisTree.tau_eta[it];
	ta_pt[it]=analysisTree.tau_pt[it];
	ta_phi[it]=analysisTree.tau_phi[it];
	ta_charge[it]=analysisTree.tau_charge[it];
	ta_dxy[it]=analysisTree.tau_dxy[it];
	ta_dz[it]=analysisTree.tau_dz[it];
      }
      jet_count=(int)analysisTree.pfjet_count;
      for (unsigned int jj=0;jj<analysisTree.pfjet_count;jj++){

	jet_e[jj] = analysisTree.pfjet_e[jj];
	jet_px[jj] = analysisTree.pfjet_px[jj];
	jet_py[jj] = analysisTree.pfjet_py[jj];
	jet_pz[jj] = analysisTree.pfjet_pz[jj];
	jet_pt[jj] = analysisTree.pfjet_pt[jj];
	jet_eta[jj] = analysisTree.pfjet_eta[jj];
	jet_phi[jj] = analysisTree.pfjet_phi[jj];
	jet_flavour[jj] = analysisTree.pfjet_flavour[jj];
	jet_btag[jj] = analysisTree.pfjet_btag[jj][0];
      }
      met_ex = analysisTree.pfmet_ex;
      met_ey = analysisTree.pfmet_ey;
      met_ez = analysisTree.pfmet_ez;
      met_pt = analysisTree.pfmet_pt;
      met_phi = analysisTree.pfmet_phi;

      all_weight = weight;

      //cout<<" weight here ->>>>>>>>>>>>>>>>>>> "<<weight<<"  "<<all_weight<<endl;

      /////////////////////////////////////////////////////////

      ////////jets cleaning 
      vector<int> jets; jets.clear();
      TLorentzVector leptonsV, muonJ, jetsLV;


      //JetsV.SetPxPyPzE(analysisTree.pfjet_px[ij], analysisTree.pfjet_py[ij], analysisTree.pfjet_pz[ij], analysisTree.pfjet_e[ij]);

      bool btagged= false;


      JetsMV.clear();

      float jetEtaCut = 2.4;
      float DRmax = 0.5;
      int countjets = 0;
      bool dRmuJet = false;
      bool dRtauJet = false;
      for (unsigned int jet=0; jet<analysisTree.pfjet_count; ++jet) {
	float absJetEta = fabs(analysisTree.pfjet_eta[jet]);

	if (absJetEta > etaJetCut) continue;
	if (fabs(analysisTree.pfjet_pt[jet])<ptJetCut) continue;



	bool isPFJetId = false ; 
	isPFJetId =looseJetiD(analysisTree,jet);

	//				cout<<" 1- jet is Loose "<<isPFJetId<<"  "<<jet_isLoose[jet]<<"  iEntry "<<iEntry<<endl;
	if (!isPFJetId) continue;
	jet_isLoose[jet] = isPFJetId;
	//				cout<<"  jet is Loose "<<isPFJetId<<"  "<<jet_isLoose[jet]<<"  "<<iEntry<<endl;
	//for (unsigned int lep=0;LeptMV.size();lep++){
	//double Dr=(LeptMV.at(lep).Eta(),LeptMV.at(lep).Phi(),
	double Dr=deltaR(analysisTree.muon_eta[mu_index],analysisTree.muon_phi[mu_index],
			 analysisTree.pfjet_eta[jet],analysisTree.pfjet_phi[jet]);
	if (  Dr  < DRmax)  continue;

	double Drr=deltaR(analysisTree.tau_eta[tau_index],analysisTree.tau_phi[tau_index],
			  analysisTree.pfjet_eta[jet],analysisTree.pfjet_phi[jet]);
	if (  Drr  < DRmax)  continue;


	if (analysisTree.pfjet_btag[jet][0]  > bTag) btagged = true;
	JetsV.SetPxPyPzE(analysisTree.pfjet_px[jet], analysisTree.pfjet_py[jet], analysisTree.pfjet_pz[jet], analysisTree.pfjet_e[jet]);
	JetsMV.push_back(JetsV);
	countjets++;
      }

      T->Fill();

      continue;
      /////////////////////////////////////////////////


      selEvents++;

    } // end of file processing (loop over events in one file)


    nFiles++;
    delete _tree;
    file_->Close();
    delete file_;
  }
  cout<<"done"<<endl;


  //gen_weight  = histWeightsH->GetSumOfWeights();
  //gen_weight.Fill;

  cout<<" Total events  "<<nEvents<<"  Will use weight  "<<histWeightsH->GetSumOfWeights()<<" Norm Factor for a Lumi of "<<Lumi<<"/pb is "<<XSec*Lumi/( histWeightsH->GetSumOfWeights())<<endl;
  cout<<" First content "<<CFCounter[0]<<endl;
  cout<<" Run range from -----> "<<RunMin<<" to  "<<RunMax<<endl;

  //write out cutflow
  ofstream tfile;
  TString outname=argv[2];
  TString textfilename = "cutflow_"+outname+"_"+Channel+"_"+argv[3]+".txt";
  //  tfile.open(textfilename);
  cout<<" iCFCounter is the pure event count, CFCounter is normalized to "<<Lumi<<" /pb and CFCounter_ is the sum of weights for a given bin"<<endl;
  //  tfile << "########################################" << endl;
  for(int ci = 0; ci < CutNumb; ci++)
    {
      // tfile << CutList[ci]<<"\t & \t"
      //	    << CFCounter[ci]  <<"\t & \t"<< statUnc[ci] <<"\t & \t"<< iCFCounter[ci] << endl;
      CFCounter_[iCut] = iCFCounter[iCut];
      CutFlowUnW->SetBinContent(1+ci,0);
      CutFlow->SetBinContent(1+ci,0);
      CutFlowUnW->SetBinContent(1+ci,float(CFCounter[ci]) );


      CFCounter[ci] *= float(XSec*Lumi/( histWeightsH->GetSumOfWeights()));

      CutFlow->SetBinContent(1+ci,float(CFCounter[ci]));

      cout << " i "<<ci<<" iCFCounter "<<iCFCounter[ci]<<" CFCounter  "<<CFCounter[ci]<<" CFCounter_ " <<CFCounter_[ci]<<"  "<<XSec*Lumi/( histWeightsH->GetSumOfWeights())<<" UnWBinContent "<<CutFlowUnW->GetBinContent(1+ci)<<" Norm to Lumi Content "<<CutFlow->GetBinContent(1+ci)<<" Cut "<<CutList[ci]<<endl;   
      if (iCFCounter[ci] <0.2) statUnc[ci] =0;
      else statUnc[ci] = sqrt(CFCounter[ci]);
    }



  std::cout << std::endl;
  int allEvents = (int)inputEventsH->GetEntries();
  std::cout << "Total number of input events    = " << allEvents << std::endl;
  std::cout << "Total number of events in Tree  = " << nEvents << std::endl;
  std::cout << "Total number of selected events = " << selEvents << std::endl;
  std::cout << std::endl;


  file->cd(Channel.c_str());
  hxsec->Fill(XSec);
  hxsec->Write();
  inputEventsH->Write();
  histWeightsH->Write();
  histRuns->Write();
  CutFlowUnW->Write();
  CutFlow->Write();
  MuSF_IdIso_Mu1H->Write();
  file->Write();
  file->Close();

  delete file;

}

