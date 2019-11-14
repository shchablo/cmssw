#! /usr/bin/env python
from optparse import OptionParser
import sys
import os 
import imp

parser = OptionParser()
parser.usage = "%prog <file> : expand this python configuration"

(options,args) = parser.parse_args()

inDeltaTimeY = args[1]
inThrHR = args[2]
inThrLR = args[3]

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
                                   thrTimeHR = cms.double(float(inThrHR)), # [ns] thrTimeHR: threshold for high radius clustering;
                                   thrTimeLR = cms.double(float(inThrLR)), # [ns] thrTimeLR: threshold for low radius clustering;
                                   thrDeltaTimeMin = cms.double(-20), # [ns] thrDeltaTimeMin: min delta time threshold for association clusters between HR and LR;
                                   thrDeltaTimeMax = cms.double(20), # [ns] thrDeltaTimeMax: max delta time threshold for association clusters between HR and LR;
                                   thrDeltaTimeY = cms.double(float(inDeltaTimeY)), # [ns] thrDeltaTimeY: position uncertainty;
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
#fileNames = cms.untracked.vstring('file:/eos/cms/store/group/dpg_dt/comm_dt/TriggerSimulation/SamplesReco/SingleMu_FlatPt-2to100/Version_10_5_0/SimRECO_1.root')
readFiles = cms.untracked.vstring()
readFiles.extend([
'file:/afs/cern.ch/work/k/kshchabl/public/DEE59424-11F1-B34E-A151-77D6642AA834.root', 
'file:/afs/cern.ch/work/k/kshchabl/public/6EA092BE-94A5-AD49-80DB-6A73ADBD033E.root',
'file:/afs/cern.ch/work/k/kshchabl/public/560F7499-5721-654B-BC83-3AE76477D8F1.root', 
'file:/afs/cern.ch/work/k/kshchabl/public/4B69420D-7BBC-BC46-8FF8-4090BB22E52F.root', 
'file:/afs/cern.ch/work/k/kshchabl/public/41A8FF37-7D2E-7249-B152-FE89164826F2.root', 
'file:/afs/cern.ch/work/k/kshchabl/public/3D6E0381-37F8-CA44-99D2-F0F07A27AB2A.root', 
'file:/afs/cern.ch/work/k/kshchabl/public/23509529-CA46-0D44-91B3-CA5B0CD03AB5.root', 
'file:/afs/cern.ch/work/k/kshchabl/public/025D27E1-8363-B54D-8E59-15E4E4D8D0A4.root', 
'file:/afs/cern.ch/work/k/kshchabl/public/77DD67E9-434B-8242-B08C-C99FBC95F477.root', 
'file:/afs/cern.ch/work/k/kshchabl/public/7E51FB46-8F9C-614D-830E-1DC00A69B753.root', 
'file:/afs/cern.ch/work/k/kshchabl/public/7EABBE9A-4C49-BA4C-87C9-8E7E245174B3.root', 
'file:/afs/cern.ch/work/k/kshchabl/public/7F4AF09D-D268-C74E-B06C-259D965857BE.root', 
'file:/afs/cern.ch/work/k/kshchabl/public/8659033B-7DE7-C448-B7D6-5DBC29F502A9.root', 
'file:/afs/cern.ch/work/k/kshchabl/public/871278EB-3E2F-4342-B4CF-B7BDBF38975A.root' 
  ]);
process.source = cms.Source('PoolSource', fileNames = readFiles)

# Output
outFile = "iRPC_delta_%s_HR_%s_LR_%s_.root" % (inDeltaTimeY, inThrHR, inThrLR)
process.out = cms.OutputModule("PoolOutputModule",
                fileName = cms.untracked.string(outFile),
                outputCommands = cms.untracked.vstring('drop *',
                "keep *_*_*_ReDoRecHit*",
                "keep *RPC*_*_*_*")
              )

process.p = cms.Path(process.rpcRecHits)
process.e = cms.EndPath(process.out)
