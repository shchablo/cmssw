import FWCore.ParameterSet.Config as cms

process = cms.Process('ReDoRecHit')

process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = "93X_upgrade2023_realistic_v5"

process.maxEvents = cms.untracked.PSet(
            input = cms.untracked.int32(300)
            )

from RecoLocalMuon.RPCRecHit.rpcRecHits_cfi import rpcRecHits
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
                                                                          "keep *RPC*_*_*_*"
                                                                          )
)
process.p = cms.Path(process.rpcRecHits)
process.e = cms.EndPath(process.out)
