###################################################################################################################
#
# This is a datacard producer 
# Author: Valeria Botta
#
# Please read the instructions in instructions.txt before running it
#
# Arguments ( MUST be in THIS order )
# 1. channel (et, mt)
# 2. json with selection cuts
# 3. json with weights
# 4. json with hitogram settings
# 5. mode ("histos" or "datacard")
# 6. name of the systematic (optional, only in datacard mode)
#
# ---- IMPORTANT -----
# - Please adjust the total lumi (1/pb) and the input directory
# - Stick to conventional names for root files, as in "define file lists" section
#
# ---- HOW TO RUN -----
#
# -- histos mode
# > python datacard_producer_htt2016.py mt cuts_mt.json weights.json histos.json histos
#
# -- datacard mode, without syst variations
# > python datacard_producer_htt2016.py mt cuts_mt.json weights.json histos.json datacard 
#
# -- datacard mode, with syst variation (additional argument is the suffix of the syst variation)
# > python datacard_producer_htt2016.py mt cuts_mt.json weights.json histos.json datacard _CMS_scale_t_13TeVUp
# (run many times with all syst variations you want to include, then you can hadd the root files)
#
#################################################################################################################

import os, sys, json, ROOT
from ROOT import TFile, TH1F, TH1D, TCanvas, TPad, gStyle, kRed, kBlack, TLine, TLegend, TROOT, TChain

def makeDatacard_ssos(treeName, files, cut, weight, histo, histo_ss):
	print "opposite sign"
	makeDatacard_category(treeName, files, cut, "q_1 * q_2 < 0.", weight, "_os", histo)
	print "same sign"
	makeDatacard_category(treeName, files, cut, "q_1 * q_2 > 0.", weight, "_ss", histo_ss)

def makeDatacard_sample_inc(treeName, sampleKey,sampleDict, catName, weight, cut, additionalCut):
	hsample = TH1F(sampleKey+catName, sampleKey+catName, nbins, xmin, xmax)
	for fileName in sampleDict["files"]:
		print "file name", fileName
		f=TFile(indir+"/"+fileName+".root", "read")
		#t = f.Get("TauCheck")
		t = f.Get(treeName)
		h = TH1F(fileName+catName, fileName+catName, nbins, xmin, xmax)
		t.Draw(var+" >> "+fileName+catName, weight + " * ("+ cut + " && "+additionalCut+")")
		xs = sampleDict["xsec"]
		hevents=f.Get("nWeightedEvents")
		nevents=hevents.GetBinContent(1)
		if (nevents == 0):
			print "nevents : ", nevents
		h.Scale(xs*lumi/nevents)
		hsample.Add(h)
	return hsample

def makeDatacard_sample_bin(treeName, sampleKey, sampleDict, catName, weight, cut, additionalCut):
	t = TChain(treeName)
	for fileName in sampleDict["files"]:
		t.Add(indir+"/"+fileName+".root")
	h = TH1F(sampleKey+catName, sampleKey+catName, nbins, xmin, xmax)
	binString = makeBinString(sampleDict["weights"],sampleDict["binVar"])
	
	print "Drawing Entries : ", t.Draw(var+" >> "+sampleKey+catName, binString + " * "+ weight + " * ("+ cut + " && "+additionalCut+")")
	h.Scale(lumi)
	return h

def makeDatacard_category(treeName, catDict, cut, additionalCut, weight, catName, ohisto): #ohisto is output histogram for that category
	for sampleKey,sampleDict in catDict.iteritems():
		if (isBinned(sampleDict)):
			h = makeDatacard_sample_bin(treeName, sampleKey, sampleDict, catName, weight, cut, additionalCut)
		else: 
			h = makeDatacard_sample_inc(treeName, sampleKey, sampleDict, catName, weight, cut, additionalCut)
		ohisto.Add(h)


def isBinned(sampleDict):
		if 'weights' in sampleDict:
			return True 
		else:
			return False

