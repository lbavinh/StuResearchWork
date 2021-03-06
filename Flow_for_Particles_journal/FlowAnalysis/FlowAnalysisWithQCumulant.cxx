#include <FlowAnalysisWithQCumulant.h>
ClassImp(FlowAnalysisWithQCumulant);
FlowAnalysisWithQCumulant::FlowAnalysisWithQCumulant() : fEtaGap(0)
{
  Zero();
}

FlowAnalysisWithQCumulant::~FlowAnalysisWithQCumulant()
{
}

void FlowAnalysisWithQCumulant::Init()
{
  for (Int_t icent = 0; icent < ncent; icent++)
  { // loop over centrality classes
    hv22[icent] = new TProfile(Form("hv22_%i", icent), Form("hv22_%i", icent), 1, 0., 1.);
    hv24[icent] = new TProfile(Form("hv24_%i", icent), Form("hv24_%i", icent), 1, 0., 1.);
    hcov24[icent] = new TProfile(Form("hcov24_%i", icent), Form("hcov24_%i", icent), 1, 0., 1.);
    hcov24Gap[icent] = new TProfile(Form("hcov24Gap_%i", icent), Form("hcov24Gap_%i", icent), 1, 0., 1.);
    hv22Gap[icent] = new TProfile(Form("hv22Gap_%i", icent), Form("hv22Gap_%i", icent), 1, 0., 1.);
    hv24Gap[icent] = new TProfile(Form("hv24Gap_%i", icent), Form("hv24Gap_%i", icent), 1, 0., 1.);
    for (Int_t id = 0; id < npid; id++)
    {
      for (Int_t ipt = 0; ipt < npt; ipt++)
      { // loop over pt bin
        hv22pt[icent][ipt][id] = new TProfile(Form("hv22pt_%i_%i_%i", icent, ipt, id), Form("hv22pt_%i_%i_%i", icent, ipt, id), 1, 0., 1.);
        hv24pt[icent][ipt][id] = new TProfile(Form("hv24pt_%i_%i_%i", icent, ipt, id), Form("hv24pt_%i_%i_%i", icent, ipt, id), 1, 0., 1.);
        hcov22prime[icent][ipt][id] = new TProfile(Form("hcov22prime_%i_%i_%i", icent, ipt, id), Form("hcov22prime_%i_%i_%i", icent, ipt, id), 1, 0., 1.);
        hcov24prime[icent][ipt][id] = new TProfile(Form("hcov24prime_%i_%i_%i", icent, ipt, id), Form("hcov24prime_%i_%i_%i", icent, ipt, id), 1, 0., 1.);
        hcov42prime[icent][ipt][id] = new TProfile(Form("hcov42prime_%i_%i_%i", icent, ipt, id), Form("hcov42prime_%i_%i_%i", icent, ipt, id), 1, 0., 1.);
        hcov44prime[icent][ipt][id] = new TProfile(Form("hcov44prime_%i_%i_%i", icent, ipt, id), Form("hcov44prime_%i_%i_%i", icent, ipt, id), 1, 0., 1.);
        hcov2prime4prime[icent][ipt][id] = new TProfile(Form("hcov2prime4prime_%i_%i_%i", icent, ipt, id), Form("hcov2prime4prime_%i_%i_%i", icent, ipt, id), 1, 0., 1.);
        hcounter[icent][ipt][id] = new TProfile(Form("hcounter_%i_%i_%i", icent, ipt, id), Form("hcounter_%i_%i_%i", icent, ipt, id), 3, 0., 3.);
        hv22ptGap[icent][ipt][id] = new TProfile(Form("hv22ptGap_%i_%i_%i", icent, ipt, id), Form("hv22ptGap_%i_%i_%i", icent, ipt, id), 1, 0., 1.);
        hv24ptGap[icent][ipt][id] = new TProfile(Form("hv24ptGap_%i_%i_%i", icent, ipt, id), Form("hv24ptGap_%i_%i_%i", icent, ipt, id), 1, 0., 1.);
        hcov22primeGap[icent][ipt][id] = new TProfile(Form("hcov22primeGap_%i_%i_%i", icent, ipt, id), Form("hcov22primeGap_%i_%i_%i", icent, ipt, id), 1, 0., 1.);
        hcov24primeGap[icent][ipt][id] = new TProfile(Form("hcov24primeGap_%i_%i_%i", icent, ipt, id), Form("hcov24primeGap_%i_%i_%i", icent, ipt, id), 1, 0., 1.);
        hcov42primeGap[icent][ipt][id] = new TProfile(Form("hcov42primeGap_%i_%i_%i", icent, ipt, id), Form("hcov42primeGap_%i_%i_%i", icent, ipt, id), 1, 0., 1.);      
        hcov44primeGap[icent][ipt][id] = new TProfile(Form("hcov44primeGap_%i_%i_%i", icent, ipt, id), Form("hcov44primeGap_%i_%i_%i", icent, ipt, id), 1, 0., 1.);
        hcov2prime4primeGap[icent][ipt][id] = new TProfile(Form("hcov2prime4primeGap_%i_%i_%i", icent, ipt, id), Form("hcov2prime4primeGap_%i_%i_%i", icent, ipt, id), 1, 0., 1.);
      } // end of loop over pt bin
    }
  } // end of loop over centrality classes
}

