#ifndef ALIANLYSISTASKGAMMAPUREMC_cxx
#define ALIANLYSISTASKGAMMAPUREMC_cxx

#include "AliAnalysisTaskSE.h"
#include "AliAnalysisManager.h"

#include <fastjet/PseudoJet.hh>
#include <fastjet/ClusterSequenceArea.hh>


class AliAnalysisTaskGammaPureMC : public AliAnalysisTaskSE {
  public:
    /**
     * @enum AcceptanceType_t
     * @brief Enumeration for acceptance type
     */
    enum AcceptanceType_t {
      kPCMAcceptance = 1,       //!< PCM Acceptance
      kEMCALAcceptance = 2,     //!< EMCAL Acceptance
      kPHOSAcceptance = 3       //!< PHOS Acceptance
    };

    /**
     * @enum SupportedPdg_t
     * @brief Definition of constants for PDG codes used within the task
     */
    enum SupportedPdg_t {
      kPdgPi0 = 111,           //!< kPdgPi0
      kPdgRho0 = 113,          //!< kPdgRho0
      kPdgK0Long = 130,        //!< kPdgK0Long
      kPdgPiPlus = 211,        //!< kPdgPiPlus
      kPdgPiMinus = -211,      //!< kPdgPiMinus
      kPdgRhoPlus = 213,       //!< kPdgRhoPlus
      kPdgRhoMinus = -213,     //!< kPdgRhoMinus
      kPdgEta = 221,           //!< kPdgEta
      kPdgOmega = 223,         //!< kPdgOmega
      kPdgK0Short = 310,       //!< kPdgK0Short
      kPdgKStar = 313,         //!< kPdgKStar
      kPdgKPlus = 321,         //!< kPdgKPlus
      kPdgKMinus = -321,       //!< kPdgKMinus
      kPdgEtaPrime = 331,      //!< kPdgEtaPrime
      kPdgPhi = 333,           //!< kPdgPhi
      kPdgJPsi = 443,          //!< kPdgJPsi
      kPdgDeltaMinus = 1114,   //!< kPdgDeltaMinus
      kPdgDelta0 = 2114,       //!< kPdgDelta0s
      kPdgDeltaPlus = 2214,    //!< kPdgDeltaPlus
      kPdgDeltaPlusPlus = 2224,//!< kPdgDeltaPlusPlus
      kPdgSigmaMinus = 3112,   //!< kPdgSigmaMinus
      kPdgSigma0 = 3212,       //!< kPdgSigma0
      kPdgLambda = 3122,       //!< kPdgLambda
      kPdgSigmaPlus = 3222,    //!< kPdgSigmaPlus
      kPdgXiMinus = 3312,      //!< kPdgXiMinus
      kPdgXi0 = 3322,          //!< kPdgXi0
      kPdgd = 1,               //!< kPdgd
      kPdgu = 2,               //!< kPdgu
      kPdgs = 3,               //!< kPdgs
      kPdgc = 4,               //!< kPdgc
      kPdgb = 5,               //!< kPdgb
      kPdgt = 6,               //!< kPdgt
      kPdgg = 21,              //!< kPdgg
      kGamma = 22              //!< kGamma
    };

    AliAnalysisTaskGammaPureMC();
    AliAnalysisTaskGammaPureMC(const char *name);
    virtual ~AliAnalysisTaskGammaPureMC();

    virtual void   UserCreateOutputObjects();
    virtual void   UserExec(Option_t *);
    virtual void   Terminate(const Option_t*);

    // MC functions
    void SetIsMC(Int_t isMC){fIsMC=isMC;}
    void ProcessMCParticles();
    void ProcessMultiplicity();
    bool IsInPCMAcceptance(AliVParticle* part) const;
    bool IsInPHOSAcceptance(AliVParticle* part) const;
    bool IsInEMCalAcceptance(AliVParticle* part) const;
    bool IsInV0Acceptance(AliVParticle* part) const;
    bool IsSecondary(AliVParticle* motherParticle) const;
    void ProcessJets();
    bool IsParticleInJet(const std::vector<fastjet::PseudoJet>& vecJet, double eta, double phi, int& index, double& R);
    double GetFrag(const fastjet::PseudoJet& jet, const AliVParticle* part);

