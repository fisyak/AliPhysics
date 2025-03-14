/************************************************************************************
 * Copyright (C) 2017, Copyright Holders of the ALICE Collaboration                 *
 * All rights reserved.                                                             *
 *                                                                                  *
 * Redistribution and use in source and binary forms, with or without               *
 * modification, are permitted provided that the following conditions are met:      *
 *     * Redistributions of source code must retain the above copyright             *
 *       notice, this list of conditions and the following disclaimer.              *
 *     * Redistributions in binary form must reproduce the above copyright          *
 *       notice, this list of conditions and the following disclaimer in the        *
 *       documentation and/or other materials provided with the distribution.       *
 *     * Neither the name of the <organization> nor the                             *
 *       names of its contributors may be used to endorse or promote products       *
 *       derived from this software without specific prior written permission.      *
 *                                                                                  *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND  *
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED    *
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           *
 * DISCLAIMED. IN NO EVENT SHALL ALICE COLLABORATION BE LIABLE FOR ANY              *
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES       *
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;     *
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND      *
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT       *
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS    *
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                     *
 ************************************************************************************/
#include <algorithm>
#include <array>
#include <sstream>
#include <string>
#include <vector>

#include <THistManager.h>
#include <TLinearBinning.h>
#include <TVariableBinning.h>
#include <TRandom3.h>

#include "AliAODInputHandler.h"
#include "AliAnalysisManager.h"
#include "AliAnalysisTaskEmcalJetEnergySpectrum.h"
#include "AliEmcalAnalysisFactory.h"
#include "AliEmcalDownscaleFactorsOCDB.h"
#include "AliEmcalJet.h"
#include "AliEmcalTriggerDecisionContainer.h"
#include "AliEmcalTriggerStringDecoder.h"
#include "AliEventCuts.h"
#include "AliInputEventHandler.h"
#include "AliJetContainer.h"
#include "AliLog.h"
#include "AliMultSelection.h"
#include "AliVEvent.h"
#include "AliFJWrapper.h"

ClassImp(PWGJE::EMCALJetTasks::AliAnalysisTaskEmcalJetEnergySpectrum);

using namespace PWGJE::EMCALJetTasks;

AliAnalysisTaskEmcalJetEnergySpectrum::AliAnalysisTaskEmcalJetEnergySpectrum():
  AliAnalysisTaskEmcalJet(),
  fHistos(nullptr),
  fIsMC(false),
  fFillHSparse(false),
  fFillSpectrumPtNEF(false),
	fTriggerSelectionBits(AliVEvent::kAny),
  fTriggerSelectionString(""),
  fRequireSubsetMB(false),
  fMimicEJData(false),
  fMinBiasTrigger(AliVEvent::kAny),
  fNameTriggerDecisionContainer("EmcalTriggerDecision"),
  fUseTriggerSelectionForData(false),
  fUseDownscaleWeight(false),
  fNameJetContainer("datajets"),
  fRequestTriggerClusters(true),
  fRequestCentrality(false),
  fFillHistosWeighted(kNoWeightType),
  fUseRun1Range(false),
  fUseSumw2(false),
  fUseMuonCalo(false),
  fUseStandardOutlierRejection(false),
  fDoBkgSub(false),
  fFastJetWrapper(nullptr),
  fTrackGenerator(nullptr),
  fJetTypeOutliers(kOutlierPartJet),
  fScaleShift(0.),
  fEMCALClusterBias(0.),
  fMinTimeClusterBias(-20e-9),
  fMaxTimeClusterBias(15e-9),
  fCentralityEstimator("V0M"),
  fUserPtBinning(),
  fMakeClusterHistos1D(false),
  fEnergyDefinition(kDefaultEnergy),
  fDoDifferentialDpT(false),
  fRhoScaleFactor(0.)
{
}

AliAnalysisTaskEmcalJetEnergySpectrum::AliAnalysisTaskEmcalJetEnergySpectrum(EMCAL_STRINGVIEW name):
  AliAnalysisTaskEmcalJet(name.data(), true),
  fHistos(nullptr),
  fIsMC(false),
  fFillHSparse(false),
  fFillSpectrumPtNEF(false),
	fTriggerSelectionBits(AliVEvent::kAny),
  fTriggerSelectionString(""),
  fRequireSubsetMB(false),
  fMimicEJData(false),
  fMinBiasTrigger(AliVEvent::kAny),
  fNameTriggerDecisionContainer("EmcalTriggerDecision"),
  fUseTriggerSelectionForData(false),
  fUseDownscaleWeight(false),
  fNameJetContainer("datajets"),
  fRequestTriggerClusters(true),
  fRequestCentrality(false),
  fFillHistosWeighted(kNoWeightType),
  fUseRun1Range(false),
  fUseSumw2(false),
  fUseMuonCalo(false),
  fUseStandardOutlierRejection(false),
  fDoBkgSub(false),
  fFastJetWrapper(nullptr),
  fTrackGenerator(nullptr),
  fJetTypeOutliers(kOutlierPartJet),
  fScaleShift(0.),
  fEMCALClusterBias(0.),
  fMinTimeClusterBias(-20e-9),
  fMaxTimeClusterBias(15e-9),
  fCentralityEstimator("V0M"),
  fUserPtBinning(),
  fMakeClusterHistos1D(false),
  fEnergyDefinition(kDefaultEnergy),
  fDoDifferentialDpT(false),
  fRhoScaleFactor(0.)
{
  SetMakeGeneralHistograms(true);
}

AliAnalysisTaskEmcalJetEnergySpectrum::~AliAnalysisTaskEmcalJetEnergySpectrum(){
  if(fHistos) delete fHistos;
  if(fDoBkgSub){
    delete fFastJetWrapper;
    delete fTrackGenerator;
  }
}

