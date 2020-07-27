import FWCore.ParameterSet.Config as cms
import os
from optparse import OptionParser
import sys
import imp
parser = OptionParser()
parser.usage = "%prog <file> : expand this python configuration"

(options,args) = parser.parse_args()

inDeltaTimeY = 1
inThrHR = 1
inThrLR = 1
comp = 1
comp = args[1]
inDeltaTimeY = args[2]
inThrHR = args[3]
inThrLR = args[4]

process = cms.Process('ReDoRecHit')

#process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
#process.GlobalTag.globaltag = "106X_upgrade2018_realistic_v5"  #4"
##should be v5 and not v4
##in order to avoid, load it using aliases :

from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase2_realistic', '')


process.maxEvents = cms.untracked.PSet(
                        input = cms.untracked.int32(-1),
                    )
#process.load('Configuration.Geometry.GeometryExtended2023D38Reco_cff')
#process.load('Configuration.Geometry.GeometryExtended2023D38_cff')
process.load('Configuration.Geometry.GeometryExtended2023D41Reco_cff')
process.load('Configuration.Geometry.GeometryExtended2023D41_cff')


#from RecoLocalMuon.RPCRecHit.rpcRecHits_cfi import rpcRecHits

process.irpcRecHits = cms.EDProducer("RPCRecHitProducer",
               recAlgoConfig = cms.PSet(
                                 iRPCConfig = cms.PSet( # iRPC Algorithm
                                   useAlgorithm = cms.bool(bool(comp)), # useIRPCAlgorithm: if true - use iRPC Algorithm;
                                   returnOnlyAND = cms.bool(True), # returnOnlyAND: if true algorithm will return only associated clusters;
                                   returnOnlyHR = cms.bool(False), # returnOnlyHR: if true algorithm will return only HR clusters;
                                   returnOnlyLR = cms.bool(False), # returnOnlyLR: if true algorithm will return only LR clusters;
                                   thrTimeHR = cms.double(float(inThrHR)), # [ns] thrTimeHR: threshold for high radius clustering;
                                   thrTimeLR = cms.double(float(inThrLR)), # [ns] thrTimeLR: threshold for low radius clustering;
                                   thrDeltaTimeMin = cms.double(-200), # [ns] thrDeltaTimeMin: min delta time threshold for association clusters between HR and LR;
                                   thrDeltaTimeMax = cms.double(200), # [ns] thrDeltaTimeMax: max delta time threshold for association clusters between HR and LR;
                                   thrDeltaTimeY = cms.double(float(inDeltaTimeY)), # [ns] thrDeltaTimeY: position uncertainty;
                                   signalSpeed = cms.double(19.786302) # [cm/ns] signalSpeed: speed of signal inside strip.

                                   #useAlgorithm = cms.bool(True), # useIRPCAlgorithm: if true - use iRPC Algorithm;
                                   #returnOnlyAND = cms.bool(True), # returnOnlyAND: if true algorithm will return only associated clusters;
                                   #thrTimeHR = cms.double(2), # [ns] thrTimeHR: threshold for high radius clustering;
                                   #thrTimeLR = cms.double(2), # [ns] thrTimeLR: threshold for low radius clustering;
                                   #thrDeltaTimeMin = cms.double(-30), # [ns] thrDeltaTimeMin: min delta time threshold for association clusters between HR and LR;
                                   #thrDeltaTimeMax = cms.double(30), # [ns] thrDeltaTimeMax: max delta time threshold for association clusters between HR and LR;
                                   #signalSpeed = cms.double(19.786302) # [cm/ns] signalSpeed: speed of signal inside strip.
                                 ),
                               ),
               recAlgo = cms.string('RPCRecHitStandardAlgo'),
               rpcDigiLabel = cms.InputTag('simMuonRPCDigis'),
               maskSource = cms.string('File'),
               maskvecfile = cms.FileInPath('RecoLocalMuon/RPCRecHit/data/RPCMaskVec.dat'),
               deadSource = cms.string('File'),
               deadvecfile = cms.FileInPath('RecoLocalMuon/RPCRecHit/data/RPCDeadVec.dat')
             )

