
#include "/nfs/dust/cms/user/alkaloge/TauAnalysis/new/new/CMSSW_8_0_12/src/DesyTauAnalyses/NTupleMaker/interface/functionsSUSY.h"
#include "/nfs/dust/cms/user/alkaloge/TauAnalysis/new/new/CMSSW_8_0_12/src/DesyTauAnalyses/NTupleMaker/interface/lester_mt2_bisect.h"
//#include "/nfs/dust/cms/user/alkaloge/TauAnalysis/new/new/CMSSW_8_0_12/src/DesyTauAnalyses/NTupleMaker/interface/Basic_Mt2_332_Calculator.h"
//#include "/nfs/dust/cms/user/alkaloge/TauAnalysis/new/new/CMSSW_8_0_12/src/DesyTauAnalyses/NTupleMaker/interface/mTBound.h"
//#include "/nfs/dust/cms/user/alkaloge/TauAnalysis/new/new/CMSSW_8_0_12/src/DesyTauAnalyses/NTupleMaker/interface/TMctLib.h"
//#include "/nfs/dust/cms/user/alkaloge/TauAnalysis/new/new/CMSSW_8_0_12/src/DesyTauAnalyses/NTupleMaker/interface/mctlib.h"
#include "TTree.h"
#include <iostream>

//#include <ScaleFactor.h>
#include <TLorentzVector.h>
#include <TVector2.h>
using namespace std;


const  int CutN=25;



unsigned int RunMin = 9999999;
unsigned int RunMax = 0;
     

//bool isData = false;


double ChiMass=0;
double mIntermediate = tauMass;
double sumpT = 0;
double XSec=-1;
double xs,fact,fact2;
 
vector<TLorentzVector> AllJets_Lepton_noMet;
vector<TLorentzVector> JetsMV;
vector<TLorentzVector>  ElMV;
vector<TLorentzVector>  MuMV;
vector<TLorentzVector>  TauMV;
vector<TLorentzVector>  LeptMV;




double HIP_SF(double pt,double eta)

{

	TString inputRootFile = "/nfs/dust/cms/user/alkaloge/TauAnalysis/new/new/CMSSW_8_0_12/src/DesyTauAnalyses/NTupleMaker/data/HIP_mediumID.root";
TFile * fileIn = new TFile(inputRootFile, "read");
	if (fileIn->IsZombie() ) { std::cout << "ERROR in ScaleFactor::init_ScaleFactor(TString inputRootFile) " <<inputRootFile << " does not exist. Please check. " <<std::endl; exit(1); };


TIter nextkey (fileIn->GetListOfKeys ());
TKey *key = 0;
key = (TKey *) nextkey ();
TObject *obj = key->ReadObj ();
//cout << "2D histos name for SF = " << obj->GetName() << endl;
TH2D *histo = (TH2D*) obj;
//TH2D *histo = (TH2D*)fileIn->Get("histo2D");
double ptN = histo->GetXaxis()->FindBin(pt);
double etaN = histo->GetYaxis()->FindBin(eta);
double result = histo->GetBinContent(ptN,etaN);
delete histo;
delete fileIn;
return result;
}





