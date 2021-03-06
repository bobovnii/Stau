#include <iostream>
#include <vector>
#include <map>
#include "boost/lexical_cast.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/format.hpp"
#include "boost/program_options.hpp"
#include "boost/range/algorithm.hpp"
#include "boost/range/algorithm_ext.hpp"
#include "Plotting.h"
#include "Plotting_Style.h"
#include "HttStylesNew.cc"
#include "TPad.h"
#include "TROOT.h"
#include "TColor.h"
#include "TEfficiency.h"
#include "TMath.h"
#include <iomanip>
#include <iostream> 
//WithoutNPV/
void PlotsForUnrolledDistrAllChannelsStauPostFit(TString directory = "",
	  TString suffix = "_SR_CR1_MuTau_all_wBinUncrt_PostFit",
	  TString File = "Templ_met_MT2lester_DZeta01J1D_18_35invfb_mt_stau-stau_left_SR_CR1.root",
	  TString variable = "",
	  TString Suffix = "",
	  TString xtitle = "",
	  TString ytitle = "Events",
      bool logY = true,
 	    bool BlindData = false,
	    bool OverlayFit = true

          )


{
	  vector<TString> vars_;vars_.clear();
	  vars_.push_back("met_MT2lester_DZeta01J1D_18");
  //ModTDRStyle();


int mycolor=TColor::GetColor("#ffcc66");
int mycolorvv=TColor::GetColor("#8646ba");
int mycolorqcd=TColor::GetColor("#ffccff");
int mycolortt=TColor::GetColor("#9999cc");
int mycolorttx=TColor::GetColor("#33CCFF");
int mycolorwjet=TColor::GetColor("#de5a6a");
int mycolordyj=TColor::GetColor("#ffcc66");
int mycolorztt=TColor::GetColor("#58d885");
int mycolorst=TColor::GetColor("#b6d0ea");
int mycolorww=TColor::GetColor("#C390D4");




  bool plotLeg = true;
  int position = 2; // 0 - right, 1 - left, 2 - central
  int npos = 1;

  TH1::SetDefaultSumw2();
  TH2::SetDefaultSumw2();


  //  TString topweight("");

TString BinLabels[100] = {
"1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16","17","18","19","20","21","22","23","24","25","26","27","28","29","30","31",
"32","33","34","35","36","37","38","39","40","41","42","43","44","45","46","47","48","49","50","51","52","53","54","55","56","57","58","59","60",
"61","62","63","64","65","66","67","68","69","70","71","72","73","74","75","76","77","78","79","80","81","82","83","84","85","86","87","88","89","90","91","92","93"
};


	ifstream BinLabelsFile;
	BinLabelsFile.open("LabelListTESR");
	int i=0;
	string name;
   	 while(getline(BinLabelsFile, name))
		{
//		BinLabels[i]=name;
		i++;
		}


	directory="";
    TFile * filee, *fileStat;//
    TH1D * fitHisto, *fitHisto2, *fitHistoPostH0, *fitHistoPostH1, *errPreFit, *errPreFitB, *errNormOnly,*errNormOnlyB;
    TH1D *TT,*WJ,*DY,*ZTT,*ST,*VV,*WW,*TTX,*QCD;
    int nB=1;


    if (OverlayFit) 
	{
    //filee = new TFile("/nfs/dust/cms/user/alkaloge/Workdir/CMSSW_7_4_7/src/Limits/STau/Results_SR_CR1/mlfit_mt_only.root","read");
    //filee = new TFile("/nfs/dust/cms/user/alkaloge/Workdir/CMSSW_7_4_7/src/Limits/STau/Results_SR_CR1/mlfit_norm_shape_all_Stau.root","read");
    filee = new TFile("/nfs/dust/cms/user/alkaloge/Workdir/CMSSW_7_4_7/src/Limits/STau/Fit_SR_CR1_NoSignal/mlfit.root","read");
    //filee = new TFile("mlfit_all_noS.root","read");
    fitHisto = (TH1D*)filee->Get("shapes_fit_b/ch1/total_background");
    fitHisto2 = (TH1D*)filee->Get("shapes_fit_b/ch1/total_background");
    errPreFit = (TH1D*)filee->Get("shapes_prefit/ch1/total_background");

    nB = fitHisto->GetNbinsX();

    fitHistoPostH0 =  new TH1D ("","",nB,1,nB+1);
    fitHistoPostH1 =  new TH1D ("","",nB,1,nB+1);
    errPreFitB =  new TH1D ("","",nB,1,nB+1);
    TT =  new TH1D ("","",nB,1,nB+1);
    WJ =  new TH1D ("","",nB,1,nB+1);
    DY =  new TH1D ("","",nB,1,nB+1);
    ZTT =  new TH1D ("","",nB,1,nB+1);
    ST =  new TH1D ("","",nB,1,nB+1);
    VV =  new TH1D ("","",nB,1,nB+1);
    WW =  new TH1D ("","",nB,1,nB+1);
    TTX =  new TH1D ("","",nB,1,nB+1);
    QCD =  new TH1D ("","",nB,1,nB+1);

//    errNormOnlyB =  new TH1D ("","",nB,1,nB+1);

    for (int nb=1;nb <= fitHisto->GetNbinsX()+1; ++nb){ fitHistoPostH0->SetBinContent(nb,fitHisto->GetBinContent(nb)); fitHistoPostH0->SetBinError(nb,fitHisto->GetBinError(nb)); cout<<"  i  "<<nb<<"  "<<fitHisto->GetBinError(nb)<<endl;}
    for (int nb=1;nb <= fitHisto2->GetNbinsX()+1; ++nb){ fitHistoPostH1->SetBinContent(nb,fitHisto2->GetBinContent(nb)); fitHistoPostH1->SetBinError(nb,fitHisto2->GetBinError(nb)); cout<<"  i  "<<nb<<"  "<<fitHisto2->GetBinError(nb)<<endl;}
    for (int nb=1;nb <= fitHisto2->GetNbinsX()+1; ++nb){ errPreFitB->SetBinContent(nb,errPreFit->GetBinContent(nb)); errPreFitB->SetBinError(nb,errPreFit->GetBinError(nb)); cout<<"  Norm + Stat i  "<<nb<<"  "<<errPreFitB->GetBinError(nb)<<endl;}
//    for (int nb=1;nb <= fitHisto2->GetNbinsX()+1; ++nb){ errNormOnlyB->SetBinContent(nb,errNormOnly->GetBinContent(nb)); errNormOnlyB->SetBinError(nb,errNormOnly->GetBinError(nb)); cout<<"  Norm Only error i  "<<nb<<"  "<<errNormOnlyB->GetBinError(nb)<<endl;}

    fitHistoPostH0->SetMarkerSize(1.5);
    fitHistoPostH0->SetLineColor(kBlack);
    fitHistoPostH0->SetLineWidth(3);
    fitHistoPostH0->SetLineStyle(1);

    fitHistoPostH1->SetMarkerSize(1.5);
    fitHistoPostH1->SetLineColor(kMagenta);
    fitHistoPostH1->SetLineWidth(3);
    fitHistoPostH1->SetLineStyle(2);


	}

    TFile * file = new TFile(File);
    
for (int svar=0;svar<vars_.size();++svar)
{
	TString Variable=vars_.at(svar);

    //TH1D * TT = (TH1D*)file->Get("tt_"+Variable);
    //TH1D * TTcl = (TH1D*)file->Get("tt_"+Variable);
    TH1D * TTq = (TH1D*)filee->Get("shapes_fit_b/ch1/tt_"+Variable);
    TH1D * WJq = (TH1D*)filee->Get("shapes_fit_b/ch1/wj_"+Variable);
    TH1D * DYq = (TH1D*)filee->Get("shapes_fit_b/ch1/dyj_"+Variable);
    TH1D * ZTTq = (TH1D*)filee->Get("shapes_fit_b/ch1/ztt_"+Variable);
    TH1D * STq = (TH1D*)filee->Get("shapes_fit_b/ch1/sT_"+Variable);
    TH1D * VVq = (TH1D*)filee->Get("shapes_fit_b/ch1/dib_"+Variable);
    TH1D * WWq = (TH1D*)filee->Get("shapes_fit_b/ch1/ww_"+Variable);
    TH1D * TTXq = (TH1D*)filee->Get("shapes_fit_b/ch1/ttx_"+Variable);
    TH1D * QCDq = (TH1D*)filee->Get("shapes_fit_b/ch1/qcd_"+Variable);
    TH1D * histSignal = (TH1D*)file->Get("stau-stau_left_200_LSP1_B_"+Variable);
    TH1D * histSignal2 = (TH1D*)file->Get("stau-stau_left_200_LSP20_B_"+Variable);
    TH1D * histSignal3 = (TH1D*)file->Get("stau-stau_left_200_LSP30_B_"+Variable);
    TH1D * histData = (TH1D*)file->Get("data_obs");
    cout<<" ok "<<endl;

    for (int nb=1;nb <= fitHisto->GetNbinsX()+1; ++nb){ 
	   TT ->SetBinContent(nb,TTq->GetBinContent(nb)); TT->SetBinError(nb,TTq->GetBinError(nb)); 
	   WJ ->SetBinContent(nb,WJq->GetBinContent(nb)); WJ->SetBinError(nb,WJq->GetBinError(nb)); 
	   DY ->SetBinContent(nb,DYq->GetBinContent(nb)); DY->SetBinError(nb,DYq->GetBinError(nb)); 
	   ZTT ->SetBinContent(nb,ZTTq->GetBinContent(nb)); ZTT->SetBinError(nb,ZTTq->GetBinError(nb)); 
	   ST ->SetBinContent(nb,STq->GetBinContent(nb)); ST->SetBinError(nb,STq->GetBinError(nb)); 
	   VV ->SetBinContent(nb,VVq->GetBinContent(nb)); VV->SetBinError(nb,VVq->GetBinError(nb)); 
	   WW ->SetBinContent(nb,WWq->GetBinContent(nb)); WW->SetBinError(nb,WWq->GetBinError(nb)); 
	   TTX ->SetBinContent(nb,TTXq->GetBinContent(nb)); TTX->SetBinError(nb,TTXq->GetBinError(nb)); 
	   QCD ->SetBinContent(nb,QCDq->GetBinContent(nb)); QCD->SetBinError(nb,QCDq->GetBinError(nb)); 
    
    
    }
/*
    TH1D * WJ = (TH1D*)file->Get("wj_"+Variable);
    TH1D * DY = (TH1D*)file->Get("dyj_"+Variable);
    TH1D * ZTT = (TH1D*)file->Get("ztt_"+Variable);
    TH1D * ST = (TH1D*)file->Get("sT_"+Variable);
    TH1D * VV = (TH1D*)file->Get("dib_"+Variable);
    TH1D * WW = (TH1D*)file->Get("ww_"+Variable);
    TH1D * TTX = (TH1D*)file->Get("ttx_"+Variable);
    TH1D * QCD = (TH1D*)file->Get("qcd_"+Variable);
    TH1D * allbkg = (TH1D*)file->Get("tt_"+Variable);
 //   TH1D * histData = (TH1D*)file->Get("data_obs_"+Variable);
    TH1D * histData = (TH1D*)file->Get("data_obs");
    TH1D * histSignal = (TH1D*)file->Get("C1N2_100_LSP1_B_"+Variable);
//    TH1D * histSignal2 = (TH1D*)file->Get("C1N2_100_LSP50_B_"+Variable);
//    TH1D * histSignal3 = (TH1D*)file->Get("C1N2_500_LSP100_B_"+Variable);
//    TH1D * histSignal4 = (TH1D*)file->Get("stau-stau_100_LSP1_B_"+Variable);
*/




    //TH1D * hist = (TH1D*)file->Get("");
int nBins  =   histData->GetNbinsX();
if (BlindData) {for (int iB=1; iB<=nBins; ++iB) {histData->SetBinContent(iB,0);}}
//{for (int iB=1; iB<=nBins; ++iB) {TTcl->SetBinContent(iB,0);}}

  TH1D * TTcl = (TH1D*)TT->Clone("dummy");
  TH1D * allbkg = (TH1D*)TT->Clone("allbkg");

//   allbkg ->SetName("allbkg");
   allbkg->Add(WJ);
   allbkg->Add(DY);
   allbkg->Add(ST);
   allbkg->Add(ZTT);
   allbkg->Add(WW);
   allbkg->Add(VV);
   allbkg->Add(TTX);
   allbkg->Add(QCD);


 if (!OverlayFit) { fitHistoPostH0 = (TH1D*)allbkg->Clone();  fitHistoPostH1 = (TH1D*)allbkg->Clone();}

  std::cout << "TT  : " << TTcl->GetSumOfWeights() << " : "  <<" GetNbinsX= " << TTcl->GetNbinsX()<< "  Purity  "<<TTcl->GetSumOfWeights()/allbkg->GetSumOfWeights()<< std::endl;
  std::cout << "WJ   : " << WJ->GetSumOfWeights() << " : "  <<" GetNbinsX= " << WJ->GetNbinsX()<< "  Purity  "<<WJ->GetSumOfWeights()/allbkg->GetSumOfWeights()<< std::endl;
  std::cout << "DY : " << DY->GetSumOfWeights() << " : "  <<" GetNbinsX= " << DY->GetNbinsX()<< "  Purity  "<<DY->GetSumOfWeights()/allbkg->GetSumOfWeights()<< std::endl;
  std::cout << "ZTT : " << ZTT->GetSumOfWeights() << " : "  <<" GetNbinsX= " << ZTT->GetNbinsX()<< "  Purity  "<<ZTT->GetSumOfWeights()/allbkg->GetSumOfWeights()<< std::endl;
  std::cout << "ST   : " << ST->GetSumOfWeights() << " : "  <<" GetNbinsX= " << ST->GetNbinsX()<< "  Purity  "<<ST->GetSumOfWeights()/allbkg->GetSumOfWeights()<< std::endl;
  std::cout << "VV   : " << VV->GetSumOfWeights() << " : "  <<" GetNbinsX= " << VV->GetNbinsX() << "  Purity  "<<VV->GetSumOfWeights()/allbkg->GetSumOfWeights()<< std::endl;
  std::cout << "WW   : " << WW->GetSumOfWeights() << " : "  <<" GetNbinsX= " << WW->GetNbinsX() << "  Purity  "<<WW->GetSumOfWeights()/allbkg->GetSumOfWeights()<< std::endl;
  std::cout << "QCD   : " << QCD->GetSumOfWeights() << " : " <<" GetNbinsX= " << QCD->GetNbinsX()<< "  Purity  "<<QCD->GetSumOfWeights()/allbkg->GetSumOfWeights()<< std::endl;
  std::cout << "TTX  : " << TTX->GetSumOfWeights() << " : "  <<" GetNbinsX= " << TTX->GetNbinsX()<< "  Purity  "<<TTX->GetSumOfWeights()/allbkg->GetSumOfWeights()<< std::endl;
  std::cout << "histData : " << histData->GetSumOfWeights() << " : "  <<" GetNbinsX= " << histData->GetNbinsX()<< endl;
  std::cout << "histSignal : " << histSignal->GetSumOfWeights() << " : "  <<" GetNbinsX= " << histSignal->GetNbinsX()<<endl;
if (OverlayFit) std::cout << "fitHisto : " << fitHisto->GetSumOfWeights() << " : "  <<" GetNbinsX= " << fitHisto->GetNbinsX()<< std::endl;

  
  std::cout << "BKG : " << allbkg->GetSumOfWeights() << " : " << allbkg->Integral(0,nBins+1) << std::endl;
  std::cout << "DAT : " << histData->GetSumOfWeights() << " : " << histData->Integral(0,nBins+1) << std::endl;
  std::cout << "DAT/BKG = " << histData->GetSumOfWeights()/allbkg->GetSumOfWeights() << "+/-" 
	    << TMath::Sqrt(histData->GetSumOfWeights())/allbkg->GetSumOfWeights() << std::endl;
cout<<"==========="<<endl;
///////////////////////////////////////////////////////////////////////////////////////////////
    //ModTDRStyle();
    TH1D * bkgdErr = (TH1D*)allbkg->Clone("bkgdErr");
    TH1D * bkgdErrPost, *bkgdErrPostH1; 

    if (OverlayFit) { bkgdErrPost = (TH1D*)fitHistoPostH0->Clone("bkgdErrPost"); bkgdErrPostH1 = (TH1D*)fitHistoPostH1->Clone("bkgdErrPost");}
    else{  bkgdErrPost = (TH1D*)allbkg->Clone("bkgdErrPost");  bkgdErrPostH1 = (TH1D*)allbkg->Clone("bkgdErrPost");}

    ModTDRStyle();

  
    TCanvas* canv1 = new TCanvas("c1", "c1",1200,900);
    canv1->cd();
    std::vector<TPad*> pads = TwoPadSplit(0.29, 0.00, 0.00);
    pads[0]->SetLogy(logY);
    pads[1]->SetBottomMargin(0.16);
    pads[0]->SetLeftMargin(0.1);
    pads[1]->SetLeftMargin(0.1);
    histData->GetXaxis()->SetLabelSize(0.022);
 //   histData->GetXaxis()->SetLabelAngle(90);
    //histData->GetXaxis()->SetTimeFormat("#splitline{%s}{%s}");
    histData->GetXaxis()->LabelsOption("v");
//     for (int iB=1; iB<=nBins; ++iB) {histData->GetXaxis()->SetBinLabel(iB,BinLabels[iB-1]);}
    std::vector<TH1*> h = CreateAxisHists(2, histData, histData->GetXaxis()->GetXmin(), histData->GetXaxis()->GetXmax()-0.01);
    h[0]->Draw();
    
    
    std::string units="";
    std::string xtitle_ = (std::string) xtitle;
    size_t pos = xtitle_.find("[");
    if(pos!=std::string::npos) {
        units = xtitle_.substr(pos+1, xtitle_.find("]") - pos -1 );
        xtitle_ = xtitle_.substr(0, pos);
    }
    


    pads[1]->cd();
    h[1]->Draw();
    SetupTwoPadSplitAsRatio(pads, "Obs/Exp", true, 0.35, 1.55);
    StandardAxes(h[1]->GetXaxis(), h[0]->GetYaxis(),xtitle_ ,units);
    h[1]->GetYaxis()->SetNdivisions(4);
    h[1]->GetXaxis()->SetTitleOffset(1.2);
    h[1]->GetYaxis()->SetTitleOffset(1.0);
    pads[0]->cd();
    h[0]->GetYaxis()->SetTitleOffset(1.0);
    h[0]->GetYaxis()->SetTitleSize(0.05);
    pads[1]->SetGrid(0,1);
    //it complains if the minimum is set to 0 and you try to set log y scale
    if(logY) h[0]->SetMinimum(0.009);
    pads[0]->cd();
    
    // Setup legend
    TLegend *legend = PositionedLegend(0.50, 0.30, 3, 0.03);
    legend->SetTextFont(42);
    legend-> SetNColumns(2);
    
    histData->SetMarkerColor(1);
    histData->SetLineColor(1);
    histData->SetFillColor(1);
    histData->SetFillStyle(0);
    histData->SetLineWidth(2);
    histData->SetMarkerStyle(20);
    histData->SetMarkerSize(1);


    int color = CreateTransparentColor(13,0.);
    color=kBlue;
    histSignal->SetMarkerColor(kBlue);
    histSignal->SetLineColor(color);
    histSignal->SetFillColor(color);
    histSignal->SetFillStyle(0);
    //histSignal->SetLineWidth(2);
    histSignal->SetMarkerStyle(31);
    histSignal->SetMarkerSize(1.5);

    color=kRed;
    histSignal2->SetMarkerColor(kRed);
    histSignal2->SetLineColor(color);
    histSignal2->SetFillColor(color);
    histSignal2->SetFillStyle(0);
    //histSignal->SetLineWidth(2);
    histSignal2->SetMarkerStyle(31);
    histSignal2->SetMarkerSize(1.5);
    

    color=TColor::GetColor("#66ccc3");
    histSignal3->SetMarkerColor(color);
    histSignal3->SetLineColor(color);
    histSignal3->SetFillColor(color);
    histSignal3->SetFillStyle(0);
    //histSignal->SetLineWidth(2);
    histSignal3->SetMarkerStyle(31);
    histSignal3->SetMarkerSize(1.5);

/*
    color=kRed;
    histSignal4->SetMarkerColor(color);
    histSignal4->SetLineColor(color);
    histSignal4->SetFillColor(color);
    histSignal4->SetFillStyle(0);
    //histSignal->SetLineWidth(2);
    histSignal4->SetMarkerStyle(31);
    histSignal4->SetMarkerSize(1.5);
    color=kBlue;
    histSignal6->SetMarkerColor(color);
    histSignal6->SetLineColor(color);
    histSignal6->SetFillColor(color);
    histSignal6->SetFillStyle(0);
    //histSignal->SetLineWidth(2);
    histSignal6->SetMarkerStyle(31);
    histSignal6->SetMarkerSize(1.5);
    color=kMagenta;
    histSignal4->SetMarkerColor(color);
    histSignal4->SetLineColor(color);
    histSignal4->SetFillColor(color);
    histSignal4->SetFillStyle(0);
    histSignal4->SetMarkerStyle(31);
    histSignal4->SetMarkerSize(1.5);
*/

cout<<" check "<<endl;
    if (!BlindData)legend->AddEntry(histData, "Observed", "ple");
    



    InitHist(QCD,mycolorqcd);
    InitHist(DY,mycolordyj);
    InitHist(TT,mycolortt);
    InitHist(TTcl,mycolortt);
    InitHist(ST,mycolorst);
    InitHist(TTX,mycolorttx);
    InitHist(WJ,mycolorwjet);
    InitHist(VV,mycolorvv);
    InitHist(ZTT,mycolorztt);
    InitHist(WW,mycolorww);
    
    legend->AddEntry(TT,"TTJets","f");
    legend->AddEntry(ST,"singleTop","f");
    legend->AddEntry(TTX,"TTX/TG","f");
    legend->AddEntry(WJ,"WJets","f");
    legend->AddEntry(QCD,"QCD","f");
    legend->AddEntry(ZTT,"DY#rightarrow#tau#tau","f");
    legend->AddEntry(DY,"DY#rightarrow#mu#mu/ee","f");
    legend->AddEntry(WW,"WW","f");
    legend->AddEntry(VV,"Rest","f");
    legend->AddEntry(histSignal,"#tau_{L}_200_LSP_1","ple");
    legend->AddEntry(histSignal2,"#tau_{L}_200_LSP_10","ple");
    legend->AddEntry(histSignal3,"#tau_{L}_200_LSP_30","ple");



//for (int iB=1; iB<=nBins; ++iB) {WJ->SetBinError(iB,0);TT->SetBinError(iB,0);DY->SetBinError(iB,0);QCD->SetBinError(iB,0);VV->SetBinError(iB,0);TTX->SetBinError(iB,0);}
    WJ->SetMarkerStyle(0);
    TT->SetMarkerStyle(0);
    TTcl->SetMarkerStyle(0);
    DY->SetMarkerStyle(0);
    ZTT->SetMarkerStyle(0);
    QCD->SetMarkerStyle(0);
    VV->SetMarkerStyle(0);
    WW->SetMarkerStyle(0);
    TTX->SetMarkerStyle(0);
    ST->SetMarkerStyle(0);

    THStack *hs = new THStack("","");
    TTcl->SetFillColor(mycolortt);
    hs->Add(TTX);
    hs->Add(ST);
    hs->Add(WW);
    hs->Add(VV);
    hs->Add(QCD);
    hs->Add(TTcl);
    hs->Add(WJ);
    hs->Add(DY);
    hs->Add(ZTT);



    TH1D *hsum = ((TH1D*)(hs->GetStack()->Last()));
 //   for (int nb=0;nb<hsum->GetNbinsX()+1;++nb){if (hsum->GetBinContent(nb)==0) hsum->SetBinContent(nb,0.1);}
 //   */
 
   // hsum->Draw("h");
   /* ST->Draw("sameh");
    TT->Draw("sameh");
    WJ->Draw("sameh");
    QCD->Draw("sameh");   
    DY->Draw("sameh");
    TTX->Draw("sameh");
    VV->Draw("sameh");
*/
  //  TTcl->Draw("h");
  //  DY->Draw("sameh");
 /*   WJ->Draw("sameh");
    QCD->Draw("sameh");   
    VV->Draw("sameh");
    ST->Draw("sameh");
    TTX->Draw("sameh");
*/

    cout<<" check again"<<endl;
  hsum->Draw("same hist");
  hs->Draw("same hist");
  for (int n=1;n<histSignal->GetNbinsX()+1;++n) cout<<" Signal metrics  "<<histSignal->GetBinContent(n)<<endl;


  for (int n=1;n<histSignal->GetNbinsX()+1;++n) histSignal->SetBinError(n,0);
    histSignal->Draw("same p");

//if(OverlayFit)    fitHistoPostH0->Draw("sameh");
//if(OverlayFit)    fitHistoPostH1->Draw("sameh");


cout<<" check "<<endl;

    histSignal2->Draw("samep");
    histSignal3->Draw("samep");
    
//    histSignal5->SetLineStyle(4);
//    histSignal6->SetLineStyle(4);
//    histSignal4->Draw("samep");

float sum;
    for (int b=1;b<allbkg->GetNbinsX()+1;++b) cout<< "  b  "<<b<<" mc "<<allbkg->GetBinContent(b)<<" data  "<<histData->GetBinContent(b)<<"  "<<histData->GetBinContent(b)/allbkg->GetBinContent(b)<<" deviation "<<(histData->GetBinContent(b) - allbkg->GetBinContent(b))/sqrt(histData->GetBinContent(b))<<" DYJets "<<DY->GetBinContent(b)<<" TT "<<TTcl->GetBinContent(b)<<endl;

    cout<<" =============== SF study  ======================"<<endl;


    cout<<" ========================================== Table ================================== "<<endl;


cout<<"	    \\begin{table}"<<endl;
cout<<"        \\caption{CR}"<<endl;
cout<<"       \\begin{center}"<<endl;
cout<<" \\resizebox{1.\\textwidth}{!} {"<<endl;
cout<<"         \\begin{tabular}{|l|c|c|c|c|c|c|c|c|c|c|c|c|c|c|c|c|c|c|c|c|}  \\hline "<<endl;
cout<<"	 SR bin    &  TTJets     & WJets    & DYJets       & QCD   & SingleT    & VVJets   & TTXJets       & Total Bkg ($\\pm total error)    & Observed \\\\"<<endl;
     cout<<" \\hline "<<endl;

     for (int b=1;b<allbkg->GetNbinsX()+1;++b){
float sumBin = TTcl->GetBinContent(b)+WJ->GetBinContent(b)+DY->GetBinContent(b)+ZTT->GetBinContent(b)+QCD->GetBinContent(b)+ST->GetBinContent(b)+VV->GetBinContent(b)+WW->GetBinContent(b)+TTX->GetBinContent(b);
    std::cout << std::setprecision(1);

    float total_err = errPreFitB->GetBinError(b);
//    float norm_err = errNormOnlyB->GetBinError(b);
//    float shapes_err =sqrt(total_err*total_err - norm_err*norm_err);

     //cout<<"SL"<<histData->GetXaxis()->GetBinLabel(b)<<" & "<<TTcl->GetBinContent(b)<< " & "<<WJ->GetBinContent(b)<<" & "<<DY->GetBinContent(b)+ZTT->GetBinContent(b)<<" & "<<QCD->GetBinContent(b)<<" & "<<ST->GetBinContent(b)<<" & "<<VV->GetBinContent(b)+WW->GetBinContent(b)<<" & "<<TTX->GetBinContent(b)<<" & "<<sumBin<<" $\\pm$ "<<sqrt(sumBin)<<" $\\pm$ "<< errNormOnlyB->GetBinError(b)<<" & "<<histData->GetBinContent(b)<<" \\\\\\hline "<<endl;
     //cout<<histData->GetXaxis()->GetBinLabel(b)<<" & "<<TTcl->GetBinContent(b)<< " & "<<WJ->GetBinContent(b)<<" & "<<DY->GetBinContent(b)+ZTT->GetBinContent(b)<<" & "<<QCD->GetBinContent(b)<<" & "<<ST->GetBinContent(b)<<" & "<<VV->GetBinContent(b)+WW->GetBinContent(b)<<" & "<<TTX->GetBinContent(b)<<" & "<<sumBin<<" $\\pm$ "<<norm_err<<" $\\pm$ "<< shapes_err<<" & "<<histData->GetBinContent(b)<<" \\\\\\hline "<<endl;
 //   std::cout<<histData->GetXaxis()->GetBinLabel(b)<<" & "<<std::fixed<<std::setprecision(1)<<TTcl->GetBinContent(b)<< " & "<<std::setprecision(1)<<WJ->GetBinContent(b)<<" & "<<std::setprecision(1)<<DY->GetBinContent(b)+ZTT->GetBinContent(b)<<" & "<<std::setprecision(1)<<QCD->GetBinContent(b)<<" & "<<std::setprecision(1)<<ST->GetBinContent(b)<<" & "<<std::setprecision(1)<<VV->GetBinContent(b)+WW->GetBinContent(b)<<" & "<<std::setprecision(1)<<TTX->GetBinContent(b)<<" & "<<std::setprecision(1)<<sumBin<<" $\\pm$ "<<std::setprecision(1)<<total_err<<" & "<< std::setprecision(0)<<histData->GetBinContent(b)<<" \\\\\\hline "<<std::endl;

  	    std::cout<<histData->GetXaxis()->GetBinLabel(b)<<" & "<<std::fixed<<std::setprecision(2)<<TTcl->GetBinContent(b)<<" $\\pm$ "<<TTcl->GetBinError(b)
	    << " & "<<std::setprecision(1)<<WJ->GetBinContent(b)<<" $\\pm$ "<<WJ->GetBinError(b)
	    << " & "<<std::setprecision(1)<<DY->GetBinContent(b)+ZTT->GetBinContent(b)<<" $\\pm$ "<<sqrt(DY->GetBinError(b)*DY->GetBinError(b)+ZTT->GetBinError(b)*ZTT->GetBinError(b))
	    << " & "<<std::setprecision(1)<<QCD->GetBinContent(b)<<" $\\pm$ "<<QCD->GetBinError(b)
	    << " & "<<std::setprecision(1)<<ST->GetBinContent(b)<<" $\\pm$ "<<ST->GetBinError(b)
	    << " & "<<std::setprecision(1)<<VV->GetBinContent(b)+WW->GetBinContent(b)<<" $\\pm$ "<<sqrt(VV->GetBinError(b)*VV->GetBinError(b)+WW->GetBinError(b)*WW->GetBinError(b))
	    << " & "<<std::setprecision(1)<<TTX->GetBinContent(b)<<" $\\pm$ "<<TTX->GetBinError(b)
	    << " & "<<std::setprecision(1)<<sumBin<<" $\\pm$ "<<total_err
	    << " & "<<std::setprecision(0)<<histData->GetBinContent(b)<<" \\\\\\hline "<<std::endl;

	//<<endl;




     }

cout<<"	    \\end{tabular}}"<<endl;
cout<<"       \\end{center}"<<endl;

cout<<"	    \\end{table}"<<endl;








    canv1->Update();

    canv1->Update();



    float errLumi = 0.025;
    float errMuon = 0.02;
    float errElectron = 0.03;
    float errTau = 0.03;
    float errTauID = 0.05;
    float errBTag = 0.04;
    float errMET = 0.04;
    float errTFR = 0.1;
    float errLTFR = 0.1;

    for (int iB=1; iB<=nBins; ++iB) {

        allbkg->SetBinError(iB,0);
        fitHistoPostH0->SetBinError(iB,0);

        float eStat =  bkgdErr->GetBinError(iB);
        float X = bkgdErr->GetBinContent(iB);
        float eLumi = errLumi * X;

        float eStatPost =  bkgdErrPost->GetBinError(iB);
        float XPost = bkgdErrPost->GetBinContent(iB);
        float eLumiPost = errLumi * XPost;

        float eMuon = errMuon * X;
        float eElectron = errElectron * X;
        float eMET = errMET * X;
        float eTau = errTau * X;
        float eTauID = errTauID * X;
        float eTFR = errTFR * X;
        float eLTFR = errLTFR * X;

        float eMuonP = errMuon * XPost;
        float eElectronP = errElectron * XPost;
        float eMETP = errMET * XPost;
        float eTauP = errTau * XPost;
        float eTauIDP = errTauID * XPost;
        float eTFRP = errTFR * XPost;
        float eLTFRP = errLTFR * XPost;

        //float eBkg = dummy->GetBinError(iB);
        //float Err = TMath::Sqrt(eStat*eStat+eLumi*eLumi+eBkg*eBkg+eMuon*eMuon+eElectron*eElectron);
	float Err = TMath::Sqrt(eStat*eStat+eLumi*eLumi + eElectron*eElectron + eMuon*eMuon + eTau*eTau + eMET*eMET + eTFR*eTFR + eLTFR*eLTFR + eTauID*eTauID);
	float ErrPost = eStatPost;//TMath::Sqrt(eStatPost*eStatPost+eLumiPost*eLumiPost + eElectronP*eElectronP + eMuonP*eMuonP + eTauP*eTauP + eMETP*eMETP + eTFRP*eTFRP + eLTFRP*eLTFRP + eTauIDP*eTauIDP);
	
	
      if (OverlayFit)  bkgdErr->SetBinError(iB,errPreFitB->GetBinError(iB));
      else bkgdErr->SetBinError(iB,allbkg->GetBinError(iB));
        //bkgdErrPost->SetBinError(iB,ErrPost);
	cout<<" iB "<<iB<<" Pre "<<float(eStat/X)<<" Post  "<<float(eStatPost/XPost)<<"  "<<ErrPost<<"  "<<Err<<endl;
    }

    
    bkgdErr->SetMarkerSize(0);
    int new_idx = CreateTransparentColor(kBlue,0.9);
    int new_idxP = CreateTransparentColor(kGreen,0.3);

    bkgdErr->SetFillStyle(3003);
    bkgdErr->SetFillColor(new_idx);
    bkgdErr->SetLineWidth(1);
//if (OverlayFit)   bkgdErr->Draw("e2same");

    bkgdErrPost->SetFillColor(new_idxP);
    bkgdErrPost->SetLineColor(new_idxP);
    bkgdErrPost->SetLineWidth(1);
if (OverlayFit)    bkgdErrPost->Draw("e2same");


if (OverlayFit)    legend->AddEntry(bkgdErrPost, "Bkg. uncertainty PostFit H_{0} - Bkg Only" , "F" );
//    legend->AddEntry(bkgdErr, "Bkg. uncertainty PreFit" , "F" );
    canv1->Update();

    TH1D * ratioH = (TH1D*)histData->Clone("ratioH");
    TH1D * ratioHPost = (TH1D*)histData->Clone("ratioHPost");
    TH1D * ratioHPostH1 = (TH1D*)histData->Clone("ratioHPostH1");

    TH1D * ratioErrH = (TH1D*)bkgdErr->Clone("ratioErrH");
    TH1D * ratioErrHPost = (TH1D*)bkgdErrPost->Clone("ratioErrHPost");
    TH1D * ratioErrHPostH1 = (TH1D*)bkgdErrPost->Clone("ratioErrHPostH1");
 
    float x2Post=0.;float x2PostH1 =0.;
    for (int iB=1; iB<=nBins; ++iB) {

        float x1 = histData->GetBinContent(iB);
  	  float x2 = allbkg->GetBinContent(iB);

       if(OverlayFit) x2Post = fitHistoPostH0->GetBinContent(iB);
       if(OverlayFit) x2PostH1 = fitHistoPostH1->GetBinContent(iB);
       if(!OverlayFit) x2Post = x2;
       if(!OverlayFit) x2PostH1 = x2;

	ratioErrH->SetBinError(iB,0.0);
	ratioErrHPost->SetBinError(iB,0.0);
	ratioErrHPostH1->SetBinError(iB,0.0);

        ratioErrH->SetBinContent(iB,1.0);
	ratioErrHPost->SetBinContent(iB,1.0);
	ratioErrHPostH1->SetBinContent(iB,1.0);


        float xBkg = bkgdErr->GetBinContent(iB);
        float errBkg = bkgdErr->GetBinError(iB);

        float xBkgPost = bkgdErrPost->GetBinContent(iB);
        float errBkgPost = bkgdErrPost->GetBinError(iB);

        float xBkgPostH1 = bkgdErrPostH1->GetBinContent(iB);
        float errBkgPostH1 = bkgdErrPostH1->GetBinError(iB);

        if (xBkg>0) {
            float relErr = errBkg/xBkg;
            float relErrP = errBkgPost/xBkgPost;
            float relErrPH1 = errBkgPostH1/xBkgPostH1;

            ratioErrH->SetBinError(iB,relErr);

            ratioErrHPost->SetBinError(iB,relErrP);
            ratioErrHPostH1->SetBinError(iB,relErrPH1);
            
        }
        if (x1>0&&x2>0) {
            float e1 = histData->GetBinError(iB);
            float ratio = x1/x2;
            float ratioPost = x1/x2Post;
            float ratioPostH1 = x1/x2PostH1;

            float eratio = e1/x2;
            float eratioPost = e1/x2Post;
            float eratioPostH1 = e1/x2PostH1;

            ratioH->SetBinContent(iB,ratio);
            ratioHPost->SetBinContent(iB,ratioPost);
            ratioHPostH1->SetBinContent(iB,ratioPostH1);

            ratioH->SetBinError(iB,eratio);
            ratioHPost->SetBinError(iB,eratioPost);
            ratioHPostH1->SetBinError(iB,eratioPostH1);
        }
        else {
            ratioH->SetBinContent(iB,1000);
            ratioHPost->SetBinContent(iB,1000);
            ratioHPostH1->SetBinContent(iB,1000);
        }
    }
    
    pads[1]->cd();
    ratioErrH->GetYaxis()->SetRangeUser(0.8,1.2);
    ratioErrHPost->GetYaxis()->SetRangeUser(0.8,1.2);

//    ratioErrH->Draw("e2same");
if (OverlayFit)    ratioErrHPost->Draw("e2same");
//if (OverlayFit)    ratioErrHPostH1->Draw("e2same");



//    ratioH->Draw("pe0same");

    if (OverlayFit){
    ratioHPost->SetMarkerStyle(20);
    ratioHPost->SetMarkerColor(kBlack);
    ratioHPost->SetLineColor(kBlack);
    ratioHPost->SetLineWidth(2);
    ratioHPost->SetLineStyle(1);
    ratioHPost->Sumw2();
if (OverlayFit)    ratioHPost->Draw("pe0same");
 //   legend->AddEntry(ratioHPost, "PostFit H_{0} Bkg Only" , "ple" );

    ratioHPostH1->SetMarkerStyle(24);
    ratioHPostH1->SetMarkerColor(kBlue);
    ratioHPostH1->SetLineColor(kBlue);
    ratioHPostH1->SetLineWidth(2);
    ratioHPostH1->SetLineStyle(2);
    ratioHPostH1->Sumw2();
//if (OverlayFit)    ratioHPostH1->Draw("pe0same");
//    legend->AddEntry(ratioHPostH1, "PostFit H_{1} -Bkg+C1N2 (100,1)" , "ple" );


    }

    pads[0]->cd();
    histData->Draw("pesame");
    
    string v="";
    FixTopRange(pads[0], GetPadYMax(pads[0]), 0.35);
    DrawCMSLogo(pads[0], "CMS", "Preliminary", 11, 0.045, 0.035, 1.2);
    DrawTitle(pads[0], "35.9 fb^{-1} (13 TeV)", 3);
    DrawTitle(pads[0], v, 1);
    TLine *l=new TLine(23,hsum->GetBinContent(23)+100,23,3e3);
    TArrow *lr=new TArrow(23,15e3,27,15e3,0.02,"|>");
    TArrow *ll=new TArrow(23,15e3,19,15e3,0.02,"|>");
    l->SetLineStyle(2);
    l->SetLineWidth(3);
    l->SetLineColor(kBlack);
    lr->SetLineStyle(2);
    lr->SetLineWidth(2);
    lr->SetLineColor(kBlack);
    lr->SetFillColor(kBlack);
    ll->SetLineStyle(2);
    ll->SetLineWidth(2);
    ll->SetLineColor(kBlack);
    ll->SetFillColor(kBlack);

    TLatex *tex,*texx;
    tex = new TLatex(24,1e3,"1-Jet");
    //tex->SetLineWidth(2);
    texx = new TLatex(17,1e3,"0-Jet");
    //texx->SetLineWidth(2);
//    tex->SetTextSize(0.02);
    tex->Draw();
    texx->Draw();

    FixBoxPadding(pads[0], legend, 0.15);
    legend->Draw();
    l->Draw();
//    lr->Draw();
//    ll->Draw();
    FixOverlay();
    canv1->Update();
    pads[0]->GetFrame()->Draw();
    canv1->Print(Variable+Suffix+suffix+".pdf");
}
}
