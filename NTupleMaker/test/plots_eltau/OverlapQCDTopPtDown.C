#include <cmath>
#include <sstream>
#include <iomanip>
#include "TChain.h"
#include "TH1.h"
#include "TH1D.h"
#include "THStack.h"
#include "TTree.h"
#include "TKey.h"
#include "Riostream.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TH2.h"
#include "TH3.h"
#include "TLegend.h"
#include "TROOT.h"
#include "TFrame.h"
#include "TGaxis.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include "TList.h"
#include <string>
#include "TObject.h"
#include "TBranch.h"
#include <functional>
#include "TAxis.h"
#include "TChain.h"
#include "TMath.h"
#include "Riostream.h"
#include <iostream>
#include <fstream>


using namespace std;

void Impose( TDirectory *ttarget, TList *ssourcelist, string &np_legend , vector<string> titles_ ,vector<float> xsecs);
void ModifyHist (TH1D* &h, int cl_,float &w, float &lumi, string &title,bool norm=false);
void CheckHistZero (TH1D* &h1);			

TH1D* WInclw,*W1Jw,*W2Jw,*W3Jw,*W4Jw;
TH1D* DYInclw,*DY1Jw,*DY2Jw,*DY3Jw,*DY4Jw;

void OverlapQCDTopPtDown()
{

	gROOT->SetStyle ("Plain");
	gStyle->SetPalette (1);
	gStyle->SetTextFont(22) ;
	gStyle->SetTitleFont(22,"xyz") ;
	gStyle->SetLabelFont(22,"xyz") ;



	vector <string> titles;
	// void MergeRootfile( TDirectory *target, TList *sourcelist );
	//TTrees
	TList *FileList;
	TFile *Target;
	titles.clear();
	int np=1;

	Float_t value=0;
	vector<float> xsecs_;
	ifstream ifs("QCDsamplesTopPtDown");
	string line;
	while(std::getline(ifs, line)) // read one line from ifs
	{
		istringstream iss(line); // access line as a stream
		string dataname;
		float XSec;	
		float xs,fact,fact2,fact3;
		xs=0;fact=1;fact2=1;fact3=1;
		iss >> dataname >> xs >> fact >> fact2 >> fact3;
		//titles.push_back(dataname+".root");
		titles.push_back(dataname+".root");
		XSec= xs*fact*fact2*fact3;
		cout<<" Found the correct cross section "<<xs<<" for Dataset "<<dataname<<" XSec "<<XSec<<"  "<<fact<<"  "<<fact2<<" "<<fact3<<endl;
		xsecs_.push_back(XSec);
	}

	string fout = "QCD_TopPtDown_B.root";


	FileList = new TList ();


	for (unsigned int i=0; i <titles.size();++i){
		//string ext=".root";
		cout<<" loading dataset "<<titles[i]<<endl;
		//string file=titles[i]+".root";
		string file=titles[i];
		FileList->Add (TFile::Open (file.c_str()));
	}


	//return;
	Target = TFile::Open (fout.c_str (), "RECREATE");

	string np_title = titles[0];



				TString WInclname, W1Jname, W2Jname, W3Jname, W4Jname;
				TString DYInclname, DY1Jname, DY2Jname, DY3Jname, DY4Jname;
				TString app_ = "";
				TString s_ = "TopPtDown";
				if (s_!="Nominal") app_="_"+s_;
								
					WInclname = "WJetsToLNu_TuneCUETP8M1_13TeV-madgraphMLM-pythia8"+app_+"_B.root";
					W1Jname = "W1JetsToLNu_TuneCUETP8M1_13TeV-madgraphMLM-pythia8"+app_+"_B.root";
					W2Jname = "W2JetsToLNu_TuneCUETP8M1_13TeV-madgraphMLM-pythia8"+app_+"_B.root";
					W3Jname = "W3JetsToLNu_TuneCUETP8M1_13TeV-madgraphMLM-pythia8"+app_+"_B.root";
					W4Jname = "W4JetsToLNu_TuneCUETP8M1_13TeV-madgraphMLM-pythia8"+app_+"_B.root";

					DYInclname = "DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8"+app_+"_B.root";
					DY1Jname = "DY1JetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8"+app_+"_B.root";
					DY2Jname = "DY2JetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8"+app_+"_B.root";
					DY3Jname = "DY3JetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8"+app_+"_B.root";
					DY4Jname = "DY4JetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8"+app_+"_B.root";

				TFile *WIncl = TFile::Open (WInclname, "read");
				TFile *W1J = TFile::Open (W1Jname, "read");
				TFile *W2J = TFile::Open (W2Jname, "read");
				TFile *W3J = TFile::Open (W3Jname, "read");
				TFile *W4J = TFile::Open (W4Jname, "read");

				TFile *DYIncl = TFile::Open (DYInclname, "read");
				TFile *DY1J = TFile::Open (DY1Jname, "read");
				TFile *DY2J = TFile::Open (DY2Jname, "read");
				TFile *DY3J = TFile::Open (DY3Jname, "read");
				TFile *DY4J = TFile::Open (DY4Jname, "read");

			TString Channel="mutau";
			DYInclw= (TH1D*)DYIncl->Get(Channel+"/histWeightsH");
			DY1Jw= (TH1D*)DY1J->Get(Channel+"/histWeightsH");
			DY2Jw= (TH1D*)DY2J->Get(Channel+"/histWeightsH");
			DY3Jw= (TH1D*)DY3J->Get(Channel+"/histWeightsH");
			DY4Jw= (TH1D*)DY4J->Get(Channel+"/histWeightsH");


//			cout<<" ==================================== now the weights DY..... "<<DYInclw->GetSumOfWeights()<<"  "<<DY1Jw->GetSumOfWeights()<<"   "<<DY2Jw->GetSumOfWeights()<<"  "<<DY3Jw->GetSumOfWeights()<<"  "<<DY4Jw->GetSumOfWeights()<<endl;


			WInclw= (TH1D*)WIncl->Get(Channel+"/histWeightsH");
			W1Jw= (TH1D*)W1J->Get(Channel+"/histWeightsH");
			W2Jw= (TH1D*)W2J->Get(Channel+"/histWeightsH");
			W3Jw= (TH1D*)W3J->Get(Channel+"/histWeightsH");
			W4Jw= (TH1D*)W4J->Get(Channel+"/histWeightsH");









	Impose (Target, FileList, np_title,titles,xsecs_);

	delete FileList;
	delete Target;
}


	void
