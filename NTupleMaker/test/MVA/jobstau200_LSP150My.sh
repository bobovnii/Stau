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

cd /nfs/dust/cms/user/bobovnii/new/CMSSW_8_0_20/src/DesyTauAnalyses/NTupleMaker/test/MVA/ ;  eval `scramv1 runtime -sh` ;

root -l -b -q 'myTMVA.C("","TMVA_stau200_LSP150My.root","mutau_stau200_LSP150My",0.02181,"stau200_LSP150My")' > logTMVA_stau200_LSP150My
