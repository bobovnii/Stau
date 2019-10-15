#This file has been autogenerated by openHLT.py
# This is the python configuration file template for openHLT
# It is not intended to run directly with cmsRun
# openHLT.py reads this file and creates the actual configuration file according to the user's requests

import os, sys, ConfigParser, socket
try: import FWCore.ParameterSet.Config as cms
except ImportError:
    print 'Cannot import CMSSW python modules. Did you forget to "cmsenv"?'
    sys.exit()

# print out the OS
try: print file("/etc/redhat-release").read().strip()
except: pass

#Configuration options not meant to be modified from command line (at least for the time being)
outputRAW=False
outputSummary=True
hltProcName="HLT3"

#process.load("setup_cff")
from hlt import *    # module name is chosen by the user and injected here by openHLT.py

#-- Command line options -> configuration code injection and script configuration code -------
interpreter=sys.argv.pop(0)
myname=sys.argv.pop(0)

#####################################################################################
# configuration options injected by openHLT.py, as requested by the user
verbose=False 
isCrabJob=False  
runProducers=False 
runOpen=False 
ifiles=['$inputFileNames'] 
ofile="$outputFileName" 
maxNrEvents=-1

# other code injected by openHLT.py (if any), as requested by the user
# add additional code below

#####################################################################################

if isCrabJob:
    print "using crab specified filename"
    ofile = "OUTPUTFILE"

if ofile == "":   # output file name must be given
    raise Exception("["+myname+"] Output file not given")

if runProducers:
        print "** Will run producers **"
        print "Open mode:", runOpen
else:
        print "** Filter mode **"
# -- Configuration code ends here -----------------------------------------------------------

# drop empty strings
ifiles = filter(None, ifiles)

filePrefex="file:"
if(ifiles[0].find("/store/")==0):
    filePrefex=""
if(ifiles[0].find("/castor/")==0):
    filePrefex="rfio:"
    
process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(),
                            eventsToProcess =cms.untracked.VEventRange()
                            )

print "Input/output file(s):\n--------------"
for i in range(len(ifiles)):
    fname=filePrefex+ifiles[i].strip()
    print "< :",fname
    process.source.fileNames.extend([fname,])
print "> : ("+os.environ["USER"]+"@"+socket.gethostname()+":) "+ofile
print "--------------"
if verbose: print "Maximum number of events:", maxNrEvents

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(maxNrEvents)
)

# enable the TrigReport and TimeReport
process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool( outputSummary ),
    SkipEvent = cms.untracked.vstring('ProductNotFound')
)


if runProducers==False:
    hltProcName=hltProcName+"PB"
    
process.load('Configuration/EventContent/EventContent_cff')
process.output = cms.OutputModule("PoolOutputModule",
                                  splitLevel = cms.untracked.int32(0),
                                  outputCommands =cms.untracked.vstring("drop *",
                                                                        "keep *_TriggerResults_*_*",
                                                                        "keep *_hltTriggerSummaryAOD_*_*"),
                                  
                                  fileName = cms.untracked.string(ofile),
                                  dataset = cms.untracked.PSet(dataTier = cms.untracked.string('HLTDEBUG'),)
                                  )
if outputRAW:
    process.output.outputCommands=cms.untracked.vstring("drop *","keep *_rawDataCollector_*_*","keep *_addPileupInfo_*_*","keep *_TriggerResults_*_*","keep *_hltTriggerSummaryAOD_*_*")

process.HLTOutput_openhlt = cms.EndPath(process.output)
                                                                
#hlt stuff
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(500),
    limit = cms.untracked.int32(10000000)
)

# override the process name
process.setName_(hltProcName)

# En-able HF Noise filters in GRun menu
if 'hltHfreco' in process.__dict__:
    process.hltHfreco.setNoiseFlags = cms.bool( True )

# adapt HLT modules to the correct process name
if 'hltTrigReport' in process.__dict__:
    process.hltTrigReport.HLTriggerResults                    = cms.InputTag( 'TriggerResults', '', hltProcName )

if 'hltPreExpressCosmicsOutputSmart' in process.__dict__:
    process.hltPreExpressCosmicsOutputSmart.TriggerResultsTag = cms.InputTag( 'TriggerResults', '', hltProcName )

