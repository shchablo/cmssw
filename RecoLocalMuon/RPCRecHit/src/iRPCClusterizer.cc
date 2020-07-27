#include "iRPCClusterizer.h"

/* iRPC */
#include "iRPCCluster.h"
#include "iRPCInfo.h"
#include "iRPCHit.h"
#include "iRPCHitContainer.h"
/* std */
#include <algorithm>
#include <limits>
#include <map>
#include <thread>

#include <iostream> // tests
#include <iomanip> // tests

iRPCClusterizer::iRPCClusterizer() {}
iRPCClusterizer::~iRPCClusterizer() {}

iRPCClusterContainer iRPCClusterizer::doAction(const RPCRoll& roll, const RPCDigiCollection::Range& digiRange, iRPCInfo& info)
{
    iRPCClusterContainer clusters;

    // Return empty container for null input
    if(std::distance(digiRange.second, digiRange.first) == 0) return clusters;

    // Test output data
    //std::cout << "------------" << std::endl;
    //for(auto digi = digiRange.first; digi != digiRange.second; ++digi) {
    //  std::cout <<"strip=" << digi->strip() << " time=" <<  digi->time() << " position=" << digi->coordinateY() << " bx=" << digi->bx() << std::endl;
    //}

    std::map<int, std::pair<iRPCHitContainer, iRPCHitContainer>> hits; // <map<bunchX, std::pair<hr, lr>>
    auto it = hits.begin();
    //std::cout << "Clusterizer: " << std::endl;
    // Fill digi (simulation)
    for(auto digi = digiRange.first; digi != digiRange.second; ++digi) {
        int bunchX = digi->bx();
        float timeHR = digi->time();
        int ch = digi->strip(); int strip = digi->strip();

        it = hits.find(bunchX); if(it == hits.end()) hits.insert(std::make_pair(bunchX, std::make_pair(iRPCHitContainer(), iRPCHitContainer())));
        hits.find(bunchX)->second.first.push_back(iRPCHit(ch, strip, timeHR, bunchX, digi->coordinateY()));
        hits.find(bunchX)->second.first.back().setHR(true);

        if(digi->hasY()) {
            float stripLen = roll.specificTopology().stripLength();
            float timeLR = timeHR-(stripLen-2*digi->coordinateY())/info.speed();
            //std::cout << "L:" << stripLen/2 << " y=" << digi->coordinateY() << " delta=" << timeHR - timeLR << std::endl;
            hits.find(bunchX)->second.second.push_back(iRPCHit(ch, strip, timeLR, bunchX, digi->coordinateY()));
            hits.find(bunchX)->second.second.back().setLR(true);
            // ---
            //std::cout <<"strip=" << digi->strip() << " time=" <<  digi->time() << " position=" << digi->coordinateY() << " bx=" << digi->bx()  << " dt=" << timeHR - timeLR << std::endl;
        }
    }
    // Fill digi (real)
    // Here need to write code from real format.

    //// Test output data
    //std::cout << "\n HITS ";
    //for(auto& it: hits) {
    //    std::cout << "\nBX: " << it.first << std::endl;
    //    std::cout << "HR: " << it.first << " :";
    //    for(auto hit = it.second.first.begin(); hit != it.second.first.end(); ++hit)
    //        std::cout << " " << hit->channel() << ":" << hit->time();
    //    std::cout << "\nLR: " << it.first << " :";
    //    for(auto hit = it.second.second.begin(); hit != it.second.second.end(); ++hit)
    //        std::cout << " " << hit->channel() << ":" << hit->time();
    //} std::cout << std::endl;
    // ---------------------------------------------
    
    iRPCClusterContainer chr, clr, associated;
    for(auto& it: hits) { // per bunchx

        // Clustering for HR and LR separate.
        std::thread thr(&iRPCClusterizer::clustering, this, info.thrTimeHR(), std::ref(it.second.first), std::ref(chr));
        std::thread tlr(&iRPCClusterizer::clustering, this, info.thrTimeLR(), std::ref(it.second.second), std::ref(clr));
        thr.join(); tlr.join();

        // Compute clusters parameters HR.
        for(auto cl = chr.begin(); cl != chr.end(); ++cl)
            cl->compute(std::ref(info));
        if(info.isOnlyHR()) return chr;
        // Compute clusters parameters LR.
        for(auto cl = clr.begin(); cl != clr.end(); ++cl)
            cl->compute(std::ref(info));
        if(info.isOnlyLR()) return clr;

        // Association between HR and LR.
        associated = association(info, chr, clr);
        clusters.insert(clusters.end(), associated.begin(), associated.end());
        chr.clear(); clr.clear(); associated.clear();
    }

    // Compute clusters parameters.
    for(auto cl = clusters.begin(); cl != clusters.end(); ++cl)
        cl->compute(std::ref(info));

    //////// Print data (test)
    //if( clusters.size() > 1) {
    //std::cout << "\nCouple: " << clusters.size();
    //for(unsigned int i = 0; i < clusters.size(); i++) {
    //    std::cout << "\n\nfirst: " << clusters.at(i).firstStrip() << " last: " << clusters.at(i).lastStrip() << " size: " << clusters.at(i).clusterSize() << " bx: " << clusters.at(i).bx()
    //    <<  " time: " << clusters.at(i).deltaTime() << " position: " << clusters.at(i).y();
    //    std::cout << "\nn: " <<  clusters.at(i).hits()->size() << "  ";
    //    for(unsigned int j = 0; j < clusters.at(i).hits()->size(); j++)
    //        std::cout << std::setprecision(4) << clusters.at(i).hits()->at(j).channel() << "|" 
    //        << clusters.at(i).hits()->at(j).time() << " " << clusters.at(i).hits()->at(j).isHR() << "; ";
    //}
    //std::cout << "\n------------------------------------------------------\n";
    //}
    hits.clear();
    return clusters;
}

