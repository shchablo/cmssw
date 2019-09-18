import FWCore.ParameterSet.Config as cms
import os

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
                                   useAlgorithm = cms.bool(True), # useIRPCAlgorithm: if true - use iRPC Algorithm;
                                   returnOnlyAND = cms.bool(True), # returnOnlyAND: if true algorithm will return only associated clusters;
                                   thrTimeHR = cms.double(2), # [ns] thrTimeHR: threshold for high radius clustering;
                                   thrTimeLR = cms.double(2), # [ns] thrTimeLR: threshold for low radius clustering;
                                   thrDeltaTimeMin = cms.double(-30), # [ns] thrDeltaTimeMin: min delta time threshold for association clusters between HR and LR;
                                   thrDeltaTimeMax = cms.double(30), # [ns] thrDeltaTimeMax: max delta time threshold for association clusters between HR and LR;
                                   thrDeltaTimeY = cms.double(0.2), # [ns] thrDeltaTimeY: position uncertainty;
                                   signalSpeed = cms.double(19.786302) # [cm/ns] signalSpeed: speed of signal inside strip.
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
#                                   thrDeltaTimeY = cms.double(0.2), # [ns] thrDeltaTimeY: position uncertainty;
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
inputPath = "/eos/cms/store/group/dpg_rpc/comm_rpc/UpgradePhaseII/iRPCClustering/SingleMu/reco_iRPConeRollSingleMu100_200k_v2/190810_102015/0000/"
for input_file in os.listdir(inputPath):
    if "step3" in input_file:
        input_files.append("file:" + os.path.join(inputPath, input_file))
        break
print input_files
process.source = cms.Source('PoolSource',
                  fileNames = input_files
                  #cms.untracked.vstring(
#'file:/eos/cms/store/group/dpg_rpc/comm_rpc/UpgradePhaseII/iRPCClustering/SingleMu/reco_iRPConeRollSingleMu100_200k_v2/190810_102015/0000/step3_*.root'
#'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Sandbox/mileva/testiRPCGeo/SingleMu/digi_iRPConeRollSingleMu100/190801_184558/0000/step2_1.root'
#'file:/eos/cms/store/group/dpg_dt/comm_dt/TriggerSimulation/SamplesReco/SingleMu_FlatPt-2to100/Version_10_5_0/SimRECO_1.root'

#)
                 ) 
# Output
process.out = cms.OutputModule("PoolOutputModule",
                fileName = cms.untracked.string('MC_RPC_RecHit.root'),
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