void AliAnalysisTaskEmcalJetEnergySpectrum::UserCreateOutputObjects(){
  AliAnalysisTaskEmcalJet::UserCreateOutputObjects();

  if(!fUserPtBinning.GetSize()) {
    // binning not set. apply default binning
    AliInfoStream() << "Using default pt binning";
    fUserPtBinning.Set(301);
    double current(0.);
    for(int istep = 0; istep < 301; istep++) {
      fUserPtBinning[istep] = current;
      current += 1;
    }
  }
  double runmin = fUseRun1Range ? 100000. : 200000.,
         runmax = fUseRun1Range ? 200000. : 300000.;
  fHistos = new THistManager(Form("Histos_%s", GetName()));
  fHistos->CreateTH1("hEventCounter", "Event counter histogram", 1, 0.5, 1.5);
  fHistos->CreateTH1("hEventCounterAbs", "Event counter histogram absolute", 1, 0.5, 1.5);
  fHistos->CreateTH1("hEventCounterRun", "Runwise event counter", 100000, runmin, runmax);
  fHistos->CreateTH1("hEventCounterRunWeighted", "Runwise event counter (weighted)", 100000, runmin, runmax);
  fHistos->CreateTProfile("hDownscaleFactorsRunwise", "Runwise downscale factors", 100000, runmin, runmax);
  fHistos->CreateTH1("hEventCentrality", "Event centrality", 100., 0., 100.);
  fHistos->CreateTH1("hEventCentralityAbs", "Event centrality absolute", 100., 0., 100.);
  fHistos->CreateTH1("hClusterCounter", "Event counter histogram", kTrgClusterN, -0.5, kTrgClusterN - 0.5);
  fHistos->CreateTH1("hClusterCounterAbs", "Event counter histogram absolute", kTrgClusterN, -0.5, kTrgClusterN - 0.5);
  fHistos->CreateTH2("hJetSpectrum", "Jet pt spectrum", kTrgClusterN, -0.5, kTrgClusterN - 0.5, 350., 0., 350., "s");
  fHistos->CreateTH2("hJetSpectrumAbs", "Jet pt spectrum (absolute counts)", kTrgClusterN, -0.5, kTrgClusterN - 0.5, 350., 0., 350., "s");
  fHistos->CreateTH2("hJetSpectrumMax", "Max jet pt spectrum", kTrgClusterN, -0.5, kTrgClusterN - 0.5, 350., 0., 350., "s");
  fHistos->CreateTH2("hJetSpectrumMaxAbs", "Max jet pt spectrum (absolute counts)", kTrgClusterN, -0.5, kTrgClusterN - 0.5, 350., 0., 350., "s");
  if(fFillSpectrumPtNEF) {
    fHistos->CreateTH3("hJetSpectrumPtNEF", "Jet pt spectrum (vs. NEF)", 350, 0., 350., 105, 0., 1.05, kTrgClusterN, -0.5, kTrgClusterN - 0.5, "s");
    fHistos->CreateTH3("hJetSpectrumPtNEFAbs", "Jet pt spectrum (vs. NEF, absolute counts)", 350, 0., 350., 105, 0., 1.05, kTrgClusterN, -0.5, kTrgClusterN - 0.5, "s");
    fHistos->CreateTH3("hJetSpectrumPtNEFMax", "Max jet pt spectrum (vs. NEF)", 350, 0., 350., 105, 0., 1.05, kTrgClusterN, -0.5, kTrgClusterN - 0.5, "s");
    fHistos->CreateTH3("hJetSpectrumPtNEFMaxAbs", "Max jet pt spectrum (vs. NEF, absolute counts)", 350, 0., 350., 105, 0., 1.05, kTrgClusterN, -0.5, kTrgClusterN - 0.5, "s");
  }

  if(fDoBkgSub){
    fHistos->CreateTH2("hRhoVsDeltaPtRC", "Rho Vs Delta Pt RC;#delta P_{T}^{RC} (Gev/c);#rho (Gev/c)", 170, -20, 150, 30, 0, 30, "s");
    if(fDoDifferentialDpT) fHistos->CreateTH3("hPtJetVsRhoVsDeltaPtRC", "Momentum Differential Rho Vs Delta Pt RC;p_{T}^{jet} (GeV/c);#delta P_{T}^{RC} (Gev/c);#rho (Gev/c)", 70., 0., 350., 170, -20, 150, 30, 0, 30, "s");
    fHistos->CreateTH2("hRhoVsDeltaPtEmbed", "Rho Vs Delta Pt embedding;#delta P_{T}^{RC} (Gev/c);#rho (Gev/c)", 170, -20, 150, 30, 0, 30, "s");

    fFastJetWrapper = new AliFJWrapper("FJWrapper", "FJWrapper");
    fFastJetWrapper->SetAreaType(fastjet::active_area);
    fFastJetWrapper->SetGhostArea(0.005);
    fFastJetWrapper->SetR(this->GetJetContainer(fNameJetContainer)->GetJetRadius());
    fFastJetWrapper->SetAlgorithm(fastjet::antikt_algorithm);
    fFastJetWrapper->SetRecombScheme(fastjet::pt_scheme);
    fTrackGenerator = new TRandom(0);
  }

  if(fFillHSparse) {
    TLinearBinning centralitybinning(100, 0., 100.), etabinning(100, -1., 1.), phibinning(100., 0., 7.), nefbinning(100, 0., 1.), trgclusterbinning(kTrgClusterN, -0.5, kTrgClusterN -0.5);
    TVariableBinning jetptbinning(fUserPtBinning);
    const TBinning *binnings[6] = {&centralitybinning, &jetptbinning, &etabinning, &phibinning, &nefbinning, &trgclusterbinning};
    fHistos->CreateTHnSparse("hJetTHnSparse", "jet thnsparse", 6, binnings, fUseSumw2 ? "s" : "");
    fHistos->CreateTHnSparse("hMaxJetTHnSparse", "jet thnsparse", 6, binnings, fUseSumw2 ? "s" : "");
  }

  if(fMakeClusterHistos1D){
    fHistos->CreateTH1("hClusterEnergy1D", "Cluster Energy Spectrum", 800., 0., 200.);
  }

  // A bit of QA stuff
  fHistos->CreateTH2("hQANEFPt", "Neutral energy fraction; p_{t} (GeV/c); NEF", 350, 0., 350., 105, 0., 1.05);
  fHistos->CreateTH2("hQAZchPt", "z_{ch,max}; p_{t} (GeV/c); z_{ch,max}", 350, 0., 350., 100, 0., 1.);
  fHistos->CreateTH2("hQAZnePt", "z_{ne,max}; p_{t} (GeV/c); z_{ne,max}", 350, 0., 350., 100, 0., 1.);
  fHistos->CreateTH2("hQAZchPtMax", "z_{ch,max}; p_{t} (GeV/c); z_{ch,max}", 350, 0., 350., 100, 0., 1.);
  fHistos->CreateTH2("hQAZnePtMax", "z_{ne,max}; p_{t} (GeV/c); z_{ne,max}", 350, 0., 350., 100, 0., 1.);
  fHistos->CreateTH2("hQANChPt", "Number of charged constituents; p_{t} (GeV/c); N_{ch}", 350, 0., 350., 100, 0., 100.);
  fHistos->CreateTH2("hQANnePt", "Number of neutral constituents; p_{t} (GeV/c); N_{ne}", 350, 0., 350., 100, 0., 100.);
  fHistos->CreateTH2("hQAConstPtCh", "p_{t} of charged constituents; p_{t,j} (GeV/c); p_{t,ch} (GeV/c)", 350, 0., 350., 350., 0., 350.);
  fHistos->CreateTH2("hQAConstPtNe", "p_{t} of neutral constituents; p_{t,j} (GeV/c); p_{t,ne} (GeV/c)", 350, 0., 350., 350., 0., 350.);
  fHistos->CreateTH2("hQAConstPtChMax", "p_{t} of max charged constituents; p_{t,j} (GeV/c); p_{t,ch} (GeV/c", 350, 0., 350., 350., 0., 350.);
  fHistos->CreateTH2("hQAConstPtNeMax", "p_{t} of max neutral constituents; p_{t,j} (GeV/c); p_{t,ne} (GeV/c)", 350, 0., 350., 350., 0., 350.);
  fHistos->CreateTH2("hQAEtaPhi", "#eta vs. #phi for selected jets; #eta; #phi", 100, -1., 1., 100, 0., 7.);
  fHistos->CreateTH2("hQAEtaPhiConstCh", "#eta vs. #phi for charged constituents; #eta; #phi", 100, -1., 1., 100, 0., 7.);
  fHistos->CreateTH2("hQAEtaPhiConstNe", "#eta vs. #phi for neutral constituents; #eta; #phi", 100, -1., 1., 100, 0., 7.);
  fHistos->CreateTH2("hQAEtaPhiMaxConstCh", "#eta vs. #phi for max charged constituents; #eta; #phi", 100, -1., 1., 100, 0., 7.);
  fHistos->CreateTH2("hQAEtaPhiMaxConstNe", "#eta vs. #phi for max neutral constituents; #eta; #phi", 100, -1., 1., 100, 0., 7.);
  fHistos->CreateTH2("hQADeltaRCharged", "#DeltaR vs. p_{t,jet} of charged constituents; p_{t, jet} (GeV/c); #DeltaR", 350., 0., 350, 100, 0., 1.);
  fHistos->CreateTH2("hQADeltaRNeutral", "#DeltaR vs. p_{t,jet} of neutral constituents; p_{t, jet} (GeV/c); #DeltaR", 350., 0., 350, 100, 0., 1);
  fHistos->CreateTH2("hQADeltaRMaxCharged", "#DeltaR vs. p_{t,jet} of charged constituents; p_{t, jet} (GeV/c); #DeltaR", 350., 0., 350, 100, 0., 1.);
  fHistos->CreateTH2("hQADeltaRMaxNeutral", "#DeltaR vs. p_{t,jet} of neutral constituents; p_{t, jet} (GeV/c); #DeltaR", 350., 0., 350, 100, 0., 1);
  fHistos->CreateTH2("hQAJetAreaVsJetPt", "Jet area vs. jet pt at detector level; p_{t} (GeV/c); Area", 350, 0., 350., 200, 0., 2.);
  fHistos->CreateTH2("hQAJetAreaVsNEF", "Jet area vs. NEF at detector level; NEF; Area", 105, 0., 1.05, 200, 0., 2.);
  fHistos->CreateTH2("hQAJetAreaVsNConst", "Jet area vs. number of consituents at detector level; Number of constituents; Area", 101, -0.5, 100.5, 200, 0., 2.);
  fHistos->CreateTH1("hQAPosTrVsPt", "N_{tr}^{+} vs. p_{t,tr}", 350., 0., 350.);
  fHistos->CreateTH1("hQANegTrVsPt", "N_{tr}^{-} vs. p_{t,tr}", 350., 0., 350.);
  // Cluster constituent QA
  fHistos->CreateTH2("hQAClusterTimeVsE", "Cluster time vs. energy; time (ns); E (GeV)", 1200, -600, 600, 200, 0., 200);
  fHistos->CreateTH2("hQAClusterTimeVsEFine", "Cluster time vs. energy (main region); time (ns); E (GeV)", 1000, -100, 100, 200, 0., 200);
  fHistos->CreateTH2("hQAClusterNCellVsE", "Cluster number of cells vs. energy; Number of cells; E (GeV)", 201, -0.5, 200.5, 200, 0., 200.);
  fHistos->CreateTH2("hQAClusterM02VsE", "Cluster M02 vs energy; M02; E (GeV)", 150, 0., 1.5, 200, 0., 200.);
  fHistos->CreateTH2("hQAClusterFracLeadingVsE", "Cluster frac leading cell vs energy; E (GeV); Frac. leading cell", 200, 0., 200., 100, 0., 1.1);
  fHistos->CreateTH2("hQAClusterFracLeadingVsNcell", "Cluster frac leading cell vs number of cells; Number of cells; Frac. leading cell", 201, -0.5, 200.5, 110, 0., 1.1);
  fHistos->CreateTH1("hFracPtHardPart", "Part. level jet Pt relative to the Pt-hard of the event", 100, 0., 10.);
  fHistos->CreateTH1("hFracPtHardDet", "Det. level jet Pt relative to the Pt-hard of the event", 100, 0., 10.);

  if(fEMCALClusterBias > 1e-5 && !this->GetClusterContainer()) {
    std::cout << "Adding cluster bias for L0 trigger studies" << std::endl;
    this->AddClusterContainer(AliEmcalAnalysisFactory::ClusterContainerNameFactory(fInputHandler->IsA() == AliAODInputHandler::Class()));
  }

  for(auto h : *fHistos->GetListOfHistograms()) fOutput->Add(h);
  PostData(1, fOutput);
}

