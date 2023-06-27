/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. */
/* See cxx source for full Copyright notice */
/* $Id$ */

#ifndef AliSPDppAnalysisTaskData_H
#define AliSPDppAnalysisTaskData_H

#include "AliAnalysisTaskSE.h"
#include "AliEventCuts.h"

class AliSPDppAnalysisTaskData : public AliAnalysisTaskSE  
{
    public:
                                AliSPDppAnalysisTaskData();
                                AliSPDppAnalysisTaskData(const char *name);
        virtual                 ~AliSPDppAnalysisTaskData();

        virtual void            UserCreateOutputObjects();
        virtual void            UserExec(Option_t* option);
        virtual void            Terminate(Option_t* option);
    

    private:
        
        AliAODEvent*            fAOD;           //! input event
        TList*                  fOutputList;    //! output list
        TH1F*                   MultDist05;     //! multiplicity distribution histogram for eta<.5>
    
        AliEventCuts            fEventCuts;     //!
    
        AliVMultiplicity*       fMultiplicity=nullptr;
    
        AliAODVZERO                 *fAODV0;
    
    
        AliSPDppAnalysisTaskData(const AliSPDppAnalysisTaskData&); // not implemented
        AliSPDppAnalysisTaskData& operator=(const AliSPDppAnalysisTaskData&); // not implemented

        ClassDef(AliSPDppAnalysisTaskData, 1);
};

#endif