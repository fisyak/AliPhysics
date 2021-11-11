#if !defined(__CINT__) || defined(__MAKECINT__)
#include "TString.h"
#include "TObjArray.h"

#include "AliLog.h"

#include "AliAnalysisManager.h"
#include "AliAnalysisDataContainer.h"

#include "AliAnalysisTaskFlowPPTask.h"
#endif

AliAnalysisTaskFlowPPTask* AddFlowPPTask(
    Int_t		fFilterbit 		= 96,
		Double_t	fMinPt			= 0.2,
		Double_t	fMaxPt			= 3.0,
        Int_t           trigger                 = 0,
        Int_t           fSystFlag               = 0,
        TString         fPeriod                 = "LHC17",
        TString         fNtrksName              = "Mult",
		TString		uniqueID        	= "Mult",
    TString name = "MyFlowPPTask")
{
     // The common parameters
	Double_t	fEtaCut 			= 0.8;
	Double_t	fVtxCut				= 10.0;
	Int_t		TPCclusters		        = 70;
	Double_t        chi2PerTPCcluster               = 10000;
	Int_t		fMinITSClus		        = 5;
	Double_t	fMaxChi2			= 2.5;
	Bool_t		fUseDCAzCut		        = false;
	Double_t	fDCAz				= 1.0;
	Bool_t		fUseDCAxyCut	                = false;
	Double_t	fDCAxy				= 0.2;
	Int_t		IsSample			= 10;
	Short_t		nCentFl				= 0;
	Bool_t		fLS				= false;
	Bool_t		fNUE 				= false;
	Bool_t		fNUA				= true;
	


    // get the manager via the static access member. since it's static, you don't need
    // to create an instance of the class here to call the function
    AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
    if (!mgr) {
        return 0x0;
    }

    // get the input event handler, again via a static method. 
    // this handler is part of the managing system and feeds events
    // to your task
    if (!mgr->GetInputEventHandler()) {
        return 0x0;
    }
    TString type = mgr->GetInputEventHandler()->GetDataType(); // can be "ESD" or "AOD"

    // by default, a file is open for writing. here, we get the filename
    TString fileName = AliAnalysisManager::GetCommonFileName();
    fileName += ":MyResults";      // create a subfolder in the file
    // now we create an instance of your task
    AliAnalysisTaskFlowPPTask* task = new AliAnalysisTaskFlowPPTask(name.Data());   
    if(!task) return 0x0;
	task->SetDebugLevel(3);
	task->SetFilterbit(fFilterbit); // For systematics
	task->SetFilterbitDefault(fFilterbit);
	task->SetEtaCut(fEtaCut);
	task->SetVtxCut(fVtxCut); // For systematics
	task->SetVtxCutDefault(fVtxCut);
	task->SetMinPt(fMinPt);
	task->SetMaxPt(fMaxPt);
	task->SetTPCclusters(TPCclusters); // For systematics
	task->SetTPCclustersDefault(TPCclusters);
	task->SetChi2PerTPCcluster(chi2PerTPCcluster); // max. chi2 per TPC cluster
	task->SetMinITSClusters(fMinITSClus);
	task->SetMaxChi2(fMaxChi2);
	task->SetUseDCAzCut(fUseDCAzCut);
	task->SetDCAzCut(fDCAz); // For systematics
	task->SetDCAzCutDefault(fDCAz); 
	task->SetUseDCAxyCut(fUseDCAxyCut);
	task->SetDCAxyCut(fDCAxy); // For systematics
	task->SetDCAxyCutDefault(fDCAxy); 
	task->SetIsSample(IsSample);
	task->SetCentFlag(nCentFl);
	task->SetTrigger(trigger);
	task->SetLSFlag(fLS);
	task->SetNUEFlag(fNUE);
	task->SetNUA(fNUA);
	task->SetNtrksName(fNtrksName);
    task->SetSystFlag(fSystFlag);
    task->SetUseWeigthsRunByRun(false);
    task->SetUsePeriodWeigths(false);
    task->SetUseWeights3D(false); 
	task->SetPeriod(fPeriod);
    
    //task->SelectCollisionCandidates(AliVEvent::kINT7);
    /*
    task->SetDebugLevel(3);
	task->SetFilterbit(fFilterbit); // For systematics
	task->SetFilterbitDefault(fFilterbit);
	task->SetEtaCut(fEtaCut);
	task->SetVtxCut(fVtxCut); // For systematics
	task->SetVtxCutDefault(fVtxCut);
	task->SetMinPt(fMinPt);
	task->SetMaxPt(fMaxPt);
	task->SetTPCclusters(TPCclusters); // For systematics
	task->SetTPCclustersDefault(TPCclusters);
	task->SetChi2PerTPCcluster(chi2PerTPCcluster); // max. chi2 per TPC cluster
	task->SetMinITSClusters(fMinITSClus);
	task->SetMaxChi2(fMaxChi2);
	task->SetUseDCAzCut(fUseDCAzCut);
	task->SetDCAzCut(fDCAz); // For systematics
	task->SetDCAzCutDefault(fDCAz); 
	task->SetUseDCAxyCut(fUseDCAxyCut);
	task->SetDCAxyCut(fDCAxy); // For systematics
	task->SetDCAxyCutDefault(fDCAxy); 
	task->SetIsSample(IsSample);
	task->SetCentFlag(nCentFl);
	task->SetTrigger(trigger);
	task->SetLSFlag(fLS);
	task->SetNUEFlag(fNUE);
	task->SetNUA(fNUA);
	task->SetNtrksName(fNtrksName);
    task->SetSystFlag(fSystFlag);
    task->SetUseWeigthsRunByRun(false);
    task->SetUsePeriodWeigths(false);
    task->SetUseWeights3D(false); 
	task->SetPeriod(fPeriod);
    */
    
    // add your task to the manager
    mgr->AddTask(task);


    // your task needs input: here we connect the manager to your task
    mgr->ConnectInput(task,0,mgr->GetCommonInputContainer());
    // same for the output
    mgr->ConnectOutput(task,1,mgr->CreateContainer("MyOutputContainer", TList::Class(), AliAnalysisManager::kOutputContainer, fileName.Data()));
    // in the end, this macro returns a pointer to your task. this will be convenient later on
    // when you will run your analysis in an analysis train on grid


	//Config Weights Input
	Int_t inSlotCounter=1;
	if(fNUA || fNUE)TGrid::Connect("alien:");
	//NUA
	if(fNUA) {
		AliAnalysisDataContainer *cin_NUA = mgr->CreateContainer(Form("NUA%s", uniqueID.Data()), TFile::Class(), AliAnalysisManager::kInputContainer);
               
                TFile *inNUA;

                if (fPeriod.EqualTo("LHC15o")) {
			// inNUA = TFile::Open("alien:///alice/cern.ch/user/z/zumoravc/weights/LHC15o/RBRweights.root");
			inNUA = TFile::Open("alien:///alice/cern.ch/user/e/enielsen/Weights/NUA/WeightsPbPb15o.root");
			//inNUA = TFile::Open("alien:///alice/cern.ch/user/z/zhlu/NUA_from_enielsen/WeightsPbPb15o.root");
			//inNUA = TFile::Open("/home/liscklu/Workdir/alice/NonlinearFlow/Analysis/AnaFlow20210423Mult/WeightsPbPb15o.root");
			task->SetUseWeigthsRunByRun(true);
                } else if (fPeriod.EqualTo("LHC17")) {
	            task->SetUsePeriodWeigths(true);
                    if (trigger == 0) {
                        inNUA = TFile::Open("alien:///alice/cern.ch/user/z/zumoravc/weights/pp_LHC17/weights_LHC17_MB_periods.root");
                    } else {
                        inNUA = TFile::Open("alien:///alice/cern.ch/user/z/zumoravc/weights/pp_LHC17/weights_LHC17_HM_periods.root");
                    }
                } else if (fPeriod.EqualTo("LHC15i")) {
	            task->SetUsePeriodWeigths(true);
		    if (trigger == 0) {
                        inNUA = TFile::Open("alien:///alice/cern.ch/user/z/zumoravc/weights/pp_LHC15/weights_LHC15i_MB.root");
                    } else {
                        inNUA = TFile::Open("alien:///alice/cern.ch/user/z/zumoravc/weights/pp_LHC15/weights_LHC15i_HM.root");
                    }
                } else if (fPeriod.EqualTo("LHC15l")) {
	            task->SetUsePeriodWeigths(true);
		    if (trigger == 0) {
                        inNUA = TFile::Open("alien:///alice/cern.ch/user/z/zumoravc/weights/pp_LHC15/weights_LHC15l_MB.root");
                    } else {
                        inNUA = TFile::Open("alien:///alice/cern.ch/user/z/zumoravc/weights/pp_LHC15/weights_LHC15l_HM.root");
                    }
                } else if (fPeriod.EqualTo("LHC16")) {
	            task->SetUsePeriodWeigths(true);
		    if (trigger == 0) {
                        inNUA = TFile::Open("alien:///alice/cern.ch/user/z/zumoravc/weights/pp_LHC16/weights_LHC16_MB_periods.root");
                    } else {
                        inNUA = TFile::Open("alien:///alice/cern.ch/user/z/zumoravc/weights/pp_LHC16/weights_LHC16_periods.root");
                    }
                } else if (fPeriod.EqualTo("LHC18")) {
	            task->SetUsePeriodWeigths(true);
		    if (trigger == 0) {
                        inNUA = TFile::Open("alien:///alice/cern.ch/user/z/zumoravc/weights/pp_LHC18/weights_LHC18_MB_periods.root");
                    } else {
                        //inNUA = TFile::Open("alien:///alice/cern.ch/user/z/zumoravc/weights/pp_LHC18/weights_LHC18_periods.root");
						inNUA = TFile::Open("alien:///alice/cern.ch/user/z/zumoravc/weights/pp_LHC18/weights_LHC18_allHM.root");
					}
                } 
					
                TList* weight_list = NULL;
		if (fSystFlag == 0 && !fPeriod.EqualTo("LHC15o")) {
			weight_list = dynamic_cast<TList*>(inNUA->Get("weights"));
		} else {
			weight_list = dynamic_cast<TList*>(inNUA->Get("WeightList"));
		}
		cin_NUA->SetData(weight_list);
		//Connect the weight input with task
		mgr->ConnectInput(task,inSlotCounter,cin_NUA);
		inSlotCounter++;
	}
	//NUE
	if(fNUE) {
		AliAnalysisDataContainer *cin_NUE = mgr->CreateContainer(Form("NUE%s", uniqueID.Data()), TFile::Class(), AliAnalysisManager::kInputContainer);
		TFile *inNUE = (fFilterbit==96)?TFile::Open("alien:///alice/cern.ch/user/k/kgajdoso/EfficienciesWeights/2015/TrackingEfficiency_PbPb5TeV_LHC15o_HIR.root"): TFile::Open("alien:///alice/cern.ch/user/k/kgajdoso/EfficienciesWeights/2015/TrackingEfficiency_PbPb5TeV_LHC15o_HIR_FB768.root");
		if(!inNUE) {
			printf("Could not open efficiency file!\n");
			return 0;
		}
		cin_NUE->SetData(inNUE);
		mgr->ConnectInput(task,inSlotCounter,cin_NUE);
		inSlotCounter++;
	}


    return task;
}