    // additional functions
    void SetLogBinningXTH1(TH1* histoRebin);
    void SetLogBinningXTH2(TH2* histoRebin);
    void SetIsK0(Int_t isK0){fIsK0 = isK0;}
    void SetMaxPt(Double_t pTmax){fMaxpT = pTmax;}
    void SetDoMultStudies(Int_t tmp){fDoMultStudies = tmp;}
    void SetDoJetStudies(int tmp) {fDoJetStudies = tmp;}
    void SetDoFeedDownStudies(int tmp) {fDoFeedDownStudies = tmp;}
    int ReturnFeedDownBinFromPDG(int pdgcode);
    bool isPhotonFromDecay(int pdgCodeMother);
    

  protected:
    TList*                fOutputContainer;           //! Output container
    // histograms events
    TH1F*                 fHistNEvents;               //! number of events histo
    TH1D*                 fHistXSection;              //! xSection
    TH1F*                 fHistPtHard;                //! ptHard
    // histograms mesons
    TH2F*                 fHistPtYPi0;                //! histo for Pi0s
    TH2F*                 fHistPtYPiPl;               //! histo for Pi+s
    TH2F*                 fHistPtYPiMi;               //! histo for Pi-s
    TH2F*                 fHistPtYEta;                //! histo for Etas
    TH2F*                 fHistPtYEtaPrime;           //! histo for EtaPrims
    TH2F*                 fHistPtYOmega;              //! histo for Omegas
    TH2F*                 fHistPtYRho0;               //! histo for rho0
    TH2F*                 fHistPtYRhoPl;              //! histo for rho+
    TH2F*                 fHistPtYRhoMi;              //! histo for rho-
    TH2F*                 fHistPtYPhi;                //! histo for phi
    TH2F*                 fHistPtYJPsi;               //! histo for J/psi
    TH2F*                 fHistPtYSigma0;             //! histo for Sigma0
    TH2F*                 fHistPtYK0s;                //! histo for K0s
    TH2F*                 fHistPtYK0l;                //! histo for K0l
    TH2F*                 fHistPtYK0star;             //! histo for K0*
    TH2F*                 fHistPtYDeltaPlPl;          //! histo for Delta++
    TH2F*                 fHistPtYDeltaPl;            //! histo for Delta+
    TH2F*                 fHistPtYDeltaMi;            //! histo for Delta-
    TH2F*                 fHistPtYDelta0;             //! histo for Delta0
    TH2F*                 fHistPtYLambda;             //! histo for Lambda
    TH2F*                 fHistPtYKPl;                //! histo for K+s
    TH2F*                 fHistPtYKMi;                //! histo for K-s
    TH2F*                 fHistPtYGamma;              //! histo for Photons
    TH2F*                 fHistPtYDirGamma;           //! histo for direct photons

    TH2F*                 fHistPtYPi0FromEta;         //! histo for Pi0s from Etas
    TH2F*                 fHistPtYPi0FromLambda;      //! histo for Pi0s from Lambdas
    TH2F*                 fHistPtYPi0FromK;           //! histo for Pi0s from Ks
    TH2F*                 fHistPtYPiPlFromK;          //! histo for Pi+s form Ks
    TH2F*                 fHistPtYPiMiFromK;          //! histo for Pi-s from Ks
    TH2F*                 fHistPtYPi0GG;              //! histo for Pi0s gamma gamma channel
    TH2F*                 fHistPtYPi0GGPCMAcc;        //! histo for Pi0s gamma gamma channel, PCM acceptance
    TH2F*                 fHistPtYPi0GGEMCAcc;        //! histo for Pi0s gamma gamma channel, EMCal acceptance
    TH2F*                 fHistPtYPi0GGPHOAcc;        //! histo for Pi0s gamma gamma channel, PHOS acceptance
    TH2F*                 fHistPtYPi0GGPCMEMCAcc;     //! histo for Pi0s gamma gamma channel, PCM-EMCal acceptance
    TH2F*                 fHistPtYPi0GGPCMPHOAcc;     //! histo for Pi0s gamma gamma channel, PCM-PHOS acceptance
    TH2F*                 fHistPtAlphaPi0GGPCMAcc;    //! histo for Pi0s gamma gamma channel, PCM acceptance
    TH2F*                 fHistPtAlphaPi0GGEMCAcc;    //! histo for Pi0s gamma gamma channel, EMCal acceptance
    TH2F*                 fHistPtAlphaPi0GGPHOAcc;    //! histo for Pi0s gamma gamma channel, PHOS acceptance
    TH2F*                 fHistPtAlphaPi0GGPCMEMCAcc; //! histo for Pi0s gamma gamma channel, PCM-EMCal acceptance
    TH2F*                 fHistPtAlphaPi0GGPCMPHOAcc; //! histo for Pi0s gamma gamma channel, PCM-PHOS acceptance