def makeBinString(weightsDict, binVar):
	htwstr = "("
	for htwkey, htwvalue in weightsDict.iteritems():
		htwstr += "( "+ binVar +">="+ str(htwkey[0])
		htwstr += " && "+ binVar+ " < " + str(htwkey[1]) + ")"
		htwstr += " * " + str(htwvalue) + " + "
	htwstr = htwstr[0:-2]
	htwstr += ")"
	return htwstr


##########################
# read arguments 
##########################

channel = sys.argv[1] #et, mt
cuts = json.loads(open(sys.argv[2]).read())
weighting = json.loads(open(sys.argv[3]).read())
histos = json.loads(open(sys.argv[4]).read())
mode = sys.argv[5]

systName = ""
if mode == "datacard":
	if len(sys.argv)>6:
		systName = sys.argv[6] #name of the tree with the systematic

#eg:
#"_CMS_scale_t_13TeVUp"
#"_topPtWeightUp"


##########################################
# adjust lumi and input directory  here  #
##########################################
lumi = 12900
#lumi = 6260
#lumi = 3990
#directory with input root files (synch ntuples) with conventional names!

if channel == "mt":
	indir = "/nfs/dust/cms/user/bottav/CMSSW_8_0_12/src/DesyTauAnalyses/NTupleMaker/test/NTuple_10Oct/mutau/final"
	indirData = indir

if channel == "et":
	indir = "/nfs/dust/cms/user/bottav/CMSSW_8_0_12/src/DesyTauAnalyses/NTupleMaker/test/NTuple_10Oct/etau/final"
	indirData = indir

isBlinded = True 

ROOT.TH1.SetDefaultSumw2(True)

##########################
# get data tree 
##########################

treeName = "TauCheck"

if channel == "mt":
	fdata =  TFile(indirData+"/"+"SingleMuon_Run2016BCD.root","read")
if channel == "et":
	fdata = TFile(indirData+"/"+"SingleElectron_Run2016BCD.root","read")

tdata = fdata.Get(treeName)


#redefine tree name for MC
treeName = "TauCheck"+systName

##########################
# define file lists
##########################


lvv={"ST_t-channel_top_4f_leptonDecays" : {"xsec": 136.95*3*0.108, "isBinned": False, "binVar": "", "files": ["ST_t-channel_top_4f_leptonDecays"]},
     "ST_t-channel_antitop_4f_leptonDecays" : {"xsec": 80.95*3*0.108, "isBinned": False, "binVar": "", "files": ["ST_t-channel_antitop_4f_leptonDecays"]},
     "ST_tW_antitop_5f_inclusiveDecays" : {"xsec": 35.6, "isBinned": False, "binVar": "", "files": ["ST_tW_antitop_5f_inclusiveDecays"]},
     "ST_tW_top_5f_inclusiveDecays" : {"xsec": 35.6, "isBinned": False, "binVar": "", "files": ["ST_tW_top_5f_inclusiveDecays"]},
     "ZZTo2L2Q" : {"xsec": 3.22, "isBinned": False, "binVar": "", "files": ["ZZTo2L2Q"]},
     "ZZTo4L" : {"xsec": 1.212, "isBinned": False, "binVar": "", "files": ["ZZTo4L"]},
     "WZTo1L3Nu" : {"xsec": 3.05, "isBinned": False, "binVar": "", "files": ["WZTo1L3Nu"]},
     #"WZJTo3L1Nu" : {"xsec": 4.708, "isBinned": False, "binVar": "", "files": ["WZJTo3L1Nu"]},
     "WWTo1L1Nu2Q" : {"xsec": 49.997, "isBinned": False, "binVar": "", "files": ["WWTo1L1Nu2Q"]},
     "WZTo1L1Nu2Q" : {"xsec": 10.71, "isBinned": False, "binVar": "", "files": ["WZTo1L1Nu2Q"]},
     "VVTo2L2Nu" : {"xsec": 11.95, "isBinned": False, "binVar": "", "files": ["VVTo2L2Nu"]},
     "WZTo2L2Q" : {"xsec": 5.595, "isBinned": False, "binVar": "", "files": ["WZTo2L2Q"]}
}

ltt = {"TTPowHeg": {"xsec": 831.76, "files": ["TTpowheg"]}}

