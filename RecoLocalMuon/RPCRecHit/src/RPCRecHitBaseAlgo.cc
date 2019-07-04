/*
 *  See header file for a description of this class.
 *
 *  \author M. Maggi -- INFN Bari, Shchablo -- IPNL Lyon
 */

#include "RecoLocalMuon/RPCRecHit/interface/RPCRecHitBaseAlgo.h"
#include "RecoLocalMuon/RPCRecHit/src/RPCClusterContainer.h"
#include "RecoLocalMuon/RPCRecHit/src/RPCCluster.h"
#include "RecoLocalMuon/RPCRecHit/src/RPCClusterizer.h"
#include "RecoLocalMuon/RPCRecHit/src/RPCMaskReClusterizer.h"

/* iRPC */
#include "RecoLocalMuon/RPCRecHit/src/iRPCClusterContainer.h"
#include "RecoLocalMuon/RPCRecHit/src/iRPCCluster.h"
#include "RecoLocalMuon/RPCRecHit/src/iRPCClusterizer.h"

RPCRecHitBaseAlgo::RPCRecHitBaseAlgo(const edm::ParameterSet& config) {
  //  theSync = RPCTTrigSyncFactory::get()->create(config.getParameter<string>("tTrigMode"),
  //config.getParameter<ParameterSet>("tTrigModeConfig"));
  
  /* iRPC Info */
   //iInfo.isUse(config.getParameter<ParameterSet>("useiRPCClustering"));
   //iInfo.isAND(config.getParameter<ParameterSet>("useiRPCAnd"));
   //iInfo.setMapHR(config.getParameter<ParameterSet>("iRPCMapHR"));
   //iInfo.setMapLR(config.getParameter<ParameterSet>("iRPCMapLR"));
   //iInfo.setThrTimeHR(config.getParameter<ParameterSet>("iRPCThrHR"));
   //iInfo.setThrTimeLR(config.getParameter<ParameterSet>("iRPCThrLR"));
   //iInfo.setThrDeltaTimeMin(config.getParameter<ParameterSet>("iRPCThrMin"));
   //iInfo.setThrDeltaTimeMax(config.getParameter<ParameterSet>("iRPCThrMax"));
   //iInfo.setSpeed(config.getParameter<ParameterSet>("iRPCSpeed"));

  
}

// Build all hits in the range associated to the layerId, at the 1st step.
edm::OwnVector<RPCRecHit> RPCRecHitBaseAlgo::reconstruct(const RPCRoll& roll,
                                                         const RPCDetId& rpcId,
                                                         const RPCDigiCollection::Range& digiRange,
                                                         const RollMask& mask) {
  edm::OwnVector<RPCRecHit> result;
  //if(iInfo.isUse() && roll.isIRPC()) {
  
   iInfo.setThrTimeHR(10); iInfo.setThrTimeLR(10);
   iInfo.setThrDeltaTimeMin(-35); iInfo.setThrDeltaTimeMax(30);
   iInfo.isAND(true);
   iInfo.isUse(true);
  if(iInfo.isUse()) {
    /* iRPC Clustering */
    iRPCClusterizer clizer;
    iRPCClusterContainer cls = clizer.doAction(digiRange, iInfo);
    
    for ( auto cl : cls ) {
      LocalError tmpErr;
      LocalPoint point;
      float time = 0, timeErr = -1;
      
      //// Call the compute method
      //const bool OK = this->compute(roll, cl, point, tmpErr, time, timeErr);
      //if (!OK) continue;

      // Build a new pair of 1D rechit
      time = cl.deltaTime();
      timeErr = cl.deltaTimeRMS();
      const int firstClustStrip = cl.firstStrip();
      const int clusterSize = cl.clusterSize();
      RPCRecHit* recHit = new RPCRecHit(rpcId, cl.bx(), firstClustStrip, clusterSize, point, tmpErr);
      recHit->setTimeAndError(time, timeErr);

      result.push_back(recHit);
    }
  }
  else {
    
    /* RPC Clustering */
    RPCClusterizer clizer;
    RPCClusterContainer tcls = clizer.doAction(digiRange);
    RPCMaskReClusterizer mrclizer;
    RPCClusterContainer cls = mrclizer.doAction(rpcId,tcls,mask);

    for ( auto cl : cls ) {
      LocalError tmpErr;
      LocalPoint point;
      float time = 0, timeErr = -1;

      // Call the compute method
      const bool OK = this->compute(roll, cl, point, tmpErr, time, timeErr);
      if (!OK) continue;

      // Build a new pair of 1D rechit
      const int firstClustStrip = cl.firstStrip();
      const int clusterSize = cl.clusterSize();
      RPCRecHit* recHit = new RPCRecHit(rpcId,cl.bx(), firstClustStrip, clusterSize, point, tmpErr);
      recHit->setTimeAndError(time, timeErr);

      result.push_back(recHit);
    }
  }



  return result;
}