bool iRPCClusterizer::clustering(float thrTime, iRPCHitContainer &hits, iRPCClusterContainer &clusters)
{
    if(hits.size() == 0) return false;

    // Sort hits by channel number from lowest to highest.
    std::sort(hits.begin(), hits.end(),
        [] (iRPCHit & h1, iRPCHit & h2) -> bool { return h1.channel() < h2.channel(); });

    //// Print data (test) RAW DATA
    //for(auto hit = hits.begin(); hit != hits.end(); ++hit)
    //    std::cout << hit->channel() << " ";

    // Fill all hits to map for group all hits per channel. Key is channel.
    std::map<int, iRPCHitContainer> channels;
    for(auto hit = hits.begin(); hit != hits.end(); ++hit) {
        auto channel = channels.find(hit->channel());
        if(channel == channels.end()) channels.insert(std::pair<int, iRPCHitContainer>(hit->channel(), iRPCHitContainer()));
        channels.find(hit->channel())->second.push_back(*hit);
    }

    // Sort hits by time from lowest to highest.
    for(auto channel = channels.begin(); channel != channels.end(); ++channel) {
        std::sort(channel->second.begin(), channel->second.end(),
            [] (iRPCHit & h1, iRPCHit & h2) -> bool { return h1.time() < h2.time(); });
    }

    //// Print data (test) Groups of hits
    //for(auto channel = channels.begin(); channel != channels.end(); ++channel) {
    //    std::cout << '\n' << channel->first << ": ";
    //    for(unsigned int i = 0; i < channel->second.size(); i++) {
    //        std::cout << std::setprecision(4) << channel->second.at(i).time() << " ";
    //    }
    //}

    // Fill clusters
    unsigned int used = 0; unsigned int nHits = hits.size();
    auto front = channels.begin(); auto next = std::next(channels.begin());
    float minDelta = std::numeric_limits<float>::max();
    float delta = std::numeric_limits<float>::max();
    unsigned int size = front->second.size();
    unsigned int nextSize = next->second.size();
    unsigned int i = 0, ni = 0;
    unsigned int minI = 0, minNI = 0;

    while(used != nHits) {
        front = channels.begin(); next = std::next(channels.begin());
        if((channels.size() >= 2) & (front->first + 1 == next->first)) {
            // find min delta between couple of hits for cluster (only when init)
            minDelta = std::numeric_limits<float>::max(); delta = std::numeric_limits<float>::max();
            size = front->second.size(); nextSize = next->second.size();
            i = 0; ni = 0; minI = 0; minNI = 0;
            while (i < size && ni < nextSize) {
                delta = abs(front->second.at(i).time() - next->second.at(ni).time());
                if(minDelta > delta) { minDelta = delta; minI = i; minNI = ni; }
                if(front->second.at(i).time() <= next->second.at(ni).time()) ++i; else ++ni;
            }
            // if minDelta below thresholt (param) fill two first hits in cluster
            if(minDelta < thrTime) {
                clusters.push_back(iRPCCluster()); clusters.back().addHit(front->second.at(minI));
                clusters.back().addHit(next->second.at(minNI));

                next->second.erase(next->second.begin() + minNI); used = used + 1;
                if(next->second.size() == 0) channels.erase(next);
                front->second.erase(front->second.begin() + minI); used = used + 1;
                if(front->second.size() == 0) channels.erase(front);
            }
            else { // fill only first hit in cluster
                clusters.push_back(iRPCCluster()); clusters.back().addHit(front->second.at(minI));
                front->second.erase(front->second.begin() + minI); used = used + 1;
                if(front->second.size() == 0) channels.erase(front);
            }
            // Сontinue Clustering
            for(auto channel = channels.begin(); channel != channels.end(); ++channel) {
                if(clusters.back().hits()->back().channel() + 1 == channel->first) {
                    minDelta = std::numeric_limits<float>::max(); delta = std::numeric_limits<float>::max(); minI = 0;
                    for(unsigned int i = 0; i < channel->second.size(); i++) {
                        delta = abs(clusters.back().hits()->back().time() - channel->second.at(i).time());
                        if(minDelta > delta) { minDelta = delta; minI = i; }
                    }
                    if(minDelta < thrTime) { // add hit to cluster
                        clusters.back().addHit(channel->second.at(minI));
                        channel->second.erase(channel->second.begin() + minI); used = used + 1;
                        if(channel->second.size() == 0) channels.erase(channel);
                    }
                }
            }
        }
        else { // if it last channel or it isn't first channel in cluster - make cluster with single hit
            clusters.push_back(iRPCCluster()); clusters.back().addHit(channels.begin()->second.front());
            channels.begin()->second.erase(channels.begin()->second.begin()); used = used + 1;
            if(channels.begin()->second.size() == 0) channels.erase(channels.begin());
        }
    }

    //// Print data (test) Cluster form single side
    //std::cout << "\n----\n";
    //std::cout << "\nN: " << clusters.size();
    //for(int i = 0; i < clusters.size(); i++) {
    //    std::cout << "\nn: " <<  clusters.at(i).hits()->size() << "  ";
    //    for(int j = 0; j < clusters.at(i).hits()->size(); j++)
    //        std::cout << std::setprecision(4) << clusters.at(i).hits()->at(j).channel() << "|" << clusters.at(i).hits()->at(j).time() << " ";
    //}
    //std::cout << "\n------------------------------------------------------\n";

    return true;
}