    TH2F*                 fHistPtYEtaGG;              //! histo for Etas gamma gamma channel
    TH2F*                 fHistPtYEtaGGPCMAcc;        //! histo for Etas gamma gamma channel, PCM acceptance
    TH2F*                 fHistPtYEtaGGEMCAcc;        //! histo for Etas gamma gamma channel, EMCal acceptance
    TH2F*                 fHistPtYEtaGGPHOAcc;        //! histo for Etas gamma gamma channel, PHOS acceptance
    TH2F*                 fHistPtYEtaGGPCMEMCAcc;     //! histo for Etas gamma gamma channel, PCM-EMCal acceptance
    TH2F*                 fHistPtYEtaGGPCMPHOAcc;     //! histo for Etas gamma gamma channel, PCM-PHOS acceptance
    TH2F*                 fHistPtAlphaEtaGGPCMAcc;    //! histo for Etas gamma gamma channel, PCM acceptance
    TH2F*                 fHistPtAlphaEtaGGEMCAcc;    //! histo for Etas gamma gamma channel, EMCal acceptance
    TH2F*                 fHistPtAlphaEtaGGPHOAcc;    //! histo for Etas gamma gamma channel, PHOS acceptance
    TH2F*                 fHistPtAlphaEtaGGPCMEMCAcc; //! histo for Etas gamma gamma channel, PCM-EMCal acceptance
    TH2F*                 fHistPtAlphaEtaGGPCMPHOAcc; //! histo for Etas gamma gamma channel, PCM-PHOS acceptance

    TH2F*                 fHistPtYEtaPrimeGG;          //! histo for Etas gamma gamma channel
    TH2F*                 fHistPtYEtaPrimeGGPCMAcc;    //! histo for EtaPrims gamma gamma channel, PCM acceptance
    TH2F*                 fHistPtYEtaPrimeGGEMCAcc;    //! histo for EtaPrims gamma gamma channel, EMCal acceptance
    TH2F*                 fHistPtYEtaPrimeGGPHOAcc;    //! histo for EtaPrims gamma gamma channel, PHOS acceptance
    TH2F*                 fHistPtYEtaPrimeGGPCMEMCAcc; //! histo for EtaPrims gamma gamma channel, PCM-EMCal acceptance
    TH2F*                 fHistPtYEtaPrimeGGPCMPHOAcc; //! histo for Pi0s gamma gamma channel, PCM-PHOS acceptance
    TH2F*                 fHistPtAlphaEtaPrimeGGPCMAcc;    //! histo for Eta's gamma gamma channel, PCM acceptance
    TH2F*                 fHistPtAlphaEtaPrimeGGEMCAcc;    //! histo for Eta's gamma gamma channel, EMCal acceptance
    TH2F*                 fHistPtAlphaEtaPrimeGGPHOAcc;    //! histo for Eta's gamma gamma channel, PHOS acceptance
    TH2F*                 fHistPtAlphaEtaPrimeGGPCMEMCAcc; //! histo for Eta's gamma gamma channel, PCM-EMCal acceptance
    TH2F*                 fHistPtAlphaEtaPrimeGGPCMPHOAcc; //! histo for Eta's gamma gamma channel, PCM-PHOS acceptance