ldy = {"DYJetsToLL_M-50": { "isBinned": True, "binVar": "gen_noutgoing", 
                            "files": ["DYJetsToLL_M-50_MG",
                                      "DY1JetsToLL_M-50_MG",
                                      "DY2JetsToLL_M-50_MG",
                                      "DY3JetsToLL_M-50_MG",
                                      "DY4JetsToLL_M-50_MG"], 
                            "weights":{ (-0.5, 0.5) : 0.000118442,
                                        (0.5, 1.5) :  0.000015831,
                                        (1.5, 2.5) :  0.0000169775,
                                        (2.5, 3.5) :  0.0000169091,
                                        (3.5, 100.5) : 0.0000131252
                              }
                    },
       "DYJetsToLL_M-10to50" : {"xsec": 18610, "isBinned": False, "binVar": "", "files": ["DYJetsToLL_M-10to50_MG"]}
}

lw = {"WJetsToLNu_MG": {"isBinned": True, "binVar": "gen_noutgoing", 
                              "files": ["WJetsToLNu_MG",
                                        "W1JetsToLNu_MG",
                                        "W2JetsToLNu_MG",
                                        "W3JetsToLNu_MG",
                                        "W4JetsToLNu_MG"], 
                              "weights":{ (-0.5, 0.5) : 0.00241933,
                                          (0.5, 1.5) : 0.000258246, 
                                          (1.5, 2.5) : 0.000120034,
                                          (2.5, 3.5) : 0.000056226,
                                          (3.5, 100.5) : 0.000067417
                                  }
                   }
}

#Higgs to tau tau samples
lvbfhtt = {"VBFHTT_M125" : {"xsec": 0.22692, "isBinned": False, "binVar": "", "files": ["VBFHToTauTau_M125"]}}

lwhtt = {"WmHTT_M125" : {"xsec": 0.0504, "isBinned": False, "binVar": "", "files": ["WminusHToTauTau_M125"]},
         "WpHTT_M125" : {"xsec": 0.031968, "isBinned": False, "binVar": "", "files": ["WplusHToTauTau_M125"]} 
}

lzhtt = {"ZHTT_M125" : {"xsec": 0.053034, "isBinned": False, "binVar": "", "files": ["ZHToTauTau_M125"]}}

lgghtt = {"GluGluHTT_M125" : {"xsec": 2.9148, "isBinned": False, "binVar": "", "files": ["GluGluHToTauTau_M125"]}}


if mode == "datacard": 
	for hkey, hvalue in histos.iteritems():
		for wkey, wvalue in weighting.iteritems():
			filename = wkey+"_htt_"+channel+".inputs-sm-13TeV-"+hkey+systName+".root"
			#remove old files with same name as the one that will be created, if any
			os.system("if [ -f "+filename+" ]; then rm "+filename+";fi")

