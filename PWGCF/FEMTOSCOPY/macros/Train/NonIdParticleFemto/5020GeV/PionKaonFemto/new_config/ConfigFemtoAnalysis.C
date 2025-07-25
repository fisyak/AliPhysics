/*********************************************************************
 *                                                                   *
 * ConfigFemtoAnalysis.C - configuration macro for the femtoscopic   *
 * analysis, meant as a QA process for two-particle effects          *
 *                                                                   *
 * Author: Adam Kisiel (Adam.Kisiel@cern.ch)                         *
 *                                                                   *
 *********************************************************************/

#if !defined(__CINT__) || defined(__MAKECINT_)
#include "AliFemtoManager.h"
#include "AliFemtoEventReaderESDChain.h"
#include "AliFemtoEventReaderESDChainKine.h"
#include "AliFemtoEventReaderAODChain.h"
#include "AliFemtoSimpleAnalysis.h"
#include "AliFemtoBasicEventCut.h"
#include "AliFemtoESDTrackCut.h"
#include "AliFemtoCorrFctn.h"
#include "AliFemtoCutMonitorParticleYPt.h"
#include "AliFemtoCutMonitorParticleVertPos.h"
#include "AliFemtoCutMonitorParticleMomRes.h"
#include "AliFemtoCutMonitorParticlePID.h"
#include "AliFemtoCutMonitorEventMult.h"
#include "AliFemtoCutMonitorEventVertex.h"
#include "AliFemtoShareQualityTPCEntranceSepPairCut.h"
#include "AliFemtoPairCutAntiGamma.h"
#include "AliFemtoPairCutRadialDistance.h"
#include "AliFemtoQinvCorrFctn.h"
#include "AliFemtoShareQualityCorrFctn.h"
#include "AliFemtoTPCInnerCorrFctn.h"
#include "AliFemtoVertexMultAnalysis.h"
#include "AliFemtoCorrFctn3DSpherical.h"
#include "AliFemtoChi2CorrFctn.h"
#include "AliFemtoCorrFctnTPCNcls.h"
#include "AliFemtoBPLCMS3DCorrFctn.h"
#include "AliFemtoCorrFctn3DLCMSSym.h"
#include "AliFemtoModelBPLCMSCorrFctn.h"
#include "AliFemtoModelCorrFctn3DSpherical.h"
#include "AliFemtoModelGausLCMSFreezeOutGenerator.h"
#include "AliFemtoModelGausRinvFreezeOutGenerator.h"
#include "AliFemtoModelManager.h"
#include "AliFemtoModelWeightGeneratorBasic.h"
#include "AliFemtoModelWeightGeneratorLednicky.h"
#include "AliFemtoCorrFctnDirectYlm.h"
#include "AliFemtoModelCorrFctnDirectYlm.h"
#include "AliFemtoModelCorrFctnSource.h"
#include "AliFemtoCutMonitorParticlePtPDG.h"
#include "AliFemtoKTPairCut.h"
#include "AliFemtoCutMonitorCollections.h"
#include "AliFemtoCorrFctnNonIdDR.h"
#include "AliFemtoCorrFctnDPhiStarDEta.h"
#include "AliFemtoPairCutRadialDistanceAsymmetric.h"
#include "AliFemtoKKTrackCut.h"
#include "AliFemtoKKTrackCutFull.h"
#include "AliFemtoPairCutMergedFraction.h"
#include "AliFemtoCorrFctnDPhiStarKStarMergedFraction.h"
#include "AliFemtoCorrFctnDPhiStarKStarAverageMergedPointsFraction.h"
#include "AliFemtoBetaTPairCut.h"
#include "AliFemtoCutMonitorPairBetaT.h"
#include "AliFemtoPairkTPairCut.h"
#include "AliFemtoEventReaderAODMultSelection.h"
#include "AliFemtoCorrFctnPtKstarBeta.h"                                                                                                                                                                      
#endif

