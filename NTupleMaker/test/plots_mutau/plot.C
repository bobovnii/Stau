void plot()
{
//=========Macro generated from canvas: Canvas_1/Canvas_1
//=========  (Fri Mar  3 10:48:54 2017) by ROOT version6.06/01
   TCanvas *Canvas_1 = new TCanvas("Canvas_1", "Canvas_1",391,242,1174,773);
   Canvas_1->Range(-6.750001,-3.427256,65.75,13.46616);
   Canvas_1->SetFillColor(0);
   Canvas_1->SetBorderMode(0);
   Canvas_1->SetBorderSize(2);
   Canvas_1->SetFrameBorderMode(0);
   Canvas_1->SetFrameBorderMode(0);
   
   TH1D *R__1 = new TH1D("R__1","met_MT2lester_DZeta01J1D JetsCuts",58,0.5,58.5);
   R__1->SetBinContent(3,0.3716554);
   R__1->SetBinContent(7,1.059955);
   R__1->SetBinContent(9,0.4203468);
   R__1->SetBinContent(13,0.3581546);
   R__1->SetBinContent(14,1.283118);
   R__1->SetBinContent(15,0.7603572);
   R__1->SetBinContent(16,0.7330251);
   R__1->SetBinContent(17,2.456359);
   R__1->SetBinContent(20,1.361333);
   R__1->SetBinContent(21,1.349423);
   R__1->SetBinContent(22,1.190559);
   R__1->SetBinContent(23,0.6228469);
   R__1->SetBinContent(24,1.005377);
   R__1->SetBinContent(25,1.524979);
   R__1->SetBinContent(26,0.7447878);
   R__1->SetBinContent(28,1.091574);
   R__1->SetBinContent(29,0.7157787);
   R__1->SetBinContent(32,0.9701192);
   R__1->SetBinContent(33,1.513136);
   R__1->SetBinContent(34,0.9673605);
   R__1->SetBinContent(36,0.7954839);
   R__1->SetBinContent(38,1.023683);
   R__1->SetBinContent(39,1.127421);
   R__1->SetBinContent(40,1.048701);
   R__1->SetBinContent(41,5.004128);
   R__1->SetBinContent(42,0.8162037);
   R__1->SetBinContent(43,0.7506574);
   R__1->SetBinContent(44,0.7128703);
   R__1->SetBinContent(45,1.190648);
   R__1->SetBinContent(46,0.7419405);
   R__1->SetBinContent(47,0.5920656);
   R__1->SetBinContent(48,0.7957007);
   R__1->SetBinContent(49,0.9138293);
   R__1->SetBinContent(50,0.7044755);
   R__1->SetBinContent(51,1.075022);
   R__1->SetBinContent(52,0.9319962);
   R__1->SetBinContent(53,0.8206474);
   R__1->SetBinContent(54,1.176517);
   R__1->SetBinContent(55,0.8107859);
   R__1->SetBinContent(56,1.639664);
   R__1->SetBinContent(57,0.6105896);
   R__1->SetBinContent(58,0.9230052);
   R__1->SetBinError(3,0.200199);
   R__1->SetBinError(7,0.3549518);
   R__1->SetBinError(9,0.227323);
   R__1->SetBinError(13,0.1922054);
   R__1->SetBinError(14,0.9095303);
   R__1->SetBinError(15,0.3836414);
   R__1->SetBinError(16,0.2410088);
   R__1->SetBinError(17,2.007317);
   R__1->SetBinError(20,0.4968624);
   R__1->SetBinError(21,1.175847);
   R__1->SetBinError(22,0.5902273);
   R__1->SetBinError(23,0.6963716);
   R__1->SetBinError(24,0.3948973);
   R__1->SetBinError(25,0.4096831);
   R__1->SetBinError(26,0.1432772);
   R__1->SetBinError(28,1.337901);
   R__1->SetBinError(29,0.2870403);
   R__1->SetBinError(32,0.2530268);
   R__1->SetBinError(33,1.314683);
   R__1->SetBinError(34,0.5767231);
   R__1->SetBinError(36,0.1662417);
   R__1->SetBinError(38,0.4042458);
   R__1->SetBinError(39,0.6007305);
   R__1->SetBinError(40,0.5126363);
   R__1->SetBinError(41,6.12913);
   R__1->SetBinError(42,0.1765261);
   R__1->SetBinError(43,0.4946227);
   R__1->SetBinError(44,0.2085648);
   R__1->SetBinError(45,0.2729855);
   R__1->SetBinError(46,0.4206886);
   R__1->SetBinError(47,0.662433);
   R__1->SetBinError(48,0.5347681);
   R__1->SetBinError(49,0.172379);
   R__1->SetBinError(50,0.2456128);
   R__1->SetBinError(51,0.4307318);
   R__1->SetBinError(52,0.4030004);
   R__1->SetBinError(53,0.243653);
   R__1->SetBinError(54,0.2227683);
   R__1->SetBinError(55,0.1031485);
   R__1->SetBinError(56,0.943595);
   R__1->SetBinError(57,0.4911426);
   R__1->SetBinError(58,0.240977);
   R__1->SetEntries(37.32668);
   
   TPaveStats *ptstats = new TPaveStats(0.78,0.775,0.98,0.935,"brNDC");
   ptstats->SetName("stats");
   ptstats->SetBorderSize(1);
   ptstats->SetFillColor(0);
   ptstats->SetTextAlign(12);
   ptstats->SetTextFont(42);
   TText *AText = ptstats->AddText("R");
   AText->SetTextSize(0.0368);
   AText = ptstats->AddText("Entries = 37     ");
   AText = ptstats->AddText("Mean  =  35.07");
   AText = ptstats->AddText("Std Dev   =  14.27");
   ptstats->SetOptStat(1111);
   ptstats->SetOptFit(0);
   ptstats->Draw();
   R__1->GetListOfFunctions()->Add(ptstats);
   ptstats->SetParent(R__1);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   R__1->SetLineColor(ci);
   R__1->GetXaxis()->SetLabelFont(42);
   R__1->GetXaxis()->SetLabelSize(0.035);
   R__1->GetXaxis()->SetTitleSize(0.035);
   R__1->GetXaxis()->SetTitleFont(42);
   R__1->GetYaxis()->SetLabelFont(42);
   R__1->GetYaxis()->SetLabelSize(0.035);
   R__1->GetYaxis()->SetTitleSize(0.035);
   R__1->GetYaxis()->SetTitleFont(42);
   R__1->GetZaxis()->SetLabelFont(42);
   R__1->GetZaxis()->SetLabelSize(0.035);
   R__1->GetZaxis()->SetTitleSize(0.035);
   R__1->GetZaxis()->SetTitleFont(42);
   R__1->Draw("");
   
   TPaveText *pt = new TPaveText(0.1866724,0.9342996,0.8133276,0.995,"blNDC");
   pt->SetName("title");
   pt->SetBorderSize(0);
   pt->SetFillColor(0);
   pt->SetFillStyle(0);
   pt->SetTextFont(42);
   AText = pt->AddText("met_MT2lester_DZeta01J1D JetsCuts");
   pt->Draw();
   TLine *line = new TLine(0.5804177,1.000453,58.41958,1.000453);
   line->SetLineColor(2);
   line->Draw();
   Canvas_1->Modified();
   Canvas_1->cd();
   Canvas_1->SetSelected(Canvas_1);
   Canvas_1->ToggleToolBar();
}