void FlowAnalysisWithQCumulant::Zero()
{
  Q2x = 0.;
  Q2y = 0.;
  Q4x = 0.;
  Q4y = 0.;
  Q2 = TComplex(0., 0.);
  Q4 = TComplex(0., 0.);
  M = 0.;
  w2 = 0.;
  w4 = 0.;
  cor22 = 0.;
  cor24 = 0.;
  for (Int_t ipt = 0; ipt < npt; ipt++)
  {
    for (Int_t ipid = 0; ipid < npid; ipid++)
    {
      p2x[ipt][ipid] = 0.;
      p2y[ipt][ipid] = 0.;
      q2x[ipt][ipid] = 0.;
      q2y[ipt][ipid] = 0.;
      q4x[ipt][ipid] = 0.;
      q4y[ipt][ipid] = 0.;

      mq[ipt][ipid] = 0.;
      mp[ipt][ipid] = 0.;
      redCor22[ipt][ipid] = 0.;
      redCor24[ipt][ipid] = 0.;
      wred2[ipt][ipid] = 0.;
      wred4[ipt][ipid] = 0.;
      p2[ipt][ipid] = TComplex(0., 0.);
      // p4[ipt][ipid] = TComplex(0., 0.);
      q2[ipt][ipid] = TComplex(0., 0.);
      q4[ipt][ipid] = TComplex(0., 0.);
    }
  }

  // 2,4-QC with 2-sub

  w2Gap = 0.;
  w4Gap = 0.;
  cor22Gap = 0.;
  cor24Gap = 0.;
  for (Int_t ieta = 0; ieta < neta; ieta++)
  {
    Q2xGap[ieta] = 0.;
    Q2yGap[ieta] = 0.;
    Q4xGap[ieta] = 0.;
    Q4yGap[ieta] = 0.;
    MGap[ieta] = 0.;
    Q2Gap[ieta] = TComplex(0., 0.);
    Q4Gap[ieta] = TComplex(0., 0.);
    for (Int_t ipt = 0; ipt < npt; ipt++)
    {
      for (Int_t ipid = 0; ipid < npid; ipid++)
      {
        p2xGap[ieta][ipt][ipid] = 0;
        p2yGap[ieta][ipt][ipid] = 0;
        p2Gap[ieta][ipt][ipid] = TComplex(0., 0.);
        mpGap[ieta][ipt][ipid] = 0;
        q2xGap[ieta][ipt][ipid] = 0;
        q2yGap[ieta][ipt][ipid] = 0;
        q2Gap[ieta][ipt][ipid] = TComplex(0., 0.);
        q4xGap[ieta][ipt][ipid] = 0;
        q4yGap[ieta][ipt][ipid] = 0;
        q4Gap[ieta][ipt][ipid] = TComplex(0., 0.);        
        mqGap[ieta][ipt][ipid] = 0;        
        wred2Gap[ieta][ipt][ipid] = 0;
        wred4Gap[ieta][ipt][ipid] = 0;
        redCor22Gap[ieta][ipt][ipid] = 0;
        redCor24Gap[ieta][ipt][ipid] = 0;
      }
    }
  }
}