Bool_t AliAnalysisTaskEmcalJetEnergySpectrum::CheckMCOutliers() {
  if(!fMCRejectFilter) return true;
  if(!(fIsPythia || fIsHerwig || fIsHepMC)) return true;    // Only relevant for pt-hard production
  if(fUseStandardOutlierRejection) return AliAnalysisTaskEmcal::CheckMCOutliers();
  AliDebugStream(1) << "Using custom MC outlier rejection" << std::endl;
  AliJetContainer *outlierjets(nullptr);
  switch(fJetTypeOutliers){
    case kOutlierPartJet: outlierjets = GetJetContainer("partjets"); break;
    case kOutlierDetJet: outlierjets = GetJetContainer(fNameJetContainer); break;
  };
  if(!outlierjets) return true;

  // Check whether there is at least one particle level jet with pt above n * event pt-hard
  auto jetiter = outlierjets->accepted();
  auto max = std::max_element(jetiter.begin(), jetiter.end(), [](const AliEmcalJet *lhs, const AliEmcalJet *rhs ) { return lhs->Pt() < rhs->Pt(); });
  if(max != jetiter.end())  {
    // At least one jet found with pt > n * pt-hard
    AliDebugStream(1) << "Found max jet with pt " << (*max)->Pt() << " GeV/c" << std::endl;
    if((*max)->Pt() > fPtHardAndJetPtFactor * fPtHard) return false;
  }
  return true;
}