#No need to redo the normal recHits since they are already in the input rootfile
#process.rpcRecHits = cms.EDProducer("RPCRecHitProducer",
#               recAlgoConfig = cms.PSet(
#                                 iRPCConfig = cms.PSet( # iRPC Algorithm
#                                   useAlgorithm = cms.bool(False), # useIRPCAlgorithm: if true - use iRPC Algorithm;
#                                   returnOnlyAND = cms.bool(True), # returnOnlyAND: if true algorithm will return only associated clusters;
#                                   thrTimeHR = cms.double(2), # [ns] thrTimeHR: threshold for high radius clustering;
#                                   thrTimeLR = cms.double(2), # [ns] thrTimeLR: threshold for low radius clustering;
#                                   thrDeltaTimeMin = cms.double(-30), # [ns] thrDeltaTimeMin: min delta time threshold for association clusters between HR and LR;
#                                   thrDeltaTimeMax = cms.double(30), # [ns] thrDeltaTimeMax: max delta time threshold for association clusters between HR and LR;
#                                   signalSpeed = cms.double(19.786302) # [cm/ns] signalSpeed: speed of signal inside strip.
#                                 ),
#                               ),
#               recAlgo = cms.string('RPCRecHitStandardAlgo'),
#               rpcDigiLabel = cms.InputTag('simMuonRPCDigis'),
#               maskSource = cms.string('File'),
#               maskvecfile = cms.FileInPath('RecoLocalMuon/RPCRecHit/data/RPCMaskVec.dat'),
#               deadSource = cms.string('File'),
#               deadvecfile = cms.FileInPath('RecoLocalMuon/RPCRecHit/data/RPCDeadVec.dat')
#             )

# Input source
input_files = cms.untracked.vstring()
#inputPath = "/eos/cms/store/group/dpg_rpc/comm_rpc/UpgradePhaseII/iRPCClustering/SingleMu/reco_iRPConeRollSingleMu100_200k_v2/190810_102015/0000/"

inputPath = "/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/0PU/0000/"
#inputPath = "/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/"
for input_file in os.listdir(inputPath):
    if "_" in input_file:
        input_files.append("file:" + os.path.join(inputPath, input_file))
print input_files
process.source = cms.Source('PoolSource',
                fileNames = input_files
                #fileNames = cms.untracked.vstring(
#'file:/afs/cern.ch/user/b/brfranco/work/public/RPC/apply_Reco_On_L1T_TDR_Samples_v2/CMSSW_10_6_0_pre4/src/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco.root'
#'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples/PhaseIITDRSpring19DR-PU200_106X_upgrade2023_realistic_v3/Mu_FlatPt2to100-pythia8-gun_withLocalMuonReco/0000/Mu_FlatPt2to100-pythia8-gun_PhaseIITDRSpring19DR-PU200_106X_upgrade2023_realistic_v3_withLocalMuonReco_100.root'
#'file:/eos/cms/store/group/dpg_rpc/comm_rpc/UpgradePhaseII/iRPCClustering/SingleMu/reco_iRPConeRollSingleMu100_200k_v2/190810_102015/0000/step3_*.root'
#'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Sandbox/mileva/testiRPCGeo/SingleMu/digi_iRPConeRollSingleMu100/190801_184558/0000/step2_1.root'
#'file:/eos/cms/store/group/dpg_dt/comm_dt/TriggerSimulation/SamplesReco/SingleMu_FlatPt-2to100/Version_10_5_0/SimRECO_1.root'

#)
                 )
# Output
process.out = cms.OutputModule("PoolOutputModule",
                fileName =
                cms.untracked.string('/afs/cern.ch/work/k/kshchabl/public/residual/MC_RPC_RecHit' + '.root'),
                outputCommands = cms.untracked.vstring(
                    'drop *_*_*_*',
                    'keep *_standAloneMuons_*_*',
                    'keep *RPC*_*_*_*',
                    'keep *_csc*_*_*',
                    'keep *_dt*_*_*',
                    'keep *_gem*_*_*',
                    'keep *_me0*_*_*'
)
              )

#process.p = cms.Path(process.rpcRecHits + process.irpcRecHits)
process.p = cms.Path(process.irpcRecHits)
process.e = cms.EndPath(process.out)