if 'hltPreExpressOutputSmart' in process.__dict__:
    process.hltPreExpressOutputSmart.TriggerResultsTag        = cms.InputTag( 'TriggerResults', '', hltProcName )

if 'hltPreDQMForHIOutputSmart' in process.__dict__:
    process.hltPreDQMForHIOutputSmart.TriggerResultsTag       = cms.InputTag( 'TriggerResults', '', hltProcName )

if 'hltPreDQMForPPOutputSmart' in process.__dict__:
    process.hltPreDQMForPPOutputSmart.TriggerResultsTag       = cms.InputTag( 'TriggerResults', '', hltProcName )

if 'hltPreHLTDQMResultsOutputSmart' in process.__dict__:
    process.hltPreHLTDQMResultsOutputSmart.TriggerResultsTag  = cms.InputTag( 'TriggerResults', '', hltProcName )

if 'hltPreHLTDQMOutputSmart' in process.__dict__:
    process.hltPreHLTDQMOutputSmart.TriggerResultsTag         = cms.InputTag( 'TriggerResults', '', hltProcName )

if 'hltPreHLTMONOutputSmart' in process.__dict__:
    process.hltPreHLTMONOutputSmart.TriggerResultsTag         = cms.InputTag( 'TriggerResults', '', hltProcName )

if 'hltDQMHLTScalers' in process.__dict__:
    process.hltDQMHLTScalers.triggerResults                   = cms.InputTag( 'TriggerResults', '', hltProcName )
    process.hltDQMHLTScalers.processname                      = hltProcName

if 'hltDQML1SeedLogicScalers' in process.__dict__:
    process.hltDQML1SeedLogicScalers.processname              = hltProcName

# remove the HLT prescales
if 'PrescaleService' in process.__dict__:
    process.PrescaleService.lvl1DefaultLabel = cms.string( '0' )
    process.PrescaleService.lvl1Labels       = cms.vstring( '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' )
    process.PrescaleService.prescaleTable    = cms.VPSet( )


# override the GlobalTag, connection string and pfnPrefix
if 'GlobalTag' in process.__dict__:
    process.GlobalTag.connect   = 'frontier://FrontierProd/CMS_COND_31X_GLOBALTAG'
    process.GlobalTag.pfnPrefix = cms.untracked.string('frontier://FrontierProd/')

process.GlobalTag.RefreshEachRun = cms.untracked.bool( False )
process.GlobalTag.ReconnectEachRun = cms.untracked.bool( False )

if 'MessageLogger' in process.__dict__:
#    process.MessageLogger.categories.append('TriggerSummaryProducerAOD')
#    process.MessageLogger.categories.append('L1GtTrigReport')
#    process.MessageLogger.categories.append('HLTrigReport')
#    process.MessageLogger.categories.append('FastReport')
    process.MessageLogger.suppressInfo = cms.untracked.vstring('ElectronSeedProducer',"hltL1NonIsoStartUpElectronPixelSeeds","hltL1IsoStartUpElectronPixelSeeds","BasicTrajectoryState")

prodWhiteList=set()
prodWhiteList.add("hltFastPVJetTracksAssociator")
prodWhiteList.add("hltCombinedSecondaryVertex")

# This small loop is adding the producers that
# needs to be re-run for Btagging and Tau paths

for moduleName in process.producerNames().split():
    prod = getattr(process,moduleName)
    if moduleName.endswith("Discriminator"):
        #print moduleName
        prodWhiteList.add(moduleName)
    for paraName in prod.parameters_():
        para = prod.getParameter(paraName)
        if type(para).__name__=="VInputTag":
            if paraName == "tagInfos":
                prodWhiteList=prodWhiteList.union(para.value())
                prodWhiteList.add(moduleName)
        if type(para).__name__=="InputTag":
            if paraName == "jetTracks":
                #print para.getModuleLabel()
                paranew2 = para.getModuleLabel()
                prodWhiteList.add(paranew2)
                #print moduleName
                prodWhiteList.add(moduleName)

#prodWhiteList = uniq(prodWhiteList)
prodTypeWhiteList=[]