bool AliAnalysisTaskEmcalJetEnergySpectrum::Run(){
  auto datajets = this->GetJetContainer(fNameJetContainer);
  auto clusters = this->GetClusterContainer(0);
  auto tracks = this->GetTrackContainer(0);
  if(!datajets) {
    AliErrorStream() << "Jet container " << fNameJetContainer << " not found" << std::endl;
    return false;
  }

  // Trigger studies for run3
  // Do not use "accepted" of the cluster container because the
  // the cut will most likely select clusters after correction for
  // the hadronic energy, which is unwanted for trigger studies
  // (need to be non-lin. corr energy)
  if(fEMCALClusterBias > 1e-5 && clusters) {
    bool hasTrigger = false;
    for(auto cluster : clusters->all()) {
      if(cluster->GetIsExotic()) continue;
      if(cluster->GetNonLinCorrEnergy() < fEMCALClusterBias) continue;
      if(!fIsMC) {
        if(cluster->GetTOF() < fMinTimeClusterBias || cluster->GetTOF() > fMaxTimeClusterBias) continue;
      }
      TLorentzVector ptvec;
      cluster->GetMomentum(ptvec, fVertex);
      if(TVector2::Phi_0_2pi(ptvec.Phi()) > 4) continue; // DCAL cluster
      hasTrigger = true;
    }
    if(!hasTrigger) {
      return false;
    }
  }

  // 1D Cluster Histogram
  Double_t energy(-1);
  if(fMakeClusterHistos1D){
    for(auto clust : clusters->accepted()) {
      // Distinguish energy definition
      switch(fEnergyDefinition){
        case kDefaultEnergy:
    	    AliDebugStream(2) << GetName() << ": Using cluster energy definition: default" << std::endl;
    	    energy = clust->E();
    	    break;
        case kNonLinCorrEnergy:
    	    AliDebugStream(2) << GetName() << ": Using cluster energy definition: corrected for non-linearity" << std::endl;
    	    energy = clust->GetNonLinCorrEnergy();
    	    break;
        case kHadCorrEnergy:
    	    AliDebugStream(2) << GetName() << ": Using cluster energy definition: corrected for hadronic contribution" << std::endl;
    	    energy = clust->GetHadCorrEnergy();
    	    break;
        default:
          energy = -1;
          break;
      };

      AliDebugStream(2) << GetName() << ": Using energy " << energy << " (def: " << clust->E()
    		<< " | NL: " << clust->GetNonLinCorrEnergy()
			  << " | HD: " << clust->GetHadCorrEnergy()
			  << ")" << std::endl;

      fHistos->FillTH1("hClusterEnergy1D", energy);
    }
  }

  double eventCentrality = 99;   // without centrality put everything in the peripheral bin
  if(fRequestCentrality){
    AliMultSelection *mult = dynamic_cast<AliMultSelection *>(InputEvent()->FindListObject("MultSelection"));
    if(!mult){
      AliErrorStream() << GetName() << ": Centrality selection enabled but no centrality estimator found" << std::endl;
      return false;
    }
    if(mult->IsEventSelected()) return false;
    eventCentrality = mult->GetEstimator(fCentralityEstimator)->GetPercentile();
    AliDebugStream(1) << GetName() << ": Centrality " <<  eventCentrality << std::endl;
  } else {
    AliDebugStream(1) << GetName() << ": No centrality selection applied" << std::endl;
  }
  bool isMC = (GetJetContainer("partjets") != nullptr);
  double qaClusterTimeShift = isMC ? 600. : 0.;

  auto trgclusters = GetTriggerClustersANY();
  if(!fIsMC && fRequestTriggerClusters) trgclusters = GetTriggerClusterIndices(fInputEvent->GetFiredTriggerClasses().Data());
  Double_t weight = 1.;
  if(fUseDownscaleWeight) {
    weight = 1./PWG::EMCAL::AliEmcalDownscaleFactorsOCDB::Instance()->GetDownscaleFactorForTriggerClass(MatchTrigger(fInputEvent->GetFiredTriggerClasses().Data(), fTriggerSelectionString.Data(), fUseMuonCalo));
  }
  if(fFillHistosWeighted != EWeightType_t::kNoWeightType && isMC) {
    // Get cross section weight from supported generator event header
    double mcweight = 1.;
    switch(fFillHistosWeighted) {
      case EWeightType_t::kCrossSectionWeightType: mcweight = GetCrossSectionFromHeader(); break;
      case EWeightType_t::kEventWeightType: mcweight = GetEventWeightFromHeader(); break;
      case EWeightType_t::kNoWeightType: break;
    };
    if(mcweight > 0) {
      weight *= mcweight;
    }
  }
  AliDebugStream(2) << "Found downscale weight " << weight << " for trigger " << fTriggerSelectionString << std::endl;
  fHistos->FillTH1("hEventCounterAbs", 1.);
  fHistos->FillTH1("hEventCounter", 1., weight);
  fHistos->FillTH1("hEventCounterRun", fRunNumber);
  fHistos->FillTH1("hEventCounterRunWeighted", fRunNumber, weight);
  fHistos->FillProfile("hDownscaleFactorsRunwise", fRunNumber, 1./weight);
  fHistos->FillTH1("hEventCentralityAbs", eventCentrality);
  fHistos->FillTH1("hEventCentrality", eventCentrality, weight);

  // Calculate rho value
  Double_t rhoVal = 0.;
  if(fDoBkgSub && datajets->GetRhoParameter()){
    if(fRhoScaleFactor > 0.) {
      rhoVal = datajets->GetRhoVal() * fRhoScaleFactor;
    }else{
      rhoVal = datajets->GetRhoVal();
    }
  }

  AliEmcalJet *maxjet(nullptr);
  for(auto t : trgclusters) {
    fHistos->FillTH1("hClusterCounterAbs", t);
    fHistos->FillTH1("hClusterCounter", t, weight);
  }
  for(auto j : datajets->accepted()){
    if(!maxjet || (j->E() > maxjet->E())) maxjet = j;
    Double_t ptjet = j->Pt();

    if (fDoBkgSub && datajets->GetRhoParameter()){
      ptjet = ptjet - rhoVal * j->Area();
    }

    if(TMath::Abs(fScaleShift) > DBL_EPSILON){
      // Apply artificial (fixed) shift of the jet energy scale to det. level jets
      ptjet += fScaleShift * ptjet;
    }
    double datapoint[6] = {eventCentrality, ptjet, j->Eta(), TVector2::Phi_0_2pi(j->Phi()), j->NEF(), 0.};
    for(auto t : trgclusters){
      fHistos->FillTH2("hJetSpectrum", static_cast<double>(t), ptjet, weight);
      fHistos->FillTH2("hJetSpectrumAbs", static_cast<double>(t), ptjet);
      if(fFillSpectrumPtNEF) {
        fHistos->FillTH3("hJetSpectrumPtNEF", ptjet, j->NEF(), static_cast<double>(t), weight);
        fHistos->FillTH3("hJetSpectrumPtNEFAbs", ptjet, j->NEF(), static_cast<double>(t));
      }
      if(fFillHSparse) {
        datapoint[5] = static_cast<double>(t);
        fHistos->FillTHnSparse("hJetTHnSparse", datapoint, weight);
      }
    }

    if(fDoBkgSub && datajets->GetRhoParameter() && fDoDifferentialDpT){
        Double_t randomConePt = GetDeltaPtRandomCone();
        fHistos->FillTH3("hPtJetVsRhoVsDeltaPtRC", ptjet, randomConePt, rhoVal);
    }

    // Fill QA plots - trigger cluster independent
    // Those plots have been in before (as part of the THnSparse) but were
    // removed in order to reduce the memory consumption.
    fHistos->FillTH2("hQANEFPt", ptjet, j->NEF(), weight);
    fHistos->FillTH2("hQAEtaPhi", j->Eta(), j->Phi(), weight);
    TVector3 jetvec(j->Px(), j->Py(), j->Pz());
    if(clusters && (datajets->GetJetType() != AliJetContainer::kChargedJet)){
      auto leadcluster = j->GetLeadingCluster(clusters->GetArray());
      if(leadcluster) {
        TLorentzVector ptvec;
        leadcluster->GetMomentum(ptvec, fVertex, (AliVCluster::VCluUserDefEnergy_t)clusters->GetDefaultClusterEnergy());
        fHistos->FillTH2("hQAZnePtMax", ptjet, j->GetZ(ptvec.Px(), ptvec.Py(), ptvec.Pz()), weight);
        fHistos->FillTH2("hQAConstPtNeMax", ptjet, ptvec.Pt(), weight);
        fHistos->FillTH2("hQAEtaPhiMaxConstNe", ptvec.Eta(), TVector2::Phi_0_2pi(ptvec.Phi()));
        fHistos->FillTH2("hQADeltaRMaxNeutral", ptjet, jetvec.DeltaR(ptvec.Vect()));
      } else {
        fHistos->FillTH2("hQAZnePtMax", ptjet, 0., weight);
        fHistos->FillTH2("hQAConstPtNeMax", ptjet, 0., weight);
      }
      for(auto ine = 0; ine < j->GetNumberOfClusters(); ine++){
        auto cluster = j->Cluster(ine);
        TLorentzVector ptvec;
        cluster->GetMomentum(ptvec, fVertex,(AliVCluster::VCluUserDefEnergy_t)clusters->GetDefaultClusterEnergy());
        fHistos->FillTH2("hQAConstPtNe", ptjet, ptvec.Pt(), weight);
        fHistos->FillTH2("hQAZnePt", ptjet, j->GetZ(ptvec.Px(), ptvec.Py(), ptvec.Pz()), weight);
        fHistos->FillTH2("hQAEtaPhiConstNe", ptvec.Eta(), TVector2::Phi_0_2pi(ptvec.Phi()));
        fHistos->FillTH2("hQADeltaRNeutral", ptjet, jetvec.DeltaR(ptvec.Vect()));
        fHistos->FillTH2("hQAClusterTimeVsE", cluster->GetTOF() * 1e9 - qaClusterTimeShift, ptvec.E());
        fHistos->FillTH2("hQAClusterTimeVsEFine", cluster->GetTOF() * 1e9 - qaClusterTimeShift, ptvec.E());
        fHistos->FillTH2("hQAClusterNCellVsE", cluster->GetNCells(), ptvec.E());
        fHistos->FillTH2("hQAClusterM02VsE", cluster->GetM02(), ptvec.E());
        double maxamplitude = 0.;
        for(int icell = 0; icell < cluster->GetNCells(); icell++) {
          double amplitude = fInputEvent->GetEMCALCells()->GetAmplitude(fInputEvent->GetEMCALCells()->GetCellPosition(cluster->GetCellAbsId(icell)));
          if(amplitude > maxamplitude) maxamplitude = amplitude;
        }
        fHistos->FillTH2("hQAClusterFracLeadingVsE", ptvec.E(), maxamplitude/cluster->E());
        fHistos->FillTH2("hQAClusterFracLeadingVsNcell", cluster->GetNCells(), maxamplitude/cluster->E());
      }
    }
    if(tracks){
      auto leadingtrack = j->GetLeadingTrack(tracks->GetArray());
      if(leadingtrack) {
        fHistos->FillTH2("hQAZchPtMax", ptjet, j->GetZ(leadingtrack->Px(), leadingtrack->Py(), leadingtrack->Pz()), weight);
        fHistos->FillTH2("hQAConstPtChMax", ptjet, leadingtrack->Pt(), weight);
        fHistos->FillTH2("hQAEtaPhiMaxConstCh", leadingtrack->Eta(), TVector2::Phi_0_2pi(leadingtrack->Phi()));
        fHistos->FillTH2("hQADeltaRMaxCharged", ptjet, j->DeltaR(leadingtrack));
      } else {
        fHistos->FillTH2("hQAZchPtMax", ptjet, 0., weight);
        fHistos->FillTH2("hQAConstPtChMax", ptjet, 0., weight);
      }
      for(int ich = 0; ich < j->GetNumberOfTracks(); ich++){
        auto track = j->Track(ich);
        fHistos->FillTH2("hQAConstPtCh", ptjet, track->Pt(), weight);
        fHistos->FillTH2("hQAZchPt", ptjet, j->GetZ(track->Px(), track->Py(), track->Pz()), weight);
        fHistos->FillTH2("hQAEtaPhiConstCh", track->Eta(), TVector2::Phi_0_2pi(track->Phi()), weight);
        fHistos->FillTH2("hQADeltaRCharged", ptjet, j->DeltaR(track), weight);
        if(track->Charge() > 0)      fHistos->FillTH1("hQAPosTrVsPt", track->Pt(), weight);
        else if(track->Charge() < 0) fHistos->FillTH1("hQANegTrVsPt", track->Pt(), weight);
        else{
          AliErrorStream() << "Found track with zero charge! Returning..." << std::endl;
          return false;
        }
      }
    }
    fHistos->FillTH2("hQANChPt", ptjet, j->GetNumberOfTracks(), weight);
    fHistos->FillTH2("hQANnePt", ptjet, j->GetNumberOfClusters(), weight);
    fHistos->FillTH2("hQAJetAreaVsJetPt", ptjet, j->Area(), weight);
    fHistos->FillTH2("hQAJetAreaVsNEF", j->NEF(), j->Area(), weight);
    fHistos->FillTH2("hQAJetAreaVsNConst", j->GetNumberOfClusters() + j->GetNumberOfTracks(), j->Area(), weight);
    // comparison to pt-hard
    if(fPtHard > 0.) {
      fHistos->FillTH1("hFracPtHardDet", j->Pt()/fPtHard);
    }
  }

  double maxdata[6];
  memset(maxdata, 0., sizeof(double) * 5);
  maxdata[0] = eventCentrality;
  if(maxjet){
    maxdata[1] = maxjet->Pt();
    if(TMath::Abs(fScaleShift) > DBL_EPSILON) {
      maxdata[1] += fScaleShift * maxdata[1];
    }
    maxdata[2] = maxjet->Eta();
    maxdata[3] = maxjet->Phi();
    maxdata[4] = maxjet->NEF();
  }
  for(auto t : trgclusters){
    fHistos->FillTH2("hJetSpectrumMax", t, maxdata[1], weight);
    fHistos->FillTH2("hJetSpectrumMaxAbs", t, maxdata[1]);
    if(fFillSpectrumPtNEF) {
      fHistos->FillTH3("hJetSpectrumPtNEFMax",  maxdata[1], maxdata[4], t, weight);
      fHistos->FillTH3("hJetSpectrumPtNEFMaxAbs", maxdata[1], maxdata[4], t);
    }
    if(fFillHSparse){
      maxdata[5] = static_cast<double>(t);
      fHistos->FillTHnSparse("hMaxJetTHnSparse", maxdata, weight);
    }
  }

  if(fDoBkgSub){
    Double_t randomConePt = GetDeltaPtRandomCone();
    Double_t EmbeddingPt = GetDeltaPtEmbedding();
    fHistos->FillTH2("hRhoVsDeltaPtRC", randomConePt, rhoVal);
    fHistos->FillTH2("hRhoVsDeltaPtEmbed", EmbeddingPt, rhoVal);
  }


  // outlier cut (MC only)
  if(fPtHard > 0.) {
    auto partjets = GetJetContainer("partjets");
    if(partjets) {
      for(auto j : partjets->accepted()) fHistos->FillTH1("hFracPtHardPart",  j->Pt()/fPtHard);
    }
  }
  return true;
}

