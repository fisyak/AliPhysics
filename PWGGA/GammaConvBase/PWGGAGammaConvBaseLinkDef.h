#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

// Base classes
#ifdef PWGGAUSEKFPARTICLE
#pragma link C++ class AliGAKFParticle+;
#pragma link C++ class AliGAKFVertex+;
#endif // PWGGAUSEKFPARTICLE
#pragma link C++ class AliConversionPhotonBase+;
#pragma link C++ class AliAODConversionParticle+;
#pragma link C++ class AliAODConversionMother+;
#pragma link C++ class AliAODConversionPhoton+;
#pragma link C++ class AliKFConversionPhoton+;
#pragma link C++ class AliKFConversionMother+;
#pragma link C++ class AliCaloPhotonCuts+;
#pragma link C++ class AliConvEventCuts+;
#pragma link C++ class AliConversionPhotonCuts+;
#pragma link C++ class AliConversionCuts+;
#pragma link C++ class AliConversionSelection+;
#pragma link C++ class AliV0ReaderV1+;
#pragma link C++ class AliConversionAODBGHandlerRP+;
#pragma link C++ class AliConversionTrackCuts+;
#pragma link C++ class AliConversionMesonCuts+;
#pragma link C++ class AliDalitzElectronCuts+;
#pragma link C++ class AliDalitzElectronSelector+;
#pragma link C++ class AliCaloTrackMatcher+;
#pragma link C++ class AliPhotonIsolation+;
#pragma link C++ class MatrixHandler4D+;
#pragma link C++ class MatrixHandlerNDim+;
#pragma link C++ class EventMixPoolMesonJets+;
#pragma link C++ class EventWithJetAxis+;
#pragma link C++ class ElectronMixing+;
#pragma link C++ class AliConvK0LambdaCuts+;


// User tasks
#pragma link C++ class AliDalitzAODESD+;
#pragma link C++ class AliDalitzAODESDMC+;
#pragma link C++ class AliDalitzEventMC+;
#pragma link C++ class AliDalitzData+;
#pragma link C++ class AliCaloSigmaCuts+;

#pragma link C++ class utils_TH1+ ;

#endif