void FlowAnalysisWithQCumulant::ProcessFirstTrackLoopRP(const double &eta, const double &phi)
{
  Double_t cos4phi = TMath::Cos(4. * phi);
  Double_t sin4phi = TMath::Sin(4. * phi);
  Double_t cos2phi = TMath::Cos(2. * phi);
  Double_t sin2phi = TMath::Sin(2. * phi);

  // 2,4-QC
  Q2x += cos2phi;
  Q2y += sin2phi;
  Q4x += cos4phi;
  Q4y += sin4phi;
  M++;

  // 2-QC, eta-gapped
  if (eta < -fEtaGap)
  { // Left TPC subevent selection
    Q2xGap[0] += cos2phi;
    Q2yGap[0] += sin2phi;
    Q4xGap[0] += cos4phi;
    Q4yGap[0] += sin4phi;
    MGap[0]++;
  }
  if (eta > fEtaGap)
  { // Right TPC subevent selection
    Q2xGap[1] += cos2phi;
    Q2yGap[1] += sin2phi;
    Q4xGap[1] += cos4phi;
    Q4yGap[1] += sin4phi;
    MGap[1]++;
  }
}

void FlowAnalysisWithQCumulant::ProcessFirstTrackLoopPOI(const Int_t &ipt, const double &eta, const double &phi, const Int_t &pid, const double &charge)
{
  Double_t cos4phi = TMath::Cos(4. * phi);
  Double_t sin4phi = TMath::Sin(4. * phi);
  Double_t cos2phi = TMath::Cos(2. * phi);
  Double_t sin2phi = TMath::Sin(2. * phi);

  // Differential Flow of 2,4-QC
  if (charge > 0)
  {
    p2x[ipt][0] += cos2phi;
    p2y[ipt][0] += sin2phi;
    mp[ipt][0]++;

    q2x[ipt][0] += cos2phi;
    q2y[ipt][0] += sin2phi;
    q4x[ipt][0] += cos4phi;
    q4y[ipt][0] += sin4phi;
    mq[ipt][0]++;
  }
  if (charge < 0)
  {
    p2x[ipt][4] += cos2phi;
    p2y[ipt][4] += sin2phi;
    mp[ipt][4]++;

    q2x[ipt][4] += cos2phi;
    q2y[ipt][4] += sin2phi;
    q4x[ipt][4] += cos4phi;
    q4y[ipt][4] += sin4phi;
    mq[ipt][4]++;
  }
  if (pid > 0)
  {
    p2x[ipt][pid] += cos2phi;
    p2y[ipt][pid] += sin2phi;
    mp[ipt][pid]++;

    q2x[ipt][pid] += cos2phi;
    q2y[ipt][pid] += sin2phi;
    q4x[ipt][pid] += cos4phi;
    q4y[ipt][pid] += sin4phi;
    mq[ipt][pid]++;
  }

  // Differential Flow of 2-QC, eta-gapped
  if (eta < -fEtaGap)
  { // Left TPC subevent selection
    if (charge > 0)
    { // Inclusive positively charged hadrons
      p2xGap[1][ipt][0] += cos2phi;
      p2yGap[1][ipt][0] += sin2phi;
      mpGap[1][ipt][0]++;
      q2xGap[1][ipt][0] += cos2phi;
      q2yGap[1][ipt][0] += sin2phi;
      q4xGap[1][ipt][0] += cos4phi;
      q4yGap[1][ipt][0] += sin4phi;
      mqGap[1][ipt][0]++;
    }
    if (charge < 0)
    { // Inclusive negatively charged hadrons
      p2xGap[1][ipt][4] += cos2phi;
      p2yGap[1][ipt][4] += sin2phi;
      mpGap[1][ipt][4]++;
      q2xGap[1][ipt][4] += cos2phi;
      q2yGap[1][ipt][4] += sin2phi;
      q4xGap[1][ipt][4] += cos4phi;
      q4yGap[1][ipt][4] += sin4phi;
      mqGap[1][ipt][4]++;
    }
    if (pid > 0)
    { // Identified charged hadrons
      p2xGap[1][ipt][pid] += cos2phi;
      p2yGap[1][ipt][pid] += sin2phi;
      mpGap[1][ipt][pid]++;
      q2xGap[1][ipt][pid] += cos2phi;
      q2yGap[1][ipt][pid] += sin2phi;
      q4xGap[1][ipt][pid] += cos4phi;
      q4yGap[1][ipt][pid] += sin4phi;
      mqGap[1][ipt][pid]++;      
    }
  } // end of Left TPC subevent selection
  if (eta > fEtaGap)
  { // Right TPC subevent selection
    if (charge > 0)
    { // Inclusive positively charged hadrons
      p2xGap[0][ipt][0] += cos2phi;
      p2yGap[0][ipt][0] += sin2phi;
      mpGap[0][ipt][0]++;
      q2xGap[0][ipt][0] += cos2phi;
      q2yGap[0][ipt][0] += sin2phi;
      q4xGap[0][ipt][0] += cos4phi;
      q4yGap[0][ipt][0] += sin4phi;
      mqGap[0][ipt][0]++;
    }
    if (charge < 0)
    { // Inclusive negatively charged hadrons
      p2xGap[0][ipt][4] += cos2phi;
      p2yGap[0][ipt][4] += sin2phi;
      mpGap[0][ipt][4]++;
      q2xGap[0][ipt][4] += cos2phi;
      q2yGap[0][ipt][4] += sin2phi;
      q4xGap[0][ipt][4] += cos4phi;
      q4yGap[0][ipt][4] += sin4phi;
      mqGap[0][ipt][4]++;
    }
    if (pid > 0)
    { // Identified charged hadrons
      p2xGap[0][ipt][pid] += cos2phi;
      p2yGap[0][ipt][pid] += sin2phi;
      mpGap[0][ipt][pid]++;
      q2xGap[0][ipt][pid] += cos2phi;
      q2yGap[0][ipt][pid] += sin2phi;
      q4xGap[0][ipt][pid] += cos4phi;
      q4yGap[0][ipt][pid] += sin4phi;
      mqGap[0][ipt][pid]++;
    }
  } // end of Right TPC subevent selection
}