for wkey, wvalue in weighting.iteritems():
	print "weight : ", wkey
	if mode == "histos":
		suffix = os.path.splitext(os.path.basename(sys.argv[4]))[0] #add name of the (histos)json file as suffix
		outfile=TFile( wkey+"_htt_"+channel+".inputs-sm-13TeV-histos-"+suffix+".root", "recreate")	

	for ckey, cvalue in cuts.iteritems():
		print "cut : ", ckey

		#########################################
		# relaxed selection for some categories #
		#########################################
		applyRelaxedSel = False
		print "ckey : ", ckey
		if ckey in ["1jet_high","vbf_low","vbf_high"]:
			applyRelaxedSel = True
			relaxedSel = "njets==2 && mjj>500 && (mjj<800 || pt_sv<100) && mt_1<50 && pt_1>23 && pt_2>20 && iso_1 < 0.15 && byTightIsolationMVArun2v1DBoldDMwLT_2>0.5 && againstElectronVLooseMVA6_2>0.5 && againstMuonTight3_2>0.5  && dilepton_veto == 0 && extraelec_veto == 0 && extramuon_veto == 0 "
		print " applyRelaxedSel : " , applyRelaxedSel
 
		for hkey, hvalue in histos.iteritems():

			var = hvalue["var"]	
			nbins = hvalue["nbins"]
			xmin = hvalue["xmin"]
			xmax = hvalue["xmax"]
			xtit = hvalue["xtit"]
			ytit = hvalue["ytit"]

			print "plotting ", hkey, ": var ", var, " | bins ", nbins, " | xmin ", xmin, " | xmax ", xmax
			#build string for axis titles
			axisTit = ";"+xtit+";"+ytit

			# data
			hdata = TH1F("hdata", "hdata"+axisTit, nbins, xmin, xmax)
			hdata_ss = TH1F("hdata_ss", "hdata_ss"+axisTit,  nbins, xmin, xmax) 
			tdata.Draw(var+" >> hdata_ss", cvalue + " && q_1 * q_2 > 0.")
			if (applyRelaxedSel):
				hdata_rel = TH1F("hdata_rel", "hdata_rel"+axisTit, nbins, xmin, xmax)
				hdata_rel_ss = TH1F("hdata_rel_ss", "hdata_rel_ss"+axisTit, nbins, xmin, xmax)
				tdata.Draw(var+" >> hdata_rel",  relaxedSel + " && q_1 * q_2 < 0. ")
				tdata.Draw(var+" >> hdata_rel_ss",  relaxedSel + " && q_1 * q_2 > 0. ")

			#insert blinding to update cvalue in OS region
			blinding = {
			"m_vis":"(m_vis<60 || m_vis>120)", 
			"m_sv":"(m_sv<100 || m_sv>150)",
			"mt_tot":"(mt_tot<80 || mt_tot>130)"
			}
			if (isBlinded and (var in blinding) and ("inclusive" not in ckey)):
				blindingCut = blinding[var]
				print "applying blinding for " , var , " : " , blindingCut
			else:
				blindingCut = " 1 "

			print "cut data: ", cvalue, "&&" , blindingCut , "&& q_1 * q_2 < 0."
			print " Data OS: ", tdata.Draw(var+" >> hdata", cvalue + "&&" + blindingCut + "&& q_1 * q_2 < 0.")
			print "Data, done"

			#HTT section (signal)
			#hHtt = TH1F("HTT", "HTT"+axisTit, nbins, xmin, xmax)
			#hHtt.SetDirectory(0)
			#makeDatacard_category(treeName, lhtt, cvalue, "q_1 * q_2 < 0.", wvalue, "_os", hHtt)
			#no need for SS for signal

			### SIGNAL ###

			#ggH
			hggHtt = TH1F("ggH125", "ggH125"+axisTit, nbins, xmin, xmax)
			hggHtt.SetDirectory(0)
			makeDatacard_category(treeName, lgghtt, cvalue, "q_1 * q_2 < 0.", wvalue, "_os", hggHtt)
			#VBF 
			hvbfHtt = TH1F("qqH125", "qqH125"+axisTit, nbins, xmin, xmax)
			hvbfHtt.SetDirectory(0)
			makeDatacard_category(treeName, lvbfhtt, cvalue, "q_1 * q_2 < 0.", wvalue, "_os", hvbfHtt)
			#WH
			hWHtt = TH1F("WH125", "WH125"+axisTit, nbins, xmin, xmax)
			hWHtt.SetDirectory(0)
			makeDatacard_category(treeName, lwhtt, cvalue, "q_1 * q_2 < 0.", wvalue, "_os", hWHtt)
			#ZH
			hZHtt = TH1F("ZH125", "ZH125"+axisTit, nbins, xmin, xmax)
			hZHtt.SetDirectory(0)
			makeDatacard_category(treeName, lzhtt, cvalue, "q_1 * q_2 < 0.", wvalue, "_os", hZHtt)
			#no need for SS for signal

			### BACKGROUNDS ###

			#VV section
			hvv = TH1F("VV", "VV"+axisTit, nbins, xmin, xmax)
			hvv_ss = TH1F("VV_ss", "VV_ss"+axisTit, nbins, xmin, xmax)
			hvv.SetDirectory(0)
			hvv_ss.SetDirectory(0)
			makeDatacard_ssos(treeName, lvv, cvalue, wvalue, hvv, hvv_ss)
			if (applyRelaxedSel):
				hvv_rel = TH1F("VV_rel", "VV_rel"+axisTit, nbins, xmin, xmax)
				hvv_rel_ss = TH1F("VV_rel_ss", "VV_rel_ss"+axisTit, nbins, xmin, xmax)
				makeDatacard_ssos(treeName, lvv, relaxedSel, wvalue, hvv_rel, hvv_rel_ss)
			print "VV, done"
			#TT section
			htt = TH1F("TT", "TT"+axisTit, nbins, xmin, xmax)
			htt_ss = TH1F("TT_ss", "TT_ss"+axisTit, nbins, xmin, xmax)
			htt.SetDirectory(0)
			htt_ss.SetDirectory(0)
			makeDatacard_ssos(treeName, ltt, cvalue, wvalue, htt, htt_ss)
			if (applyRelaxedSel):
				htt_rel = TH1F("TT_rel", "TT_rel"+axisTit, nbins, xmin, xmax)
				htt_rel_ss = TH1F("TT_rel_ss", "TT_rel_ss"+axisTit, nbins, xmin, xmax)
				makeDatacard_ssos(treeName, ltt, relaxedSel, wvalue, htt_rel, htt_rel_ss)
			print "TT, done"
			#Wjets
			hw =  TH1F("W", "W"+axisTit,nbins, xmin, xmax)
			hw_ss =  TH1F("W_ss","W_ss"+axisTit,nbins, xmin, xmax) 
			hw.SetDirectory(0)
			hw_ss.SetDirectory(0)
			makeDatacard_ssos(treeName, lw, cvalue, wvalue, hw, hw_ss)
			if (applyRelaxedSel):
				hw_rel = TH1F("W_rel", "W_rel"+axisTit, nbins, xmin, xmax)
				hw_rel_ss = TH1F("W_rel_ss", "W_rel_ss"+axisTit, nbins, xmin, xmax)
				makeDatacard_ssos(treeName, lw, relaxedSel, wvalue, hw_rel, hw_rel_ss)

			#DY
			hdy = TH1F("DY", "DY"+axisTit, nbins, xmin, xmax)
			hdy_ss = TH1F("DY_ss", "DY_ss"+axisTit, nbins, xmin, xmax) 
			hdy.SetDirectory(0)
			hdy_ss.SetDirectory(0)
			makeDatacard_ssos(treeName, ldy, cvalue, wvalue, hdy, hdy_ss)
			if (applyRelaxedSel):
				hdy_rel = TH1F("DY_rel", "DY_rel"+axisTit, nbins, xmin, xmax)
				hdy_rel_ss = TH1F("DY_rel_ss", "DY_rel_ss"+axisTit, nbins, xmin, xmax)
				makeDatacard_ssos(treeName, ldy, relaxedSel, wvalue, hdy_rel, hdy_rel_ss)

			#ZTT , ZL, ZJ, ZLL 
			hztt = TH1F("ztt", "ztt"+axisTit, nbins, xmin, xmax)
			hzl = TH1F("zl", "zl"+axisTit, nbins, xmin, xmax)
			hzj = TH1F("zj", "zj"+axisTit, nbins, xmin, xmax)
			hzll = TH1F("zll", "zll"+axisTit, nbins, xmin, xmax)
			
			hztt.SetDirectory(0)
			hzl.SetDirectory(0)		
			hzj.SetDirectory(0)		
			hzll.SetDirectory(0)

			#looping on different cuts that define categories 
			#for now, done by hand here with additional cut string
			ZTTcut = "gen_match_2 == 5 && q_1 * q_2 < 0." 
			ZLcut = "gen_match_2 < 5 && q_1 * q_2 < 0." 
			ZJcut = "gen_match_2 == 6 && q_1 * q_2 < 0." 
			ZLLcut = "gen_match_2 != 5 && q_1 * q_2 < 0."

			makeDatacard_category(treeName, ldy, cvalue, ZTTcut, wvalue, "ZTT", hztt)
			makeDatacard_category(treeName, ldy, cvalue, ZLcut, wvalue, "ZL", hzl)
			makeDatacard_category(treeName, ldy, cvalue, ZJcut, wvalue, "ZJ", hzj)
			makeDatacard_category(treeName, ldy, cvalue, ZLLcut, wvalue, "ZLL", hzll)

			print "DY all done"
			##########################
			# QCD estimate
			##########################

			print "qcd calculation..."
			
			SSOSratio = 1.06
		
			#histograms to put the *total* MC yield 
			hMC = TH1F("hMC", "hMC"+axisTit, nbins, xmin, xmax)
			hMC_ss = TH1F("hMC_ss", "hMC_ss"+axisTit, nbins, xmin, xmax)
		
			hMC.Add(hvv)
			hMC.Add(htt)
			hMC.Add(hdy)
			hMC.Add(hw)

			hMC_ss.Add(hvv_ss)
			hMC_ss.Add(htt_ss)		
			hMC_ss.Add(hdy_ss)
			hMC_ss.Add(hw_ss)

			hqcd = TH1F("QCD", "QCD"+axisTit, nbins, xmin, xmax)
			hqcd_ss = TH1F("QCD_ss", "QCD_ss"+axisTit, nbins, xmin, xmax)

			hqcd_ss.Add(hdata_ss)
			hqcd_ss.Add(hMC_ss,-1)
			hqcd.Add(hqcd_ss, SSOSratio)

			print " ... done"

			if (applyRelaxedSel): 
				print "QCD relaxed selection"
				#hqcd_rel = TH1F("QCD_rel", "QCD_rel"+axisTit, nbins, xmin, xmax)
				hqcd_rel_ss = TH1F("QCD_rel_ss", "QCD_rel_ss"+axisTit, nbins, xmin, xmax)
				hqcd_rel_ss.Add(hdata_rel_ss)
				hqcd_rel_ss.Add(hvv_rel_ss,-1)
				hqcd_rel_ss.Add(htt_rel_ss,-1)
				hqcd_rel_ss.Add(hdy_rel_ss,-1)
				hqcd_rel_ss.Add(hw_rel_ss,-1)
				norm_nom = hqcd_ss.Integral()
				norm_rel = hqcd_rel_ss.Integral()
				hqcd_rel_ss.Scale(SSOSratio*norm_nom/norm_rel)
				hqcd = hqcd_rel_ss

			#Wjets from control region 
			if (applyRelaxedSel):
				print "Wjets relaxed selection "
				norm_nom = hw.Integral()
				norm_rel = hw_rel.Integral()
				hw_rel.Scale(norm_nom/norm_rel)
				hw = hw_rel 




			##########################
			#writing to output file
			##########################

			
			if mode == "datacard":
				outfile=TFile( wkey+"_htt_"+channel+".inputs-sm-13TeV-"+hkey+systName+".root", "update")

				outfile.cd()
				outfile.mkdir(channel+"_"+ckey)
				outfile.cd(channel+"_"+ckey)

			elif mode == "histos":
				outfile.cd()
				outfile.mkdir(channel+"_"+ckey+"/"+hkey)
				outfile.cd(channel+"_"+ckey+"/"+hkey)

			print "writing to out file..."


			if (mode == "histos"):
				hdata.Write("data_obs")

			#write data histogram only once, in the file w/o syst variations
			if (mode == "datacards" and systName == ""): 
				hdata.Write("data_obs")

			hggHtt.Write("ggH125"+systName)
			hvbfHtt.Write("qqH125"+systName)
			hWHtt.Write("WH125"+systName)
			hZHtt.Write("ZH125"+systName)

			hztt.Write("ZTT"+systName)
			hzl.Write("ZL"+systName)
			hzj.Write("ZJ"+systName)
			hzll.Write("ZLL"+systName)
			htt.Write("TT"+systName)
			hw.Write("W"+systName)
			hvv.Write("VV"+systName)
			hqcd.Write("QCD"+systName)
			print "... done"

			if mode == "datacard":
				outfile.Close()

	if mode == "histos":
		outfile.Close()


