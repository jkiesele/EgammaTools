//--------------------------------------------------------------------------------------------------
//
// EGammaPCAHelper
//
// Helper Class to compute PCA
//
//
//--------------------------------------------------------------------------------------------------
#ifndef EGammaPCAHelper_H
#define EGammaPCAHelper_H

#include "FWCore/Framework/interface/Frameworkfwd.h"

#include "DataFormats/CaloRecHit/interface/CaloCluster.h"
#include "DataFormats/HGCRecHit/interface/HGCRecHitCollections.h"
#include "DataFormats/ForwardDetId/interface/HGCalDetId.h"
#include "DataFormats/Math/interface/Vector3D.h"
#include "DataFormats/ParticleFlowReco/interface/HGCalMultiCluster.h"

#include "EgammaTools/EgammaAnalysis/interface/Spot.h"
#include "EgammaTools/EgammaAnalysis/interface/LongDeps.h"
#include "EgammaTools/EgammaAnalysis/interface/ShowerDepth.h"
#include "RecoLocalCalo/HGCalRecAlgos/interface/RecHitTools.h"
#include "FastSimulation/CaloGeometryTools/interface/Transform3DPJ.h"
#include <map>

#include "TPrincipal.h"

class HGCalRecHit;

class EGammaPCAHelper
{
public:
    typedef ROOT::Math::Transform3DPJ Transform3D;
    typedef ROOT::Math::Transform3DPJ::Point Point;

    EGammaPCAHelper();
    ~EGammaPCAHelper();

    // for the GsfElectrons
    void storeRecHits(const reco::CaloCluster & theCluster );
    void storeRecHits(const reco::HGCalMultiCluster &cluster );

    const TPrincipal & pcaResult();
    /// to set from outside - once per event
    void setHitMap( std::map<DetId,const HGCRecHit *> * hitMap) ;
    /// to compute from inside - once per event
    void fillHitMap(const HGCRecHitCollection & HGCEERecHits,
                    const HGCRecHitCollection & HGCFHRecHits,
                    const HGCRecHitCollection & HGCBHRecHits);

    void setRecHitTools(const hgcal::RecHitTools * recHitTools );

    inline void setdEdXWeights(const std::vector<double> & dEdX){ dEdXWeights_ = dEdX;}

    void pcaInitialComputation() {
        computePCA(-1.,false);
    }

    void computePCA(float radius, bool withHalo=true);
    const math::XYZPoint  & barycenter() const {return barycenter_;}
    const math::XYZVector & axis() const {return axis_;}

    void computeShowerWidth(float radius, bool withHalo=true);

    inline double sigmaUU() const { return checkIteration()? sigu_ : -1. ;}
    inline double sigmaVV() const { return checkIteration()? sigv_ : -1. ;}
    inline double sigmaEE() const { return checkIteration()? sige_ : -1. ;}
    inline double sigmaPP() const { return checkIteration()? sigp_ : -1. ;}

    inline TVectorD eigenValues () const {return checkIteration()? *pca_->GetEigenValues() : TVectorD(-1.,-1.,-1.);}
    inline TVectorD sigmas() const {return checkIteration()? *pca_->GetSigmas() : TVectorD(-1.,-1.,-1.);}
    // contains maxlayer+1 values, first layer is [1]
    LongDeps  energyPerLayer(float radius, bool withHalo=true) ;

    float clusterDepthCompatibility(const LongDeps &, float & measuredDepth, float& expectedDepth, float& expectedSigma );
    void printHits( float radius) const;
    void clear();

private:
    bool checkIteration() const ;
    void storeRecHits(const std::vector<std::pair<DetId, float>> &hf);
    float findZFirstLayer(const LongDeps&) const;

private:
    bool recHitsStored_;
    bool debug_;

    //parameters
    std::vector<double> dEdXWeights_;
    std::vector<double> invThicknessCorrection_;

    int hitMapOrigin_; // 0 not initialized; 1 set from outside ; 2 set from inside
    const reco::CaloCluster * theCluster_;
    std::map<DetId, const HGCRecHit *> * hitMap_;
    std::vector<Spot> theSpots_;
    int pcaIteration_;

    // output quantities
    math::XYZPoint barycenter_;
    math::XYZVector axis_;

    Transform3D trans_;
    double sigu_,sigv_,sige_,sigp_;

    // helper
    std::unique_ptr<TPrincipal> pca_;
    const hgcal::RecHitTools * recHitTools_;
    ShowerDepth showerDepth_;

};

#endif
