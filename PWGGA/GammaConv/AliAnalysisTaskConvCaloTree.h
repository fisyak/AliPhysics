#ifndef AliAnalysisConvCaloTree_cxx
#define AliAnalysisConvCaloTree_cxx

#include "AliAnalysisTaskSE.h"
#include "AliConversionPhotonBase.h"
#include "AliAnalysisTaskConvJet.h"
#include "TH1.h"
#include "TH2.h"
#include "TString.h"
#include "TTreeStream.h"
#include "AliLog.h"
#include <vector>
#include "AliV0ReaderV1.h"
#include "AliCaloPhotonCuts.h"
#include "AliConvEventCuts.h"
#include "AliConversionPhotonCuts.h"
#include "AliConversionMesonCuts.h"
#include "TList.h"
#include "AliMCEvent.h"
#include "TClonesArray.h"

using namespace std;

class AliAnalysisTaskConvCaloTree : public AliAnalysisTaskSE
{

 public:
  AliAnalysisTaskConvCaloTree();
  AliAnalysisTaskConvCaloTree(const char* name);
  virtual ~AliAnalysisTaskConvCaloTree();

  virtual void UserCreateOutputObjects();
  virtual Bool_t Notify();
  virtual void UserExec(Option_t* option);
  virtual void Terminate(Option_t*);

  void SetV0Reader(AliV0ReaderV1* v0Reader) { fV0Reader = v0Reader; }
  void SetV0ReaderName(TString name) { fV0ReaderName = name; }
  void SetJetReaderName(TString name) { fAddNameConvJet = name; }
  void SetIsoTask(AliPhotonIsolation* isoTask) { fCaloIsolation = isoTask; }
  void SetIsoTaskName(TString name) { fCaloIsolationName = name; }
  void SetEventCuts(AliConvEventCuts* conversionCuts,
                    Bool_t IsHeavyIon)
  {
    fEventCuts = conversionCuts;
    fIsHeavyIon = IsHeavyIon;
  }
  void SetClusterCutsEMC(AliCaloPhotonCuts* clusterCuts) { fClusterCutsEMC = clusterCuts; }
  void SetClusterCutsPHOS(AliCaloPhotonCuts* clusterCuts) { fClusterCutsPHOS = clusterCuts; }
  void SetConversionCuts(AliConversionPhotonCuts* convCuts) { fConversionCuts = convCuts; }
  void SetMesonCuts(AliConversionMesonCuts* mesonCuts) { fMesonCuts = mesonCuts; }
  void SetIsMC(Int_t isMC) { fIsMC = isMC; }
  void SetSaveMCInformation(int val) { fSaveMCInformation = val; }
  void SetSaveClusters(Bool_t val) { fSaveClusters = val; }
  void SetSaveConversions(Int_t val) { fSaveConversions = val; }
  void SetSaveTracks(Int_t val) { fSaveTracks = val; }
  void SetUseClusterIsolation(Bool_t val) { fUseClusterIsolation = val; }
  void SetSaveJets(int val) { fSaveJets = val; }
  void SetSaveV0Sectors(bool val) { fSaveV0Sectors = val; }
  void SetStoreTriggerCond(int val) { fStoreTriggerCondition = val; }

  void SetMinTrackPt(Double_t tmp) { fMinTrackPt = tmp; }

  void SetCorrectionTaskSetting(TString setting) { fCorrTaskSetting = setting; }

  void SetDownscalingFac(double tmp)
  {
    if (tmp < 0.99) {
      fDoDownscale = true;
    }
    fDownscaleFac = tmp;
  }

 private:
  AliAnalysisTaskConvCaloTree(const AliAnalysisTaskConvCaloTree&);            // Prevent copy-construction
  AliAnalysisTaskConvCaloTree& operator=(const AliAnalysisTaskConvCaloTree&); // Prevent assignment

  void ProcessClustersAOD();
  void ProcessConversionsAOD();
  void ProcessTracksAOD();
  bool IsMCParticleRelevant(AliAODMCParticle* particle);
  void ProcessAODMCParticles();
  void FillConversionsTree();
  void RelabelAODPhotonCandidates(Bool_t mode);
  void ResetBuffer();
  void ResetBufferVectors();

  void InitJets();