void AliAnalysisTaskEmcalJetEnergySpectrum::RunChanged(Int_t newrun){
  if(fUseDownscaleWeight) {
    auto downscalehandler = PWG::EMCAL::AliEmcalDownscaleFactorsOCDB::Instance();
    if(downscalehandler->GetCurrentRun() != newrun){
      downscalehandler->SetRun(newrun);
    }
  }
}

bool AliAnalysisTaskEmcalJetEnergySpectrum::IsTriggerSelected() {
  if(!fIsMC){
    // Pure data - do EMCAL trigger selection from selection string
    UInt_t triggerbits = fTriggerSelectionBits;
    if(fUseMuonCalo) fTriggerSelectionBits = AliVEvent::kMuonCalo;  // in case of the muon-calo / calo(fast) cluster all data is in the
    if(fMimicEJData) triggerbits = AliVEvent::kINT7;                // for mimiced trigger require INT7
    if(!(fInputHandler->IsEventSelected() & triggerbits)) return false;
    if(fTriggerSelectionString.Length()) {
      if(!fMimicEJData && !fInputEvent->GetFiredTriggerClasses().Contains(fTriggerSelectionString)) return false;
      if(fRequireSubsetMB && !(fInputHandler->IsEventSelected() & fMinBiasTrigger)) return false;   // Require EMCAL trigger to be subset of the min. bias trigger (for efficiency studies)
      if((fTriggerSelectionString.Contains("EJ") || fTriggerSelectionString.Contains("EG") || fTriggerSelectionString.Contains("DJ") || fTriggerSelectionString.Contains("DG")) && fUseTriggerSelectionForData) {
        auto trgselresult = static_cast<PWG::EMCAL::AliEmcalTriggerDecisionContainer *>(fInputEvent->FindListObject(fNameTriggerDecisionContainer));
        AliDebugStream(1) << "Found trigger decision object: " << (trgselresult ? "yes" : "no") << std::endl;
        if(!trgselresult){
          AliErrorStream() <<  "Trigger decision container with name " << fNameTriggerDecisionContainer << " not found in event - not possible to select EMCAL triggers" << std::endl;
          return false;
        }
        if(!trgselresult->IsEventSelected(fTriggerSelectionString)) return false;
      }
    }
  } else {
    if(!(fInputHandler->IsEventSelected() & AliVEvent::kINT7)) return false;
    if(IsSelectEmcalTriggers(fTriggerSelectionString.Data())){
      // Simulation - do EMCAL trigger selection from trigger selection object
      auto mctrigger = static_cast<PWG::EMCAL::AliEmcalTriggerDecisionContainer *>(fInputEvent->FindListObject(fNameTriggerDecisionContainer));
      AliDebugStream(1) << "Found trigger decision object: " << (mctrigger ? "yes" : "no") << std::endl;
      if(!mctrigger){
        AliErrorStream() <<  "Trigger decision container with name " << fNameTriggerDecisionContainer << " not found in event - not possible to select EMCAL triggers" << std::endl;
        return false;
      }
      if(!mctrigger->IsEventSelected(fTriggerSelectionString)) return false;
    }
  }
  return true;
}

void AliAnalysisTaskEmcalJetEnergySpectrum::ConfigureMCPtHard(MCProductionType_t mcprodtype, const TArrayI &pthardbinning, Bool_t doMCFilter, Double_t jetptcut) {
  SetMCProductionType(mcprodtype);
  SetUsePtHardBinScaling(true);
  SetUserPtHardBinning(pthardbinning);
  if(doMCFilter) {
    SetMCFilter();
    SetJetPtFactor(jetptcut);
  }
}

void AliAnalysisTaskEmcalJetEnergySpectrum::ConfigureMCMinBias(MCProductionType_t mcprodtype){
  if(!(mcprodtype == kMCPythiaMB || mcprodtype == kMCHepMCMB)) {
    AliErrorStream() << "MC prod type not compatible with min. bias production" << std::endl;
  }
  SetMCProductionType(mcprodtype);
}

void AliAnalysisTaskEmcalJetEnergySpectrum::ConfigureDetJetSelection(Double_t minJetPt, Double_t maxTrackPt, Double_t maxClusterPt, Double_t minAreaPerc) {
  auto detjets = GetDetJetContainer();

  detjets->SetJetPtCut(minJetPt);
  if(detjets->GetJetType() == AliJetContainer::kFullJet || detjets->GetJetType() == AliJetContainer::kChargedJet) {
    detjets->SetMaxTrackPt(maxTrackPt);
  }
  if(detjets->GetJetType() == AliJetContainer::kFullJet || detjets->GetJetType() == AliJetContainer::kNeutralJet) {
    detjets->SetMaxClusterPt(maxClusterPt);
  }
  if(minAreaPerc >= 0.) {
    detjets->SetPercAreaCut(minAreaPerc);
  }
}