void FlowAnalysisWithQCumulant::ProcessEventAfterFirstTrackLoop(const Int_t &icent)
{
  // 2,QC & 4,QC without eta-gap
  Q2 = TComplex(Q2x, Q2y);
  w2 = M * (M - 1); // w(<2>)
  Q4 = TComplex(Q4x, Q4y);
  w4 = M * (M - 1) * (M - 2) * (M - 3); // w(<4>)
  if (w2 != 0 && w4 != 0)
  {
    cor22 = CalCor22(Q2, M, w2);       // <2>
    cor24 = CalCor24(Q2, Q4, M, w4);   // <4>
    hv22[icent]->Fill(0.5, cor22, w2); // <<2>>
    hv24[icent]->Fill(0.5, cor24, w4); // <<4>>
    // TProfile for covariance calculation in statistic error
    hcov24[icent]->Fill(0.5, cor22 * cor24, w2 * w4); // <2>*<4>
    for (Int_t ipt = 0; ipt < npt; ipt++)
    {
      for (Int_t id = 0; id < npid; id++)
      {
        wred2[ipt][id] = mp[ipt][id] * M - mq[ipt][id];                           // w(<2'>)
        wred4[ipt][id] = (mp[ipt][id] * M - 3 * mq[ipt][id]) * (M - 1) * (M - 2); // w(<4'>)
        if (mp[ipt][id] == 0 || wred2[ipt][id] == 0 || wred4[ipt][id] == 0)
          continue;
        p2[ipt][id] = TComplex(p2x[ipt][id], p2y[ipt][id]);
        q2[ipt][id] = TComplex(q2x[ipt][id], q2y[ipt][id]);
        q4[ipt][id] = TComplex(q4x[ipt][id], q4y[ipt][id]);
        redCor22[ipt][id] = CalRedCor22(Q2, p2[ipt][id], M, mp[ipt][id], mq[ipt][id], wred2[ipt][id]); // <2'>
        hv22pt[icent][ipt][id]->Fill(0.5, redCor22[ipt][id], wred2[ipt][id]);
        redCor24[ipt][id] = CalRedCor24(Q2, Q4, p2[ipt][id], q2[ipt][id], q4[ipt][id], M, mp[ipt][id], mq[ipt][id], wred4[ipt][id]); // <4'>
        hv24pt[icent][ipt][id]->Fill(0.5, redCor24[ipt][id], wred4[ipt][id]);
        // TProfile for covariance calculation in statistic error
        hcov22prime[icent][ipt][id]->Fill(0.5, cor22 * redCor22[ipt][id], w2 * wred2[ipt][id]); // <2>*<2'>
        hcov24prime[icent][ipt][id]->Fill(0.5, cor22 * redCor24[ipt][id], w2 * wred4[ipt][id]);
        hcov42prime[icent][ipt][id]->Fill(0.5, cor24 * redCor22[ipt][id], w4 * wred2[ipt][id]);
        hcov44prime[icent][ipt][id]->Fill(0.5, cor24 * redCor24[ipt][id], w4 * wred4[ipt][id]);
        hcov2prime4prime[icent][ipt][id]->Fill(0.5, redCor22[ipt][id] * redCor24[ipt][id], wred2[ipt][id] * wred4[ipt][id]);
        hcounter[icent][ipt][id]->Fill(0.5, 1, mp[ipt][id]);
      }
    }
  }

  // 2-QC, eta-gapped: multi-particle correlation calculation
  if (MGap[0] >= 2 && MGap[1] >= 2)
  {
    for (Int_t ieta = 0; ieta < neta; ieta++)
    {
      Q2Gap[ieta] = TComplex(Q2xGap[ieta], Q2yGap[ieta]);
      Q4Gap[ieta] = TComplex(Q4xGap[ieta], Q4yGap[ieta]);
    }
    w2Gap = MGap[0]*MGap[1];
    w4Gap = MGap[0]*(MGap[0]-1)*MGap[1]*(MGap[1]-1);
    
    cor22Gap = CalRedCor22(Q2Gap[0], Q2Gap[1], MGap[0], MGap[1], 0., w2Gap); // <2>
    cor24Gap = CalCor24TwoSub(Q2Gap[0], Q4Gap[0], Q2Gap[1], Q4Gap[1], MGap[0], MGap[1]);
    // pCorrelator2EtaGap->Fill(0.5 + icent, cor22Gap, w2Gap);
    hv22Gap[icent]->Fill(0.5, cor22Gap, w2Gap);
    hv24Gap[icent]->Fill(0.5, cor24Gap, w4Gap);
    hcov24Gap[icent]->Fill(0.5, cor22Gap * cor24Gap, w2Gap * w4Gap); // <2>*<4>
    for (Int_t ieta = 0; ieta < neta; ieta++)
    {
      for (Int_t ipt = 0; ipt < npt; ipt++)
      { // <2'>
        for (Int_t id = 0; id < npid; id++)
        {
          if (mpGap[ieta][ipt][id] == 0)
            continue;
          wred2Gap[ieta][ipt][id] = mpGap[ieta][ipt][id] * MGap[ieta];
          if (ieta==0) wred4Gap[ieta][ipt][id] = (mpGap[ieta][ipt][id]*MGap[ieta+1] - mqGap[ieta][ipt][id]) * MGap[ieta]*(MGap[ieta]-1);
          if (ieta==1) wred4Gap[ieta][ipt][id] = (mpGap[ieta][ipt][id]*MGap[ieta-1] - mqGap[ieta][ipt][id]) * MGap[ieta]*(MGap[ieta]-1);
          if (wred2Gap[ipt]==0 || wred4Gap[ipt]==0) continue;
          p2Gap[ieta][ipt][id] = TComplex(p2xGap[ieta][ipt][id], p2yGap[ieta][ipt][id]);
          q2Gap[ieta][ipt][id] = TComplex(q2xGap[ieta][ipt][id], q2yGap[ieta][ipt][id]);
          q4Gap[ieta][ipt][id] = TComplex(q4xGap[ieta][ipt][id], q4yGap[ieta][ipt][id]);
          redCor22Gap[ieta][ipt][id] = CalRedCor22(Q2Gap[ieta], p2Gap[ieta][ipt][id], MGap[ieta], mpGap[ieta][ipt][id], 0., wred2Gap[ieta][ipt][id]);                 // <2'>
          if (ieta==0) redCor24Gap[ieta][ipt][id] = CalRedCor24TwoSub(Q2Gap[ieta+1], Q2Gap[ieta],
                                                                  Q4Gap[ieta], p2Gap[ieta][ipt][id],
                                                                  q2Gap[ieta][ipt][id], q4Gap[ieta][ipt][id],
                                                                  MGap[ieta+1], MGap[ieta],
                                                                  mpGap[ieta][ipt][id], mqGap[ieta][ipt][id]);                                                        // <4'>
          if (ieta==1) redCor24Gap[ieta][ipt][id] = CalRedCor24TwoSub(Q2Gap[ieta-1], Q2Gap[ieta],
                                                                  Q4Gap[ieta], p2Gap[ieta][ipt][id],
                                                                  q2Gap[ieta][ipt][id], q4Gap[ieta][ipt][id],
                                                                  MGap[ieta-1], MGap[ieta],
                                                                  mpGap[ieta][ipt][id], mqGap[ieta][ipt][id]);             
          
          hv22ptGap[icent][ipt][id]->Fill(0.5, redCor22Gap[ieta][ipt][id], wred2Gap[ieta][ipt][id]);
          hv24ptGap[icent][ipt][id]->Fill(0.5, redCor24Gap[ieta][ipt][id], wred4Gap[ieta][ipt][id]); 
          hcounter[icent][ipt][id]->Fill(1.5, 1, mpGap[ieta][ipt][id]);
          // Covariances
          hcov22primeGap[icent][ipt][id]->Fill(0.5, cor22Gap * redCor22Gap[ieta][ipt][id], w2Gap * wred2Gap[ieta][ipt][id]);                                          // <2>*<2'>
          hcov24primeGap[icent][ipt][id]->Fill(0.5, cor22Gap * redCor24Gap[ieta][ipt][id], w2Gap * wred4Gap[ieta][ipt][id]);                                          // <2>*<4'>
          hcov42primeGap[icent][ipt][id]->Fill(0.5, cor24Gap * redCor22Gap[ieta][ipt][id], w4Gap * wred2Gap[ieta][ipt][id]);                                          // <4>*<2'>  
          hcov44primeGap[icent][ipt][id]->Fill(0.5, cor24Gap * redCor24Gap[ieta][ipt][id], w4Gap * wred4Gap[ieta][ipt][id]);                                          // <4>*<4'>
          hcov2prime4primeGap[icent][ipt][id]->Fill(0.5, redCor22Gap[ieta][ipt][id]*redCor24Gap[ieta][ipt][id], wred2Gap[ieta][ipt][id]*wred4Gap[ieta][ipt][id]);     // <2'>*<4'>

        }
      }
    }
  }
}