double TauFakeRate(float pt,float eta, string sel,string working_point){
float SF = 1.;


//80x MVAid

if ( working_point == "MVA"){


if ( sel =="mutau"){

if (  fabs(eta) < 0.9 )
        {
                if (pt>20 && pt<30) SF = 1.07968;
                if (pt>30 && pt<40) SF = 0.847355;
                if (pt>40 ) SF = 0.833833;
        }
if (  fabs(eta) > 0.9 && fabs(eta) < 1.2 )
        {

                if (pt>20 && pt<30) SF = 1.03273;
                if (pt>30 && pt<40) SF = 0.943387;
                if (pt>40 ) SF = 1.07113;
        }

if (  fabs(eta) > 1.2 && fabs(eta) < 2.1 )
        {

                if (pt>20 && pt<30) SF = 1.08635;
                if (pt>30 && pt<40) SF = 1.13754;
                if (pt>40) SF = 1.0387;
        }
if (  fabs(eta) > 2.1 && fabs(eta) < 2.4 )
        {

                if (pt>20 && pt<30) SF = 0.977868;
                if (pt>30 && pt<40) SF = 0.974665;
                if (pt>40) SF = 0.902185;
        }

}//mutau MVA


/*
///mutau for 15invfb
if ( sel =="mutau"){

if (  fabs(eta) < 0.9 )
        {
                if (pt>20 && pt<30) SF = 1.17285;
                if (pt>30 && pt<40) SF = 1.12899;
                if (pt>40 ) SF = 1.19492;
        }
if (  fabs(eta) > 0.9 && fabs(eta) < 1.2 )
        {

                if (pt>20 && pt<30) SF = 1.11631;
                if (pt>30 && pt<40) SF = 1.04866;
                if (pt>40 ) SF = 1.36044;
        }

if (  fabs(eta) > 1.2 && fabs(eta) < 2.1 )
        {

                if (pt>20 && pt<30) SF = 1.08863;
                if (pt>30 && pt<40) SF = 1.13749;
                if (pt>40) SF = 1.21236;
        }
if (  fabs(eta) > 2.1 && fabs(eta) < 2.4 )
        {

                if (pt>20 && pt<30) SF = 0.986986;
                if (pt>30 && pt<40) SF = 0.982437;
                if (pt>40) SF = 0.995135;
        }

}//mutau MVA
//////////////
//
*/


if ( sel =="eltau"){

if (  fabs(eta) < 0.8 )
        {
                if (pt>20 && pt<30) SF = 1.04111;
                if (pt>30 && pt<50) SF = 1.34817;
                if (pt>50 ) SF = 1.60973;
        }
if (  fabs(eta) > 0.8 && fabs(eta) < 1.44 )
        {

                if (pt>20 && pt<30) SF = 1.04158;
                if (pt>30 && pt<50) SF = 1.56632;
                if (pt>50 ) SF = 1.08038;
        }

if (  fabs(eta) > 1.44 && fabs(eta) < 1.566 )
	{


		if (pt>20 && pt<30) SF = 1.15617;
		if (pt>30 && pt<50) SF = 1.30554;
		if (pt>50) SF = 1.;
	}
if (  fabs(eta) > 1.566 && fabs(eta) < 2.1 )
	{


		if (pt>20 && pt<30) SF = 1.01365;
		if (pt>30 && pt<50) SF = 1.00284;
		if (pt>50) SF = 1.20248;
	}
}//eltau MVA


/*
//eltau from 15invfb
if ( sel =="eltau"){

        {
                if (pt>20 && pt<30) SF = 1.08763;
                if (pt>30 && pt<50) SF = 1.52724;
                if (pt>50 ) SF = 1.70454;
        }
if (  fabs(eta) > 1.48 && fabs(eta) < 2.1 )
        {

                if (pt>20 && pt<30) SF = 1.05136;
                if (pt>30 && pt<50) SF = 1.09525;
                if (pt>50 ) SF = 1.29595;
        }

if (  fabs(eta) > 2.1 && fabs(eta) < 2.4 )
	{


		if (pt>20 && pt<30) SF = 0.752314;
		if (pt>30 && pt<50) SF = 1.41862;
		if (pt>50) SF = 1.26267;
	}
}//eltau MVA
if ( sel =="eltau"){

if (  fabs(eta) < 1.48 )
        {
                if (pt>20 && pt<30) SF = 1.08763;
                if (pt>30 && pt<50) SF = 1.52724;
                if (pt>50 ) SF = 1.70454;
        }
if (  fabs(eta) > 1.48 && fabs(eta) < 2.1 )
        {

                if (pt>20 && pt<30) SF = 1.05136;
                if (pt>30 && pt<50) SF = 1.09525;
                if (pt>50 ) SF = 1.29595;
        }

if (  fabs(eta) > 2.1 && fabs(eta) < 2.4 )
	{


		if (pt>20 && pt<30) SF = 0.752314;
		if (pt>30 && pt<50) SF = 1.41862;
		if (pt>50) SF = 1.26267;
	}
}//eltau MVA
if ( sel =="eltau"){

if (  fabs(eta) < 1.48 )
        {
                if (pt>20 && pt<30) SF = 1.08763;
                if (pt>30 && pt<50) SF = 1.52724;
                if (pt>50 ) SF = 1.70454;
        }
if (  fabs(eta) > 1.48 && fabs(eta) < 2.1 )
        {

                if (pt>20 && pt<30) SF = 1.05136;
                if (pt>30 && pt<50) SF = 1.09525;
                if (pt>50 ) SF = 1.29595;
        }

if (  fabs(eta) > 2.1 && fabs(eta) < 2.4 )
	{


		if (pt>20 && pt<30) SF = 0.752314;
		if (pt>30 && pt<50) SF = 1.41862;
		if (pt>50) SF = 1.26267;
	}
}//eltau MVA
if ( sel =="eltau"){

if (  fabs(eta) < 1.48 )
        {
                if (pt>20 && pt<30) SF = 1.08763;
                if (pt>30 && pt<50) SF = 1.52724;
                if (pt>50 ) SF = 1.70454;
        }
if (  fabs(eta) > 1.48 && fabs(eta) < 2.1 )
        {

                if (pt>20 && pt<30) SF = 1.05136;
                if (pt>30 && pt<50) SF = 1.09525;
                if (pt>50 ) SF = 1.29595;
        }

if (  fabs(eta) > 2.1 && fabs(eta) < 2.4 )
	{


		if (pt>20 && pt<30) SF = 0.752314;
		if (pt>30 && pt<50) SF = 1.41862;
		if (pt>50) SF = 1.26267;
	}
}//eltau MVA
*/
}




//CutBased
if (working_point == "CutBased" ){

	if (sel =="eltau"){

if (  fabs(eta) < 1.48 ) 
	{
		if (pt>20 && pt<30) SF = 1.125;
		if (pt>30 && pt<50) SF = 1.75138;
		if (pt>50 ) SF = 2.02406;
	}
if (  fabs(eta) > 1.48 && fabs(eta) < 2.1 ) 
	{

		if (pt>20 && pt<30) SF = 1.06778;
		if (pt>30 && pt<50) SF = 1.36354;
		if (pt>50 ) SF = 1.91359;
	}

if (  fabs(eta) > 2.1 && fabs(eta) < 2.4 ) 
	{

		if (pt>20 && pt<30) SF = 0.856469;
		if (pt>30 && pt<50) SF = 1.28117;
		if (pt>60) SF = 1.80822;
	}
	
}//eltau CutBased


if (sel == "mutau"){

	if (  fabs(eta) < 0.9 )
        {
                if (pt>20 && pt<30) SF = 0.921993;
                if (pt>30 && pt<50) SF = 1.06514;
                if (pt>50 ) SF = 1.44677;
        }
if (  fabs(eta) > 0.9 && fabs(eta) < 1.2 )
        {

                if (pt>20 && pt<30) SF = 1.16935;
                if (pt>30 && pt<50) SF = 1.38829;
                if (pt>50 ) SF = 1.016;
        }

if (  fabs(eta) > 1.2 && fabs(eta) < 2.1 )
	{


		if (pt>20 && pt<30) SF = 1.0466;
		if (pt>30 && pt<50) SF = 1.10949;
		if (pt>50) SF = 1.265;
	}
if (  fabs(eta) > 2.1 && fabs(eta) < 2.4 )
	{

		if (pt>20 && pt<30) SF = 1.1374;
		if (pt>30 && pt<50) SF = 0.859957;
		if (pt>50) SF = 0.584229;
	}

}//mutau CutBased


}//CutBased

//cout<<" working point "<<working_point<<"  "<<sel<<"  "<<pt<<"  "<<eta<<"  "<<SF<<endl;

return SF;

}

double TauiD(string sel,string working_point){

float SF = 1;

if ((sel =="mutau" || sel == "eltau")  && working_point == "CutBased_Loose" )
	SF = 0.81;

if ((sel =="mutau" || sel == "eltau")  && working_point == "CutBased_Medim" )
	SF = 0.79;
if ((sel =="mutau" || sel == "eltau")  && working_point == "CutBased_Tight" )
	SF = 0.79;


if ((sel =="mutau" || sel == "eltau")  && working_point == "MVA_Vloose" )
	SF = 0.83;
if ((sel =="mutau" || sel == "eltau")  && working_point == "MVA_Vloose" )
	SF= 0.84;
if ((sel =="mutau" || sel == "eltau")  && working_point == "MVA_Medium" )
	SF = 0.84;

if ((sel =="mutau" || sel == "eltau")  && working_point == "MVA_Tight" )
	SF = 0.83;

if ((sel =="mutau" || sel == "eltau")  && working_point == "MVA_Vtight" )
	SF = 0.80;



return SF;

}