Double_t AliAnalysisTaskEmcalJetEnergySpectrum::GetDeltaPtRandomCone()
{
    Double_t deltaPt = -1000.;

    auto jetcont = this->GetJetContainer(fNameJetContainer);
    auto partcont = this->GetTrackContainer(0);

    Double_t jetradius = jetcont->GetJetRadius();
    Double_t minEta = -0.7 + jetradius;
    Double_t maxEta = 0.7 - jetradius;
    Double_t tmpRandConeEta = -999;
    Double_t tmpRandConePhi = -999;
    Double_t tmpConePt = -0.;

    AliEmcalJet *LeadingJet = NULL;
    AliEmcalJet *SubLeadingJet = NULL;
    Double_t LJeta = 999;
    Double_t LJphi = 999;

    Double_t SLJeta = 999;
    Double_t SLJphi = 999;

    Double_t EmcalMinPhi = fGeom->GetArm1PhiMin() * TMath::DegToRad();
    Double_t EmcalMaxPhi = fGeom->GetEMCALPhiMax() * TMath::DegToRad();

    if (jetcont)
    {

        Float_t maxJetPts[] = {0, 0};

        jetcont->ResetCurrentID();

        AliEmcalJet *jet = 0x0;

        while ((jet = jetcont->GetNextAcceptJet()))
        {

            if (!jet)
                continue;

            if (jet->Pt() > maxJetPts[0])
            {
                maxJetPts[1] = maxJetPts[0];
                SubLeadingJet = LeadingJet;
                maxJetPts[0] = jet->Pt();
                LeadingJet = jet;
            }
            else if (jet->Pt() > maxJetPts[1])
            {
                maxJetPts[1] = jet->Pt();
                SubLeadingJet = jet;
            }
        }

        if (LeadingJet)
        {
            LJeta = LeadingJet->Eta();
            LJphi = LeadingJet->Phi();
        }
        if (SubLeadingJet)
        {
            SLJeta = SubLeadingJet->Eta();
            SLJphi = SubLeadingJet->Phi();
        }
    }

    Double_t dLJ = 0;
    Double_t dSLJ = 0;
    Int_t repeats = 0;

    if(jetcont->GetJetType() != AliJetContainer::kChargedJet){
      do
      {
          tmpRandConeEta = minEta + gRandom->Rndm() * (maxEta - minEta);
          tmpRandConePhi = gRandom->Rndm() * (EmcalMaxPhi - EmcalMinPhi) + EmcalMinPhi;
          dLJ = TMath::Sqrt((LJeta - tmpRandConeEta) * (LJeta - tmpRandConeEta) + (LJphi - tmpRandConePhi) * (LJphi - tmpRandConePhi));
          repeats++;

      } while (dLJ < (2*jetradius + 0.05));
    }else{
      do
      {
          tmpRandConeEta = minEta + gRandom->Rndm() * (maxEta - minEta);
          tmpRandConePhi = gRandom->Rndm() * TMath::TwoPi();
          dLJ = TMath::Sqrt((LJeta - tmpRandConeEta) * (LJeta - tmpRandConeEta) + (LJphi - tmpRandConePhi) * (LJphi - tmpRandConePhi));
          dSLJ = TMath::Sqrt((SLJeta - tmpRandConeEta) * (SLJeta - tmpRandConeEta) + (SLJphi - tmpRandConePhi) * (SLJphi - tmpRandConePhi));
          repeats++;

      } while (dLJ < (2*jetradius + 0.05) || dSLJ < (2*jetradius + 0.05));
    }

    AliAODTrack *tmpTrack = 0x0;

    partcont->ResetCurrentID();
    while (AliAODTrack *tmpTrack = (AliAODTrack *)partcont->GetNextAcceptParticle())
    {
        if (!tmpTrack)
          continue;

        if (sqrt((tmpTrack->Eta() - tmpRandConeEta) * (tmpTrack->Eta() - tmpRandConeEta) +
                 TVector2::Phi_mpi_pi((tmpTrack->Phi() - tmpRandConePhi)) *
                     TVector2::Phi_mpi_pi((tmpTrack->Phi() - tmpRandConePhi))) < jetradius)
        {
            tmpConePt += tmpTrack->Pt();
        }
    }

    if (tmpConePt > 0)
    {
        // Calculate rho value
        Double_t rhoVal = 0.;
        if(fDoBkgSub && jetcont->GetRhoParameter()){
          if(fRhoScaleFactor > 0.) {
            rhoVal = jetcont->GetRhoVal() * fRhoScaleFactor;
          }else{
            rhoVal = jetcont->GetRhoVal();
          }
        }
        deltaPt = tmpConePt - jetradius * jetradius * TMath::Pi() * rhoVal;
        return deltaPt;
    }
    return deltaPt;
}

Double_t AliAnalysisTaskEmcalJetEnergySpectrum::GetDeltaPtEmbedding()
{

    fFastJetWrapper->Clear();
    //----------------Generating NEW perpendicular track

    auto partcont = this->GetTrackContainer(0);
    auto jetcont = this->GetJetContainer(fNameJetContainer);

    Double_t EmcalMinPhi = fGeom->GetArm1PhiMin() * TMath::DegToRad();
    Double_t EmcalMaxPhi = fGeom->GetEMCALPhiMax() * TMath::DegToRad();
    Double_t jetradius = jetcont->GetJetRadius();


    AliEmcalJet* leadingJet = jetcont->GetLeadingJet();

    if(!leadingJet)
      return -999;

    Double_t signalEta = leadingJet->Eta();
    Double_t signalPhi = leadingJet->Phi();

    Double_t gen_pt = fTrackGenerator->Uniform(0, 100); //this will be pT of the track that you embedd
    TLorentzVector lVec;
    if(jetcont->GetJetType() != AliJetContainer::kChargedJet){
      Double_t diffHigh = TMath::Abs(EmcalMaxPhi - (signalPhi + jetradius));
      Double_t diffLow = TMath::Abs((signalPhi - jetradius) - EmcalMinPhi);
      Double_t randomPhi = 0;
      if (diffHigh > diffLow){
        randomPhi = TMath::Abs(EmcalMaxPhi - gRandom->Rndm()*diffHigh);
      }else{
        randomPhi = TMath::Abs(gRandom->Rndm()*diffLow + EmcalMinPhi);
      }
      lVec.SetPtEtaPhiM(gen_pt, signalEta, randomPhi, 0);               //here signalEta, signalPhi are some directions that you choose
    }else{
      lVec.SetPtEtaPhiM(gen_pt, signalEta, signalPhi + TMath::Pi() / 2, 0);               //here signalEta, signalPhi are some directions that you choose
    }
    fFastJetWrapper->AddInputVector(lVec.Px(), lVec.Py(), lVec.Pz(), lVec.E(), -99999); //fill embedded track to the array of proto-jets

    //-----Filling   fFastJetWrapper with tracks from track container
    partcont->ResetCurrentID();
    while (AliAODTrack *trk = (AliAODTrack *)partcont->GetNextAcceptParticle())
    {
      if (!trk)
        continue;

        fFastJetWrapper->AddInputVector(trk->Px(), trk->Py(), trk->Pz(), trk->P(), 1); //fill reconstructed tracks to the array of proto-jets
    }

    fFastJetWrapper->Run(); //this creates jets including the embedded track

    //--------------- DelPt analysis of the new container
    std::vector<fastjet::PseudoJet> jets_incl = fFastJetWrapper->GetInclusiveJets(); //this is the new jet array which included also the jet with embedded track
    Double_t deltaPtEmb = -999;
    Double_t sumTrkEmbeddedPt = 0;

    for (UInt_t ijet = 0; ijet < jets_incl.size(); ++ijet)
    {                                                                                            //loop over these jets and search for jet with embedded track.
        std::vector<fastjet::PseudoJet> constituents(fFastJetWrapper->GetJetConstituents(ijet)); //get list of constituents of each jet
        sumTrkEmbeddedPt = 0;
        for (UInt_t ic = 0; ic < constituents.size(); ++ic)
        { //loop over constituents
            if (constituents[ic].user_index() == -99999)
            {                                              //this is the constituent which was embedded
                sumTrkEmbeddedPt += constituents[ic].pt(); // get pT of this embedded track
                break;                                     //we have embedded just 1 track so we can break, there will be not other in this event
            }
        }

        if (sumTrkEmbeddedPt > 0)
        {   
            // Calculate rho value
            Double_t rhoVal = 0.;
            if(fDoBkgSub && jetcont->GetRhoParameter()){
              if(fRhoScaleFactor > 0.) {
                rhoVal = jetcont->GetRhoVal() * fRhoScaleFactor;
              }else{
                rhoVal = jetcont->GetRhoVal();
              }
            }             
            //the jet with embedded track was found
            deltaPtEmb = jets_incl.at(ijet).pt() - jets_incl.at(ijet).area() * rhoVal - sumTrkEmbeddedPt; //calculate delta pT and subtract pT of embedded track
            break;
        }
    }

    return deltaPtEmb;
}