//_
AliFemtoManager* ConfigFemtoAnalysis(int runcentrality0, int runcentrality1, int runcentrality2, int runcentrality3, int runcentrality4,int runcentrality5, int runcentrality6, double Vz, int Vz_bin, double eta, double dcaxy, double dcaz, int ncls, int pid_default, int pid_loose, int pid_strict,  int runSHCorrFctn, int runNonIdCorrFctn, int paircutantigammaon, int paircutmergedfractionon, double distance, double fraction1, int runDPhiStarKStarMergedFraction, int runDPhiStarKStarAverageMergedPointsFraction, int runDPhiStarDEta, int turnOnMonitors, int turnOnBetaTMonitor, int runbetatdep, int runbetatylm, int runbetatnonid, int lmax, int no_mix_event, int ispileup, int trackpileup, double pTmin, double pTmax, int kt1, int kt2, int iskt, int isbeta) {

  double PionMass = 0.13957018;//0.13956995;
  double KaonMass = 0.493677;
  double ProtonMass = 0.938272013;

  const int numOfMultBins = 7;  
  const int numOfChTypes = 4;
  const int numOfkTbins = 7;

    int runmults[numOfMultBins] = {runcentrality0, runcentrality1, runcentrality2, runcentrality3, runcentrality4, runcentrality5, runcentrality6};
    int multbins[numOfMultBins + 1] = {0, 50, 100, 200, 300, 400, 500, 900};

  //  int runmults[numOfMultBins] = {runcentrality6, runcentrality7, runcentrality8};
  //int multbins[numOfMultBins + 1] = {500, 600, 700, 800};
  
  int runch[numOfChTypes] = {1, 1, 1, 1};
  const char *chrgs[numOfChTypes] = { "PIpKp", "PImKm", "PIpKm","PImKp"};
  //  double ktrngkt[numOfkTbins + 1];
  //  double ktrngbeta[numOfkTbins + 1];
  //int runktdep = 1;

double   ktrngkt[numOfkTbins + 1] = {0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 1.0, 1.5};		
double   ktrngbeta[numOfkTbins + 1] = {0.65, 0.7, 0.75, 0.8, 0.85, 0.9, 0.95, 1.0};
  
  int runshlcms = 0;// 0:PRF(PAP), 1:LCMS(PP,APAP)

  int runtype = 2; // Types 0 - global, 1 - ITS only, 2 - TPC Inner

  int gammacut = 1;
  double shqmax = 0.5;
  //if (runshlcms) shqmax = 2.0;
  //else shqmax = 0.9;

  int nbinssh = 100;

  //AliFemtoEventReaderAODChain *Reader = new AliFemtoEventReaderAODChain();
  AliFemtoEventReaderAODMultSelection *Reader = new AliFemtoEventReaderAODMultSelection();
  Reader->SetFilterBit(7);
  Reader->SetDCAglobalTrack(1);  
  Reader->SetUseAliEventCuts(kTRUE);

  Reader->SetUseMultiplicity(AliFemtoEventReaderAOD::kCentrality);
  Reader->SetEPVZERO(kTRUE);
  Reader->SetCentralityFlattening(kTRUE);
  Reader->SetPrimaryVertexCorrectionTPCPoints(kTRUE);

  if(trackpileup == 1){
    Reader->SetTrackPileUpRemoval(kTRUE);} 
  if(ispileup == 1){
  Reader->SetIsPileUpEvent(kTRUE);}
  //Reader->SetCentralityPreSelection(0.001, 950);

  AliFemtoManager* Manager=new AliFemtoManager();
  Manager->SetEventReader(Reader);

  const int size = numOfMultBins * numOfChTypes;
  AliFemtoVertexMultAnalysis    *anetaphitpc[size];
  AliFemtoBasicEventCut         *mecetaphitpc[size];
  AliFemtoCutMonitorEventMult   *cutPassEvMetaphitpc[size];
  AliFemtoCutMonitorEventMult   *cutFailEvMetaphitpc[size];
  AliFemtoCutMonitorEventVertex *cutPassEvVetaphitpc[size];
  AliFemtoCutMonitorEventVertex *cutFailEvVetaphitpc[size];
  AliFemtoCutMonitorCollections   *cutPassColletaphitpc[size];
  AliFemtoCutMonitorCollections   *cutFailColletaphitpc[size];
  AliFemtoESDTrackCut           *dtc1etaphitpc[size];
  //AliFemtoESDTrackCut           *dtc2etaphitpc[size];
  AliFemtoKKTrackCutFull            *dtc2etaphitpc[size];
  AliFemtoCutMonitorParticleYPt *cutPass1YPtetaphitpc[size];
  AliFemtoCutMonitorParticleYPt *cutFail1YPtetaphitpc[size];
  AliFemtoCutMonitorParticlePID *cutPass1PIDetaphitpc[size];
  AliFemtoCutMonitorParticlePID *cutFail1PIDetaphitpc[size];
  AliFemtoCutMonitorParticleYPt *cutPass2YPtetaphitpc[size];
  AliFemtoCutMonitorParticleYPt *cutFail2YPtetaphitpc[size];
  AliFemtoCutMonitorParticlePID *cutPass2PIDetaphitpc[size];
  AliFemtoCutMonitorParticlePID *cutFail2PIDetaphitpc[size];
  AliFemtoPairCutAntiGamma      *sqpcetaphitpc[size];
  //AliFemtoShareQualityTPCEntranceSepPairCut      *sqpcetaphitpc[20];
  //AliFemtoPairCutRadialDistance      *sqpcetaphitpc[size];
  //AliFemtoPairCutRadialDistanceAsymmetric *sqpcetaphitpc[size];
  //AliFemtoPairCutMergedFraction *sqpcetaphitpcmf[size];
  //AliFemtoPairCutRadialDistanceLM      *sqpcetaphitpcRD[size];
  AliFemtoCorrFctnDirectYlm     *cylmetaphitpc[size];
  //AliFemtoCorrFctnDEtaDPhi      *cdedpetaphi[size];
  //AliFemtoChi2CorrFctn          *cchiqinvetaphitpc[size];
  AliFemtoCorrFctnDPhiStarKStarMergedFraction *dphistarkstarmftpc[size];
  AliFemtoCorrFctnDPhiStarKStarAverageMergedPointsFraction *dphistarkstarampftpc[size];
  //AliFemtoKTPairCut             *ktpcuts[size*numOfkTbins];
  //  if (isbeta==1){	
  AliFemtoBetaTPairCut          *ktpcutsbeta[size*numOfkTbins];
//if (iskt==1){	
  AliFemtoPairkTPairCut          *ktpcutskt[size*numOfkTbins];
	
  AliFemtoCutMonitorPairBetaT   *cutpassbetatcutmonitor[size*numOfkTbins];
  AliFemtoCutMonitorPairBetaT   *cutfailbetatcutmonitor[size*numOfkTbins];
  //AliFemtoPairCutMergedFraction *ktpcuts[size*numOfkTbins];
  AliFemtoCorrFctnDirectYlm     *cylmkttpc[size*numOfkTbins];
  //AliFemtoQinvCorrFctn          *cqinvkttpc[size*numOfkTbins];
  AliFemtoCorrFctn3DLCMSSym     *cq3dlcmskttpc[size*numOfkTbins];
  //AliFemtoCorrFctn3DSpherical   *cq3dspherical[size*numOfkTbins];
  //AliFemtoCorrFctnTPCNcls       *cqinvnclstpc[size];
  //AliFemtoChi2CorrFctn          *cqinvchi2tpc[size];

  //AliFemtoModelGausLCMSFreezeOutGenerator *gausLCMSFreezeOutGenerator[size];
  //AliFemtoModelWeightGeneratorBasic      *weightGeneratorLednicky[size];
  //AliFemtoModelManager          *tModelManager[size];
  AliFemtoCorrFctnNonIdDR       *cnonidtpc[size];
  AliFemtoCorrFctnDPhiStarDEta  *cdphistardeta08[size];
  AliFemtoCorrFctnDPhiStarDEta  *cdphistardeta12[size];
  AliFemtoCorrFctnDPhiStarDEta  *cdphistardeta16[size];
  AliFemtoCorrFctnDPhiStarDEta  *cdphistardeta20[size];
  AliFemtoCorrFctnNonIdDR       *cnonidkttpc[size*numOfkTbins];
  AliFemtoCorrFctnPtKstarBeta       *ckstarpt[size*numOfkTbins];                                                                                                                                               


  // *** Begin pion-kaon analysis ***
  int aniter = 0;  

  for (int imult = 0; imult < numOfMultBins; imult++) {
    if (runmults[imult]) {

      for (int ichg = 0; ichg < numOfChTypes; ichg++) {
	if (runch[ichg]) {
	  
	  //Iterator:
	  aniter = ichg * numOfMultBins + imult;


	  //Mix events with respect to the z position of the primary vertex and event total multipliticy:
	  anetaphitpc[aniter] = new AliFemtoVertexMultAnalysis(Vz_bin, -Vz, Vz, 2, multbins[imult], multbins[imult+1]);
	  anetaphitpc[aniter]->SetNumEventsToMix(no_mix_event);
	  anetaphitpc[aniter]->SetMinSizePartCollection(1);
	  anetaphitpc[aniter]->SetVerboseMode(kFALSE);
	  
	  //Select basic cuts:
	  mecetaphitpc[aniter] = new AliFemtoBasicEventCut();
	  mecetaphitpc[aniter]->SetEventMult(0,100000);
	  mecetaphitpc[aniter]->SetVertZPos(-Vz,Vz);

	  //Study the multiplicity distribution:
	  if(turnOnMonitors == 1) {
	    cutPassEvMetaphitpc[aniter] = new AliFemtoCutMonitorEventMult(Form("cutPass%stpcM%i", chrgs[ichg], imult),500);
	    cutFailEvMetaphitpc[aniter] = new AliFemtoCutMonitorEventMult(Form("cutFail%stpcM%i", chrgs[ichg], imult),500);
	    mecetaphitpc[aniter]->AddCutMonitor(cutPassEvMetaphitpc[aniter], cutFailEvMetaphitpc[aniter]);
	  }
	  //Study the distribution and error of the primary vertex:
	  if(turnOnMonitors == 1) {
	    cutPassEvVetaphitpc[aniter] = new AliFemtoCutMonitorEventVertex(Form("cutPass%stpcM%i", chrgs[ichg], imult));
	    cutFailEvVetaphitpc[aniter] = new AliFemtoCutMonitorEventVertex(Form("cutFail%stpcM%i", chrgs[ichg], imult));
	    mecetaphitpc[aniter]->AddCutMonitor(cutPassEvVetaphitpc[aniter], cutFailEvVetaphitpc[aniter]);
	  }
	  //Study the multiplicity distribution:
	  if(turnOnMonitors == 1) {
	    cutPassColletaphitpc[aniter] = new AliFemtoCutMonitorCollections(Form("cutPass%stpcM%i", chrgs[ichg], imult));
	    cutFailColletaphitpc[aniter] = new AliFemtoCutMonitorCollections(Form("cutFail%stpcM%i", chrgs[ichg], imult));
	    mecetaphitpc[aniter]->AddCutMonitor(cutPassColletaphitpc[aniter], cutFailColletaphitpc[aniter]);
	  }

	  if(pid_default == 1) { 	  
	  //Basic track cut for pions:
	  dtc1etaphitpc[aniter] = new AliFemtoESDTrackCut();
	  dtc1etaphitpc[aniter]->SetNsigmaTPCTOF(true);
	  dtc1etaphitpc[aniter]->SetNsigma(3.0);
	  
	  //Basic track cut for kaons:
	  dtc2etaphitpc[aniter] = new AliFemtoKKTrackCutFull();
	  dtc2etaphitpc[aniter]->SetNsigmaTPCle250(2.0);
	  dtc2etaphitpc[aniter]->SetNsigmaTPC250_400(2.0);
	  dtc2etaphitpc[aniter]->SetNsigmaTPC400_450(1.0);
	  dtc2etaphitpc[aniter]->SetNsigmaTPC450_500(3.0);
	  dtc2etaphitpc[aniter]->SetNsigmaTOF450_500(2.0);
	  dtc2etaphitpc[aniter]->UseNsigmaTOF450_500(true);
	  dtc2etaphitpc[aniter]->SetNsigmaTPCge500(3.0);
	  dtc2etaphitpc[aniter]->SetNsigmaTOF500_800(2.0);
	  dtc2etaphitpc[aniter]->SetNsigmaTOF800_1000(1.5);
	  dtc2etaphitpc[aniter]->SetNsigmaTOFge1000(1.0);
	}

	  if(pid_strict == 1) { 	  

	  //Basic track cut for pions:
	  dtc1etaphitpc[aniter] = new AliFemtoESDTrackCut();
	  dtc1etaphitpc[aniter]->SetNsigmaTPCTOF(true);
	  dtc1etaphitpc[aniter]->SetNsigma(2.7);
	  
	  //Basic track cut for kaons:
	  dtc2etaphitpc[aniter] = new AliFemtoKKTrackCutFull();
	  dtc2etaphitpc[aniter]->SetNsigmaTPCle250(2.0);
	  dtc2etaphitpc[aniter]->SetNsigmaTPC250_400(2.0);
	  dtc2etaphitpc[aniter]->SetNsigmaTPC400_450(1.0);
	  dtc2etaphitpc[aniter]->SetNsigmaTPC450_500(3.0);
	  dtc2etaphitpc[aniter]->SetNsigmaTOF450_500(2.0);
	  dtc2etaphitpc[aniter]->UseNsigmaTOF450_500(true);
	  dtc2etaphitpc[aniter]->SetNsigmaTPCge500(3.0);
	  dtc2etaphitpc[aniter]->SetNsigmaTOF500_800(2.0);
	  dtc2etaphitpc[aniter]->SetNsigmaTOF800_1000(1.5);
	  dtc2etaphitpc[aniter]->SetNsigmaTOFge1000(1.0);
	}

	  if(pid_loose == 1) { 	  	  
	  //Basic track cut for pions:
	  dtc1etaphitpc[aniter] = new AliFemtoESDTrackCut();
	  dtc1etaphitpc[aniter]->SetNsigmaTPCTOF(true);
	  dtc1etaphitpc[aniter]->SetNsigma(3.3);
	  
	  //Basic track cut for kaons:
	  dtc2etaphitpc[aniter] = new AliFemtoKKTrackCutFull();
	  dtc2etaphitpc[aniter]->SetNsigmaTPCle250(2.0);
	  dtc2etaphitpc[aniter]->SetNsigmaTPC250_400(2.0);
	  dtc2etaphitpc[aniter]->SetNsigmaTPC400_450(1.0);
	  dtc2etaphitpc[aniter]->SetNsigmaTPC450_500(3.0);
	  dtc2etaphitpc[aniter]->SetNsigmaTOF450_500(2.0);
	  dtc2etaphitpc[aniter]->UseNsigmaTOF450_500(true);
	  dtc2etaphitpc[aniter]->SetNsigmaTPCge500(3.0);
	  dtc2etaphitpc[aniter]->SetNsigmaTOF500_800(2.0);
	  dtc2etaphitpc[aniter]->SetNsigmaTOF800_1000(1.5);
	  dtc2etaphitpc[aniter]->SetNsigmaTOFge1000(1.0);
	}
	  
	  //Set charge of particles:
	  if (ichg == 0) {
	    dtc1etaphitpc[aniter]->SetCharge(1.0);
	    dtc2etaphitpc[aniter]->SetCharge(1.0);
	  }
	  else if (ichg == 1) {
	    dtc1etaphitpc[aniter]->SetCharge(-1.0);
	    dtc2etaphitpc[aniter]->SetCharge(-1.0);
	  }
	  else if (ichg == 2) {
	    dtc1etaphitpc[aniter]->SetCharge(1.0);
	    dtc2etaphitpc[aniter]->SetCharge(-1.0);
	  }
	  else if (ichg == 3) {
	    dtc1etaphitpc[aniter]->SetCharge(-1.0);
	    dtc2etaphitpc[aniter]->SetCharge(1.0);
	  }

	  //Set particle 1:
	  dtc1etaphitpc[aniter]->SetPt(pTmin, pTmax);
	  dtc1etaphitpc[aniter]->SetEta(-eta,eta);
	  dtc1etaphitpc[aniter]->SetMass(PionMass);	  
	  dtc1etaphitpc[aniter]->SetMostProbablePion();	 
 
	  //Set particle 2:
	  dtc2etaphitpc[aniter]->SetPt(pTmin, pTmax);
          dtc2etaphitpc[aniter]->SetEta(-eta,eta);
	  dtc2etaphitpc[aniter]->SetMass(KaonMass);	  
	  dtc2etaphitpc[aniter]->SetMostProbableKaon();
	  
	  //** DCA******
	  dtc1etaphitpc[aniter]->SetMaxImpactXY(dcaxy); 	//DCA xy 
	  dtc1etaphitpc[aniter]->SetMaxImpactZ(dcaz);	//DCA Z
	  dtc2etaphitpc[aniter]->SetMaxImpactXY(dcaxy); 	//DCA xy
	  dtc2etaphitpc[aniter]->SetMaxImpactZ(dcaz);	//DCA Z
	  
	  //****** Track quality cuts ******
	  dtc1etaphitpc[aniter]->SetminTPCncls(ncls);
	  dtc2etaphitpc[aniter]->SetminTPCncls(ncls);
	  
	  //============PION============

	  //The cut monitor for particles to study the difference between reconstructed and true momentum: 
	  if(turnOnMonitors == 1) { 
	    cutPass1YPtetaphitpc[aniter] = new AliFemtoCutMonitorParticleYPt(Form("cutPass1%stpcM%i", chrgs[ichg], imult),PionMass);
	    cutFail1YPtetaphitpc[aniter] = new AliFemtoCutMonitorParticleYPt(Form("cutFail1%stpcM%i", chrgs[ichg], imult),PionMass);
	    dtc1etaphitpc[aniter]->AddCutMonitor(cutPass1YPtetaphitpc[aniter], cutFail1YPtetaphitpc[aniter]);
	  }
	  //The cut monitor for particles to study various aspects of the PID determination:
	  if(turnOnMonitors == 1) {
	    cutPass1PIDetaphitpc[aniter] = new AliFemtoCutMonitorParticlePID(Form("cutPass1%stpcM%i", chrgs[ichg], imult),0);//0-pion,1-kaon,2-proton
	    cutFail1PIDetaphitpc[aniter] = new AliFemtoCutMonitorParticlePID(Form("cutFail1%stpcM%i", chrgs[ichg], imult),0);
	    dtc1etaphitpc[aniter]->AddCutMonitor(cutPass1PIDetaphitpc[aniter], cutFail1PIDetaphitpc[aniter]);
	  }
	  
	  //============KAON============

	  if(turnOnMonitors == 1) { 
	    cutPass2YPtetaphitpc[aniter] = new AliFemtoCutMonitorParticleYPt(Form("cutPass2%stpcM%i", chrgs[ichg], imult),KaonMass);
	    cutFail2YPtetaphitpc[aniter] = new AliFemtoCutMonitorParticleYPt(Form("cutFail2%stpcM%i", chrgs[ichg], imult),KaonMass);
	    dtc2etaphitpc[aniter]->AddCutMonitor(cutPass2YPtetaphitpc[aniter], cutFail2YPtetaphitpc[aniter]);
	  }
	  if(turnOnMonitors == 1) {
	    cutPass2PIDetaphitpc[aniter] = new AliFemtoCutMonitorParticlePID(Form("cutPass2%stpcM%i", chrgs[ichg], imult),1);//0-pion,1-kaon,2-proton
	    cutFail2PIDetaphitpc[aniter] = new AliFemtoCutMonitorParticlePID(Form("cutFail2%stpcM%i", chrgs[ichg], imult),1);
	    dtc2etaphitpc[aniter]->AddCutMonitor(cutPass2PIDetaphitpc[aniter], cutFail2PIDetaphitpc[aniter]);
	  }
	  
	  //A pair cut which checks for some pair qualities that attempt to identify slit/doubly reconstructed tracks and also selects pairs based on their separation at the entrance to the TPC
	  //sqpcetaphitpc[aniter] = new AliFemtoPairCutRadialDistance();
          //sqpcetaphitpcRD[aniter] = new AliFemtoPairCutRadialDistanceLM();

	  if (paircutmergedfractionon == 1) 
	    sqpcetaphitpc[aniter] = new AliFemtoPairCutMergedFraction(distance, fraction1, 0.01, 0.8, 2.5);
	  else
	    sqpcetaphitpc[aniter] = new AliFemtoPairCutAntiGamma();
	  
	  if(paircutantigammaon == 1)
	    sqpcetaphitpc[aniter]->SetDataType(AliFemtoPairCut::kAOD);
	  else
	    sqpcetaphitpc[aniter]->SetDataType(AliFemtoPairCut::kKine);

	  if(gammacut == 1) {
	    sqpcetaphitpc[aniter]->SetMaxEEMinv(0.002);
	    sqpcetaphitpc[aniter]->SetMaxThetaDiff(0.008);
	  }

	  if( runtype==0){
	    sqpcetaphitpc[aniter]->SetMaxEEMinv(0.0);
	    sqpcetaphitpc[aniter]->SetMaxThetaDiff(0.0);
	    sqpcetaphitpc[aniter]->SetTPCEntranceSepMinimum(1.5);
	  }

	  else if (runtype==1){
	    //sqpcetaphitpc[aniter]->SetTPCEntranceSepMinimum(5.0);
	    //sqpcetaphitpc[aniter]->SetPhiStarDistanceMinimum(0.03);

	    //sqpcetaphitpc[aniter]->SetRadialDistanceMinimum(1.2, 0.03);
	    //sqpcetaphitpc[aniter]->SetEtaDifferenceMinimum(0.02);
	  }
	  else if ( runtype==2 ){
	    sqpcetaphitpc[aniter]->SetShareQualityMax(1.0);
	    sqpcetaphitpc[aniter]->SetShareFractionMax(0.05);
	    sqpcetaphitpc[aniter]->SetRemoveSameLabel(kFALSE);
	    sqpcetaphitpc[aniter]->SetTPCEntranceSepMinimum(0.0);
	  }
	  
	  // Study the betaT distribution:
	  if(turnOnBetaTMonitor == 1) {
	  cutpassbetatcutmonitor[aniter] = new AliFemtoCutMonitorPairBetaT(Form("cutPass%stpcM%i", chrgs[ichg], imult), 100, 0.0, 1.0, PionMass, KaonMass);
	  cutfailbetatcutmonitor[aniter] = new AliFemtoCutMonitorPairBetaT(Form("cutFail%stpcM%i", chrgs[ichg], imult), 100, 0.0, 1.0, PionMass, KaonMass);
	  sqpcetaphitpc[aniter]->AddCutMonitor(cutpassbetatcutmonitor[aniter], cutfailbetatcutmonitor[aniter]);
	  }
	  
	  anetaphitpc[aniter]->SetEnablePairMonitors(true);
	  anetaphitpc[aniter]->SetEventCut(mecetaphitpc[aniter]);
	  anetaphitpc[aniter]->SetFirstParticleCut(dtc1etaphitpc[aniter]);
	  anetaphitpc[aniter]->SetSecondParticleCut(dtc2etaphitpc[aniter]);
	  anetaphitpc[aniter]->SetPairCut(sqpcetaphitpc[aniter]);
	  
	  
	  
	  
	  //Correlation functions

	  //Spherical harmonics (without kT bins)
	  if(runSHCorrFctn == 1) {
	    cylmetaphitpc[aniter] = new AliFemtoCorrFctnDirectYlm(Form("cylm%stpcM%i", chrgs[ichg], imult),lmax,nbinssh,0.0,shqmax,runshlcms);
	    anetaphitpc[aniter]->AddCorrFctn(cylmetaphitpc[aniter]);
	  }

	  // NonId (without kT bins)
	  // Correlation function for non-identical particles uses k* as a function variable. Stores the correlation 
	  // function separately for positive and negative signs of k* projections into out, side and long directions, 
	  // enabling the calculations of double ratios
	  if(runNonIdCorrFctn == 1) {
	    cnonidtpc[aniter] = new AliFemtoCorrFctnNonIdDR(Form("cnonid%stpcM%i", chrgs[ichg], imult), nbinssh, 0.0, shqmax);
	    anetaphitpc[aniter]->AddCorrFctn(cnonidtpc[aniter]);
	  }

	  if(runDPhiStarKStarMergedFraction == 1) {
	    dphistarkstarmftpc[aniter] = new AliFemtoCorrFctnDPhiStarKStarMergedFraction(Form("cdphistarkstarmergedfraction%stpcM%iD%lfF%lf", chrgs[ichg], imult, distance, fraction1), 0.8, 2.5, distance, fraction1, 0.01, 51, 0.0, 0.5, 127, -1.0, 1.0);
	    anetaphitpc[aniter]->AddCorrFctn(dphistarkstarmftpc[aniter]);
	  }
	  
	  if(runDPhiStarKStarAverageMergedPointsFraction == 1) {
	    dphistarkstarampftpc[aniter] = new AliFemtoCorrFctnDPhiStarKStarAverageMergedPointsFraction(Form("cdphistarkstaraveragemergedpointsfraction%stpcM%iD%lf", chrgs[ichg], imult, distance), 0.8, 2.5, distance, 0.01, 51, 0.0, 0.5, 127, -0.5, 0.5);
	    anetaphitpc[aniter]->AddCorrFctn(dphistarkstarampftpc[aniter]);
	  }
	  // DPhiStarDEta (without bins)
	  // Correlation function for two particle correlations which uses dPhi* and dEta as a function variables.
	  if(runDPhiStarDEta == 1) {
	  cdphistardeta08[aniter] = new AliFemtoCorrFctnDPhiStarDEta(Form("cdphistardeta08%stpcM%i", chrgs[ichg], imult), 0.8, 51, -0.05, 0.05, 127, -1.0, 1.0);
	  anetaphitpc[aniter]->AddCorrFctn(cdphistardeta08[aniter]);
	  cdphistardeta12[aniter] = new AliFemtoCorrFctnDPhiStarDEta(Form("cdphistardeta12%stpcM%i", chrgs[ichg], imult), 1.2, 51, -0.05, 0.05, 127, -1.0, 1.0);
	  anetaphitpc[aniter]->AddCorrFctn(cdphistardeta12[aniter]);
	  cdphistardeta16[aniter] = new AliFemtoCorrFctnDPhiStarDEta(Form("cdphistardeta16%stpcM%i", chrgs[ichg], imult), 1.6, 51, -0.05, 0.05, 127, -1.0, 1.0);
	  anetaphitpc[aniter]->AddCorrFctn(cdphistardeta16[aniter]);
	  cdphistardeta20[aniter] = new AliFemtoCorrFctnDPhiStarDEta(Form("cdphistardeta20%stpcM%i", chrgs[ichg], imult), 2.0, 51, -0.05, 0.05, 127, -1.0, 1.0);
	  anetaphitpc[aniter]->AddCorrFctn(cdphistardeta20[aniter]);
	  }

	  if (runbetatdep) {
	    int ktm;
	    for (int ikt=0; ikt<numOfkTbins; ikt++) {
	      ktm = aniter*numOfkTbins + ikt;

	     if (isbeta==1){		    		    
	      //ktpcuts[ktm] = new AliFemtoKTPairCut(ktrng[ikt], ktrng[ikt+1]);
	      ktpcutsbeta[ktm] = new AliFemtoBetaTPairCut(ktrngbeta[ikt], ktrngbeta[ikt + 1], PionMass, KaonMass);
	     }
	     if (iskt==1){	
  	      ktpcutskt[ktm] = new AliFemtoPairkTPairCut(ktrngkt[ikt], ktrngkt[ikt + 1], PionMass, KaonMass);
	     }
	      
	      if (runbetatylm){
		cylmkttpc[ktm] = new AliFemtoCorrFctnDirectYlm(Form("cylm%stpcM%iD%lfF%lfbetat%d", chrgs[ichg], imult, distance, fraction1, ikt),lmax,nbinssh, 0.0,shqmax, runshlcms);
		cylmkttpc[ktm]->SetPairSelectionCut(ktpcutsbeta[ktm]);
		anetaphitpc[aniter]->AddCorrFctn(cylmkttpc[ktm]);
		//ckstarpt[ktm] = new AliFemtoCorrFctnPtKstarBeta(Form("ckstarpT%stpcM%i_beta%d", chrgs[ichg], imult, ikt));
		//ckstarpt[ktm]->SetPairSelectionCut(ktpcutsbeta[ktm]);
		//anetaphitpc[aniter]->AddCorrFctn(ckstarpt[ktm]);

	      }
	      
	      if (runbetatnonid){
		cnonidkttpc[ktm] = new AliFemtoCorrFctnNonIdDR(Form("cnonid%stpcM%ibetaT%i", chrgs[ichg], imult,ikt), nbinssh, 0.0, shqmax);
		cnonidkttpc[ktm]->SetPairSelectionCut(ktpcutskt[ktm]);
		anetaphitpc[aniter]->AddCorrFctn(cnonidkttpc[ktm]);
	      }
	      
	    }
	  }
	  
	
	  
	  Manager->AddAnalysis(anetaphitpc[aniter]);	
	}
      }
    }
  }
  // *** End pion-kaon analysis

  return Manager;
}