void FlowAnalysisWithQCumulant::SaveHist()
{
  for (int icent=0; icent<ncent; icent++){ // loop over centrality classes
    hv22[icent]->Write();
    hv24[icent]->Write();
    hcov24[icent]->Write();
    hv22Gap[icent]->Write();
    hcov24Gap[icent]->Write();
    hv24Gap[icent]->Write();    
    for(int ipt=0; ipt<npt; ipt++){ // loop over pt bin
      for (int id=0;id<npid;id++){
        hv22pt[icent][ipt][id]->Write();
        hv24pt[icent][ipt][id]->Write();
        hcov22prime[icent][ipt][id]->Write();
        hcov24prime[icent][ipt][id]->Write();
        hcov42prime[icent][ipt][id]->Write();
        hcov44prime[icent][ipt][id]->Write();
        hcov2prime4prime[icent][ipt][id]->Write();
        hcounter[icent][ipt][id]->Write();
        hv22ptGap[icent][ipt][id]->Write();
        hv24ptGap[icent][ipt][id]->Write();
        hcov22primeGap[icent][ipt][id]->Write();
        hcov24primeGap[icent][ipt][id]->Write();
        hcov42primeGap[icent][ipt][id]->Write();
        hcov44primeGap[icent][ipt][id]->Write();
        hcov2prime4primeGap[icent][ipt][id]->Write();
      }
    } // end of loop over pt bin
  } // end of loop over centrality classes
}