AliAnalysisTaskEmcalJetEnergySpectrum *AliAnalysisTaskEmcalJetEnergySpectrum::AddTaskJetEnergySpectrumBase(Bool_t isMC, AliJetContainer::EJetType_t jettype, AliJetContainer::ERecoScheme_t recoscheme, AliVCluster::VCluUserDefEnergy_t energydef, double radius, EMCAL_STRINGVIEW namepartcont, EMCAL_STRINGVIEW trigger, EMCAL_STRINGVIEW nametrackcont, EMCAL_STRINGVIEW nameclustercont, EMCAL_STRINGVIEW suffix){
  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  if(!mgr) {
    std::cerr << "Analysis manager not initialized" << std::endl;
    return nullptr;
  }

  Bool_t isAOD(kFALSE);
  AliInputEventHandler *inputhandler = static_cast<AliInputEventHandler *>(mgr->GetInputEventHandler());
  if(inputhandler) {
    if(inputhandler->IsA() == AliAODInputHandler::Class()){
      std::cout << "Analysing AOD events\n";
      isAOD = kTRUE;
    } else {
      std::cout << "Analysing ESD events\n";
    }
  }

  std::string jettypestring;
  UInt_t acctype(AliJetContainer::kTPCfid);
  switch(jettype){
    case AliJetContainer::kChargedJet:  jettypestring = "ChargedJets"; acctype = AliJetContainer::kTPCfid; break;
    case AliJetContainer::kFullJet:     jettypestring = "FullJets";    acctype = AliJetContainer::kEMCALfid; break;
    case AliJetContainer::kNeutralJet:  jettypestring = "NeutralJets"; acctype = AliJetContainer::kEMCALfid; break;
    case AliJetContainer::kUndefinedJetType: break;
  };

  std::stringstream tag, outfilename;
  tag << jettypestring << "_R" << std::setw(2) << std::setfill('0') << int(radius * 10.) << "_" << trigger;
  if(suffix.length()) {
    tag << "_" << suffix;
  }
  auto task = new AliAnalysisTaskEmcalJetEnergySpectrum(Form("JetEnergySpectrum_%s", tag.str().data()));
  task->SetIsMC(isMC);
  mgr->AddTask(task);

  auto contains = [](EMCAL_STRINGVIEW str, EMCAL_STRINGVIEW test) {
    return str.find(test) != std::string::npos;
  };

  std::string trgstr(trigger);
  if(contains(trgstr, "INT7")) task->SetTriggerSelection(AliVEvent::kINT7, "INT7");
  else if(contains(trgstr, "EMC7")) task->SetTriggerSelection(AliVEvent::kEMC7, "EMC7");
  else if(contains(trgstr, "EJE")) task->SetTriggerSelection(AliVEvent::kEMCEJE, "EJE");
  else if(contains(trgstr, "EJ1")) task->SetTriggerSelection(AliVEvent::kEMCEJE, "EJ1");
  else if(contains(trgstr, "EJ2")) task->SetTriggerSelection(AliVEvent::kEMCEJE, "EJ2");
  else if(contains(trgstr, "EG1")) task->SetTriggerSelection(AliVEvent::kEMCEGA, "EG1");
  else if(contains(trgstr, "EG2")) task->SetTriggerSelection(AliVEvent::kEMCEGA, "EG2");

  // Connect particle and cluster container
  AliTrackContainer *tracks(nullptr);
  AliClusterContainer *clusters(nullptr);
  TString trackcontname   = nametrackcont.data();
  TString clustercontname = nameclustercont.data();

  if(jettype == AliJetContainer::kChargedJet || jettype == AliJetContainer::kFullJet) {
    if(trackcontname == "usedefault") tracks = task->AddTrackContainer(AliEmcalAnalysisFactory::TrackContainerNameFactory(isAOD));
    else tracks = task->AddTrackContainer(trackcontname.Data());
    tracks->SetMinPt(0.15);
  }

  if(jettype == AliJetContainer::kNeutralJet || jettype == AliJetContainer::kFullJet){
    if(clustercontname == "usedefault") clusters = task->AddClusterContainer(AliEmcalAnalysisFactory::ClusterContainerNameFactory(isAOD));
    else clusters = task->AddClusterContainer(clustercontname.Data());
    clusters->SetDefaultClusterEnergy(energydef);
    clusters->SetClusUserDefEnergyCut(energydef, 0.3);
  }


  // Create proper jet container
  auto jetcont = task->AddJetContainer(jettype, AliJetContainer::antikt_algorithm, recoscheme, radius, acctype, tracks, clusters);
  jetcont->SetName("datajets");
  task->SetNameJetContainer("datajets");
  std::cout << "Adding jet container with underlying array:" << jetcont->GetArrayName() << std::endl;

  if(isMC){
    // Create also particle and particle level jet container for outlier rejection
    TString partcontname = namepartcont.data();
    if(partcontname == "usedefault") partcontname = "mcparticles";
    auto partcont = task->AddMCParticleContainer(partcontname.Data());
    partcont->SetMinPt(0.);

    //AliJetContainer::EJetType_t mcjettype = (jettype == AliJetContainer::kNeutralJet) ? AliJetContainer::kFullJet : jettype;
    AliJetContainer::EJetType_t mcjettype = AliJetContainer::kFullJet;
    auto pjcont = task->AddJetContainer(mcjettype, AliJetContainer::antikt_algorithm, recoscheme, radius, AliJetContainer::kTPCfid, partcont, nullptr);
    pjcont->SetName("partjets");
    pjcont->SetMinPt(0);
    pjcont->SetMaxTrackPt(1000.);
  }

  // Link input and output container
  outfilename << mgr->GetCommonFileName() << ":JetSpectrum_" << tag.str().data();
  mgr->ConnectInput(task, 0, mgr->GetCommonInputContainer());
  mgr->ConnectOutput(task, 1, mgr->CreateContainer(Form("JetSpectrum_%s", tag.str().data()), TList::Class(), AliAnalysisManager::kOutputContainer, outfilename.str().data()));

  return task;
}