pathBlackList=[]
# We don't really care about emulating these triggers..
# these version numbers need to be updated
pathBlackList.append("HLT_BeamHalo_v13")
pathBlackList.append("HLT_IsoTrackHE_v15")
pathBlackList.append("HLT_IsoTrackHB_v14")
pathBlackList.append("DQM_FEDIntegrity_v11")
pathBlackList.append("AlCa_EcalEtaEBonly_v6")
pathBlackList.append("AlCa_EcalEtaEEonly_v6")
pathBlackList.append("AlCa_EcalPi0EBonly_v6")
pathBlackList.append("AlCa_EcalPi0EEonly_v6")


filterBlackList=[]

if runProducers==False:
    for pathName in process.pathNames().split():
        path = getattr(process,pathName)
        for moduleName in path.moduleNames():
            if moduleName in filterBlackList:
                notAllCopiesRemoved=True
                while notAllCopiesRemoved:
                    notAllCopiesRemoved = path.remove(getattr(process,moduleName))

for pathName in process.pathNames().split():
    if pathName in pathBlackList:
        path = getattr(process,pathName)
        for moduleName in path.moduleNames():
            notAllCopiesRemoved=True
            while notAllCopiesRemoved:
                notAllCopiesRemoved = path.remove(getattr(process,moduleName))

if runProducers==False:
    for path in process.pathNames().split():
       # print path
       for producer in process.producerNames().split():
            if producer not in prodWhiteList:
                if getattr(process,producer).type_() not in prodTypeWhiteList:
                    notAllCopiesRemoved=True
                    #print producer
                    while notAllCopiesRemoved:
                        notAllCopiesRemoved = getattr(process,path).remove(getattr(process,producer))

#this removes opperators such as not and ignore from filters
#this is because you cant ignore twice or not ignore
def rmOperatorsFromFilters(process,path):
    if path._seq!=None:
        for obj in path._seq._collection:
            if obj.isOperation():
                moduleName = obj.dumpSequencePython()
                if moduleName.startswith("~"):
                    moduleName = moduleName.lstrip("~process.")
                    module =getattr(process,moduleName)
                    path.replace(obj,module)
                elif moduleName.startswith("cms.ignore"):
                    moduleName = moduleName.lstrip("cms.ignore(process.")
                    moduleName = moduleName.rstrip(")")
                    module = getattr(process,moduleName)
                    path.replace(obj,module)
            if type(obj).__name__=="Sequence":
                rmOperatorsFromFilters(process,obj)

if runOpen:
    for pathName in process.pathNames().split():
        path = getattr(process,pathName)
        rmOperatorsFromFilters(process,path)
        for filterName in path.moduleNames():
            filt = getattr(process,filterName)
            if type(filt).__name__=="EDFilter":
	        if not filt.type_() == "HLTPrescaler":	
                    path.replace(filt,cms.ignore(filt))		       

def cleanList(input,blacklist):
    output = set()
    for x in input:
        if x not in blacklist:
            output.add(x)
    return output

productsToKeep = set()
for pathName in process.pathNames().split():
    path = getattr(process,pathName)
    for filterName in path.moduleNames():
        filt = getattr(process,filterName)
        if type(filt).__name__=="EDFilter":
            for paraName in filt.parameters_():
                para = filt.getParameter(paraName)
                if type(para).__name__=="InputTag":
                    if para.getModuleLabel()!="":
                        productsToKeep.add(para.getModuleLabel())
                if type(para).__name__=="VInputTag":
                    for tag in para:
                        if tag!="":
                            productsToKeep.add(tag)

# This adds all the producers to be kept, just to be safe
# Later on it should be optimized so that only the
# producers we run on should be saved.
for moduleName in process.producerNames().split():
    if moduleName.startswith("hlt"):
        productsToKeep.add(moduleName)
    
productsToKeep = cleanList(productsToKeep,process.filterNames().split())

process.output.outputCommands=cms.untracked.vstring("drop *","keep *_TriggerResults_*_*",
                                                    "keep *_hltTriggerSummaryAOD_*_*")

for product in productsToKeep:
    process.output.outputCommands.append("keep *_"+product+"_*_*")

# version specific customizations
#import os
#cmsswVersion = os.environ['CMSSW_VERSION']

# ---- dump ----
#dump = open('dump.py', 'w')
#dump.write( process.dumpPython() )
#dump.close()