TComplex FlowAnalysisWithQCumulant::Qstar(const TComplex &Q)
{
  TComplex QStar = TComplex::Conjugate(Q);
  return QStar;
}

double FlowAnalysisWithQCumulant::CalCor22(const TComplex &Q2, const double &M, const double &w2)
{
  // single-event average 2-particle azimuthal correlation <2>
  double Q2Square = Q2.Rho2();
  double coor22 = Q2Square - M;
  return coor22 / w2;
}

double FlowAnalysisWithQCumulant::CalCor24(const TComplex &Q2, const TComplex &Q4, const double &M, const double &w4)
{
  // single-event average 4-particle azimuthal correlation <4>

  TComplex Q2Star = Qstar(Q2);
  TComplex Q4Star = Qstar(Q4);

  double Q2Square = Q2.Rho2();
  double Q4Square = Q4.Rho2();
  double ReQQQ = (Q4 * Q2Star * Q2Star).Re();

  double coor24 = (Q2Square * Q2Square + Q4Square - 2 * ReQQQ - 4 * (M - 2) * Q2Square + 2 * M * (M - 3));

  return coor24 / w4;
}

double FlowAnalysisWithQCumulant::CalRedCor22(const TComplex &Q2, const TComplex &p2, const double &M, const double &mp,
                                              const double &mq, const double &wred2)
{

  // Calculate the average reduced single-event 2-particle correlations
  TComplex Q2Star = TComplex::Conjugate(Q2);
  double coor22 = (p2 * Q2Star - mq).Re();

  return coor22 / wred2;
}