 protected:
  AliV0ReaderV1* fV0Reader;               ///< V0 Reader
  TString fV0ReaderName;                  ///< V0 Reader Name
  TString fAddNameConvJet;                ///< Jet reader additional name
  AliAnalysisTaskConvJet* fConvJetReader; ///< Jet reader
  AliPhotonIsolation* fCaloIsolation;     ///< Isolation Task
  TString fCaloIsolationName;             ///< Photon Isolation task Name
  TClonesArray* fReaderGammas;            ///< Array with conversion photons selected by V0Reader Cut
  AliPIDResponse* fPIDResponse;           ///< PID response
  TString fCorrTaskSetting;               ///<
  AliVEvent* fInputEvent;                 ///< pointer to event
  AliMCEvent* fMCEvent;                   ///< pointer to MC event
  Double_t fWeightJetJetMC;               ///< weight for Jet-Jet MC
  AliConvEventCuts* fEventCuts;           ///< Event cuts
  AliEventCuts fAliEventCuts;
  AliCaloPhotonCuts* fClusterCutsEMC;       ///< cluster cuts for EDC clusters
  AliCaloPhotonCuts* fClusterCutsPHOS;      ///< cluster cuts for PHOS clusters
  AliCaloPhotonCuts* fClusterCuts;          ///< cluster cuts for either PHOS or EDC clusters
  AliConversionPhotonCuts* fConversionCuts; ///< conversion cuts
  AliConversionMesonCuts* fMesonCuts;       ///< meson cuts
  TTree* fPhotonTree;                       ///<
  Bool_t fIsHeavyIon;                       ///<
  TClonesArray* fAODMCTrackArray;           ///< AOD track array
  TClonesArray* farrClustersProcess;        ///< cluster array
  TList* fOutputList;                       ///<
  Int_t fIsMC;                              ///<
  std::vector<Int_t> fMCEventPos;           ///< AOD relabeling
  std::vector<Int_t> fMCEventNeg;           ///< AOD relabeling
  std::vector<Int_t> fESDArrayPos;          ///< AOD relabeling
  std::vector<Int_t> fESDArrayNeg;          ///< AOD relabeling

  int fSaveMCInformation;      ///< flag to decide to save MC information (100 only histograms, 2 = save relevant mc particles, 3 = save all mc particles)
  Bool_t fSaveClusters;        ///< flag to decide to save clusters
  Int_t fSaveConversions;      ///< flag to decide to save conversions
  Int_t fSaveTracks;           ///< flag to decide to save tracks
  bool fStoreTriggerCondition; ///< flag to decide if trigger condition (mostly emcal) should be stored
  Bool_t fUseClusterIsolation; ///< flag to decide if isolation should be checked
  int fSaveJets;               ///< flag to decide if jets should be saved, if set to 2, only events with jets are saved
  bool fSaveV0Sectors;         ///< flag to decide if detailed V0-sector information should be stored in the tree (for flow analyses)
  bool fDoDownscale;           ///< flag to switch on downscaling
  double fDownscaleFac;        ///< downscaling factor (1= no downscaling, 0 = no events accepted)
  // track cuts
  Double_t fMinTrackPt; //!<! minimum required track pT

  TList* fInfoList; //!<! list to store MC and event info
  // histograms for InfoList
  TH1F* fHistoNEvents;      //!<! number of events
  TH2F* fHistoMCGammaPt;    //!<! photon pt vs. y
  TH2F* fHistoMCElecPt;     //!<! elec. pt vs. y
  TH2F* fHistoMCPi0Pt;      //!<! pi0 pt vs. y
  TH2F* fHistoMCEtaPt;      //!<! eta pt vs. y
  TH2F* fHistoMCOmegaPt;    //!<! omega pt vs. y
  TH2F* fHistoMCEtaPrimePt; //!<! etaprime pt vs. y
  TH2F* fHistoMCK0sPt;      //!<! K0s pt vs. y
  TH2F* fHistoMCSecPi0Pt;   //!<! Sec pi0 pt vs. y
  TH2F* fHistoMCSecEtaPt;   //!<! Sec. eta pt vs. y

  // Buffers that will be added to the tree
  Float_t fBuffer_EventWeight;                  //!<! array buffer to store event weights
  Float_t fBuffer_Event_Vertex_Z;               //!<! array buffer store Vertex Z information
  float fMCQ2;                                  //!<! Q2 (pthard) of collision
  float fV0Mult;                                //!<! V0 multiplicity percentile
  std::vector<unsigned short> fV0MultSectors;   //!<! V0C (0-31) and V0A (32-63)
  unsigned int fTriggerBits;                    //!<! Mostly relevant for MC from std::bitset(0=INT7, 1=EMC7, 2=DMC7, 3=EG2, 4=DG2, 5=EG1, 6=DG1, 7=EJ2, 8=DJ2, 9=EJ1, 10=DJ1, 11=INEL>0, 12=INEL>0 true)

