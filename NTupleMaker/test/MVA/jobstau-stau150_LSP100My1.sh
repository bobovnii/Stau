#!/bin/sh
#
#(make sure the right shell will be used)
#$ -S /bin/sh
#
#(the cpu time for this job)
#$ -l h_cpu=61:10:00
#
#(the maximum memory usage of this job)
#$ -l h_vmem=20000M
#
#(use hh site)
#$ -l site=hh
#(stderr and stdout are merged together to stdout)
#$ -j y
#
# use SL5
#$ -l os=sld6
#
# use current dir and current environment
#$ -cwd
#$ -V
#

cd /nfs/dust/cms/user/bobovnii/MVAstau/CMSSW_9_4_0_patch1/src/DesyTauAnalyses/NTupleMaker/test/MVA ;  eval `scramv1 runtime -sh` ;

root -l -b -q 'myTMVA.C("","TMVAeltau_stau-stau150_LSP100My_{1}2s0j.root","eltau_stau-stau150_LSP100My",0.06334,"stau-stau150_LSP100My","!H:!V:NTrees=850:MinNodeSize=0.4%:MaxDepth=3:BoostType=RealAdaBoost:SeparationType=GiniIndex:nCuts=35")' > logTMVA_stau-stau150_LSP100My1