Impose (TDirectory * target, TList * sourcelist, string & np_title_, vector<string> titles,vector<float> xsecs)
{
	cout << "	" << "========================================================" << endl;
	cout << "	" << "This is a macro to superimpose plots of different root files." << endl;
	cout << "	" << "Only TH1Dobjects are superimposed." << endl;
	cout << "	" << "Target path: " << target->GetPath () << endl;
	TString path ((char *) strstr (target->GetPath (), ":"));
	path.Remove (0, 2);

				TFile *foutA = TFile::Open ("QCD_TopPtDown_A.root", "RECREATE");
				TFile *foutC = TFile::Open ("QCD_TopPtDown_C.root", "RECREATE");
				TFile *foutD = TFile::Open ("QCD_TopPtDown_D.root", "RECREATE");
	
	foutA->mkdir("mutau");
	foutC->mkdir("mutau");
	foutD->mkdir("mutau");
	target->mkdir("mutau");

	float Lumi = 1.;
	Lumi=36590.;
	Lumi=35865.;
	bool norm_=false;
	cout<<titles[0]<<"   "<<titles.size()<<endl;

	//not really useful if plots already weighted to lumi - usefull is plots are in a.u.
	vector <float > lumiweights;
	lumiweights.clear();
	vector <string> label_;
	vector <float> yieldA;
	vector <float> yieldC;
	vector <float> yieldD;
	yieldA.clear();
	yieldC.clear();
	yieldD.clear();

	vector <float> yieldAp;
	vector <float> yieldCp;
	vector <float> yieldDp;
	yieldAp.clear();
	yieldCp.clear();
	yieldDp.clear();

	TFile *first_source = (TFile *) sourcelist->First ();
	first_source->cd ("mutau");
	float norm=1.;
	lumiweights.push_back(float(norm));



	TDirectory *current_sourcedir = gDirectory;
	//gain time, do not add the objects in the list in memory
	Bool_t status = TH1::AddDirectoryStatus ();
	TH1::AddDirectory (kFALSE);
	// loop over all keys in this directory
	TChain *globChain = 0;
	TIter nextkey (current_sourcedir->GetListOfKeys ());
	//TIter nextkey (((TDirectory *) current_sourcedir->Get ("ana"))->GetListOfKeys ());
	TKey *key, *oldkey = 0;
	TCanvas *c1 ;
		c1  = new TCanvas ("c1","c1",0,22,600,600);
	int count=0;
			TH1D* hh[1000];
			TH1D* allRegA;  
			TH1D* allbkgA;  
			TH1D* allbkgC;  
			TH1D* allbkgD;  
			TH1D *allRegC; 
			TH1D *allRegD;
  			TH1D *hregA;
  			TH1D *hregC;
  			TH1D *hregD;
	while ((key = (TKey *) nextkey ())) {
		count++;

		// read object from first source file and create a canvas
		first_source->cd ("mutau");
		TObject *obj = key->ReadObj ();
		string nn = obj->GetName();

		//if ( string::npos == nn.find("nJet")  && (string::npos == nn.find("CutFlow"))) continue;
 		//if (string::npos == nn.find("DZeta1D_17") && string::npos == nn.find("CutFlow")) continue;
 		//if (string::npos == nn.find("IsoMu_12")) continue;

		if (!obj->IsA ()->InheritsFrom ("TH1") ) continue;
		if (obj->IsA ()->InheritsFrom ("TH2") ) continue;
		if (obj->IsA ()->InheritsFrom ("TH3") ) continue;
		if (obj->IsA ()->InheritsFrom ("TTree") ) continue;



		if (obj->IsA ()->InheritsFrom ("TH1") ) {
		c1->SetTitle(obj->GetName());
			// descendant of TH1D-> prepare the histograms to be superimposed

			TH1D* h1 = (TH1D*) obj;

			if (h1->GetSumOfWeights()<0.001) continue;

			ModifyHist (h1,1,Lumi,lumiweights[0],titles[0],norm_);

			TFile *nextsource = (TFile *) sourcelist->After (first_source);

			Int_t cl=0;
			cl=1;
			h1->SetStats(000000);
			h1->SetLineWidth(5);

			hh[cl]=h1;

			if ((int)cl==1){	
				allRegA  = (TH1D*) hh[1]->Clone();
				hregA  = (TH1D*) hh[1]->Clone();
				allbkgA  = (TH1D*) hh[1]->Clone();
				allbkgA->Reset();
			}

			string regA, regB,regC,regD;

			string sn="stau";string sdata="Single";
			string qcd="QCD_DataDriven";
			regA="_A ";
			regC="_C ";
			regD="_D ";
			while (nextsource) {

				string fname= nextsource->GetName();

				bool flagg= false;

				if (std::string::npos != fname.find(qcd) || std::string::npos != fname.find(sdata)    ) 	flagg=true;

				nextsource->cd("mutau");
				TH1D* eventCountt ;
				  if  ( std::string::npos == fname.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8")) eventCountt = (TH1D*)nextsource->Get("mutau/histWeightsH");
				  if ( std::string::npos != fname.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8") ) eventCountt = (TH1D*)nextsource->Get("mutau/histTopPt");

				float normm =1.;
				TH1D* hxsecc = (TH1D*)nextsource->Get("mutau/xsec");
				float xsecc = xsecs[cl];
				//float xsecc = hxsecc->GetMean();


				float nGenn = eventCountt->GetSumOfWeights();

				normm = float(xsecc*Lumi) / float(nGenn);

				if (flagg) { xsecc=1;normm =1.;}


				  
					if (std::string::npos != fname.find("WJetsToLNu_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_1Parton") || std::string::npos != fname.find("W1JetsToLNu_TuneCUETP8M1_13TeV-madgraphMLM-pythia8"))
						normm = Lumi/ ( WInclw->GetSumOfWeights()/float(61526.7*0.8853) + W1Jw->GetSumOfWeights()/float(1.221*9644.5*0.8853));


					if (std::string::npos != fname.find("WJetsToLNu_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_2Parton") || std::string::npos != fname.find("W2JetsToLNu_TuneCUETP8M1_13TeV-madgraphMLM-pythia8"))
						normm = Lumi/ ( WInclw->GetSumOfWeights()/float(61526.7*0.8853) + W2Jw->GetSumOfWeights()/float(1.221*3144.5*0.8853));
				
					if (std::string::npos != fname.find("WJetsToLNu_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_3Parton") || std::string::npos != fname.find("W3JetsToLNu_TuneCUETP8M1_13TeV-madgraphMLM-pythia8"))
						normm = Lumi/ ( WInclw->GetSumOfWeights()/float(61526.7*0.8853) + W3Jw->GetSumOfWeights()/float(1.221*954.8*0.8853));

					if (std::string::npos != fname.find("WJetsToLNu_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_4Parton") || std::string::npos != fname.find("W4JetsToLNu_TuneCUETP8M1_13TeV-madgraphMLM-pythia8"))
						normm = Lumi/ ( WInclw->GetSumOfWeights()/float(61526.7*0.8853) + W4Jw->GetSumOfWeights()/float(1.221*485.6*0.8853));



					if (std::string::npos != fname.find("DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_1Parton") || std::string::npos != fname.find("DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_isZTT_1Parton") || std::string::npos != fname.find("DY1JetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8"))
						normm = Lumi/ ( float(DYInclw->GetSumOfWeights())/float(5765.4*0.9611) + float(DY1Jw->GetSumOfWeights())/float(1.1637*1012.5*0.9611));


					if (std::string::npos != fname.find("DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_2Parton") || std::string::npos != fname.find("DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_isZTT_2Parton") || std::string::npos != fname.find("DY2JetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8"))
						normm = Lumi/ ( DYInclw->GetSumOfWeights()/float(5765.4*0.9611) + float(DY2Jw->GetSumOfWeights())/float(1.1637*332.8*0.9611));

					if (std::string::npos != fname.find("DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_3Parton") || std::string::npos != fname.find("DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_isZTT_3Parton") || std::string::npos != fname.find("DY3JetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8"))
						normm = Lumi/ ( DYInclw->GetSumOfWeights()/float(5765.4*0.9611) + float(DY3Jw->GetSumOfWeights())/float(1.1637*101.8*0.9611));

					if (std::string::npos != fname.find("DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_4Parton") || std::string::npos != fname.find("DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_isZTT_4Parton") || std::string::npos != fname.find("DY4JetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8"))
						normm = Lumi/ ( DYInclw->GetSumOfWeights()/float(5765.4*0.9611) + float(DY4Jw->GetSumOfWeights())/float(1.1637*54.8*0.9611));









				lumiweights.push_back(normm);

				TKey *key2 = (TKey *) gDirectory->GetListOfKeys ()->FindObject (h1->GetName ());

				if (key2) {
					cl++;

					TH1D *h2;

					h2 = (TH1D*) key2->ReadObj ();
					h2->SetLineWidth(4);
					ModifyHist (h2, cl,Lumi,lumiweights[cl-1],titles[cl-1],norm_);
					h2->SetStats(0);
					hh[(int)cl] = h2;

			//cout<<"  Will add "<<cl<<"  "<<fname<<"  "<<h1->GetName()<<"  "<<h1->GetTitle()<<"  "<<h2->GetName()<<"  "<<h2->GetTitle()<<endl;

					if ((int)cl==2 ){	
						allRegC  = (TH1D*) h2->Clone();
						allbkgC  = (TH1D*) h2->Clone();
						allbkgC->Reset();
					}
					if ((int)cl==3){	
						allRegD  = (TH1D*) h2->Clone();
						allbkgD  = (TH1D*) h2->Clone();
						allbkgD->Reset();
					}

					double int_ = allRegA->GetSumOfWeights();
					if ((int)cl>3 && h2->GetSumOfWeights()>0.)  {

						if (std::string::npos != fname.find("_A.root") ){ 
							allRegA->Add(h2,-1);
							allbkgA->Add(h2,1);
						}

						if (std::string::npos != fname.find("_C.root") ) {
							allRegC->Add(h2,-1);
							allbkgC->Add(h2,1);
							}

						if (std::string::npos != fname.find("_D.root") ) {
							allRegD->Add(h2,-1);
							allbkgD->Add(h2,1);
							}

					}

				}
				nextsource = (TFile *) sourcelist->After (nextsource);
				}				// while ( nextsource )
			}
		
			if (!obj->IsA ()->InheritsFrom ("TH1") ) continue;
			if (obj->IsA ()->InheritsFrom ("TH2") ) continue;
			if (obj->IsA ()->InheritsFrom ("TH3") ) continue;
			if (obj->IsA ()->InheritsFrom ("TTree") ) continue;
			if (obj->IsA ()->InheritsFrom ("TH1")) {

					TH1F* v1 = new TH1F("", "", 1, 0, 1);v1->SetLineWidth(2);
					TH1F* v2 = new TH1F("", "", 1, 0, 1);v2->SetLineWidth(2);
					TH1F* v3 = new TH1F("", "", 1, 0, 1);v3->SetLineWidth(2);
					TH1F* v4 = new TH1F("", "", 1, 0, 1);v4->SetLineWidth(2);

					hh[1]->SetLineColor(kGreen-2);
					hh[1]->SetLineWidth(2);
					hh[1]->SetLineStyle(2);
					hh[1]->SetMarkerStyle(34);
					hh[1]->SetMarkerColor(kGreen-2);
					hh[1]->SetMarkerSize(0.8);
					allRegA->SetLineColor(kRed);
					allRegA->SetLineWidth(2);
					allRegA->SetLineStyle(2);
					allRegA->SetMarkerStyle(34);
					allRegA->SetMarkerColor(kRed);
					allRegA->SetMarkerSize(0.8);


					hh[2]->SetLineColor(kBlue);
					hh[2]->SetMarkerStyle(27);
					hh[2]->SetMarkerColor(kBlue);
					allRegC->SetLineColor(kBlue);
					allRegC->SetMarkerStyle(27);
					allRegC->SetMarkerColor(kBlue);

					hh[3]->SetMarkerStyle(32);
					hh[3]->SetMarkerColor(kOrange-1);
					hh[3]->SetLineColor(kOrange-1);
					allRegD->SetMarkerStyle(32);
					allRegD->SetMarkerColor(kOrange-1);
					allRegD->SetLineColor(kOrange-1);


			

		//cout<< " cl "<<cl<<" A "<< allRegA->GetSumOfWeights() <<"  purity of A "<<float( (allRegA->GetSumOfWeights()-allbkgA->GetSumOfWeights())/allRegA->GetSumOfWeights())  <<" C "<< allRegC->GetSumOfWeights() <<" D "<< allRegD->GetSumOfWeights() <<"  ratio of B/A  "<<rBA<<" C/D "<<rCD<<" purity of C  "<<float( (allRegC->GetSumOfWeights()-allbkgC->GetSumOfWeights())/allRegC->GetSumOfWeights())<<"  purity of D "<<float( (allRegD->GetSumOfWeights()-allbkgD->GetSumOfWeights())/allRegD->GetSumOfWeights())<<endl;




				//legend_c1->SetNColumns(3);
				TPad *pad1 = new TPad("pad1","pad1",0,0.25,1,1);
				TPad *pad1r = new TPad("pad1","pad1",0.5,0.25,1,1);

				TPad *pad2 = new TPad("pad2","pad2",0,0,1,0.249);


				pad1->SetTopMargin(0.1);
				pad1->SetBottomMargin(0.1);
				pad1->SetRightMargin(0.05);
				//pad1->SetGridy();
				pad1r->SetBottomMargin(0.07);
				pad1r->SetRightMargin(0.05);
				pad1r->SetGridy();

				pad2->SetBottomMargin(0.05);


				pad2->SetTopMargin(0);
				pad2->SetBottomMargin(0.25);
				pad2->SetRightMargin(0.05);


				c1->cd();


				c1->Clear();

				pad1->SetLogy();
				pad1->Draw();
				pad2->Draw();

				pad2->SetGridy(1);

				pad1->cd();
				pad1->Clear();
				char namee[100];

				sprintf(namee,"%s",key->GetName ());
			//	CheckHistZero(allRegA);
			//	CheckHistZero(allRegC);
			//	CheckHistZero(allRegD);

			    if ( string::npos != nn.find("CutFlow")  ) {			
				allRegA->Sumw2();
				allRegC->Sumw2();
				allRegD->Sumw2();
				}

				double nd = allRegD->GetSumOfWeights();
				double nc = allRegC->GetSumOfWeights();
			if (string::npos != nn.find("METLowDzeta_17")) 
				cout<<"allRegA  "<<allRegA->GetSumOfWeights()<<" ratio is nd "<<nd<<" nc "<<nc<<" nc/nd "<<double(nc/nd)<<endl;

		if (nc>0 && nd >0 && allRegA->GetSumOfWeights()>0)	allRegA->Scale(double(nc/nd));
		if (nc<0 || nd<0 || allRegA->GetSumOfWeights()<0) allRegA->Scale(0.);

				//if (nd>0) cout<<"allRegA  "<<allRegA->GetSumOfWeights()<<" ratio is nd "<<nd<<" nc "<<nc<<" nc/nd "<<double(nc/nd)<<endl;//"  and different approach  "<<hregA->GetSumOfWeights()<<"  "<<double(Anc/And)<<endl;
				//hh[1]->SetMinimum(1);
				allRegA->SetMinimum(1);
				allRegA->SetMaximum(500*allRegA->GetMaximum());
				//for (int nb=1;nb<=allRegA->GetNbinsX();++nb)cout <<allRegA->GetXaxis()->GetBinLabel(nb)<<"  "<<allRegA->GetBinContent(nb)<<"  "<<allRegC->GetBinContent(nb)<<"  "<< allRegD->GetBinContent(nb)<<endl;
				allRegA->SetMarkerSize(1.5);
				allRegA->Draw("p hist");
				allRegC->Draw("p hist sames");
				allRegD->Draw("p hist sames");
				//allRegA->GetXaxis()->SetRange(allRegA->FindFirstBinAbove(0),allRegA->FindLastBinAbove(1));
				allRegA->GetXaxis()->SetRange(2,19);
				allRegC->GetXaxis()->SetRange(2,19);
				allRegD->GetXaxis()->SetRange(2,19);


				string nnn = obj->GetName();			
				if (std::string::npos != nnn.find("nJet"))  {
					yieldA.push_back(allRegA->GetSumOfWeights());
					yieldC.push_back(allRegC->GetSumOfWeights());
					yieldD.push_back(allRegD->GetSumOfWeights());
					yieldAp.push_back(allRegA->GetSumOfWeights()/(allRegA->GetSumOfWeights()+allbkgA->GetSumOfWeights()));
					yieldCp.push_back(allRegC->GetSumOfWeights()/(allRegC->GetSumOfWeights()+allbkgC->GetSumOfWeights()));
					yieldDp.push_back(allRegD->GetSumOfWeights()/(allRegD->GetSumOfWeights()+allbkgD->GetSumOfWeights()));
				}

				c1->SetFillColor(0);
				c1->SetBorderMode(0);
				c1->SetBorderSize(0);
				c1->SetFrameBorderMode(0);
				c1->SetBorderSize(0);
				pad1->SetFrameLineColor(0);;
				pad1r->SetFrameLineColor(0);;



				pad2->cd();
				TH1D * ratio = (TH1D*) allRegC->Clone();
				ratio->SetMarkerColor(kBlack);
				ratio->SetLineColor(kBlack);
				ratio->SetStats(0);
				ratio->Divide(allRegD);
				ratio->SetMarkerStyle(7);
				ratio->SetMaximum( 1.5 );
				ratio->SetMinimum( 0.5 );
				ratio->GetYaxis()->SetTitle("");
				ratio->SetTitleSize(0);
				ratio->SetMarkerSize(0.25);
				ratio->SetMarkerColor(39);
				ratio->SetLineWidth(1);
				ratio->GetYaxis()->SetNdivisions(5);

				ratio->GetXaxis()->SetNdivisions(545);
				ratio->GetXaxis()->SetLabelSize(0.1);
				ratio->GetXaxis()->SetTitleSize(0.12);
				ratio->GetXaxis()->SetTitleOffset(0.85);

				ratio->GetYaxis()->SetLabelSize(0.1);
				ratio->GetYaxis()->SetLabelSize(0.15);
				ratio->GetYaxis()->SetTitleSize(0.12);
				ratio->GetYaxis()->SetTitleOffset(0.35);
				ratio->GetYaxis()->SetTitle("N_{C}/N_{D}");


				ratio->SetTitleFont(62);

				ratio->GetXaxis()->SetNdivisions(510);
				ratio->SetTitle("");
				ratio->Draw("ep");
				TAxis *axis = allRegA->GetXaxis();
				int MaxEventsBin =-1;
				 TLine *l=new TLine(axis->GetBinLowEdge(hh[1]->FindFirstBinAbove(0.)),1,axis->GetBinLowEdge(hh[1]->FindLastBinAbove(MaxEventsBin)+1),1);

                        	l->SetLineColor(kRed);
                        	l->SetLineWidth(2);
				l->Draw("same");


				gPad->RedrawAxis();
				gPad->Modified();
				gPad->Update();
				c1->SetName(obj->GetName());
				c1->SetTitle(obj->GetName());
				c1->Update();
				c1->cd();
				c1->Modified();
				c1->cd();
				c1->Update();


			
				string nn = obj->GetName();

				target->cd ("mutau");
				allRegA->Write();
				
				foutA->cd("mutau");
				allRegA->Scale(nd/nc);
				allRegA->Write();
				
				foutC->cd("mutau");
				allRegC->Write();
				
				foutD->cd("mutau");
				allRegD->Write();




			}

		} 			// while ( ( TKey *key = (TKey*)nextkey() ) )









		// save modifications to target file

		// for (int i=0;i<SValueVariables_.size();i++){
		//cout<<SValueVariables_[i].second<<endl;}
		target->SaveSelf (kTRUE);
		target->Write();
		foutA->SaveSelf (kTRUE);
		foutA->Write();
		foutC->SaveSelf (kTRUE);
		foutC->Write();
		foutD->SaveSelf (kTRUE);
		foutD->Write();
		TH1::AddDirectory (status);
		cout << "	" << "========================================================" << endl;
		cout<< " Ended SuperImpose of files.... " <<endl;

	ofstream tfile;
	TFile * filee = new TFile("QCD_TopPtDown_B.root","update");
	filee->cd("mutau");
	//target->ls();
	TH1D * hcut = (TH1D*)filee->Get("mutau/CutFlowUnW");
	
	for (int nb=0;nb<yieldA.size();++nb)
		cout<<hcut->GetXaxis()->GetBinLabel(nb+2)<<"  "<<yieldA[nb]<<"  "<<yieldC[nb]<<"  "<<yieldD[nb]<<"  "<<yieldC[nb]/yieldD[nb]<<" pA "<<yieldAp[nb]<<" pC "<<yieldCp[nb]<<" pD "<<yieldDp[nb]<<endl;	

	cout<<" Comparing  "<<hcut->GetNbinsX()<<"  "<<yieldA.size()<<endl;
	//if (std::string::npos != title1.find("Cut Flow") ){
	TString textfilename = "cutflow_QCD.txt";
	tfile.open(textfilename);

	tfile << "########################################" << endl;
	tfile << " No Cut  & 0" << endl;
//	tfile << " No Cut after PU & 0" << endl;
	for (int b=0;b<yieldA.size();b++){
		//for (int b=0;b<19;b++){
		//tfile <<h->GetXaxis()->GetBinLabel(b)<<" & "<<h->GetBinContent(b)<<" & "<<" & "<<endl;	
		//if (h->GetBinContent(b) > 0 ) 
		tfile <<hcut->GetXaxis()->GetBinLabel(b+2)<<" & "<<yieldA[b]<<endl;	
		//else tfile <<h->GetXaxis()->GetBinLabel(b)<<" & "<<0<<endl;	
		//cout <<title1<<" & "<<h->GetXaxis()->GetBinLabel(b)<<" & "<<h->GetBinContent(b)<<" & "<<" & "<<endl;	
	}
	//}
	tfile.close();

	for (int b=0;b<yieldA.size();b++){

		cout<<" was  "<<hcut->GetXaxis()->GetBinLabel(b+2)<<" content "<<hcut->GetBinContent(b+2)<<" will be "<<yieldA[b]<<endl;
		hcut->SetBinContent(b+2,yieldA[b]);
	}
	
	filee->cd("mutau");
	hcut->Write();


	}

	void
		ModifyHist (TH1D* &h, int cl_ ,float & lumi,float & weight,string & title_, bool norm_=false)
		{



			h->SetMinimum(1);
			h->GetXaxis()->SetNdivisions(512);
			string titlee=h->GetName();
			int col=1;//kOrange;
			string title1,title2;
			title1= h->GetTitle();



			int nb = h->GetNbinsX();
			float over_ = h->GetBinContent(nb+1);
			float contlast = h->GetBinContent(nb);
			h->SetBinContent(nb,contlast+over_);

			h->Scale(weight);


}

void
CheckHistZero (TH1D* &h)
{


                        for (int nb=0;nb<=h->GetNbinsX();++nb)
                        {
                                float bc_ = h->GetBinContent(nb);

                        if (bc_ <=0.) h->SetBinContent(nb,0.0001);
                        float SoW = h->GetSumOfWeights();
                        int En = h->GetEntries();
                        if (bc_ <=0.) h->SetBinError(nb,float(SoW/En));
                        }


}




