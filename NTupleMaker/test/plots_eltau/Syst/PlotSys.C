#include "HttStylesNew.cc"
#include <vector>
#include <algorithm>
#
using namespace std;
void PlotSys(TString fileName = "Templ_met_MT2lester_DZeta01J1D_17_35invfb_mt_C1N2_SR_CR1_out.root",
	     TString histName = "allbkg_met_MT2lester_DZeta01J1D_17",
	     //TString histName = "data_obs",
	   //  TString histName = "1D_C1N2_100_LSP1_B_met_MT2lester_DZeta01J1D_17",
	     TString dirName  = "SR_CR1",
	     TString sysName  = "PDF",
	     TString header = "",
	     TString SysLeg = "allbkg PDF",
	     float xmax = 100,
	     float upRange = -100) {

  TString variable("S");
  TString ytitle("Events / bin");
  TString xtitle("SR_CR1"); 
  double maxDev[50]={0.};
  double minDev[50]={0.};

  SetStyle();
  gStyle->SetErrorX(0);
  TFile * file = new TFile(fileName);
  TH1D * histNominal = (TH1D*)file->Get(histName);
  TH1D * histUp = (TH1D*)file->Get(histName+"_"+sysName+"Up");
  TH1D * histDown = (TH1D*)file->Get(histName+"_"+sysName+"Down");
  std::cout << histNominal << " " <<  histUp << " " << histDown << std::endl;
  histNominal->SetLineColor(1);
  histNominal->SetLineWidth(3);
  histNominal->SetTitle("");
  histUp->SetTitle("");
  histDown->SetTitle("");

  histUp->SetLineColor(kRed);
  histUp->SetLineWidth(2);
  histUp->SetLineStyle(3);

  histDown->SetLineWidth(2);
  histDown->SetLineColor(4);
  histDown->SetLineStyle(3);
  histNominal->SetMarkerColor(1);

  histUp->SetMarkerColor(kRed);
  histDown->SetMarkerColor(4);

  histNominal->SetMarkerSize(1.3);
  histNominal->SetMarkerStyle(20);
  histNominal->SetFillColor(0);
  histNominal->GetYaxis()->SetTitle(ytitle);
  histNominal->GetXaxis()->SetTitle(xtitle);
  histUp->GetYaxis()->SetTitle(ytitle);
  histUp->GetXaxis()->SetTitle(xtitle);
  histDown->GetYaxis()->SetTitle(ytitle);
  histDown->GetXaxis()->SetTitle(xtitle);
  TH1D * ratioUp = (TH1D*)histUp->Clone("ratioUp");
  TH1D * ratioDown = (TH1D*)histDown->Clone("ratioDown");
  TH1D * ratioCentral = (TH1D*)histNominal->Clone("ratioCentral");
  histUp->GetYaxis()->SetRangeUser(0.01,25*histNominal->GetMaximum());
  histUp->GetXaxis()->SetRangeUser(0.,xmax);
  //  ratioCentral->SetFillStyle(3013);
  //  ratioCentral->SetFillColor(1);
  //  ratioCentral->SetMarkerStyle(21);
  //  ratioCentral->SetMarkerSize(0);

  int nBins = histNominal->GetNbinsX(); 

  vector<double> C;
  C.clear();


  float minVarUp=0;
  float maxVarUp=0;
  float minVarDown=0;
  float maxVarDown=0;

 vector <double> maxRel; maxRel.clear();
 vector <double> minRel; minRel.clear();
  float minVar=0;
  float maxVar=0;
  for (int iB=1; iB<=nBins; ++iB) {
    histUp->SetBinError(iB,0); 
    histDown->SetBinError(iB,0); 
    float xUp = histUp->GetBinContent(iB);
    float xDown = histDown->GetBinContent(iB);
    float xCentral = histNominal->GetBinContent(iB);
    float xratioUp = 1;
    float xratioDown = 1;
    if (xCentral>0.) {
      xratioUp   = xUp/xCentral;
      xratioDown = xDown/xCentral;
    }
    ratioUp->SetBinContent(iB,xratioUp);
    ratioDown->SetBinContent(iB,xratioDown);
    ratioUp->SetBinError(iB,0);
    ratioDown->SetBinError(iB,0);
    ratioCentral->SetBinContent(iB,1);
    ratioCentral->SetBinError(iB,0);
//    if (histNominal->GetBinContent(iB)>0){
      ratioCentral->SetBinError(iB,histNominal->GetBinError(iB)/histNominal->GetBinContent(iB));
 cout<<" Bin  "<<iB<<" nom  "<<histNominal->GetBinContent(iB)<<" plus  "<<histUp->GetBinContent(iB)<<" fr "<<fabs((histNominal->GetBinContent(iB)-histUp->GetBinContent(iB))/histNominal->GetBinContent(iB))<<"  min  "<<histDown->GetBinContent(iB)<<" fr "<<fabs((histNominal->GetBinContent(iB)-histDown->GetBinContent(iB))/histNominal->GetBinContent(iB))<<"  max deviation  "<<max( fabs( (histNominal->GetBinContent(iB)-histUp->GetBinContent(iB))/histNominal->GetBinContent(iB)), fabs((histNominal->GetBinContent(iB)-histDown->GetBinContent(iB))/histNominal->GetBinContent(iB)))<<" effect "<<setprecision(2)<<max( fabs( (histNominal->GetBinContent(iB)-histUp->GetBinContent(iB))/histNominal->GetBinContent(iB)), fabs((histNominal->GetBinContent(iB)-histDown->GetBinContent(iB))/histNominal->GetBinContent(iB)))*histNominal->GetBinContent(iB)/histNominal->GetSumOfWeights()*100<<endl;

  C.push_back(fabs((histNominal->GetBinContent(iB)-histUp->GetBinContent(iB))/histNominal->GetBinContent(iB)) );
  C.push_back(fabs((histNominal->GetBinContent(iB)-histDown->GetBinContent(iB))/histNominal->GetBinContent(iB)));
  
  if (histNominal->GetBinContent(iB)>0.){
  maxDev[iB] = max( fabs(histNominal->GetBinContent(iB)-histUp->GetBinContent(iB))/histNominal->GetBinContent(iB), fabs(histNominal->GetBinContent(iB)-histDown->GetBinContent(iB))/histNominal->GetBinContent(iB));
  minDev[iB] = min( fabs(histNominal->GetBinContent(iB)-histUp->GetBinContent(iB))/histNominal->GetBinContent(iB), fabs(histNominal->GetBinContent(iB)-histDown->GetBinContent(iB))/histNominal->GetBinContent(iB));
}
else {
	maxDev[iB] = 0;
	minDev[iB] = 0;
}

//float weight = histNominal->GetBinContent(iB)/histNominal->GetSumOfWeights();
float weight = histNominal->GetBinContent(iB);///histNominal->GetSumOfWeights();
if (weight>0.5) 
{maxRel.push_back(maxDev[iB]);
minRel.push_back(minDev[iB]);
}


}

  minVarUp=ratioUp->GetMaximum();
  maxVarUp=ratioUp->GetMinimum();

  minVarDown=ratioDown->GetMaximum();
  maxVarDown=ratioDown->GetMinimum();

  minVar=min(minVarUp,minVarDown);
  maxVar=max(maxVarUp,maxVarDown);
 
double  cLower = *min_element(C.begin(), C.end());
double  cUpper = *max_element(C.begin(), C.end());
/*
	 ofstream tfilee;
  cout<<" minVarUp  "<<minVarUp<<"  maxVarUp "<<maxVarUp<<"  minVarDown "<<minVarDown<<"  maxVarDown "<<maxVarDown<<" minVar "<<minVar<< " maxVar "<<maxVar<<" cLower "<<cLower<<" cUp  "<<cUpper<<endl; 
  //TString textfilenamee = "systematics_"+fileName +"_"+sysName+".txt";
  TString textfilenamee = "systematics_allbkg_PDF.txt";
  tfilee.open(textfilenamee);
  tfilee << "########################################" << endl;


  tfilee<<sysName<<"  "<<setprecision(2)<<cLower*100<<" - "<<setprecision(2)<<cUpper*100<<" \% "<<endl;

	tfilee.close();
*/

  if (upRange>0) 
    histUp->GetYaxis()->SetRangeUser(0.1,upRange);

  histUp->GetYaxis()->SetTitleOffset(1.4);

  TCanvas * canv1 = MakeCanvas("canv1", "", 1200, 900);
  TPad *upper = new TPad("upper", "pad",0,0.31,1,1);
  upper->Draw();
  upper->SetName("");
  upper->cd();
  upper->SetFillColor(0);
  upper->SetBorderMode(0);
  upper->SetBorderSize(10);
  upper->SetTickx(1);
  upper->SetTicky(1);
  upper->SetLeftMargin(0.17);
  upper->SetRightMargin(0.05);
  upper->SetBottomMargin(0.02);
  upper->SetFrameFillStyle(0);
  upper->SetFrameLineStyle(0);
  upper->SetFrameLineWidth(2);
  upper->SetFrameBorderMode(0);
  upper->SetFrameBorderSize(10);
  upper->SetFrameFillStyle(0);
  upper->SetFrameLineStyle(0);
  upper->SetFrameLineWidth(2);
  upper->SetFrameBorderMode(0);
  upper->SetFrameBorderSize(10);
   TPaveText *pt = new TPaveText(0.2,0.9431452,0.6179933,0.995,"blNDC");
   pt->SetName("title");
   pt->SetBorderSize(0);
   pt->SetFillColor(0);
   pt->SetLineWidth(3);
   TText *AText = pt->AddText("");
   pt->Draw();

  //histUp->SetMaximum(2*histNominal->GetMaximum());
  histUp->Draw("hp");
  histNominal->Draw("same ep hist");
  histDown->Draw("hpsame");

  TLegend * leg = new TLegend(0.28,0.7,0.52,0.9);
//  leg->SetHeader(header);
  leg->SetFillStyle(0);
  leg->SetLineColor(0);
  leg->SetFillColor(1);
  leg->SetTextSize(0.04);
  leg->AddEntry(histNominal,SysLeg+" Central","lp");
  leg->AddEntry(histUp,SysLeg+" Up","lp");
  leg->AddEntry(histDown,SysLeg+" Down","lp");
  leg->Draw("same");
  upper->SetLogy();
  upper->Draw("same");
  upper->RedrawAxis();
  upper->Modified();
  upper->Update();
  TLatex * cms = new TLatex(0.20,0.94,"    Simulation      #sqrt{s} = 13 TeV,   L = 35.9 fb^{-1}");
  cms->SetNDC();
  cms->SetTextSize(0.04);
  cms->Draw();
  canv1->cd();

  ratioUp->GetYaxis()->SetRangeUser(0.9001,1.0999);
  ratioUp->GetYaxis()->SetNdivisions(505);
  ratioUp->GetXaxis()->SetLabelFont(42);
  ratioUp->GetXaxis()->SetLabelOffset(0.04);
  ratioUp->GetXaxis()->SetLabelSize(0.1);
  ratioUp->GetXaxis()->SetTitleSize(0.13);
  ratioUp->GetXaxis()->SetTitleOffset(1.2);
  ratioUp->GetYaxis()->SetTitle("ratio");
  ratioUp->GetYaxis()->SetLabelFont(42);
  ratioUp->GetYaxis()->SetLabelOffset(0.015);
  ratioUp->GetYaxis()->SetLabelSize(0.1);
  ratioUp->GetYaxis()->SetTitleSize(0.14);
  ratioUp->GetYaxis()->SetTitleOffset(0.5);
  ratioUp->GetXaxis()->SetTickLength(0.07);
  ratioUp->GetYaxis()->SetTickLength(0.04);
  ratioUp->GetYaxis()->SetLabelOffset(0.01);

  // ------------>Primitives in pad: lower
  TPad * lower = new TPad("lower", "pad",0,0,1,0.32);
  lower->Draw();
  lower->cd();
  lower->SetFillColor(0);
  lower->SetBorderMode(0);
  lower->SetBorderSize(10);
  lower->SetGridy();
  lower->SetTickx(1);
  lower->SetTicky(1);
  lower->SetLeftMargin(0.17);
  lower->SetRightMargin(0.05);
  lower->SetTopMargin(0.026);
  lower->SetBottomMargin(0.35);
  lower->SetFrameFillStyle(0);
  lower->SetFrameLineStyle(0);
  lower->SetFrameLineWidth(2);
  lower->SetFrameBorderMode(0);
  lower->SetFrameBorderSize(10);
  lower->SetFrameFillStyle(0);
  lower->SetFrameLineStyle(0);
  lower->SetFrameLineWidth(2);
  lower->SetFrameBorderMode(0);
  lower->SetFrameBorderSize(10);

  ratioUp->SetMinimum(0.45);
  ratioUp->GetXaxis()->SetLabelSize(0.075);
  ratioUp->SetMaximum(1.55);
  ratioUp->SetMarkerSize(1.5);
  ratioDown->SetMarkerSize(1.5);

  ratioUp->SetMarkerStyle(24);
  ratioDown->SetMarkerStyle(25);
  
  ratioUp->Draw("e1p");
  ratioDown->Draw("e1psame");
  TAxis *axis = ratioCentral->GetXaxis();
  TLine *l=new TLine(axis->GetBinLowEdge(ratioCentral->FindFirstBinAbove(0.)),1,axis->GetBinLowEdge(ratioCentral->FindLastBinAbove(0)+1),1);

   l->SetLineColor(kRed);
   l->SetLineWidth(2);
   l->Draw("sames");
  //ratioCentral->Draw("e1same");

  lower->Modified();
  lower->RedrawAxis();
  canv1->cd();
  canv1->Modified();
  canv1->cd();

float dNom = histNominal->GetSumOfWeights();
float dUp = histUp->GetSumOfWeights();
float dDown = histDown->GetSumOfWeights();

double  cUpRel = *max_element(maxRel.begin(), maxRel.end());
double  cDownRel = *min_element(minRel.begin(), minRel.end());


float dMaxUp = fabs(ratioUp->GetXaxis()->GetBinCenter(ratioUp->GetMaximumBin()));
float dMinUp = fabs(ratioUp->GetXaxis()->GetBinCenter(ratioUp->GetMinimumBin()));
float dMaxDown = fabs(ratioDown->GetXaxis()->GetBinCenter(ratioDown->GetMaximumBin()));
float dMinDown = fabs(ratioDown->GetXaxis()->GetBinCenter(ratioDown->GetMinimumBin()));

cout<<" max Relative diff allbkg PDF " <<std::max(   fabs((dNom-dUp)/dNom), fabs((dNom-dDown)/dNom)) *100<<endl;
cout<<" from Up max "<<setprecision(2)<<dMaxUp<<"  "<<dMinUp<<"  "<<dMaxDown<<"  "<<dMinDown<<endl;

canv1->Print("figures/systematics/"+dirName+"_"+histName+"_"+sysName+"_sys.pdf");
 // canv1->Print("figures/systematics/"+dirName+"_"+histName+"_"+sysName+"_sys.C");

	TString outname=sysName;
	 ofstream tfile;
        TString textfilename = "systematics_allbkg_PDF.txt";
	tfile.open(textfilename);

                                tfile << "########################################" << endl;
                                for (int nb=1;nb<histNominal->GetNbinsX();++nb){
				float Upvar = (histNominal->GetBinContent(nb)-histUp->GetBinContent(nb)) / histNominal->GetBinContent(nb);
				float Downvar = (histNominal->GetBinContent(nb)-histDown->GetBinContent(nb)) / histNominal->GetBinContent(nb);

                                 //tfile <<"PDF "<< histNominal->GetXaxis()->GetBinLabel(nb)<<" "<<Upvar<<"  "<<Downvar<<endl;
                                 tfile <<"PDF "<< histNominal->GetXaxis()->GetBinLabel(nb)<<" maxDev "<<maxDev[nb]<<" weight  "<<histNominal->GetBinContent(nb)/histNominal->GetSumOfWeights()<<" Bin Cont "<<histNominal->GetBinContent(nb)<<endl;
                                }
                                 tfile <<"PDF - Max deviations  "<< cDownRel*100<<" - "<<  cUpRel*100<<endl;

				


	tfile.close();





} 