    TH2F*                 fHistPtYPi0FromKGG;         //! histo for Pi0 from K gamma gamma channel
    TH2F*                 fHistPtYPi0FromKGGPCMAcc;   //! histo for Pi0 from K gamma gamma channel, PCM acceptance
    TH2F*                 fHistPtYPi0FromKGGEMCAcc;   //! histo for Pi0 from K gamma gamma channel, EMC acceptance
    TH2F*                 fHistPtYPi0FromKGGPCMEMCAcc;//! histo for Pi0 from K gamma gamma channel, PCM-EMC acceptance
    TH2F*                 fHistPtYPi0FromKGGEMCPCMAcc;//! histo for Pi0 from K gamma gamma channel, EMC-PCM acceptance
    TH2F*                 fHistPtYPi0FromKGGEMCAccSamePi0;//! histo for Pi0 from K gamma gamma channel, acceptance by same pi0
    TH2F*                 fHistPtYPi0FromKGGEMCAccDiffPi0;//! histo for Pi0 from K gamma gamma channel, mixed acceptance

    TH2F*                 fHistPtAlphaPi0FromKGG;             //! histo for Pi0 from K gamma gamma channel (Alpha)
    TH2F*                 fHistPtAlphaPi0FromKGGPCMAcc;       //! histo for Pi0 from K gamma gamma channel, PCM acceptance (Alpha)
    TH2F*                 fHistPtAlphaPi0FromKGGEMCAcc;       //! histo for Pi0 from K gamma gamma channel, EMC acceptance (Alpha)
    TH2F*                 fHistPtAlphaPi0FromKGGPCMEMCAcc;    //! histo for Pi0 from K gamma gamma channel, PCM-EMC acceptance (Alpha)
    TH2F*                 fHistPtAlphaPi0FromKGGEMCPCMAcc;    //! histo for Pi0 from K gamma gamma channel, EMC-PCM acceptance (Alpha)
    TH2F*                 fHistPtAlphaPi0FromKGGEMCAccSamePi0;//! histo for Pi0 from K gamma gamma channel, acceptance by same pi0 (Alpha)
    TH2F*                 fHistPtAlphaPi0FromKGGEMCAccDiffPi0;//! histo for Pi0 from K gamma gamma channel, mixed acceptance (Alpha)

    TH1D*                 fHistV0Mult;                        //! histo for Pi0 pt vs V0 multiplicity
    TH2F*                 fHistPtV0MultPi0GG;                 //! histo for Pi0 pt vs V0 multiplicity
    TH2F*                 fHistPtV0MultEtaGG;                 //! histo for Eta pt vs V0 multiplicity
    TH2F*                 fHistPtV0MultEtaPrimeGG;            //! histo for EtaPrime pt vs V0 multiplicity
    TH2F*                 fHistPtV0MultPi0GGPrompt;           //! histo for prompt Pi0 pt vs V0 multiplicity
    TH2F*                 fHistPtV0MultPi0GGFromEta;          //! histo for Pi0 from eta pt vs V0 multiplicity
    TH2F*                 fHistPtV0MultPi0GGFromOmega;        //! histo for Pi0 from omega pt vs V0 multiplicity
    TH2F*                 fHistPtV0MultPi0GGFromRest;         //! histo for Pi0 from rest pt vs V0 multiplicity
    TH2F*                 fHistPtV0MultPi0GGFromRho;          //! histo for Pi0 from rho pt vs V0 multiplicity
    TH2F*                 fHistPtV0MultEtaGGPrompt;           //! histo for ptompt eta pt vs V0 multiplicity
    TH2F*                 fHistPtV0MultEtaGGFromEtaPrim;      //! histo for eta from eta' pt vs V0 multiplicity
    TH2F*                 fHistPtV0MultEtaGGFromRest;         //! histo for eta from rest pt vs V0 multiplicity
    TH2F*                 fHistPtV0MultGamma;                 //! histo for Photon pt vs V0 multiplicity
    TH2F*                 fHistPtV0MultDirGamma;              //! histo for direct Photon(not from decay ) pt vs V0 multiplicity
    TH2F*                 fHistPtV0MultChargedPi;             //! histo for Pi0 pt vs V0 multiplicity

