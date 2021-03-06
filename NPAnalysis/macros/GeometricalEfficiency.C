/*****************************************************************************
 * Copyright (C) 2009   this file is part of the NPTool Project              *
 *                                                                           *
 * For the licensing terms see $NPTOOL/Licence/NPTool_Licence                *
 * For the list of contributors see $NPTOOL/Licence/Contributors             *
 *****************************************************************************/

/*****************************************************************************
 * Original Author: N. de Sereville  contact address: deserevi@ipno.in2p3.fr *
 *                                                                           *
 * Creation Date  : 22/07/09                                                 *
 * Last update    :                                                          *
 *---------------------------------------------------------------------------*
 * Decription:                                                               *
 *    + This macro calculates the geometrical efficiency for a given geometry*
 *      NPSimulation should have been run with an isotropic source event     *
 *      generator.                                                           *
 *                                                                           *
 *    + Use in a ROOT session:                                               *
 *      .x GeometricalEfficiency.C("FileToAnalyse")                          *
 *                                                                           *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Comment:                                                                  *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include <iostream>
#include <fstream>
#include <cmath>

#include "TROOT.h"
#include "TSystem.h"
#include "TFile.h"
#include "TF1.h"
#include "TString.h"
#include "TTree.h"
#include "TBranch.h"
#include "TH1F.h"
#include "TCanvas.h"

#include "TInitialConditions.h"
#include "TInteractionCoordinates.h"

using namespace std ;

void GeometricalEfficiency(const char * fname = "myResult"){
  // Open output ROOT file from NPTool simulation run
  TString path = gSystem->Getenv("NPTOOL");
  path += "/Outputs/Simulation/";
  TString inFileName = fname;
  if (!inFileName.Contains("root")) inFileName += ".root";
  TFile *inFile = new TFile(path + inFileName);
  TTree *tree   = (TTree*) inFile->Get("SimulatedTree");
  
  // Connect the branches of the TTree and activate then if necessary
  // TInitialConditions branch
  TInitialConditions *initCond = 0;
  tree->SetBranchAddress("InitialConditions", &initCond);
  tree->SetBranchStatus("InitialConditions", true);
  // TInteractionCoordinates branch
  TInteractionCoordinates *interCoord = 0;
  tree->SetBranchAddress("InteractionCoordinates", &interCoord);
  tree->SetBranchStatus("InteractionCoordinates", true);
  
  // Prepare histograms
  TH1F *hDetecTheta = new TH1F("hDetecTheta", "DetecTheta", 90, 0, 180);
  TH1F *hDetecThetaCM = new TH1F("hDetecThetaCM", "hDetecThetaCM", 90, 0, 180);
  TH1F *hEmittTheta = new TH1F("hEmittTheta", "EmittTheta", 90, 0, 180);
  TH1F *hEmittThetaCM = new TH1F("hEmittThetaCM", "hEmittThetaCM", 90, 0, 180);
  
  // Read the TTree
  Int_t nentries = tree->GetEntries();
  // cout << "TTree contains " << nentries << " events" << endl;
  for (Int_t i = 0; i < nentries; i++) {
    //if (i%1000 == 0) cout << "Entry " << i << endl;
    tree->GetEntry(i);
    // Fill histos
    hEmittTheta->Fill(initCond->GetThetaLab_WorldFrame(0));
    hEmittTheta->Fill(initCond->GetThetaCM(0));

    if (interCoord->GetDetectedMultiplicity() > 0){
      hDetecTheta->Fill(interCoord->GetDetectedAngleTheta(0));
      hDetecThetaCM->Fill(initCond->GetThetaCM(0));
    }
  }
  
  TCanvas* c4 = new TCanvas("c4", "CM Frame");
  TH1F* SolidACM = new TH1F(*hDetecThetaCM);
  SolidACM->Sumw2();
  TF1* CCM = new TF1("CCM",Form("%i /(4*%f)",nentries,M_PI),0,180);
  TF1* C = new TF1("C",Form("%i /(4*%f)",nentries,M_PI),0,180);
  SolidACM->Divide(C,1);
  SolidACM->Draw();
  TF1* f = new TF1("f",Form("2 * %f * sin(x*%f/180.) *2*%f/180.",M_PI,M_PI,M_PI),0,180);
  f->Draw("SAME");
  f->Draw("SAME");
  SolidACM->GetXaxis()->SetTitle("#theta_{CM} (deg)");
  SolidACM->GetYaxis()->SetTitle("d#Omega (sr) ");
  c4->Update();
}
