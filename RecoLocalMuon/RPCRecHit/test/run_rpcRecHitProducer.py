import FWCore.ParameterSet.Config as cms

process = cms.Process('ReDoRecHit')

process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = "93X_upgrade2023_realistic_v5"

process.maxEvents = cms.untracked.PSet(
                        input = cms.untracked.int32(300),
                    )

from RecoLocalMuon.RPCRecHit.rpcRecHits_cfi import rpcRecHits

rpcRecHits = cms.EDProducer("RPCRecHitProducer",
               recAlgoConfig = cms.PSet(
                                 iRPCConfig = cms.PSet( # iRPC Algorithm
                                   useAlgorithm = cms.bool(True), # useIRPCAlgorithm: if true - use iRPC Algorithm;
                                   returnOnlyAND = cms.bool(True), # returnOnlyAND: if true algorithm will return only associated clusters;
                                   thrTimeHR = cms.double(2), # [ns] thrTimeHR: threshold for high radius clustering;
                                   thrTimeLR = cms.double(2), # [ns] thrTimeLR: threshold for low radius clustering;
                                   thrDeltaTimeMin = cms.double(-30), # [ns] thrDeltaTimeMin: min delta time threshold for association clusters between HR and LR;
                                   thrDeltaTimeMax = cms.double(30), # [ns] thrDeltaTimeMax: max delta time threshold for association clusters between HR and LR;
                                   signalSpeed = cms.double(19.786302) # [cm/ns] signalSpeed: speed of signal inside strip.
                                 ),
                               ),
               recAlgo = cms.string('RPCRecHitStandardAlgo'),
               rpcDigiLabel = cms.InputTag('muonRPCDigis'),
               maskSource = cms.string('File'),
               maskvecfile = cms.FileInPath('RecoLocalMuon/RPCRecHit/data/RPCMaskVec.dat'),
               deadSource = cms.string('File'),
               deadvecfile = cms.FileInPath('RecoLocalMuon/RPCRecHit/data/RPCDeadVec.dat')
             )

process.rpcRecHits = rpcRecHits
process.rpcRecHits.rpcDigiLabel = cms.InputTag('simMuonRPCDigis')

# Input source
process.source = cms.Source('PoolSource',
                  fileNames = cms.untracked.vstring('file:/eos/cms/store/group/dpg_dt/comm_dt/TriggerSimulation/SamplesReco/SingleMu_FlatPt-2to100/Version_10_5_0/SimRECO_1.root')
                 )

# Output
process.out = cms.OutputModule("PoolOutputModule",
                fileName = cms.untracked.string('MC_RPC_RecHit.root'),
                outputCommands = cms.untracked.vstring('drop *',
                "keep *_*_*_ReDoRecHit*",
                "keep *RPC*_*_*_*")
              )

process.p = cms.Path(process.rpcRecHits)
process.e = cms.EndPath(process.out)