    // jet studies
    TH2F*                 fHistPi0PtJetPt;        //! histo for Pi0 pt vs jet Pt
    TH2F*                 fHistEtaPtJetPt;        //! histo for Eta pt vs jet Pt
    TH2F*                 fHistOmegaPtJetPt;      //! histo for Omega pt vs jet Pt
    TH2F*                 fHistEtaPrimPtJetPt;    //! histo for Eta prime pt vs jet Pt
    TH2F*                 fHistGammaPtJetPt;      //! histo for Gamma pt vs jet Pt
    TH2F*                 fHistGammaDirPtJetPt;   //! histo for Gamma Dir pt vs jet Pt

    TH2F*                 fHistPi0ZJetPt;        //! histo for Pi0 pt vs jet Pt
    TH2F*                 fHistEtaZJetPt;        //! histo for Eta pt vs jet Pt
    TH2F*                 fHistOmegaZJetPt;      //! histo for Omega pt vs jet Pt
    TH2F*                 fHistEtaPrimZJetPt;    //! histo for Eta prime pt vs jet Pt
    TH2F*                 fHistGammaZJetPt;      //! histo for Gamma pt vs jet Pt
    TH2F*                 fHistGammaDirZJetPt;   //! histo for Gamma dir pt vs jet Pt

    TH2F*                 fHistJetPtY;  //! histo for jet pt
    TH1D*                 fHistJetEta;  //! histo for jet eta
    TH1D*                 fHistJetPhi;  //! histo for jet phi

	  Int_t				          fIsK0;					  // k0 flag
    Int_t                 fIsMC;            // MC flag
    Double_t              fMaxpT;           // Max pT flag
    Int_t                 fDoMultStudies;   // enable multiplicity dependent studies (0 -> No mult studies, 1 -> Mult estimation with V0, 2 -> Mult estimation with V0 and INEL>0 criterium for multiplicity)
    Int_t                 fNTracksInV0Acc;  // number of tracks in V0A+C acceptance for multiplicity studies
    Bool_t                fIsEvtINELgtZERO; // flag if event is INEL>0

    int fDoFeedDownStudies;                 // 0 = off, 1 = on
    TH2F* fHistPtPi0FromDecay;              //! histo listing which mesons decayed into pi0
    TH2F* fHistPtEtaFromDecay;              //! histo listing which mesons decayed into eta
    TH2F* fHistPtOmegaFromDecay;            //! histo listing which mesons decayed into omega
    TH2F* fHistPtYPi0Primordial;            //! histo for Pi0s that did not decay from one of the listed mesons
    TH2F* fHistPtYEtaPrimordial;            //! histo for Etas that did not decay from one of the listed mesons
    TH2F* fHistPtYOmegaPrimordial;          //! histo for Omegas that did not decay from one of the listed mesons

    // jet finding
    int fDoJetStudies;                                      // 0 = off, 1 = standard, 2 = stable pi0 and eta for jet finder
    double fJetRadius;                                      // jet radius parameter
    double fJetMinE;                                        // minimum jet energy
    double fJetAccEta;                                      // eta acceptance of jet
    double fJetParticleAcc;                                 // acceptance of particles that contribute to jet for pt spectra
    double fJetParticleAccFF;                               // acceptance of particles that contribute to jet for FF
    fastjet::JetAlgorithm   	    fJetAlgorithm;            // jet algorithm
    fastjet::Strategy 		        fJetStrategy;             // jet strategy parameter
    fastjet::AreaType 		        fJetAreaType;             // jet rea type parameter
    fastjet::RecombinationScheme  fJetRecombScheme;         // jet recomb. scheme parameter
    double fJetGhostArea;                                   // jet ghost area
    double fGhostEtaMax;                                    // maximum eta of ghost particles
    int fActiveAreaRepeats;                                 // jet active area
    fastjet::AreaType 		  fAreaType;                      // jet area type
    std::vector<fastjet::PseudoJet> fVecJets;               //! vector containing all reconstructed jets
     

  private:
    AliAnalysisTaskGammaPureMC(const AliAnalysisTaskGammaPureMC&); // Prevent copy-construction
    AliAnalysisTaskGammaPureMC &operator=(const AliAnalysisTaskGammaPureMC&); // Prevent assignment

    ClassDef(AliAnalysisTaskGammaPureMC, 14);
};

#endif
