#define PERFORMANCEQC
// #include "PlotV2EtaSubEventPlane.C"
// #include "PlotV2ScalarProduct.C"
#include "PlotV2QCumulant.C"
// #include "PlotV2QCumulant_Model.C"
// #include "constants.C"
// vector<TGraphErrors*>*
void Performance_QC(TString inputModel = "FirstRun_vHLLEUrQMD_11.5_Model.root", TString inputMC = "FirstRun_vHLLEUrQMD_11.5_Reco_Nhits_16.root", TString inputRECO = "FirstRun_vHLLEUrQMD_11.5_Reco_Nhits_32.root")
{
const Int_t npt = 16; // 0-3.6 GeV/c - number of pT bins
const Double_t pTBin[npt + 1] = {0., 0.2, 0.4, 0.6, 0.8, 1., 1.2, 1.4, 1.6, 1.8, 2.0, 2.2, 2.4, 2.6, 2.8, 3.2, 3.6};
  Double_t maxpt = 3.6;    // max pt for differential flow
  Double_t minpt = 0.;     // min pt for differential flow
  Double_t maxptRF = 3.;   // max pt for reference flow
  Double_t minptRF = 0.2;  // min pt for reference flow
  Double_t eta_cut = 1.5;  // pseudorapidity acceptance window for flow measurements
  Double_t eta_gap = 0.05; // +-0.05, eta-gap between 2 eta sub-event of two-particle cumulants method with eta-gap
  const int ratioToMethod = 0;
  double X[npt];
  for (int ipt=0; ipt<npt; ipt++)
  {
    X[ipt] = (pTBin[ipt] + pTBin[ipt+1]) / 2.;
  }
  const double errX[npt] = {0.};
  bool bUseProduct = 1;
  Int_t nmethod = 3;
  TString title[]={"Model","N_{hits}=16","N_{hits}=32"};// MC GEANT4 Reco
  const int markerStyle[] = {24,20,20,21,20,25,28,26,23};
  const float markerSize = 1.3;
  TGraphErrors *graph[1][nmethod];
  for (int i = 0; i < 1; i++)
  {
    graph[i][0] = PlotV2QCumulant_Model(inputModel);
    graph[i][1] = PlotV2QCumulant(inputMC);
    graph[i][2] = PlotV2QCumulant(inputRECO);
  }
  

  for (int ic = 0; ic < 1; ic++)
  {
    for (int i=0; i<nmethod; i++)
    {
    // graph[ic][i]->RemovePoint(0);
    graph[ic][i]->SetTitle(title[i].Data());
    graph[ic][i]->SetMarkerStyle(markerStyle[i]);
    graph[ic][i]->SetMarkerSize(markerSize);
    graph[ic][i]->GetXaxis()->SetTitle("p_{T}, GeV/c");
    graph[ic][i]->GetYaxis()->SetTitle("v_{2}{4}"); // 2,|#Delta#eta|>0.1
    graph[ic][i]->SetDrawOption("P PLC PMC");
    }
  }

  vector<TGraphErrors*> vGr[ncent];
  for (int ic = 0; ic < 1; ic++)
  {
    vGr[ic].push_back(graph[ic][ratioToMethod]);
    // for (int i=0; i<nmethod; i++)
    // {
    //   if (i==ratioToMethod) continue;
    //   vGr[ic].push_back(graph[ic][i]);
    // }
    // vGr[ic].push_back(graph[ic][0]);
    vGr[ic].push_back(graph[ic][1]);
    // vGr[ic].push_back(graph[ic][2]);
    // vGr[ic].push_back(graph[ic][3]);
    // vGr[ic].push_back(graph[ic][4]);

  TCanvas *can = (TCanvas*)DrawTGraph(vGr[ic],"10-20%",0.89, 1.11, minpt, 3.6, -0.005, 0.25,
                                      // 0.65, 0.05, 0.9, 0.5,
                                      0.2, 0.65, 0.4, 0.88,
                                      "vHLLE+UrQMD, Au+Au", Form("#sqrt{s_{NN}} = 11.5 GeV, h^{#pm}, |#eta|<%1.1f",eta_cut),1,"Ratio to Model");
  can->SetName(Form("10-40%%"));
  can->SaveAs(Form("Performance_v2_4QC_1020_Model_vs_Reco_Nhits_16.pdf"));
  }


  // return vGr;
}