double FlowAnalysisWithQCumulant::CalRedCor24(const TComplex &Q2, const TComplex &Q4, const TComplex &p2, const TComplex &q2,
                                              const TComplex &q4, const double &M, const double &mp, const double &mq, const double &wred4)
{

  // Calculate the average reduced single-event 2-particle correlations
  TComplex Q2Star = TComplex::Conjugate(Q2);
  TComplex Q4Star = TComplex::Conjugate(Q4);
  TComplex q2Star = TComplex::Conjugate(q2);
  double Q2Square = Q2.Rho2();
  TComplex coorc = p2 * Q2 * Q2Star * Q2Star - q4 * Q2Star * Q2Star - p2 * Q2 * Q4Star - 2.0 * M * p2 * Q2Star - 2.0 * mq * Q2Square + 7.0 * q2 * Q2Star - Q2 * q2Star + q4 * Q4Star + 2.0 * p2 * Q2Star + 2.0 * mq * M - 6.0 * mq;
  double coor24 = coorc.Re();
  return coor24 / wred4;
}
double FlowAnalysisWithQCumulant::CalCor24TwoSub(const TComplex &Q2a, const TComplex &Q4a, 
                                                 const TComplex &Q2b, const TComplex &Q4b,
                                                 const double &Ma, const double &Mb)
{
  double dNumerator = ((Q2a*Q2a - Q4a) * TComplex::Conjugate(Q2b*Q2b - Q4b)).Re();
  double dDenominator = Ma * (Ma-1) * Mb * (Mb-1);
  if (dDenominator != 0) { return dNumerator/dDenominator; }
  else 
  { 
    cerr << "In FlowAnalysisWithQCumulant::CalCor24TwoSub, dDenominator=0" << endl;
    return 999;
  }
}

double FlowAnalysisWithQCumulant::CalRedCor24TwoSub(const TComplex &Q2a, // const TComplex &Q4a, 
                         const TComplex &Q2b, const TComplex &Q4b,
                         const TComplex &p2a, const TComplex &q2a, 
                         const TComplex &q4a,
                         const double &Ma, const double &Mb,
                         const double &mpa, const double &mqa)
{
  double dNumerator = ( p2a*Q2a*Qstar(Q2b)*Qstar(Q2b) - q4a*Qstar(Q2b)*Qstar(Q2b) -p2a*Q2a*Qstar(Q4b) + q4a*Qstar(Q4b) ).Re();
  double dDenominator = (mpa*Ma-mqa)*Mb*(Mb-1);
  if (dDenominator != 0) { return dNumerator/dDenominator; }
  else 
  { 
    cerr << "In CalRedCor24TwoSub, dDenominator=0" << endl;
    return 999;
  }
}