AliAnalysisTaskEmcalJetEnergySpectrum *AliAnalysisTaskEmcalJetEnergySpectrum::AddTaskJetEnergySpectrumBkgSubBase(Bool_t isMC, AliJetContainer::EJetType_t jettype, AliJetContainer::ERecoScheme_t recoscheme, AliVCluster::VCluUserDefEnergy_t energydef, double radius, EMCAL_STRINGVIEW namepartcont, EMCAL_STRINGVIEW trigger, EMCAL_STRINGVIEW nrho, EMCAL_STRINGVIEW nametrackcont, EMCAL_STRINGVIEW nameclustercont, EMCAL_STRINGVIEW suffix){
  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  if(!mgr) {
    std::cerr << "Analysis manager not initialized" << std::endl;
    return nullptr;
  }

  Bool_t isAOD(kFALSE);
  AliInputEventHandler *inputhandler = static_cast<AliInputEventHandler *>(mgr->GetInputEventHandler());
  if(inputhandler) {
    if(inputhandler->IsA() == AliAODInputHandler::Class()){
      std::cout << "Analysing AOD events\n";
      isAOD = kTRUE;
    } else {
      std::cout << "Analysing ESD events\n";
    }
  }

  std::string jettypestring;
  UInt_t acctype(AliJetContainer::kTPCfid);
  switch(jettype){
    case AliJetContainer::kChargedJet:  jettypestring = "ChargedJets"; acctype = AliJetContainer::kTPCfid; break;
    case AliJetContainer::kFullJet:     jettypestring = "FullJets";    acctype = AliJetContainer::kEMCALfid; break;
    case AliJetContainer::kNeutralJet:  jettypestring = "NeutralJets"; acctype = AliJetContainer::kEMCALfid; break;
    case AliJetContainer::kUndefinedJetType: break;
  };

  std::stringstream tag, outfilename;
  tag << jettypestring << "_R" << std::setw(2) << std::setfill('0') << int(radius * 10.) << "_" << trigger;
  if(suffix.length()) {
    tag << "_" << suffix;
  }
  auto task = new AliAnalysisTaskEmcalJetEnergySpectrum(Form("JetEnergySpectrum_%s", tag.str().data()));
  task->SetIsMC(isMC);
  mgr->AddTask(task);

  auto contains = [](EMCAL_STRINGVIEW str, EMCAL_STRINGVIEW test) {
    return str.find(test) != std::string::npos;
  };

  std::string trgstr(trigger);
  if(contains(trgstr, "INT7")) task->SetTriggerSelection(AliVEvent::kINT7, "INT7");
  else if(contains(trgstr, "EMC7")) task->SetTriggerSelection(AliVEvent::kEMC7, "EMC7");
  else if(contains(trgstr, "EJE")) task->SetTriggerSelection(AliVEvent::kEMCEJE, "EJE");
  else if(contains(trgstr, "EJ1")) task->SetTriggerSelection(AliVEvent::kEMCEJE, "EJ1");
  else if(contains(trgstr, "EJ2")) task->SetTriggerSelection(AliVEvent::kEMCEJE, "EJ2");
  else if(contains(trgstr, "EG1")) task->SetTriggerSelection(AliVEvent::kEMCEGA, "EG1");
  else if(contains(trgstr, "EG2")) task->SetTriggerSelection(AliVEvent::kEMCEGA, "EG2");

  // Connect particle and cluster container
  AliTrackContainer *tracks(nullptr);
  AliClusterContainer *clusters(nullptr);
  TString trackcontname   = nametrackcont.data();
  TString clustercontname = nameclustercont.data();

  if(jettype == AliJetContainer::kChargedJet || jettype == AliJetContainer::kFullJet) {
    if(trackcontname == "usedefault") tracks = task->AddTrackContainer(AliEmcalAnalysisFactory::TrackContainerNameFactory(isAOD));
    else tracks = task->AddTrackContainer(trackcontname.Data());
    tracks->SetMinPt(0.15);
  }

  if(jettype == AliJetContainer::kNeutralJet || jettype == AliJetContainer::kFullJet){
    if(clustercontname == "usedefault") clusters = task->AddClusterContainer(AliEmcalAnalysisFactory::ClusterContainerNameFactory(isAOD));
    else clusters = task->AddClusterContainer(clustercontname.Data());
    clusters->SetDefaultClusterEnergy(energydef);
    clusters->SetClusUserDefEnergyCut(energydef, 0.3);
  }


  // Create proper jet container
  auto jetcont = task->AddJetContainer(jettype, AliJetContainer::antikt_algorithm, recoscheme, radius, acctype, tracks, clusters);
  jetcont->SetName("datajets");
  jetcont->SetRhoName(nrho.data());
  task->SetNameJetContainer("datajets");
  std::cout << "Adding jet container with underlying array:" << jetcont->GetArrayName() << std::endl;

  if(isMC){
    // Create also particle and particle level jet container for outlier rejection
    TString partcontname = namepartcont.data();
    if(partcontname == "usedefault") partcontname = "mcparticles";
    auto partcont = task->AddMCParticleContainer(partcontname.Data());
    partcont->SetMinPt(0.);

    //AliJetContainer::EJetType_t mcjettype = (jettype == AliJetContainer::kNeutralJet) ? AliJetContainer::kFullJet : jettype;
    AliJetContainer::EJetType_t mcjettype = AliJetContainer::kFullJet;
    auto pjcont = task->AddJetContainer(mcjettype, AliJetContainer::antikt_algorithm, recoscheme, radius, AliJetContainer::kTPCfid, partcont, nullptr);
    pjcont->SetName("partjets");
    pjcont->SetMinPt(0);
    pjcont->SetMaxTrackPt(1000.);
  }

  // Link input and output container
  outfilename << mgr->GetCommonFileName() << ":JetSpectrum_" << tag.str().data();
  mgr->ConnectInput(task, 0, mgr->GetCommonInputContainer());
  mgr->ConnectOutput(task, 1, mgr->CreateContainer(Form("JetSpectrum_%s", tag.str().data()), TList::Class(), AliAnalysisManager::kOutputContainer, outfilename.str().data()));

  return task;
}

AliAnalysisTaskEmcalJetEnergySpectrum *AliAnalysisTaskEmcalJetEnergySpectrum::AddTaskJetEnergySpectrum(Bool_t isMC, AliJetContainer::EJetType_t jettype, AliJetContainer::ERecoScheme_t recoscheme, AliVCluster::VCluUserDefEnergy_t energydef, double radius, EMCAL_STRINGVIEW namepartcont, EMCAL_STRINGVIEW trigger, EMCAL_STRINGVIEW suffix){
  return AliAnalysisTaskEmcalJetEnergySpectrum::AddTaskJetEnergySpectrumBase(isMC, jettype, recoscheme, energydef, radius, namepartcont, trigger, "usedefault", "usedefault", suffix);
}

AliAnalysisTaskEmcalJetEnergySpectrum *AliAnalysisTaskEmcalJetEnergySpectrum::AddTaskJetEnergySpectrum(Bool_t isMC, AliJetContainer::EJetType_t jettype, AliJetContainer::ERecoScheme_t recoscheme, AliVCluster::VCluUserDefEnergy_t energydef, double radius, EMCAL_STRINGVIEW namepartcont, EMCAL_STRINGVIEW trigger, EMCAL_STRINGVIEW nametrackcont, EMCAL_STRINGVIEW nameclustercont, EMCAL_STRINGVIEW suffix){
  return AliAnalysisTaskEmcalJetEnergySpectrum::AddTaskJetEnergySpectrumBase(isMC, jettype, recoscheme, energydef, radius, namepartcont, trigger, nametrackcont, nameclustercont, suffix);
}

AliAnalysisTaskEmcalJetEnergySpectrum *AliAnalysisTaskEmcalJetEnergySpectrum::AddTaskJetEnergySpectrumBkgSub(Bool_t isMC, AliJetContainer::EJetType_t jettype, AliJetContainer::ERecoScheme_t recoscheme, AliVCluster::VCluUserDefEnergy_t energydef, double radius, EMCAL_STRINGVIEW namepartcont, EMCAL_STRINGVIEW trigger, EMCAL_STRINGVIEW nrho, EMCAL_STRINGVIEW suffix){
  return AliAnalysisTaskEmcalJetEnergySpectrum::AddTaskJetEnergySpectrumBkgSubBase(isMC, jettype, recoscheme, energydef, radius, namepartcont, trigger, nrho, "usedefault", "usedefault", suffix);
}

AliAnalysisTaskEmcalJetEnergySpectrum *AliAnalysisTaskEmcalJetEnergySpectrum::AddTaskJetEnergySpectrumBkgSub(Bool_t isMC, AliJetContainer::EJetType_t jettype, AliJetContainer::ERecoScheme_t recoscheme, AliVCluster::VCluUserDefEnergy_t energydef, double radius, EMCAL_STRINGVIEW namepartcont, EMCAL_STRINGVIEW trigger, EMCAL_STRINGVIEW nrho, EMCAL_STRINGVIEW nametrackcont, EMCAL_STRINGVIEW nameclustercont, EMCAL_STRINGVIEW suffix){
  return AliAnalysisTaskEmcalJetEnergySpectrum::AddTaskJetEnergySpectrumBkgSubBase(isMC, jettype, recoscheme, energydef, radius, namepartcont, trigger, nrho, nametrackcont, nameclustercont, suffix);
}
