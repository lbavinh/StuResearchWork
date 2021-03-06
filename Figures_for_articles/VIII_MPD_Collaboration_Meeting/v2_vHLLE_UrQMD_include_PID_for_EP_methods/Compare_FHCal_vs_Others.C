#include "DrawTGraph.C"
// #include "constants.C"
// vector<TGraphErrors*>*
void Compare_FHCal_vs_Others()
{
  TString inputName[3] = {"./Data/SecondRun_vHLLEUrQMD_11.5_Model.root",
                          "./Data/SecondRun_vHLLEUrQMD_11.5_Reco_Nhits_32_MotherID.root",
                          "./Data/SecondRun_vHLLEUrQMD_11.5_Reco_Nhits_32_DCA_0.5.root"};


  Int_t centlow = 10;
  Int_t centhigh = 40;
  Double_t maxpt = 3.6;    // max pt for differential flow
  Double_t minpt = 0.;     // min pt for differential flow
  Double_t eta_cut = 1.5;  // pseudorapidity acceptance window for flow measurements
  const int ratioToMethod = 0;
  const Int_t nmethod = 3;
  const Int_t npid = 3;
  int pidCode[npid]={8,9,3};
  TString pidName[npid]={"hadrons","pions","protons"};
  TString pidNameFancy[npid]={"h^{#pm}","#pi^{#pm}","p"};
  TString title[]={"v_{2}{SP}","v_{2}{#Psi_{2,TPC}}","v_{2}{#Psi_{1,FHCal}}"};
  const int markerStyle[] = {24,20,23,21,20,25,28,26,23};
  const float markerSize = 1.3;
  TGraphErrors *graph[npid][nmethod];
  TFile *fi[3];
  fi[0] = new TFile(inputName[0].Data(),"READ");
  
  for (int j=0; j<npid; j++)
  {
    auto pr = (TProfile2D*) fi[0]->Get(Form("SP/prV2SPvsPt_pid%i",pidCode[j]));
    TProfile *tmp = PlotV2vsPt(pr,10,40);
    graph[j][0] = Converter(tmp);
  }

  for (int j=0; j<npid; j++)
  {
    auto pr = (TProfile2D*) fi[0]->Get(Form("ETASUBEP/prV2EtaSubvsPt_pid%i",pidCode[j]));
    TProfile *tmp = PlotV2vsPt(pr,10,40);
    graph[j][1] = Converter(tmp);
  }

  for (int j=0; j<npid; j++)
  {
    auto pr = (TProfile2D*) fi[0]->Get(Form("FHCALEP/prV2FHCalEPvsPt_pid%i",pidCode[j]));
    TProfile *tmp = PlotV2vsPt(pr,10,40);
    graph[j][2] = Converter(tmp);
  }    

  for (int id = 0; id < npid; id++)
  {
    for (int i=0; i<nmethod; i++)
    {
    // graph[id][i]->RemovePoint(0);
    graph[id][i]->SetTitle(title[i].Data());
    graph[id][i]->SetMarkerStyle(markerStyle[i]);
    graph[id][i]->SetMarkerSize(markerSize);
    graph[id][i]->GetXaxis()->SetTitle("p_{T}, GeV/c");
    graph[id][i]->GetYaxis()->SetTitle("v_{2}");
    graph[id][i]->SetDrawOption("P PLC PMC");
    }
  }

  vector<TGraphErrors*> vGr[npid];
  for (int id = 0; id < npid; id++)
  {
    vGr[id].push_back(graph[id][ratioToMethod]);
    vGr[id].push_back(graph[id][1]);
    vGr[id].push_back(graph[id][2]);
    TCanvas *can = (TCanvas*)DrawTGraph(vGr[id],Form("%i-%i%%",centlow,centhigh),0.58, 1.11, minpt, 3.6, -0.005, 0.25,
                                      // 0.65, 0.05, 0.9, 0.5,
                                      0.2, 0.5, 0.45, 0.88,
                                      "vHLLE+UrQMD, Au+Au", Form("#sqrt{s_{NN}} = 11.5 GeV, %s, |#eta|<%1.1f",pidNameFancy[id].Data(), eta_cut),1,"Ratio to v_{2}{SP}");
    can->SetName(Form("10-40%%"));
    can->SaveAs(Form("Compare_FHCal_vs_Others_%i%i_Model_%s.pdf",centlow,centhigh,pidName[id].Data()));
  }


  // return vGr;
}