  std::vector<Float_t> fVBuffer_Cluster_E;                   //!<! vector buffer  Calo (PHOS or EDC) cluster energy
  std::vector<Float_t> fVBuffer_Cluster_Eta;                 //!<! vector buffer  Calo (PHOS or EDC) cluster eta
  std::vector<Float_t> fVBuffer_Cluster_Phi;                 //!<! vector buffer  Calo (PHOS or EDC) cluster phi
  std::vector<Short_t> fVBuffer_Cluster_NCells;              //!<! vector buffer  Calo (PHOS or EDC) cluster NCells
  std::vector<Short_t> fVBuffer_Cluster_M02;                 //!<! vector buffer  Calo (PHOS or EDC) cluster M02 * 1000
  std::vector<Short_t> fVBuffer_Cluster_Fcross;              //!<! vector buffer  Calo (PHOS or EDC) cluster Exoticity *1000
  std::vector<Short_t> fVBuffer_Cluster_Time;                //!<! vector buffer  Calo (PHOS or EDC) cluster Time
  std::vector<Bool_t> fVBuffer_Cluster_Exotic;               //!<! vector buffer  Calo (PHOS or EDC) cluster Exotic
  std::vector<Bool_t> fVBuffer_Cluster_Isolated;             //!<! vector buffer  Calo (PHOS or EDC) cluster Isolated
  std::vector<Short_t> fVBuffer_Cluster_CellTimes;           //!<! vector buffer  Calo (PHOS or EDC) cluster cell times (multiple clusters within one vector! So keept track of ncells per cluster)
  std::vector<Float_t> fVBuffer_Cluster_CellEnergies;        //!<! vector buffer  Calo (PHOS or EDC) cluster cell energies (multiple clusters within one vector! So keept track of ncells per cluster)
  std::vector<Short_t> fVBuffer_Cluster_CellIDs;             //!<! vector buffer  Calo (PHOS or EDC) cluster cell Ids (multiple clusters within one vector! So keept track of ncells per cluster)
  std::vector<unsigned short> fVTrueClusterPi0DaughterIndex; //!<! vector buffer   store the MC stack ID of mother pi0 for true information
  std::vector<unsigned short> fVTrueClusterEtaDaughterIndex; //!<! vector buffer   store the MC stack ID of mother eta for true information
  std::vector<unsigned short> fVTrueClusterSecondaryIndex;   //!<! vector buffer  check weather cluster originates from seconda
  std::vector<Short_t> fVTrueClusterMCId;                    //!<! vector buffer   store the MC PDG code of mother eta for true information
  std::vector<Short_t> fVTrueClusterMCId2;                   //!<! vector buffer  store the MC PDG code of MC Id of particle with 2nd most energy
  std::vector<Float_t> fVTrueClusterMCTrueEnergy;            //!<! vector buffer   store the true MC energy info
  std::vector<Float_t> fVTrueClusterMCTrueEnergy2;           //!<! vector buffer   store the MC energy of MC Id of particle with 2nd most energy
  std::vector<Bool_t> fVTrueClusterMCIsMerged;               //!<! vector buffer   store the info if cluster is merged cluster
  std::vector<Short_t> fVTrueClusterConvRadius;              //!<! vector buffer  store the radius of the production vertex of a converion cluster

  std::vector<Float_t> fVBuffer_Conv_px;             //!<! vector buffer  Conversion photon px
  std::vector<Float_t> fVBuffer_Conv_py;             //!<! vector buffer  Conversion photon py
  std::vector<Float_t> fVBuffer_Conv_pz;             //!<! vector buffer  Conversion photon pz
  std::vector<Float_t> fVBuffer_Elec1etaCalo;        //!<! vector buffer  eta of electron 1 from photon conversion on Calo surface
  std::vector<Float_t> fVBuffer_Elec2etaCalo;        //!<! vector buffer  eta of electron 2 from photon conversion on Calo surface
  std::vector<Float_t> fVBuffer_Elec1phiCalo;        //!<! vector buffer  phi of electron 1 from photon conversion on Calo surface
  std::vector<Float_t> fVBuffer_Elec2phiCalo;        //!<! vector buffer  phi of electron 2 from photon conversion on Calo surface
  std::vector<UShort_t> fVBuffer_Conv_R;             //!<! vector buffer  Radius of conversion point (multiplied by 10)
  std::vector<Short_t> fVBuffer_Conv_PsiPair;        //!<! vector buffer  Psi pair (multiplied by 1000)
  std::vector<UShort_t> fVBuffer_Conv_NTPCClusElec1; //!<! vector buffer  Number of TPC cluster elec 1
  std::vector<UShort_t> fVBuffer_Conv_NTPCClusElec2; //!<! vector buffer  Number of TPC cluster elec 2
  std::vector<Short_t> fVBuffer_Conv_dEdxElec1;      //!<! vector buffer  NSigma dedx of electron 1 (multiplied by 100)
  std::vector<Short_t> fVBuffer_Conv_dEdxElec2;      //!<! vector buffer  NSigma dedx of electron 2 (multiplied by 100)
  std::vector<Short_t> fVBuffer_Conv_PElec1;         //!<! vector buffer  Momentum of electron 1 (multiplied by 100)
  std::vector<Short_t> fVBuffer_Conv_PElec2;         //!<! vector buffer  Momentum of electron 2 (multiplied by 100)
  std::vector<Short_t> fVBuffer_Conv_Quality;        //!<! vector buffer  photon quality
  std::vector<Short_t> fVBuffer_Conv_CosPAngle;      //!<! vector buffer  pointing angle (multiplied by 1000)
  std::vector<UShort_t> fVBuffer_Conv_Chi2;          //!<! vector buffer  Chi2/ndf for photon (multiplied by 10)