iRPCClusterContainer iRPCClusterizer::association(iRPCInfo &info, iRPCClusterContainer hr, iRPCClusterContainer lr)
{
    // ---
    bool isAND = info.isReturnOnlyAND();
    float thrDeltaMin = info.thrDeltaTimeMin();
    float thrDeltaMax = info.thrDeltaTimeMax();
    float thrDeltaY = info.thrDeltaTimeY();
    // ---
    iRPCClusterContainer clusters;

    // Sort clusters by number of hits from lowest to highest.
    std::sort(hr.begin(), hr.end(),
             [] (iRPCCluster & c1, iRPCCluster & c2) -> bool
                    { return c1.hits()->size() < c2.hits()->size(); });
    std::sort(lr.begin(), lr.end(),
             [] (iRPCCluster & c1, iRPCCluster & c2) -> bool
                    { return c1.hits()->size() < c2.hits()->size(); });

    //// Print data (test)
    //std::cout << "\nHR: " << hr.size();
    //for(unsigned int i = 0; i < hr.size(); i++) {
    //    std::cout << "\nn: " <<  hr.at(i).hits()->size() << "  ";
    //    for(unsigned int j = 0; j < hr.at(i).hits()->size(); j++)
    //        std::cout << std::setprecision(4) << hr.at(i).hits()->at(j).channel() << "|" << hr.at(i).hits()->at(j).time() << " ";
    //}
    //std::cout << "\n-----\n";
    //std::cout << "\nLR: " << lr.size();
    //for(unsigned int i = 0; i < lr.size(); i++) {
    //    std::cout << "\nn: " <<  lr.at(i).hits()->size() << "  ";
    //    for(unsigned int j = 0; j < lr.at(i).hits()->size(); j++)
    //        std::cout << std::setprecision(4) << lr.at(i).hits()->at(j).channel() << "|" << lr.at(i).hits()->at(j).time() << " ";
    //}
    //std::cout << "\n-----\n";

    // associate couples
    float minDelta = std::numeric_limits<float>::max();
    float delta = std::numeric_limits<float>::max();
    unsigned int used = 0; unsigned int nClusters = hr.size() + lr.size();
    unsigned int overlap = 0; std::map<unsigned int, std::vector<std::pair<unsigned int, unsigned int>>> overlaps; // map<overlap, vector<pair<h,l>>>
    double deltaTime = 0; std::vector<std::pair<int, double>> deltaTimes; // vector<<strip, deltaTime>>
    bool isSplitHR = false; bool isSplitLR = false;
    unsigned int sizeHR = hr.size(); unsigned int sizeLR = lr.size();
    //unsigned int h = 0, l = 0;
    while(used != nClusters) {
        overlap = 0; overlaps.clear(); deltaTimes.clear();
        sizeHR = hr.size(); sizeLR = lr.size();
        //h = 0; l = 0;
        //while(h < sizeHR && l < sizeLR) {
        for(unsigned int h = 0; h < sizeHR; h++) {
            for(unsigned int l = 0; l < sizeLR; l++) {

                overlap = 0; delta = hr.at(h).highTime() - lr.at(l).lowTime();
                if(delta >= thrDeltaMin && delta <= thrDeltaMax) {
                    for(unsigned int ih = 0; ih < hr.at(h).hits()->size(); ih++) {
                        for(unsigned int il = 0; il < lr.at(l).hits()->size(); il++) {
                            if(hr.at(h).hits()->at(ih).strip() == lr.at(l).hits()->at(il).strip()) { overlap = overlap + 1; break; }
                        }
                    }
                }
                // fill info about overlaps
                if(overlap != 0) {
                    auto o = overlaps.find(overlap);
                    if(o == overlaps.end())
                        overlaps.insert(std::pair<unsigned int, std::vector<std::pair<unsigned int, unsigned int>>>
                            (overlap, std::vector<std::pair<unsigned int, unsigned int>>()));
                    overlaps.find(overlap)->second.push_back(std::make_pair(h, l));
                }
                //if(hr.at(h).hits()->size() < lr.at(l).hits()->size()) ++h; else ++l;
            }
        }
        // looking couple with lower time delta
        if(!overlaps.empty()) {
            minDelta = std::numeric_limits<float>::max(); delta = std::numeric_limits<float>::max();
            auto min = std::prev(overlaps.end())->second.begin();
            for(auto o = std::prev(overlaps.end())->second.begin(); o != std::prev(overlaps.end())->second.end(); ++o) {
                delta = abs(hr.at(o->first).highTime() - lr.at(o->second).lowTime());
                if(minDelta > delta) { minDelta = delta; min = o; }
            }
            // split cluster by Y resolution (if it need)
            for(unsigned int ih = 0; ih < hr.at(min->first).hits()->size(); ih++)
                for(unsigned int il = 0; il < lr.at(min->second).hits()->size(); il++) 
                    if(hr.at(min->first).hits()->at(ih).strip() == lr.at(min->second).hits()->at(il).strip())  
                        deltaTimes.push_back(std::make_pair(hr.at(min->first).hits()->at(ih).strip(), hr.at(min->first).hits()->at(ih).time() - lr.at(min->second).hits()->at(il).time()));
            isSplitHR = false; isSplitLR = false;
            if(deltaTimes.size() > 0) deltaTime = deltaTimes.at(0).second; 
            for(unsigned int idT = 0; idT < deltaTimes.size(); idT++) {
                if(std::abs(deltaTime - deltaTimes.at(idT).second) > thrDeltaY) {  
                    iRPCCluster cHR; isSplitHR = hr.at(min->first).split(&cHR, deltaTimes.at(idT).first);
                    if(isSplitHR) {
                        hr.at(min->first).compute(info);
                        hr.push_back(cHR); hr.back().compute(info); nClusters += 1;
                        std::sort(hr.begin(), hr.end(),
                                    [] (iRPCCluster & c1, iRPCCluster & c2) -> bool
                                    { return c1.hits()->size() < c2.hits()->size(); });
                    }
                    iRPCCluster cLR; isSplitLR = lr.at(min->second).split(&cLR, deltaTimes.at(idT).first);
                    if(isSplitLR) {
                        lr.at(min->second).compute(info);
                        lr.push_back(cLR); lr.back().compute(info); nClusters += 1;
                        std::sort(lr.begin(), lr.end(),
                                    [] (iRPCCluster & c1, iRPCCluster & c2) -> bool
                                    { return c1.hits()->size() < c2.hits()->size(); });
                    }
                    break;
                }        
            }
            if(!isSplitHR && !isSplitLR) {
              clusters.push_back(iRPCCluster());
              clusters.back().initialize(hr.at(min->first), lr.at(min->second));
              hr.erase(hr.begin() + min->first); used = used + 1;
              lr.erase(lr.begin() + min->second); used = used + 1;
            }
        }
        else {
            if(!(hr.empty())) {
                if(!isAND) clusters.push_back(hr.front());
                hr.erase(hr.begin()); used = used + 1;
            }
            if(!(lr.empty())) {
                if(!isAND) clusters.push_back(lr.front());
                lr.erase(lr.begin()); used = used + 1;
            }
        }
    }
    return clusters;
}