float Dzeta(TLorentzVector LV, TLorentzVector muV, TLorentzVector MetV)
	{
				float LUnitX = LV.Px()/LV.Pt();
				float LUnitY = LV.Py()/LV.Pt();

				//	cout<<" CHECK =========== "<<tauV.Pt()<<"  "<<ta_pt[tIndex]<<endl;	
				float muonUnitX = muV.Px()/muV.Pt();
				float muonUnitY = muV.Py()/muV.Pt();

				float zetaX = LUnitX + muonUnitX;
				float zetaY = LUnitY + muonUnitY;

				float normZeta = TMath::Sqrt(zetaX*zetaX+zetaY*zetaY);

				zetaX = zetaX/normZeta;
				zetaY = zetaY/normZeta;

				float vectorX = MetV.Px() + muV.Px() + LV.Px();
				float vectorY = MetV.Py() + muV.Py() + LV.Py();

				float vectorVisX = muV.Px() + LV.Px();
				float vectorVisY = muV.Py() + LV.Py();

				// computation of DZeta variable
				// pfmet
				float pzetamiss = MetV.Px()*zetaX + MetV.Py()*zetaY;
				float pzetavis = vectorVisX*zetaX + vectorVisY*zetaY;
				float dzeta = pzetamiss - 0.85*pzetavis;


				return dzeta;
}




  int nPtBins = 8;
  float ptBins[9] = {10,13,16,20,25,30,40,60,1000};

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
  float etaBins[4] = {0,0.9,1.2,2.4}; 
  
  TString PtBins[8] = {"Pt10to13",
		       "Pt13to16",
		       "Pt16to20",
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

//string CutList[10];
vector<string> var;
vector < string > vec;
double var_[1000];

vector<string> CutList;

TH1D * histRuns = new TH1D("histRuns","",6000,24000,30000);

TH1D * histWeightsH = new TH1D("histWeightsH","",1,-0.5,0.5);

TH1D * histTopPt = new TH1D("histTopPt","",1,-0.5,0.5);

TH1D * hWeights [CutN];
TH1D * hEventSign [CutN];

TH1D * hMudxy [CutN];
TH1D * hMudz [CutN];

TH1D * hMudxyerr [CutN];
TH1D * hMudzerr [CutN];
TH1D * hMuIPsigxy [CutN];
TH1D * hMuIPsigz [CutN];


TH1D * hEldxyerr [CutN];
TH1D * hEldzerr [CutN];

TH1D * hElIPsigxy [CutN];
TH1D * hElIPsigz [CutN];

TH1D *hElneutralHadIso[CutN]; 
TH1D *hElphotonIso[CutN];
TH1D *hElchargedHadIso[CutN];
TH1D *hElpuIso[CutN];
TH1D *hElneutralIso[CutN];
TH1D *hElabsIsoEl[CutN];
TH1D *hElrelIsoEl[CutN];


TH1D *hMuneutralHadIso[CutN]; 
TH1D *hMuphotonIso[CutN];
TH1D *hMuchargedHadIso[CutN];
TH1D *hMupuIso[CutN];
TH1D *hMuneutralIso[CutN];
TH1D *hMuabsIsoMu[CutN];
TH1D *hMurelIsoMu[CutN];


TH1D * hEldxy [CutN];
TH1D * hEldz [CutN];

TH1D * htau_dxy [CutN];
TH1D * htau_dz [CutN];

TH1D *hMeffMuon[CutN];
TH1D *hMeffEl[CutN];
TH1D *hMeffTau[CutN];
TH1D *hHTOsqrMET[CutN];
TH1D *hPtOHT[CutN];
TH1D *hMeffMuonOsqrMET[CutN];
TH1D *hMeffElOsqrMET[CutN];
TH1D *hMeffTauOsqrMET[CutN];

TH1D *hHT[CutN];
TH1D *hHText[CutN];
TH1D *hRht[CutN];
TH1D *hHT2[CutN];
TH1D *hHT3[CutN];
TH1D *hHT4[CutN];
//TH1D *hST[CutN];
//TH1D *h0JetpT[CutN];
TH1D *hnJet[CutN];
TH1D *hnpartons[CutN];
TH1D *hnBJet[CutN];

TH1D *hCentrality[CutN];

TH1D *hPtJ0[CutN];
TH1D *hPtJ1[CutN];
TH1D *hPtJ2[CutN];
TH1D *hPtJ3[CutN];

TH1D *hIsoMu[CutN];
TH1D *hIsoEl[CutN];
TH1D *hIsoTau[CutN];

TH1D *hInvMassMuTau[CutN];
TH1D *hInvMassMuEl[CutN];
TH1D *hInvMassTauTau[CutN];
TH1D *hInvMassElEl[CutN];
TH1D *hInvMassElTau[CutN];
TH1D *hInvMassMuMu[CutN];


TH1D *hnEl[CutN];
TH1D *hElpt[CutN];
TH1D *hEleta[CutN];

TH1D *hnLep[CutN];
TH1D *hLeppt[CutN];
TH1D *hLepeta[CutN];

TH1D *hnMu[CutN];
TH1D *hMupt[CutN];
TH1D *hMueta[CutN];



TH1D *hnTau[CutN];
TH1D *hTaupt[CutN];
TH1D *hTaueta[CutN];


TH1D *hMET[CutN];
TH1D *hMETphi[CutN];
//TH1D *hnOver[CutN];
TH1D *hdPhiMETLep[CutN];
TH1D *hdPhiMETMu[CutN];
TH1D *hdPhiMETEl[CutN];
TH1D *hdPhiMETTau[CutN];
TH1D *hdPhiJMET[CutN];
TH1D *hdPhiJ0Tau[CutN];
TH1D *hdPhiJ0MET[CutN];
TH1D *hdPhiJ1MET[CutN];
TH1D *hdPhiJ2MET[CutN];
TH1D *hdPhiJ3MET[CutN];
TH1D *hdPhiMuMET[CutN];
TH1D *hdPhiElMET[CutN];
TH1D *hdPhiTauMET[CutN];

TH1D *hMT[CutN];
TH1D *hMTsum[CutN];
TH1D *hMTtot[CutN];
TH1D *hMTel[CutN];
TH1D *hMTmu[CutN];
TH1D *hMTtau[CutN];
TH1D *hDZeta[CutN];

TH1D *hMTmutau[CutN];
TH1D *hMTmuel[CutN];
TH1D *hMTeltau[CutN];
TH1D *hMTtautau[CutN];

TH1D *hMt2lestermutau[CutN];
TH1D *hMt2mutau[CutN];
TH1D *hMCTmutau[CutN];
TH1D *hMCTxmutau[CutN];
TH1D *hMCTymutau[CutN];
TH1D *hMCTbmutau[CutN];


TH1D *hMt2lestermuel[CutN];
TH1D *hMt2muel[CutN];
TH1D *hMCTmuel[CutN];
TH1D *hMCTxmuel[CutN];
TH1D *hMCTymuel[CutN];
TH1D *hMCTbmuel[CutN];

TH1D *hMt2lestereltau[CutN];
TH1D *hMt2eltau[CutN];
TH1D *hMCTeltau[CutN];
TH1D *hMCTxeltau[CutN];
TH1D *hMCTyeltau[CutN];
TH1D *hMCTbeltau[CutN];

TH1D *hTBoundmutau[CutN];

TH1D *hdR_mutau[CutN];
TH1D *hdR_eltau[CutN];
TH1D *hdR_tautau[CutN];
TH1D *hdR_muel[CutN];
TH1D *hdR_taujet[CutN];


TH1D *hnpv[CutN];
TH1D *hnpu[CutN];
TH1D *hnrho[CutN];


TH2D *hmet_MT[CutN];
TH2D *hmet_MTsum[CutN];
TH2D *hmet_MTtot[CutN];
TH2D *hmet_DZeta[CutN];
TH2D *hmet_MCTb[CutN];
TH2D *hDZeta_MTsum[CutN];
TH2D *hDZeta_MTtot[CutN];

TH2D *hDZeta_MCTb[CutN];

TH2D *hMTsum_MCTb[CutN];
TH2D *hMTtot_MCTb[CutN];

TH2D *hmet_MTel[CutN];
TH2D *hmet_MTmu[CutN];
TH2D *hmet_MTtau[CutN];

TH2D *hdR_dPhi[CutN];
TH2D *hdRmt_dPhi[CutN];
TH2D *hdRet_dPhi[CutN];
TH2D *hdRme_dPhi[CutN];
TH2D *hdRtt_dPhi[CutN];

TH2D *hmet_dPhi[CutN];
TH2D *hmet_dPhiel[CutN];
TH2D *hmet_dPhimu[CutN];
TH2D *hmet_dPhitau[CutN];

TH2D *hMT_dPhi[CutN];
TH2D *hMT_dPhiel[CutN];
TH2D *hMT_dPhimu[CutN];
TH2D *hMT_dPhitau[CutN];

TH2D *hIso_sign[CutN];
 
     

TH2D *hMT2lester_MCTb[CutN];
TH2D *hMT2lester_met[CutN];
TH2D *hMT2lester_DZeta[CutN];


TH1D *CutFlow= new TH1D("CutFlow","Cut Flow",CutN,1,CutN+1);
TH1D *CutFlowUnW= new TH1D("CutFlowUnW","Cut Flow",CutN,1,CutN+1);
TH1D *CutFlowUnWNorm= new TH1D("CutFlowUnWNorm","Cut Flow",CutN,1,CutN+1);

TH1D * inputEventsH = new TH1D("inputEventsH","",1,-0.5,0.5);
TH1D * hxsec = new TH1D("xsec","",1,0,10e+20);


TH1D * muonPtAllH = new TH1D("muonPtAllH","",10,0,200);
TH1D * electronPtAllH = new TH1D("electronPtAllH","",10,0,200);
TH1D * tauPtAllH = new TH1D("tauPtAllH","",10,0,200);

// histograms (dilepton selection)
TH1D * electronPtH  = new TH1D("electronPtH","",10,0,200);
TH1D * electronEtaH = new TH1D("electronEtaH","",50,-2.5,2.5); 
TH1D * muonPtH  = new TH1D("muonPtH","",10,0,200);
TH1D * muonEtaH = new TH1D("muonEtaH","",50,-2.5,2.5); 
TH1D * tauEtaAllH = new TH1D("tauEtaAllH","",50,-2.5,2.5); 
 
TH1D * dileptonMassH = new TH1D("dileptonMassH","",10,0,200);
TH1D * dileptonPtH = new TH1D("dileptonPtH","",10,0,200);
TH1D * dileptonEtaH = new TH1D("dileptonEtaH","",100,-5,5);
TH1D * dileptondRH = new TH1D("dileptondRH","",60,0,6);
TH1D * ETmissH = new TH1D("ETmissH","",10,0,200);
TH1D * MtH = new TH1D("MtH_2l","",10,0,200);
TH1D * DZetaH = new TH1D("DZetaH","",60,-400,200);

// histograms (dilepton selection + DZeta cut DZeta)
TH1D * electronPtSelH  = new TH1D("electronPtSelH","",10,0,200);
TH1D * electronEtaSelH = new TH1D("electronEtaSelH","",50,-2.5,2.5); 
TH1D * muonPtSelH  = new TH1D("muonPtSelH","",10,0,200);
TH1D * muonEtaSelH = new TH1D("muonEtaSelH","",50,-2.5,2.5); 

TH1D * dileptonMassSelH = new TH1D("dileptonMassSelH","",10,0,200);
TH1D * dileptonPtSelH = new TH1D("dileptonPtSelH","",10,0,200);
TH1D * dileptonEtaSelH = new TH1D("dileptonEtaSelH","",100,-5,5);
TH1D * dileptondRSelH = new TH1D("dileptondRSelH","",60,0,6);
TH1D * ETmissSelH = new TH1D("ETmissSelH","",10,0,200);
TH1D * MtSelH = new TH1D("MtSelH_2l","",10,0,200);
TH1D * DZetaSelH = new TH1D("DZetaSelH","",60,-400,200);
/*
TLorentzVector ElV, MuV, TauV, JetsV, METV;

vector<TLorentzVector> AllJets_Lepton_noMet;
vector<TLorentzVector> JetsMV;
vector<TLorentzVector>  ElMV;
vector<TLorentzVector>  MuMV;
vector<TLorentzVector>  TauMV;
vector<TLorentzVector>  LeptMV;
*/

std::vector<pair<string,float> > variables_;
std::vector<pair<string,float> > variablesMC_;

TTree *T;

void WriteHists(int CutNer, TFile *in, TString dir){

  in->cd(dir);

  for(int cj = 0; cj < CutNer; cj++)
    {

      hHTOsqrMET[cj]->Write();
    }
}



//string CutList[CutN];// ={"No cut","Trigger","2- l", "dR < "};
void SetupHists(int CutNer){

for(int cj = 0; cj < CutNer; cj++)
    {
      CutFlow->GetXaxis()->SetBinLabel(cj+1,CutList[cj].c_str());
      CutFlowUnW->GetXaxis()->SetBinLabel(cj+1,CutList[cj].c_str());
  //  }

//for(int cj = 0; cj < CutNer; cj++)
  //  {
      TString cutName=CutList[cj];
      TString nCut;
      nCut.Form("%i",cj);
      ///generic variables
      
      
	
      hMuneutralHadIso[cj] = new TH1D("mu_neutralHadIso_"+nCut,"mu_neutralHadIso "+cutName,150,0,300);
      hMuneutralHadIso[cj]->Sumw2();

      hMuphotonIso[cj] = new TH1D("mu_photonIso_"+nCut,"mu_photonIso "+cutName,150,0,300);
      hMuphotonIso[cj]->Sumw2();

      hMuchargedHadIso[cj] = new TH1D("mu_chargedHadIso_"+nCut,"mu_chargedHadIso "+cutName,150,0,300);
      hMuchargedHadIso[cj]->Sumw2();

      hMupuIso[cj] = new TH1D("mu_puIso_"+nCut,"mu_puIso "+cutName,200,0,200);
      hMupuIso[cj]->Sumw2();

      hMuneutralIso[cj] = new TH1D("mu_neutralIso_"+nCut,"mu_neutralIso "+cutName,150,0,300);
      hMuneutralIso[cj]->Sumw2();

      hMuabsIsoMu[cj] = new TH1D("mu_absIsoMu_"+nCut,"mu_absIsoMu "+cutName,150,0,300);
      hMuabsIsoMu[cj]->Sumw2();

      hMurelIsoMu[cj] = new TH1D("mu_relIsoMu_"+nCut,"mu_relIsoMu "+cutName,200,0,10);
      hMurelIsoMu[cj]->Sumw2();

      hElneutralHadIso[cj] = new TH1D("el_neutralHadIso_"+nCut,"el_neutralHadIso "+cutName,150,0,300);
      hElneutralHadIso[cj]->Sumw2();

      hElphotonIso[cj] = new TH1D("el_photonIso_"+nCut,"el_photonIso "+cutName,150,0,300);
      hElphotonIso[cj]->Sumw2();

      hElchargedHadIso[cj] = new TH1D("el_chargedHadIso_"+nCut,"el_chargedHadIso "+cutName,150,0,300);
      hElchargedHadIso[cj]->Sumw2();

      hElpuIso[cj] = new TH1D("el_puIso_"+nCut,"el_puIso "+cutName,200,0,200);
      hElpuIso[cj]->Sumw2();

      hElneutralIso[cj] = new TH1D("el_neutralIso_"+nCut,"el_neutralIso "+cutName,150,0,300);
      hElneutralIso[cj]->Sumw2();

      hElabsIsoEl[cj] = new TH1D("el_absIsoEl_"+nCut,"el_absIsoEl "+cutName,150,0,300);
      hElabsIsoEl[cj]->Sumw2();

      hElrelIsoEl[cj] = new TH1D("el_relIsoEl_"+nCut,"el_relIsoEl "+cutName,200,0,10);
      hElrelIsoEl[cj]->Sumw2();

      hMudxy[cj] = new TH1D ("mu_dxy_"+nCut,"mu_dxy "+cutName,20,-.2,.2);
      hMudxy[cj]->Sumw2();
      hMudxyerr[cj] = new TH1D ("mu_dxyerr_"+nCut,"mu_dxyerr "+cutName,20,-.2,.2);
      hMudxyerr[cj]->Sumw2();
 
      hEldxy[cj] = new TH1D ("el_dxy_"+nCut,"el_dxy "+cutName,20,-.2,.2);
      hEldxy[cj]->Sumw2();
      hEldxyerr[cj] = new TH1D ("el_dxyerr_"+nCut,"el_dxyerr "+cutName,20,-.2,.2);
      hEldxyerr[cj]->Sumw2();

      htau_dxy[cj] = new TH1D ("tau_dxy_"+nCut,"tau_dxy "+cutName,20,-.2,.2);
      htau_dxy[cj]->Sumw2();
      
      htau_dz[cj] = new TH1D ("tau_dz_"+nCut,"tau_dz "+cutName,20,-.2,.2);
      htau_dz[cj]->Sumw2();
      hMudz[cj] = new TH1D ("mu_dz_"+nCut,"mu_dz "+cutName,20,-.2,.2);
      hMudz[cj]->Sumw2();
      hEldz[cj] = new TH1D ("el_dz_"+nCut,"el_dz "+cutName,20,-.2,.2);
      hEldz[cj]->Sumw2();

      hMudzerr[cj] = new TH1D ("mu_dzerr_"+nCut,"mu_dzerr "+cutName,20,-.2,.2);
      hMudzerr[cj]->Sumw2();
      hEldzerr[cj] = new TH1D ("el_dzerr_"+nCut,"el_dzerr "+cutName,20,-.2,.2);
      hEldzerr[cj]->Sumw2();
	
      hMuIPsigxy[cj] = new TH1D ("muIPsigxy_"+nCut,"muIPsigxy "+cutName,50,0,10);
      hMuIPsigz[cj] = new TH1D ("muIPsigz_"+nCut,"muIPsigz "+cutName,50,0,10);
 
      hElIPsigxy[cj] = new TH1D ("elIPsigxy_"+nCut,"elIPsigxy "+cutName,50,0,10);
      hElIPsigz[cj] = new TH1D ("elIPsigz_"+nCut,"elIPsigz "+cutName,50,0,10);

      hHTOsqrMET[cj] = new TH1D ("hHTOsqrMET_"+nCut,"hHTOsqrMET "+cutName,10,0.0,100.0);
      hHTOsqrMET[cj]->Sumw2();
      hPtOHT[cj] = new TH1D ("hPtOHT_"+nCut,"hPtOHT "+cutName,10,0.0,10.0);
      hPtOHT[cj]->Sumw2();
      
      hMeffMuonOsqrMET[cj] = new TH1D ("hMeffMuonOsqrMET_"+nCut,"hMeffMuonOsqrMET "+cutName,10,0.0,100.0);
      hMeffMuonOsqrMET[cj]->Sumw2();
      hMeffElOsqrMET[cj] = new TH1D ("hMeffElOsqrMET_"+nCut,"hMeffElOsqrMET "+cutName,10,0.0,100.0);
      hMeffElOsqrMET[cj]->Sumw2();
      hMeffTauOsqrMET[cj] = new TH1D ("hMeffTauOsqrMET_"+nCut,"hMeffTauOsqrMET "+cutName,10,0.0,100.0);
      hMeffTauOsqrMET[cj]->Sumw2();

      hMeffMuon[cj] = new TH1D ("hMeffMuon_"+nCut,"hMeffMuon "+cutName,50,0.0,500.0);
      hMeffMuon[cj]->Sumw2();
      hMeffEl[cj] = new TH1D ("hMeffEl_"+nCut,"hMeffEl "+cutName,50,0.0,500.0); 
      hMeffEl[cj]->Sumw2();
      hMeffTau[cj] = new TH1D ("hMeffTau_"+nCut,"hMeffTau "+cutName,50,0.0,500.0); 
      hMeffTau[cj]->Sumw2();

      hCentrality[cj]  = new TH1D ("hCentrality_"+nCut,"hCentrality "+cutName,20,0.,1.);
      hCentrality[cj]->Sumw2();

      hHT[cj] = new TH1D ("HT_"+nCut,"HT "+cutName,10,0.0,1000.0);
      hHT[cj]->Sumw2();
 
      hRht[cj] = new TH1D ("Rht_"+nCut,"Rht "+cutName,10,0.0,2.0);
      hRht[cj]->Sumw2();
    
      hHText[cj] = new TH1D ("HText_"+nCut,"HText "+cutName,60,0.0,1600.0);
      hHText[cj]->Sumw2();
      hHT2[cj] = new TH1D ("HT2_"+nCut,"HT2 "+cutName,60,0.0,1600.0);
      hHT2[cj]->Sumw2();
      hHT3[cj] = new TH1D ("HT3_"+nCut,"HT3 "+cutName,60,0.0,1600.0);
      hHT3[cj]->Sumw2();
      hHT4[cj] = new TH1D ("HT4_"+nCut,"HT4 "+cutName,60,0.0,1600.0);
      hHT4[cj]->Sumw2();
          
      hPtJ0[cj] = new TH1D ("hPtJ0_"+nCut,"hPtJ0 "+cutName,60,0.0,1600.0);
      hPtJ0[cj]->Sumw2();
      hPtJ1[cj] = new TH1D ("hPtJ1_"+nCut,"hPtJ1 "+cutName,60,0.0,1600.0);
      hPtJ1[cj]->Sumw2();
      hPtJ2[cj] = new TH1D ("hPtJ2_"+nCut,"hPtJ2 "+cutName,60,0.0,1600.0);
      hPtJ2[cj]->Sumw2();
      hPtJ3[cj] = new TH1D ("hPtJ3_"+nCut,"hPtJ3 "+cutName,60,0.0,1600.0);
      hPtJ3[cj]->Sumw2();
     

      //h0JetpT[cj] = new TH1D ("0JetpT_"+nCut,"0JetpT "+cutName,60,0.0,1600.0);
      //h0JetpT[cj]->Sumw2();
      hnJet[cj] = new TH1D ("nJet_"+nCut,"nJet "+cutName,25,-0.5,24.5);
      hnJet[cj]->Sumw2();
      hnpartons[cj] = new TH1D ("npartons_"+nCut,"npartons "+cutName,6,-0.5,5.5);
      hnpartons[cj]->Sumw2();
      hnBJet[cj] = new TH1D ("nBJet_"+nCut,"nBJet "+cutName,10,-0.5,9.5);
      hnBJet[cj]->Sumw2();

      hWeights[cj] = new TH1D ("hWeights_"+nCut,"hWeights "+cutName,10,-1,9);
      hWeights[cj]->Sumw2();
      hEventSign[cj] = new TH1D ("hEventSign_"+nCut,"hEventSign "+cutName,20,-10,10);
      hEventSign[cj]->Sumw2();
	
      hDZeta[cj] = new TH1D("hDZeta_"+nCut,"hDZeta"+cutName,60,-400,200);
      hDZeta[cj]->Sumw2();
      hInvMassMuTau[cj] = new TH1D ("hInvMassMuTau_"+nCut,"hInvMassMuTau "+cutName,50,0,500);
      hInvMassMuTau[cj]->Sumw2();
      hInvMassMuEl[cj] = new TH1D ("hInvMassMuEl_"+nCut,"hInvMassMuel "+cutName,50,0,500);
      hInvMassMuEl[cj]->Sumw2();
      hInvMassMuMu[cj] = new TH1D ("hInvMassMuMu_"+nCut,"hInvMassMuMu "+cutName,50,0,500);
      hInvMassMuMu[cj]->Sumw2();
      hInvMassElTau[cj] = new TH1D ("hInvMassElTau_"+nCut,"hInvMassElTau "+cutName,50,0,500);
      hInvMassElTau[cj]->Sumw2();
      hInvMassElEl[cj] = new TH1D ("hInvMassElEl_"+nCut,"hInvMassElEl "+cutName,50,0,500);
      hInvMassElEl[cj]->Sumw2();
      hInvMassTauTau[cj] = new TH1D ("hInvMassTauTau_"+nCut,"hInvMassTauTau "+cutName,50,0,500);
      hInvMassTauTau[cj]->Sumw2();
        
      //Leptons
      //
      //
      hnLep[cj] = new TH1D ("nLep_"+nCut,"nLep "+cutName,10,-0.5,9.5);
      hnLep[cj]->Sumw2();
      hLeppt[cj] = new TH1D ("LeppT_"+nCut,"Lep pT "+cutName,50,0,500);
      hLeppt[cj]->Sumw2();
      hLepeta[cj] = new TH1D ("Lepeta_"+nCut,"Lep eta "+cutName,40,-4,4);
      hLepeta[cj]->Sumw2();
      //hST[cj] = new TH1D ("ST_"+nCut,"ST "+cutName,60,0.0,1600.0);
      //hST[cj]->Sumw2();
        
      //Muons
      //
      //
      hnMu[cj] = new TH1D ("nMu_"+nCut,"nMu "+cutName,10,-0.5,9.5);
      hnMu[cj]->Sumw2();
      hMupt[cj] = new TH1D ("MupT_"+nCut,"Mu pT "+cutName,50,0,500);
      hMupt[cj]->Sumw2();
      hMueta[cj] = new TH1D ("Mueta_"+nCut,"Mu eta "+cutName,40,-4,4);
      hMueta[cj]->Sumw2();
        
      //Taus
      //
      //
      hnTau[cj] = new TH1D ("nTau_"+nCut,"nTau "+cutName,10,-0.5,9.5);
      hnTau[cj]->Sumw2();
      hTaupt[cj] = new TH1D ("TaupT_"+nCut,"Tau pT "+cutName,50,0,500);
      hTaupt[cj]->Sumw2();
      hTaueta[cj] = new TH1D ("Taueta_"+nCut,"Tau eta "+cutName,40,-4,4);
      hTaueta[cj]->Sumw2();
	
      //hnOver[cj] = new TH1D ("nOver_"+nCut,"nOver "+cutName,2,0,2);
      //Electrons
      //
      //
      hnEl[cj] = new TH1D ("nEl_"+nCut,"nEl "+cutName,10,-0.5,9.5);
      hnEl[cj]->Sumw2();
      hElpt[cj] = new TH1D ("ElpT_"+nCut,"El pT "+cutName,50,0,500);
      hElpt[cj]->Sumw2();
      hEleta[cj] = new TH1D ("Eleta_"+nCut,"El eta "+cutName,40,-4,4);
      hEleta[cj]->Sumw2();
       
      hIsoMu[cj] = new TH1D ("IsoMu_"+nCut,"Mu Iso "+cutName,100,0,5);
      hIsoMu[cj]->Sumw2();
      hIsoEl[cj] = new TH1D ("IsoEl_"+nCut,"El Iso "+cutName,100,0,5);
      hIsoEl[cj]->Sumw2();
      hIsoTau[cj] = new TH1D ("IsoTau_"+nCut,"Tau Iso "+cutName,100,0,5);
      hIsoTau[cj]->Sumw2();
       
      hMET[cj] = new TH1D("MET_"+nCut,"MET "+cutName,40.0,0.0,800.0);
      hMET[cj]->Sumw2();
	

      //dPhi
      //
      //
      hMETphi[cj] = new TH1D("METphi_"+nCut,"METphi "+cutName,64,0.0,3.2);
      hMETphi[cj]->Sumw2();

      hdPhiMETLep[cj] = new TH1D("dPhiMETLep_"+nCut,"dPhiMETLep "+cutName,64,0.0,3.2);
      hdPhiMETLep[cj]->Sumw2();
    
      hdPhiMETMu[cj] = new TH1D("dPhiMETMu_"+nCut,"dPhiMETMu "+cutName,64,0.0,3.2);
      hdPhiMETMu[cj]->Sumw2();
      hdPhiMETEl[cj] = new TH1D("dPhiMETEl_"+nCut,"dPhiMETEl "+cutName,64,0.0,3.2);
      hdPhiMETEl[cj]->Sumw2();
      hdPhiMETTau[cj] = new TH1D("dPhiMETTau_"+nCut,"dPhiMETTau "+cutName,64,0.0,3.2);
      hdPhiMETTau[cj]->Sumw2();
    
      hdPhiJMET[cj] = new TH1D("dPhiJMET_"+nCut,"dPhiJMET "+cutName,64,0.0,3.2);
      hdPhiJMET[cj]->Sumw2();
      hdPhiJ0MET[cj] = new TH1D("dPhiJ0MET_"+nCut,"dPhiJ0MET "+cutName,64,0.0,3.2);
      hdPhiJ0MET[cj]->Sumw2();
      hdPhiJ1MET[cj] = new TH1D("dPhiJ1MET_"+nCut,"dPhiJ1MET "+cutName,64,0.0,3.2);
      hdPhiJ1MET[cj]->Sumw2();
      hdPhiJ2MET[cj] = new TH1D("dPhiJ2MET_"+nCut,"dPhiJ2MET "+cutName,64,0.0,3.2);
      hdPhiJ2MET[cj]->Sumw2();
      hdPhiJ3MET[cj] = new TH1D("dPhiJ3MET_"+nCut,"dPhiJ3MET "+cutName,64,0.0,3.2);
      hdPhiJ3MET[cj]->Sumw2();

      hdPhiJ0Tau[cj] = new TH1D("dPhiJ0Tau_"+nCut,"dPhiJ0Tau "+cutName,64,0.0,3.2);
      hdPhiJ0Tau[cj]->Sumw2();
      hdPhiMuMET[cj] = new TH1D("dPhiMuMET_"+nCut,"dPhiMuMET "+cutName,64,0.0,3.2);
      hdPhiMuMET[cj]->Sumw2();

      hdPhiElMET[cj] = new TH1D("dPhiElMET_"+nCut,"dPhiElMET "+cutName,64,0.0,3.2);
      hdPhiElMET[cj]->Sumw2();
   
      hdPhiTauMET[cj] = new TH1D("dPhiTauMET_"+nCut,"dPhiTauMET "+cutName,64,0.0,3.2);
      hdPhiTauMET[cj]->Sumw2();
   
      //MT
      //
      //
      hMTsum[cj] = new TH1D ("MTsum_"+nCut,"MTsum "+cutName,30,0,300);
      hMTsum[cj]->Sumw2();
      hMTtot[cj] = new TH1D ("MTtot_"+nCut,"MTtot "+cutName,30,0,300);
      hMTtot[cj]->Sumw2();
      hMT[cj] = new TH1D ("MT_"+nCut,"MT "+cutName,30,0,300);
      hMT[cj]->Sumw2();
      hMTel[cj] = new TH1D ("MTel_"+nCut,"MTel "+cutName,30,0,300);
      hMTel[cj]->Sumw2();
      hMTmu[cj] = new TH1D ("MTmu_"+nCut,"MTmu "+cutName,30,0,300);
      hMTmu[cj]->Sumw2();

      hMTtau[cj] = new TH1D ("MTtau_"+nCut,"MTtau "+cutName,30,0,300);
      hMTtau[cj]->Sumw2();

      hMTmutau[cj] = new TH1D ("MTmutau_"+nCut,"MTmutau "+cutName,30,0,300);
      hMTmutau[cj]->Sumw2();
     
      hMTmuel[cj] = new TH1D ("MTmuel_"+nCut,"MTmuel "+cutName,30,0,300);
      hMTmuel[cj]->Sumw2();
       
      hMTeltau[cj] = new TH1D ("MTeltau_"+nCut,"MTeltau "+cutName,30,0,300);
      hMTeltau[cj]->Sumw2();
      
      hMTtautau[cj] = new TH1D ("MTtautau_"+nCut,"MTtautau "+cutName,50,0,1000);
      hMTtautau[cj]->Sumw2();

      hMt2mutau[cj] = new TH1D ("Mt2mutau_"+nCut,"Mt2mutau "+cutName,50,0,1000);
      hMt2mutau[cj]->Sumw2();
      
      hMt2lestermutau[cj] = new TH1D ("Mt2lestermutau_"+nCut,"Mt2lestermutau "+cutName,30,0,600);
      hMt2lestermutau[cj]->Sumw2();

      hMCTmutau[cj] = new TH1D ("MCTmutau_"+nCut,"MCTmutau "+cutName,50,0,500);
      hMCTmutau[cj]->Sumw2();
      hMCTxmutau[cj] = new TH1D ("MCTxmutau_"+nCut,"MCTxmutau "+cutName,50,0,500);
      hMCTxmutau[cj]->Sumw2();
      hMCTymutau[cj] = new TH1D ("MCTymutau_"+nCut,"MCTymutau "+cutName,50,0,500);
      hMCTymutau[cj]->Sumw2();
      
      hMCTbmutau[cj] = new TH1D ("MCTbmutau_"+nCut,"MCTbmutau "+cutName,50,0,500);
      hMCTbmutau[cj]->Sumw2();

      hMt2muel[cj] = new TH1D ("Mt2muel_"+nCut,"Mt2muel "+cutName,50,0,1000);
      hMt2muel[cj]->Sumw2();
      
      hMt2lestermuel[cj] = new TH1D ("Mt2lestermuel_"+nCut,"Mt2lestermuel "+cutName,30,0,600);
      hMt2lestermuel[cj]->Sumw2();

      hMCTmuel[cj] = new TH1D ("MCTmuel_"+nCut,"MCTmuel "+cutName,50,0,500);
      hMCTmuel[cj]->Sumw2();
      hMCTxmuel[cj] = new TH1D ("MCTxmuel_"+nCut,"MCTxmuel "+cutName,50,0,500);
      hMCTxmuel[cj]->Sumw2();
      hMCTymuel[cj] = new TH1D ("MCTymuel_"+nCut,"MCTymuel "+cutName,50,0,500);
      hMCTymuel[cj]->Sumw2();
      
      hMCTbmuel[cj] = new TH1D ("MCTbmuel_"+nCut,"MCTbmuel "+cutName,50,0,500);
      hMCTbmuel[cj]->Sumw2();



      hTBoundmutau[cj] = new TH1D ("TBoundmutau_"+nCut,"TBoundmutau "+cutName,100,0,2000);
      hTBoundmutau[cj] ->Sumw2();
      hMt2eltau[cj] = new TH1D ("Mt2eltau_"+nCut,"Mt2eltau "+cutName,50,0,500);
      hMt2eltau[cj]->Sumw2();
      hMt2lestereltau[cj] = new TH1D ("Mt2lestereltau_"+nCut,"Mt2lestereltau "+cutName,30,0,600);
      hMt2lestereltau[cj]->Sumw2();
 
      hMCTeltau[cj] = new TH1D ("MCTeltau_"+nCut,"MCTeltau "+cutName,50,0,500);
      hMCTeltau[cj]->Sumw2();
      hMCTxeltau[cj] = new TH1D ("MCTxeltau_"+nCut,"MCTxeltau "+cutName,50,0,500);
      hMCTxeltau[cj]->Sumw2();
      hMCTyeltau[cj] = new TH1D ("MCTyeltau_"+nCut,"MCTyeltau "+cutName,50,0,500);
      hMCTyeltau[cj]->Sumw2();
      
      hMCTbeltau[cj] = new TH1D ("MCTbeltau_"+nCut,"MCTbeltau "+cutName,50,0,500);
      hMCTbeltau[cj]->Sumw2();

 
      hdR_eltau[cj]= new TH1D ("dR_eltau_"+nCut,"dR_eltau "+cutName,60,0,6);;
      hdR_eltau[cj]->Sumw2();
        
      hdR_mutau[cj]= new TH1D ("dR_mutau_"+nCut,"dR_mutau "+cutName,60,0,6);;
      hdR_mutau[cj]->Sumw2();

      hdR_taujet[cj]= new TH1D ("dR_taujet_"+nCut,"dR_taujet "+cutName,60,0,6);;
      hdR_taujet[cj]->Sumw2();

      hdR_tautau[cj]= new TH1D ("dR_tautau_"+nCut,"dR_tautau "+cutName,60,0,6);;
      hdR_tautau[cj]->Sumw2();
	
      hdR_muel[cj]= new TH1D ("dR_muel_"+nCut,"dR_muel "+cutName,60,0,6);;
      hdR_muel[cj]->Sumw2();

      hnpv[cj]= new TH1D ("npv_"+nCut,"npv "+cutName,100,-0.5,99.5);;
      hnpv[cj]->Sumw2();
      hnpu[cj]= new TH1D ("npu_"+nCut,"npu "+cutName,100,-0.5,99.5);;
      hnpu[cj]->Sumw2();
      hnrho[cj]= new TH1D ("nrho_"+nCut,"nrho "+cutName,100,-0.5,99.5);;
      hnrho[cj]->Sumw2();
 


   int nBinsmet = 3;
   double binsmet[4] = {0, 50,100,1000};

   int nBinsMTsum = 3;
   double binsMTsum[4] = {0, 150,300,1000};

   int nBinsMCTb = 3;
   double binsMCTb[4] = {0, 50,100,1000};

   int nBinsMT = 3;
   double binsMT[4] = {0, 50,100,1000};

   int nBinsDZeta = 4;
   double binsDZeta[5] = {-500, -150,-100,100,1000};


      //hmet_MT[cj] = new TH2D ("met_MT_"+nCut,"met_MT "+cutName,40.0,0.0,800.0,10,0,200);
      hmet_MT[cj] = new TH2D ("met_MT_"+nCut,"met_MT "+cutName, nBinsmet, binsmet ,nBinsMT,binsMT);
      hmet_MT[cj]->Sumw2();

      hmet_MTsum[cj] = new TH2D ("met_MTsum_"+nCut,"met_MTsum "+cutName, nBinsmet, binsmet , nBinsMTsum,binsMTsum);
      hmet_MTsum[cj]->Sumw2();

      hmet_MTtot[cj] = new TH2D ("met_MTtot_"+nCut,"met_MTtot "+cutName, nBinsmet, binsmet , nBinsMTsum,binsMTsum);
      hmet_MTtot[cj]->Sumw2();
      
      hmet_DZeta[cj] = new TH2D ("met_DZeta_"+nCut,"met_DZeta "+cutName, nBinsmet, binsmet , nBinsDZeta,binsDZeta);
      hmet_DZeta[cj] -> Sumw2();
     
      hmet_MCTb[cj] = new TH2D ("met_MCTb_"+nCut,"met_MCTb "+cutName, nBinsmet, binsmet , nBinsMCTb,binsMCTb);
      hmet_MCTb[cj] ->Sumw2();
      
       hMT2lester_MCTb[cj] = new TH2D ("MT2lester_MCTb_"+nCut,"MT2lester_MCTb "+cutName, nBinsmet, binsmet , nBinsMCTb,binsMCTb);
       hMT2lester_MCTb[cj]->Sumw2();

       hMT2lester_met[cj] = new TH2D ("MT2lester_met_"+nCut,"MT2lester_met "+cutName, nBinsmet, binsmet , nBinsmet, binsmet);
       hMT2lester_met[cj]->Sumw2();

       hMT2lester_DZeta[cj] = new TH2D ("MT2lester_DZeta_"+nCut,"MT2lester_DZeta "+cutName, nBinsmet, binsmet , nBinsDZeta,binsDZeta);
	hMT2lester_DZeta[cj]->Sumw2();
     
      hDZeta_MCTb[cj] = new TH2D ("DZeta_MCTb_"+nCut,"DZeta_MCTb "+cutName, nBinsDZeta, binsDZeta , nBinsMCTb,binsMCTb);
      hDZeta_MCTb[cj] ->Sumw2();
  
      hDZeta_MTsum[cj] = new TH2D ("DZeta_MTsum_"+nCut,"DZeta_MTsum "+cutName, nBinsDZeta, binsDZeta , nBinsMTsum,binsMTsum);
      hDZeta_MTsum[cj] ->Sumw2();
      hDZeta_MTtot[cj] = new TH2D ("DZeta_MTtot_"+nCut,"DZeta_MTtot "+cutName, nBinsDZeta, binsDZeta , nBinsMTsum,binsMTsum);
      hDZeta_MTtot[cj] ->Sumw2();

      hMTsum_MCTb[cj] = new TH2D ("MTsum_MCTb_"+nCut,"MTsum_MCTb "+cutName, nBinsMTsum, binsMTsum , nBinsMCTb,binsMCTb);
      hMTsum_MCTb[cj] ->Sumw2();
      hMTtot_MCTb[cj] = new TH2D ("MTtot_MCTb_"+nCut,"MTtot_MCTb "+cutName, nBinsMTsum, binsMTsum , nBinsMCTb,binsMCTb);
      hMTtot_MCTb[cj] ->Sumw2();
      
      hdRmt_dPhi[cj] = new TH2D ("dRm_dPhi_"+nCut,"dRm_dPhi "+cutName,60,0.0,6.0,64,0.0,3.2);
      hdRmt_dPhi[cj]->Sumw2();
      hdRet_dPhi[cj] = new TH2D ("dRe_dPhi_"+nCut,"dRe_dPhi "+cutName,60.0,0.0,6.0,64,0.0,3.2);
      hdRet_dPhi[cj]->Sumw2();
      hdRme_dPhi[cj] = new TH2D ("dRme_dPhi_"+nCut,"dRme_dPhi "+cutName,60.0,0.0,6.0,64,0.0,3.2);
      hdRme_dPhi[cj]->Sumw2();
      hdRtt_dPhi[cj] = new TH2D ("dRtt_dPhi_"+nCut,"dRtt_dPhi "+cutName,60.0,0.0,6.0,64,0.0,3.2);
      hdRtt_dPhi[cj]->Sumw2();
 
      hmet_dPhi[cj] = new TH2D ("met_dPhi_"+nCut,"met_dPhi "+cutName,40.0,0.0,800.0,64,0.0,3.2);
      hmet_dPhi[cj]->Sumw2();

      hmet_dPhiel[cj] = new TH2D ("met_dPhiel_"+nCut,"met_dPhiel "+cutName,40.0,0.0,800.0,64,0.0,3.2);
      hmet_dPhiel[cj]->Sumw2();
      hmet_MTel[cj] = new TH2D ("met_MTel_"+nCut,"met_MTel "+cutName,40.0,0.0,800.0,10,0,200);
      hmet_MTel[cj]->Sumw2();
 

      hmet_dPhimu[cj] = new TH2D ("met_dPhimu_"+nCut,"met_dPhimu "+cutName,40.0,0.0,800.0,64,0.0,3.2);
      hmet_dPhimu[cj]->Sumw2();
      hmet_MTmu[cj] = new TH2D ("met_MTmu_"+nCut,"met_MTmu "+cutName,40.0,0.0,800.0,10,0,200);
      hmet_MTmu[cj]->Sumw2();

      hmet_dPhitau[cj] = new TH2D ("met_dPhitau_"+nCut,"met_dPhitau "+cutName,40.0,0.0,800.0,64,0.0,3.2);
      hmet_dPhitau[cj]->Sumw2();
      hmet_MTtau[cj] = new TH2D ("met_MTtau_"+nCut,"met_MTtau "+cutName,40.0,0.0,800.0,10,0,200);
      hmet_MTtau[cj]->Sumw2();


 
      hMT_dPhi[cj]= new TH2D ("MT_dPhi_"+nCut,"MT_dPhi "+cutName,10,0,200,64,0.0,3.2);
      hMT_dPhi[cj]->Sumw2();
      hMT_dPhiel[cj]= new TH2D ("MTel_dPhi_"+nCut,"MTel_dPhi "+cutName,10,0,200,64,0.0,3.2);
      hMT_dPhiel[cj]->Sumw2();
      hMT_dPhimu[cj]= new TH2D ("MTmu_dPhi_"+nCut,"MTmu_dPhi "+cutName,10,0,200,64,0.0,3.2);
      hMT_dPhimu[cj]->Sumw2();
      hMT_dPhitau[cj]= new TH2D ("MTtau_dPhi_"+nCut,"MTtau_dPhi "+cutName,10,0,200,64,0.0,3.2);
      hMT_dPhitau[cj]->Sumw2();
      hIso_sign[cj]= new TH2D ("Iso_sign_"+nCut,"Iso_sign "+cutName,50,0,5,2,0,2);
      hIso_sign[cj]->GetYaxis()->SetBinLabel(1,"SS");
      hIso_sign[cj]->GetYaxis()->SetBinLabel(2,"OS");
      hIso_sign[cj]->Sumw2();

    }

  /*
      char arg[100];
     for (unsigned int i = 0; i < vec.size (); i++)
    {
      var_[i] = -8888.;
      //string name = vec[i].c_str()+"_"+ds;
      sprintf (arg, "%s/F", vec[i].c_str ());
      T->Branch (vec[i].c_str (), &var_[i], arg);

      //cout << " creating the TTree. " << vec[i].c_str()<< "  "<<i<<endl;
    }
*/
}


//void FillHists(int CutIndex, Double_t EvWeight, TLorentzVector  &ElV, TLorentzVector  &MuV, TLorentzVector  &TauV, vector<TLorentzVector>  &JetsV, TLorentzVector  &MetV, double Chimass, double mintermediate, string & Sel, int  mIndex, int eIndex, int  tIndex){}

double Centrality (vector < TLorentzVector > AllJets_Lepton_noMet_){
    // Centrality
    double Centrality = 0, Centrality_N = 0, Centrality_D = 0;
    for (unsigned int i = 0; i < AllJets_Lepton_noMet_.size (); i++)
    {
    Centrality_N += AllJets_Lepton_noMet_[i].Pt ();
    Centrality_D += AllJets_Lepton_noMet_[i].P ();
    }
    double Centr_=-1;

    Centrality_D > 0 ? Centr_ = Centrality_N / Centrality_D : Centr_=-1;

    return Centr_ ;
  }


void WriteTree() 

  {
	  T->Print();
	  //T->Write();
	  T->AutoSave();
  }


void FillTree() {

	  T->Fill();
}