  std::vector<unsigned short> fVTrueConvPi0DaughterIndex; //!<! vector buffer   store the MC stack ID of mother pi0 for true information
  std::vector<unsigned short> fVTrueConvEtaDaughterIndex; //!<! vector buffer   store the MC stack ID of mother eta for true information
  std::vector<unsigned short> fVTrueConvSecDaughterIndex; //!<! vector buffer   store the MC stack ID of mother secondary for true information

  std::vector<float> fVTrueConvMCTruePx;   //!<! vector buffer   store the MC true px
  std::vector<float> fVTrueConvMCTruePy;   //!<! vector buffer   store the MC true py
  std::vector<float> fVTrueConvMCTruePz;   //!<! vector buffer   store the MC true pz
  std::vector<UShort_t> fVTrueConvMCLabel; //!<! vector buffer   store the MC label

  std::vector<Short_t> fVBuffer_Track_px;               //!<! vector buffer: track px (*100)
  std::vector<Short_t> fVBuffer_Track_py;               //!<! vector buffer: track py (*100)
  std::vector<Short_t> fVBuffer_Track_pz;               //!<! vector buffer: track pz (*100)
  std::vector<Short_t> fVBuffer_Track_P;                //!<! vector buffer: track P  (*100)
  std::vector<unsigned short> fVBuffer_Track_dedx;      //!<! vector buffer: track dedx (*10)
  std::vector<unsigned short> fVBuffer_Track_TOFSignal; //!<! vector buffer: TOF signal (*1000)
  std::vector<unsigned short> fVBuffer_Track_DCA;       //!<! vector buffer: track DCA (*100)
  std::vector<unsigned short> fVBuffer_Track_FracNClus; ///!<! vector buffer: Fraction of numbers of findable clusters * 100 + Chi2*1000
  std::vector<short> fVBuffer_Track_PDG;                //!<! vector buffer: track PDG code
  std::vector<unsigned short> fVBuffer_Track_StackID;   //!<! vector buffer: track associated MC stack ID
  std::vector<Short_t> fVBuffer_Track_Calo_eta;         //!<! vector buffer: track eta on Calo surface (*10000)
  std::vector<UShort_t> fVBuffer_Track_Calo_phi;        //!<! vector buffer: track eta on Calo surface (*10000)

  std::vector<float> fVBuffer_Jet_Pt;              //!<! vector buffer: Jet pt
  std::vector<short> fVBuffer_Jet_Eta;             //!<! vector buffer: Jet eta /1000
  std::vector<short> fVBuffer_Jet_Phi;             //!<! vector buffer: Jet phi /1000
  std::vector<unsigned short> fVBuffer_Jet_NNeutr; //!<! vector buffer: Jet number of neutral particles
  std::vector<unsigned short> fVBuffer_Jet_NCh;    //!<! vector buffer: Jet number of charged particles
  std::vector<unsigned short> fVBuffer_Jet_NEF;    //!<! vector buffer: neutral energy fraction / 1000

  std::vector<float> fVBuffer_TrueJet_Pt;        //!<! vector buffer: true Jet pt
  std::vector<short> fVBuffer_TrueJet_Eta;       //!<! vector buffer: true Jet eta
  std::vector<short> fVBuffer_TrueJet_Phi;       //!<! vector buffer: true Jet phi
  std::vector<float> fVBuffer_TrueJet_Parton_Pt; //!<! vector buffer: true Jet associated parton pT

  std::vector<unsigned short> fVBuffer_MCGenID;       //!<! vector buffer: MC stack id
  std::vector<short> fVBuffer_MCGenPDG;               //!<! vector buffer: MC pdg code
  std::vector<short> fVBuffer_MCGenPx;                //!<! vector buffer: px *100
  std::vector<short> fVBuffer_MCGenPy;                //!<! vector buffer: py *100
  std::vector<short> fVBuffer_MCGenPz;                //!<! vector buffer: pz *100
  std::vector<unsigned short> fVBuffer_MCGenMotherID; //!<! vector buffer: MC mother ID

  ClassDef(AliAnalysisTaskConvCaloTree, 14);
};

#endif
