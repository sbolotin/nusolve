/*
 *
 *    This file is a part of Space Geodetic Library. The library is used by
 *    nuSolve, a part of CALC/SOLVE system, and designed to make analysis of
 *    geodetic VLBI observations.
 *    Copyright (C) 2010-2020 Sergei Bolotin.
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <iostream>
#include <stdlib.h>


#include <SgVlbiSession.h>

#include <SgConstants.h>
#include <SgDbhImage.h>
#include <SgEccRec.h>
#include <SgLogger.h>
#include <SgSolutionReporter.h>
#include <SgVlbiObservation.h>
#include <SgVlbiBand.h>


#if QT_VERSION >= 0x050000
#   include <QtWidgets/QMessageBox>
#else
#   include <QtGui/QMessageBox>
#endif



const bool                      useOldAcm=true;

/*=======================================================================================================
*
*                     Auxiliary data structures
* 
*======================================================================================================*/


// locally used type:
struct DbhDescriptorX
{
  const char                   *lCode_;
  QString                       description_;
  SgDbhDatumDescriptor         *d_;
  bool                          isPresent_;
  SgDbhDatumDescriptor::Type    type_;
  int                           dimensions_[3];
  int                           numOfTc_;
  int                           expectedVersion_;
  bool                          isDimensionsFixed_;
  bool                          isMandatory_;
};


// and constants:
extern DbhDescriptorX dbhDxTable[];
extern const int numOfDxT;


// individual records:
extern DbhDescriptorX          &rNSites_;
extern DbhDescriptorX          &rNStars_;
extern DbhDescriptorX          &rNObs_;
extern DbhDescriptorX          &rSiteNames_;
extern DbhDescriptorX          &rStarNames_;
extern DbhDescriptorX          &rAxisOffsets_;
extern DbhDescriptorX          &rStar2000_;
extern DbhDescriptorX          &rSiteRecs_;
extern DbhDescriptorX          &rOloadAmpHorz_;
extern DbhDescriptorX          &rOloadPhsHorz_;
extern DbhDescriptorX          &rOloadAmpVert_;
extern DbhDescriptorX          &rOloadPhsVert_;
extern DbhDescriptorX          &rAxisTypes_;
extern DbhDescriptorX          &rCableSigns_;
extern DbhDescriptorX          &rCableStations_;
extern DbhDescriptorX          &rAtmConstr_;
extern DbhDescriptorX          &rAtmIntrv_;
extern DbhDescriptorX          &rClockConstr_;
extern DbhDescriptorX          &rClockIntrv_;
extern DbhDescriptorX          &rAcmOffsets_;
extern DbhDescriptorX          &rAcmRates_;
extern DbhDescriptorX          &rError_K_;
extern DbhDescriptorX          &rEccCoords_;
extern DbhDescriptorX          &rNRefClocks_;
extern DbhDescriptorX          &rBaselinesSelStatus_;
extern DbhDescriptorX          &rSourcesSelStatus_;
extern DbhDescriptorX          &rBaselinesDepClocks_;
extern DbhDescriptorX          &rClockSite_;
extern DbhDescriptorX          &rRefClockSites_;
extern DbhDescriptorX          &rClockBreakStatus_;
extern DbhDescriptorX          &rEccNames_;
extern DbhDescriptorX          &rEccTypes_;
extern DbhDescriptorX          &rEl_Cut_;
extern DbhDescriptorX          &rError_Bl_;
extern DbhDescriptorX          &rN4Obs_;
extern DbhDescriptorX          &rNumOfSamples_;
extern DbhDescriptorX          &rAmpByFrq_;
extern DbhDescriptorX          &rCorrCoef_;
extern DbhDescriptorX          &rObservedDelay_;
extern DbhDescriptorX          &rObservedDelaySigma_;
extern DbhDescriptorX          &rGrpAmbiguity_;
extern DbhDescriptorX          &rDelayRate_;
extern DbhDescriptorX          &rSigmaDelayRate_;
extern DbhDescriptorX          &rRecSetup_;
extern DbhDescriptorX          &rReferenceFrequency_;
extern DbhDescriptorX          &rRefFreqByChannel_;
extern DbhDescriptorX          &rSampleRate_;
extern DbhDescriptorX          &rSbDelay_;
extern DbhDescriptorX          &rSbSigmaDelay_;
extern DbhDescriptorX          &rSecTag_;
extern DbhDescriptorX          &rSnr_;
extern DbhDescriptorX          &rNumOfChannels_;
extern DbhDescriptorX          &rCalByFrq_;
extern DbhDescriptorX          &rDelayUFlag_;
extern DbhDescriptorX          &rNumOfAccPeriods_;
extern DbhDescriptorX          &rPhaseCalOffset_;
extern DbhDescriptorX          &rRateUFlag_;
extern DbhDescriptorX          &rUtcTag_;
extern DbhDescriptorX          &rBaselineName_;
extern DbhDescriptorX          &rQualityCode_;
extern DbhDescriptorX          &rSourceName_;
extern DbhDescriptorX          &rAzimuths_;
extern DbhDescriptorX          &rConsBendingDelay_;
extern DbhDescriptorX          &rConsensusDelay_;
extern DbhDescriptorX          &rElevations_;
extern DbhDescriptorX          &rNutationPart_;
extern DbhDescriptorX          &rNutationPart2K_;
extern DbhDescriptorX          &rOloadContrib_;
extern DbhDescriptorX          &rSitPart_;
extern DbhDescriptorX          &rStarPart_;
extern DbhDescriptorX          &rUT1Part_;
extern DbhDescriptorX          &rWobblePart_;
extern DbhDescriptorX          &rMeteoPressure_;
extern DbhDescriptorX          &rCableCalibration_;
extern DbhDescriptorX          &rMeteoHumidity_;
extern DbhDescriptorX          &rMeteoTemperature_;
extern DbhDescriptorX          &rGrIonFrq_;
extern DbhDescriptorX          &rIonCorr_;
extern DbhDescriptorX          &rIonRms_;
extern DbhDescriptorX          &rPhIonFrq_;
extern DbhDescriptorX          &rNAmbig_;
extern DbhDescriptorX          &rAtmCflg_;
extern DbhDescriptorX          &rClkCflg_;
extern DbhDescriptorX          &rClkBrkEpochs_;
extern DbhDescriptorX          &rClkBrkNum_;
extern DbhDescriptorX          &rClkBrkFlags_;
extern DbhDescriptorX          &rClkBrkNames_;
extern DbhDescriptorX          &rIonCode_;
extern DbhDescriptorX          &rNAmbigSecBand_;
extern DbhDescriptorX          &rQualityCodeSecBand_;
extern DbhDescriptorX          &rEffFreqsSecBand_;
extern DbhDescriptorX          &rRateObsSecBand_;
extern DbhDescriptorX          &rSigmaRateObsSecBand_;
extern DbhDescriptorX          &rDelayObsSecBand_;
extern DbhDescriptorX          &rSigmaDelayObsSecBand_;
extern DbhDescriptorX          &rPhaseObsSecBand_;
extern DbhDescriptorX          &rSigmaPhaseObsSecBand_;
extern DbhDescriptorX          &rTotalPhaseSecBand_;
extern DbhDescriptorX          &rRefFreqSecBand_;
extern DbhDescriptorX          &rSnrSecBand_;
extern DbhDescriptorX          &rCorrCoefSecBand_;
extern DbhDescriptorX          &rSBDelaySecBand_;
extern DbhDescriptorX          &rSigmaSBDelaySecBand_;
extern DbhDescriptorX          &rGrpAmbiguitySecBand_;
extern DbhDescriptorX          &rTotalPhase_;
extern DbhDescriptorX          &rBatchCnt_;
extern DbhDescriptorX          &rCorrelatorType_;
extern DbhDescriptorX          &rAcmNames_;
extern DbhDescriptorX          &rCalcVersionValue_;
extern DbhDescriptorX          &rTectPlateNames_;
extern DbhDescriptorX          &rUt1Ortho_;
extern DbhDescriptorX          &rWobOrtho_;
extern DbhDescriptorX          &rPtdContrib_;
extern DbhDescriptorX          &rEtdContrib_;
extern DbhDescriptorX          &rWobXContrib_;
extern DbhDescriptorX          &rWobYContrib_;
extern DbhDescriptorX          &rWobNutatContrib_;
extern DbhDescriptorX          &rFeedCorr_;
extern DbhDescriptorX          &rTiltRemvr_;
extern DbhDescriptorX          &rNdryCont_;
extern DbhDescriptorX          &rNwetCont_;
extern DbhDescriptorX          &rNgradParts_;
extern DbhDescriptorX          &rUnPhaseCal_;
extern DbhDescriptorX          &rAxOffsetCont_;
extern DbhDescriptorX          &rCalSites_;
extern DbhDescriptorX          &rCalList_;
extern DbhDescriptorX          &rCalFlags_;
extern DbhDescriptorX          &rAxsOfsPart_;
extern DbhDescriptorX          &rTai_2_Utc_;
extern DbhDescriptorX          &rFourFitFileName_;
extern DbhDescriptorX          &rFourFitFileNameSecBand_;
extern DbhDescriptorX          &rIonBits_;
extern DbhDescriptorX          &rUserAction4Suppression_;
extern DbhDescriptorX          &rFut1_inf_;
extern DbhDescriptorX          &rFut1_pts_;
extern DbhDescriptorX          &rFwob_inf_;
extern DbhDescriptorX          &rFwob_pts_;
extern DbhDescriptorX          &rTidalUt1_;
extern DbhDescriptorX          &rPhAmbig_S_;
extern DbhDescriptorX          &rSolData_;
extern DbhDescriptorX          &rSupMet_;
extern DbhDescriptorX          &rBlDepClocks_;
extern DbhDescriptorX          &rAtmPartFlag_;
extern DbhDescriptorX          &rFclList_;
extern DbhDescriptorX          &rFclFlags_;
extern DbhDescriptorX          &rObcList_;
extern DbhDescriptorX          &rObcFlags_;
extern DbhDescriptorX          &rScanName_;
extern DbhDescriptorX          &rRunCode_;
extern DbhDescriptorX          &rOloadHorzContrib_;
extern DbhDescriptorX          &rOloadVertContrib_;
extern DbhDescriptorX          &rWobLibra_;
extern DbhDescriptorX          &rUt1Libra_;
extern DbhDescriptorX          &rNut06xys_;
extern DbhDescriptorX          &rNut06xyp_;
extern DbhDescriptorX          &rNutWahr_;
extern DbhDescriptorX          &rNut2006_;
extern DbhDescriptorX          &rOptlContib_;
extern DbhDescriptorX          &rAcSites_;
extern DbhDescriptorX          &rCcSites_;
extern DbhDescriptorX          &rOceOld_;
extern DbhDescriptorX          &rPtdOld_;
extern DbhDescriptorX          &rFrngErr_;
extern DbhDescriptorX          &rAcmEpochs_;
extern DbhDescriptorX          &rAcmNumber_;
extern DbhDescriptorX          &rParAngle_;
extern DbhDescriptorX          &rConsensusRate_;
extern DbhDescriptorX          &rNdryPart_;
extern DbhDescriptorX          &rNwetPart_;
extern DbhDescriptorX          &rSun1BendingContrib_;
extern DbhDescriptorX          &rSun2BendingContrib_;

extern DbhDescriptorX          &rMslmIonContrib_;


// individual descriptors:
extern SgDbhDatumDescriptor   *&dNSites_;
extern bool                    &hasNSites_;
extern SgDbhDatumDescriptor   *&dNStars_;
extern bool                    &hasNStars_;
extern SgDbhDatumDescriptor   *&dNObs_;
extern bool                    &hasNObs_;
extern SgDbhDatumDescriptor   *&dSiteNames_;
extern bool                    &hasSiteNames_;
extern SgDbhDatumDescriptor   *&dStarNames_;
extern bool                    &hasStarNames_;
extern SgDbhDatumDescriptor   *&dAxisOffsets_;
extern bool                    &hasAxisOffsets_;
extern SgDbhDatumDescriptor   *&dStar2000_;
extern bool                    &hasStar2000_;
extern SgDbhDatumDescriptor   *&dSiteRecs_;
extern bool                    &hasSiteRecs_;
extern SgDbhDatumDescriptor   *&dOloadAmpHorz_;
extern bool                    &hasOloadAmpHorz_;
extern SgDbhDatumDescriptor   *&dOloadPhsHorz_;
extern bool                    &hasOloadPhsHorz_;
extern SgDbhDatumDescriptor   *&dOloadAmpVert_;
extern bool                    &hasOloadAmpVert_;
extern SgDbhDatumDescriptor   *&dOloadPhsVert_;
extern bool                    &hasOloadPhsVert_;
extern SgDbhDatumDescriptor   *&dAxisTypes_;
extern bool                    &hasAxisTypes_;
extern SgDbhDatumDescriptor   *&dCableSigns_;
extern bool                    &hasCableSigns_;
extern SgDbhDatumDescriptor   *&dCableStations_;
extern bool                    &hasCableStations_;
extern SgDbhDatumDescriptor   *&dAtmConstr_;
extern bool                    &hasAtmConstr_;
extern SgDbhDatumDescriptor   *&dAtmIntrv_;
extern bool                    &hasAtmIntrv_;
extern SgDbhDatumDescriptor   *&dClockConstr_;
extern bool                    &hasClockConstr_;
extern SgDbhDatumDescriptor   *&dClockIntrv_;
extern bool                    &hasClockIntrv_;
extern SgDbhDatumDescriptor   *&dAcmOffsets_;
extern bool                    &hasAcmOffsets_;
extern SgDbhDatumDescriptor   *&dAcmRates_;
extern bool                    &hasAcmRates_;
extern SgDbhDatumDescriptor   *&dError_K_;
extern bool                    &hasError_K_;
extern SgDbhDatumDescriptor   *&dEccCoords_;
extern bool                    &hasEccCoords_;
extern SgDbhDatumDescriptor   *&dNRefClocks_;
extern bool                    &hasNRefClocks_;
extern SgDbhDatumDescriptor   *&dBaselinesSelStatus_;
extern bool                    &hasBaselinesSelStatus_;
extern SgDbhDatumDescriptor   *&dSourcesSelStatus_;
extern bool                    &hasSourcesSelStatus_;
extern SgDbhDatumDescriptor   *&dBaselinesDepClocks_;
extern bool                    &hasBaselinesDepClocks_;
extern SgDbhDatumDescriptor   *&dClockSite_;
extern SgDbhDatumDescriptor   *&dRefClockSites_;
extern bool                    &hasRefClockSites_;
extern SgDbhDatumDescriptor   *&dClockBreakStatus_;
extern bool                    &hasClockBreakStatus_;
extern SgDbhDatumDescriptor   *&dEccNames_;
extern bool                    &hasEccNames_;
extern SgDbhDatumDescriptor   *&dEccTypes_;
extern bool                    &hasEccTypes_;
extern SgDbhDatumDescriptor   *&dEl_Cut_;
extern bool                    &hasEl_Cut_;
extern SgDbhDatumDescriptor   *&dError_Bl_;
extern bool                    &hasError_Bl_;
extern SgDbhDatumDescriptor   *&dN4Obs_;
extern bool                    &hasN4Obs_;
extern SgDbhDatumDescriptor   *&dNumOfSamples_;
extern bool                    &hasNumOfSamples_;
extern SgDbhDatumDescriptor   *&dAmpByFrq_;
extern SgDbhDatumDescriptor   *&dCorrCoef_;
extern SgDbhDatumDescriptor   *&dObservedDelay_;
extern SgDbhDatumDescriptor   *&dObservedDelaySigma_;
extern SgDbhDatumDescriptor   *&dGrpAmbiguity_;
extern SgDbhDatumDescriptor   *&dDelayRate_;
extern SgDbhDatumDescriptor   *&dSigmaDelayRate_;
extern SgDbhDatumDescriptor   *&dRecSetup_;
extern SgDbhDatumDescriptor   *&dReferenceFrequency_;
extern SgDbhDatumDescriptor   *&dRefFreqByChannel_;
extern bool                    &hasRefFreqByChannel_;
extern SgDbhDatumDescriptor   *&dSampleRate_;
extern SgDbhDatumDescriptor   *&dSbDelay_;
extern SgDbhDatumDescriptor   *&dSbSigmaDelay_;
extern SgDbhDatumDescriptor   *&dSecTag_;
extern SgDbhDatumDescriptor   *&dSnr_;
extern SgDbhDatumDescriptor   *&dNumOfChannels_;
extern bool                    &hasNumOfChannels_;
extern SgDbhDatumDescriptor   *&dCalByFrq_;
extern bool                    &hasCalByFrq_;
extern SgDbhDatumDescriptor   *&dDelayUFlag_;
extern SgDbhDatumDescriptor   *&dNumOfAccPeriods_;
extern SgDbhDatumDescriptor   *&dPhaseCalOffset_;
extern bool                    &hasPhaseCalOffset_;
extern SgDbhDatumDescriptor   *&dRateUFlag_;
extern SgDbhDatumDescriptor   *&dUtcTag_;
extern SgDbhDatumDescriptor   *&dBaselineName_;
extern SgDbhDatumDescriptor   *&dQualityCode_;
extern SgDbhDatumDescriptor   *&dSourceName_;
extern SgDbhDatumDescriptor   *&dElevations_;
extern bool                    &hasElevationData_;
extern SgDbhDatumDescriptor   *&dConsBendingDelay_;
extern bool                    &hasConsBendingDelay_;
extern SgDbhDatumDescriptor   *&dConsensusDelay_;
extern bool                    &hasConsensusDelay_;
extern SgDbhDatumDescriptor   *&dAzimuths_;
extern bool                    &hasAzimuthData_;
extern SgDbhDatumDescriptor   *&dNutationPart_;
extern bool                    &hasNutationPart_;
extern SgDbhDatumDescriptor   *&dNutationPart2K_;
extern bool                    &hasNutationPart2K_;
extern SgDbhDatumDescriptor   *&dOloadContrib_;
extern bool                    &hasOloadContrib_;
extern SgDbhDatumDescriptor   *&dSitPart_;
extern bool                    &hasSitPart_;
extern SgDbhDatumDescriptor   *&dStarPart_;
extern bool                    &hasStarPart_;
extern SgDbhDatumDescriptor   *&dUT1Part_;
extern bool                    &hasUT1Part_;
extern SgDbhDatumDescriptor   *&dWobblePart_;
extern bool                    &hasWobblePart_;
extern SgDbhDatumDescriptor   *&dMeteoPressure_;
extern bool                    &hasMeteoPressureData_;
extern SgDbhDatumDescriptor   *&dCableCalibration_;
extern bool                    &hasCableData_;
extern SgDbhDatumDescriptor   *&dMeteoHumidity_;
extern bool                    &hasMeteoRelHumidityData_;
extern SgDbhDatumDescriptor   *&dMeteoTemperature_;
extern bool                    &hasMeteoTemperatureData_;
extern SgDbhDatumDescriptor   *&dNAmbig_;
extern bool                    &hasNAmbig_;
extern SgDbhDatumDescriptor   *&dGrIonFrq_;
extern bool                    &hasGrIonFrq_;
extern SgDbhDatumDescriptor   *&dIonCorr_;
extern bool                    &hasIonCorr_;
extern SgDbhDatumDescriptor   *&dIonRms_;
extern bool                    &hasIonRms_;
extern SgDbhDatumDescriptor   *&dPhIonFrq_;
extern bool                    &hasPhIonFrq_;
extern SgDbhDatumDescriptor   *&dAtmCflg_;
extern bool                    &hasAtmCflg_;
extern SgDbhDatumDescriptor   *&dClkCflg_;
extern bool                    &hasClkCflg_;
extern SgDbhDatumDescriptor   *&dClkBrkEpochs_;
extern bool                    &hasClkBrkEpochs_;
extern SgDbhDatumDescriptor   *&dClkBrkNum_;
extern bool                    &hasClkBrkNum_;
extern SgDbhDatumDescriptor   *&dClkBrkFlags_;
extern bool                    &hasClkBrkFlags_;
extern SgDbhDatumDescriptor   *&dClkBrkNames_;
extern bool                    &hasClkBrkNames_;
extern SgDbhDatumDescriptor   *&dIonCode_;
extern SgDbhDatumDescriptor   *&dNAmbigSecBand_;
extern bool                    &hasNAmbigSecBand_;
extern SgDbhDatumDescriptor   *&dQualityCodeSecBand_;
extern bool                    &hasQualityCodeSecBand_;
extern SgDbhDatumDescriptor   *&dEffFreqsSecBand_;
extern bool                    &hasEffFreqsSecBand_;
extern SgDbhDatumDescriptor   *&dRateObsSecBand_;
extern bool                    &hasRateObsSecBand_;
extern SgDbhDatumDescriptor   *&dSigmaRateObsSecBand_;
extern bool                    &hasSigmaRateObsSecBand_;
extern SgDbhDatumDescriptor   *&dDelayObsSecBand_;
extern bool                    &hasDelayObsSecBand_;
extern SgDbhDatumDescriptor   *&dSigmaDelayObsSecBand_;
extern bool                    &hasSigmaDelayObsSecBand_;
extern SgDbhDatumDescriptor   *&dPhaseObsSecBand_;
extern bool                    &hasPhaseObsSecBand_;
extern SgDbhDatumDescriptor   *&dSigmaPhaseObsSecBand_;
extern bool                    &hasSigmaPhaseObsSecBand_;
extern SgDbhDatumDescriptor   *&dTotalPhaseSecBand_;
extern bool                    &hasTotalPhaseSecBand_;
extern SgDbhDatumDescriptor   *&dRefFreqSecBand_;
extern bool                    &hasRefFreqSecBand_;
extern SgDbhDatumDescriptor   *&dSnrSecBand_;
extern bool                    &hasSnrSecBand_;
extern SgDbhDatumDescriptor   *&dCorrCoefSecBand_;
extern bool                    &hasCorrCoefSecBand_;
extern SgDbhDatumDescriptor   *&dSBDelaySecBand_;
extern bool                    &hasSBDelaySecBand_;
extern SgDbhDatumDescriptor   *&dSigmaSBDelaySecBand_;
extern bool                    &hasSigmaSBDelaySecBand_;
extern SgDbhDatumDescriptor   *&dGrpAmbiguitySecBand_;
extern bool                    &hasGrpAmbiguitySecBand_;
extern SgDbhDatumDescriptor   *&dTotalPhase_;
extern bool                    &hasTotalPhase_;
extern SgDbhDatumDescriptor   *&dBatchCnt_;
extern bool                    &hasBatchCnt_;
extern SgDbhDatumDescriptor   *&dCorrelatorType_;
extern bool                    &hasCorrelatorType_;
extern SgDbhDatumDescriptor   *&dAcmNames_;
extern bool                    &hasAcmNames_;
extern SgDbhDatumDescriptor   *&dCalcVersionValue_;
extern bool                    &hasCalcVersionValue_;
extern SgDbhDatumDescriptor   *&dTectPlateNames_;
extern bool                    &hasTectPlateNames_;
extern SgDbhDatumDescriptor   *&dUt1Ortho_;
extern bool                    &hasUt1Ortho_;
extern SgDbhDatumDescriptor   *&dWobOrtho_;
extern bool                    &hasWobOrtho_;
extern SgDbhDatumDescriptor   *&dPtdContrib_;
extern bool                    &hasPtdContrib_;
extern SgDbhDatumDescriptor   *&dEtdContrib_;
extern bool                    &hasEtdContrib_;
extern SgDbhDatumDescriptor   *&dWobXContrib_;
extern bool                    &hasWobXContrib_;
extern SgDbhDatumDescriptor   *&dWobYContrib_;
extern bool                    &hasWobYContrib_;
extern SgDbhDatumDescriptor   *&dWobNutatContrib_;
extern bool                    &hasWobNutatContrib_;
extern SgDbhDatumDescriptor   *&dFeedCorr_;
extern bool                    &hasFeedCorr_;
extern SgDbhDatumDescriptor   *&dTiltRemvr_;
extern bool                    &hasTiltRemvr_;
extern SgDbhDatumDescriptor   *&dNdryCont_;
extern bool                    &hasNdryCont_;
extern SgDbhDatumDescriptor   *&dNwetCont_;
extern bool                    &hasNwetCont_;
extern SgDbhDatumDescriptor   *&dNgradParts_;
extern bool                    &hasNgradParts_;
extern SgDbhDatumDescriptor   *&dUnPhaseCal_;
extern bool                    &hasUnPhaseCal_;
extern SgDbhDatumDescriptor   *&dAxOffsetCont_;
extern bool                    &hasAxOffsetCont_;
extern SgDbhDatumDescriptor   *&dCalSites_;
extern bool                    &hasCalSites_;
extern SgDbhDatumDescriptor   *&dCalList_;
extern bool                    &hasCalList_;
extern SgDbhDatumDescriptor   *&dCalFlags_;
extern bool                    &hasCalFlags_;
extern SgDbhDatumDescriptor   *&dAxsOfsPart_;
extern bool                    &hasAxsOfsPart_;
extern SgDbhDatumDescriptor   *&dTai_2_Utc_;
extern bool                    &hasTai_2_Utc_;
extern SgDbhDatumDescriptor   *&dFourFitFileName_;
extern bool                    &hasFourFitFileName_;
extern SgDbhDatumDescriptor   *&dFourFitFileNameSecBand_;
extern bool                    &hasFourFitFileNameSecBand_;
extern SgDbhDatumDescriptor   *&dIonBits_;
extern bool                    &hasIonBits_;
extern SgDbhDatumDescriptor   *&dUserAction4Suppression_;
extern bool                    &hasUserAction4Suppression_;
extern SgDbhDatumDescriptor   *&dFut1_inf_;
extern bool                    &hasFut1_inf_;
extern SgDbhDatumDescriptor   *&dFut1_pts_;
extern bool                    &hasFut1_pts_;
extern SgDbhDatumDescriptor   *&dFwob_inf_;
extern bool                    &hasFwob_inf_;
extern SgDbhDatumDescriptor   *&dFwob_pts_;
extern bool                    &hasFwob_pts_;
extern SgDbhDatumDescriptor   *&dTidalUt1_;
extern bool                    &hasTidalUt1_;
extern SgDbhDatumDescriptor   *&dPhAmbig_S_;
extern bool                    &hasPhAmbig_S_;
extern SgDbhDatumDescriptor   *&dSolData_;
extern bool                    &hasSolData_;
extern SgDbhDatumDescriptor   *&dSupMet_;
extern bool                    &hasSupMet_;
extern SgDbhDatumDescriptor   *&dBlDepClocks_;
extern bool                    &hasBlDepClocks_;
extern SgDbhDatumDescriptor   *&dAtmPartFlag_;
extern bool                    &hasAtmPartFlag_;
extern SgDbhDatumDescriptor   *&dFclList_;
extern bool                    &hasFclList_;
extern SgDbhDatumDescriptor   *&dFclFlags_; 
extern bool                    &hasFclFlags_;
extern SgDbhDatumDescriptor   *&dObcList_;
extern bool                    &hasObcList_;
extern SgDbhDatumDescriptor   *&dObcFlags_;
extern bool                    &hasObcFlags_;
extern SgDbhDatumDescriptor   *&dScanName_;
extern bool                    &hasScanName_;
extern SgDbhDatumDescriptor   *&dRunCode_;
extern bool                    &hasRunCode_;
extern SgDbhDatumDescriptor   *&dOloadHorzContrib_;
extern bool                    &hasOloadHorzContrib_;
extern SgDbhDatumDescriptor   *&dOloadVertContrib_;
extern bool                    &hasOloadVertContrib_;
extern SgDbhDatumDescriptor   *&dWobLibra_;
extern bool                    &hasWobLibra_;
extern SgDbhDatumDescriptor   *&dUt1Libra_;
extern bool                    &hasUt1Libra_;
extern SgDbhDatumDescriptor   *&dNut06xys_;
extern bool                    &hasNut06xys_;
extern SgDbhDatumDescriptor   *&dNut06xyp_;
extern bool                    &hasNut06xyp_;
extern SgDbhDatumDescriptor   *&dNutWahr_;
extern bool                    &hasNutWahr_;
extern SgDbhDatumDescriptor   *&dNut2006_;
extern bool                    &hasNut2006_;
extern SgDbhDatumDescriptor   *&dOptlContib_;
extern bool                    &hasOptlContib_;
extern SgDbhDatumDescriptor   *&dAcSites_;
extern bool                    &hasAcSites_;
extern SgDbhDatumDescriptor   *&dCcSites_;
extern bool                    &hasCcSites_;
extern SgDbhDatumDescriptor   *&dOceOld_;
extern bool                    &hasOceOld_;
extern SgDbhDatumDescriptor   *&dPtdOld_;
extern bool                    &hasPtdOld_;
extern SgDbhDatumDescriptor   *&dFrngErr_;
extern bool                    &hasFrngErr_;
extern SgDbhDatumDescriptor   *&dAcmEpochs_;
extern bool                    &hasAcmEpochs_;
extern SgDbhDatumDescriptor   *&dAcmNumber_;
extern bool                    &hasAcmNumber_;
extern SgDbhDatumDescriptor   *&dParAngle_;
extern bool                    &hasParAngle_;
extern SgDbhDatumDescriptor   *&dConsensusRate_;
extern bool                    &hasConsensusRate_;
extern SgDbhDatumDescriptor   *&dNdryPart_;
extern bool                    &hasNdryPart_;
extern SgDbhDatumDescriptor   *&dNwetPart_;
extern bool                    &hasNwetPart_;
extern SgDbhDatumDescriptor   *&dSun1BendingContrib_;
extern bool                    &hasSun1BendingContrib_;
extern SgDbhDatumDescriptor   *&dSun2BendingContrib_;
extern bool                    &hasSun2BendingContrib_;

extern SgDbhDatumDescriptor   *&dMslmIonContrib_;
extern bool                    &hasMslmIonContrib_;



extern const QString            sCalList[] = {"CABL DEL", "UNPHASCL", "NDRYCONT", "NWETCONT", "        ",
                                              "WVR DELY"};

extern const QString            sFclList[] = {"CFAKBDRY", "CFAKBWET", "CFAJJDRY", "IFADRFLY", "IFADRYSS",
                                              "MTTDRFLY", "MTTDRYSS", "NMFDRFLY"};

extern const QString            sMntTypes[] ={"AZEL", "EQUA", "X_YN", "X_YE", "RICHMOND", "UNKN"};


//("PTD CONT", "WOBXCONT", "WOBYCONT", "ETD CONT", "OCE CONT", "UT1ORTHO", "WOBORTHO", "WOBLIBRA", "UT1LIBRA", "OPTLCONT", "FEED.COR", "OCE_OLD ", "TILTRMVR", "PTOLDCON")
//("PTD CONT", "WOBXCONT", "WOBYCONT", "ETD CONT", "OCE CONT", "PTOLDCON", "UT1ORTHO", "WOBORTHO", "WOBNUTAT", "FEED.COR", "WAHRCONT", "TILTRMVR", "", "")

enum Obc_v10_index
{
  O10_POLE_TIDE     =  0,
  O10_WOBBLE_X      =  1,
  O10_WOBBLE_Y      =  2,
  O10_EARTH_TIDE    =  3,
  O10_OCEAN_TIDE    =  4,
  O10_POLE_TIDE_OLD =  5,
  O10_UT1_ORTHO     =  6,
  O10_WOB_ORTHO     =  7,
  O10_WOB_NUTAT     =  8,
  O10_FEED_ROT      =  9,
  O10_WAHR_NUTAT    = 10,
  O10_TILT_RMVR     = 11
};
enum Obc_v11_index
{
  O11_POLE_TIDE     =  0,
  O11_WOBBLE_X      =  1,
  O11_WOBBLE_Y      =  2,
  O11_EARTH_TIDE    =  3,
  O11_OCEAN_TIDE    =  4,
  O11_UT1_ORTHO     =  5,
  O11_WOB_ORTHO     =  6,
  O11_WOB_LIBRA     =  7,
  O11_UT1_LIBRA     =  8,
  O11_OCN_POLE_TIDE =  9,
  O11_FEED_ROT      = 10,
  O11_OCEAN_TIDE_OLD= 11,
  O11_TILT_RMVR     = 12,
  O11_POLE_TIDE_OLD = 13,
};



/*=======================================================================================================
*
*                           I/O utilities:
* 
*======================================================================================================*/
//
bool SgVlbiSession::getDataFromDbhImages(QMap<QString, SgDbhImage*> &images, 
  bool have2LoadImmatureSession, bool guiExpected, const QStringList& masterfileSuffixes)
{
  SgMJD                         startEpoch(SgMJD::currentMJD());
  bool                          isOk=false;
  
  if (images.size()==0)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() +
      "::getDataFromDbhImages(): nothing to import");
    return false;
  }
  else if (images.size()==1)
  {
    logger->write(SgLogger::DBG, SgLogger::IO_DBH, className() +
      "::getDataFromDbhImages(): importing one-band set of DBH images");
    isOk = getDataFromDbhImage(images.values().at(0), true, true, have2LoadImmatureSession, guiExpected);
  }
  // Here we organize the order of importing.
  // standard IVS case:
  else if (images.size()==2 && images.contains("X") && images.contains("S") &&
      images["X"]->currentVersion() >= images["S"]->currentVersion())
  {
    logger->write(SgLogger::DBG, SgLogger::IO_DBH, className() +
      "::getDataFromDbhImages(): importing typical IVS set of DBH images");
    isOk = getDataFromDbhImage(images["S"], false, true, have2LoadImmatureSession, guiExpected);
    isOk = isOk && getDataFromDbhImage(images["X"], true, true, have2LoadImmatureSession, guiExpected);
  }
  else
  {
    logger->write(SgLogger::DBG, SgLogger::IO_DBH, className() +
      "::getDataFromDbhImages(): importing non-standard set of DBH images");
    // first, we want a band with highest CALC version to be a primary band:
    double                        cverMax(-1.0), cverMin(-1.0), cver(-1.0);
    for (QMap<QString, SgDbhImage*>::iterator it=images.begin(); it!=images.end(); ++it)
    {
      SgDbhImage                 *image=it.value();
      cver = -1.0;
      if ((dCalcVersionValue_=image->lookupDescriptor(rCalcVersionValue_.lCode_)))
      {
        cver = image->getR8(dCalcVersionValue_, 0,0,0);
        if (cver>cverMax)
          cverMax = cver;
        if (cverMin<0.0)
          cverMin = cver;
        else if (cverMin<cver)
          cverMin = cver;
      };
    };
    QMultiMap<double, SgDbhImage*>     imgByCver;
    if (cverMin != cverMax) // ok, we can chose:
    {
      for (QMap<QString, SgDbhImage*>::iterator it=images.begin(); it!=images.end(); ++it)
      {
        SgDbhImage                 *image=it.value();
        if ((dCalcVersionValue_=image->lookupDescriptor(rCalcVersionValue_.lCode_)))
          imgByCver.insert(image->getR8(dCalcVersionValue_, 0,0,0), image);
        else
          imgByCver.insert(0.1*image->currentVersion(), image);
      };
    }
    else // sort by freqs:
    {
      for (QMap<QString, SgDbhImage*>::iterator it=images.begin(); it!=images.end(); ++it)
      {
        SgDbhImage                 *image=it.value();
        if ((dReferenceFrequency_=image->lookupDescriptor(rReferenceFrequency_.lCode_)))
          imgByCver.insert(image->getR8(dReferenceFrequency_, 0,0,0, 0), image);
        else
          imgByCver.insert(0.1*image->currentVersion(), image);
      };
    };
    isOk = true;
    int                         idx=0;
    for (QMultiMap<double, SgDbhImage*>::iterator it=imgByCver.begin(); it!=imgByCver.end(); ++it)
    {
      isOk = isOk && 
        getDataFromDbhImage(it.value(), (idx==imgByCver.size()-1), // true is for highest CVer/freq
          false, have2LoadImmatureSession, guiExpected);
      idx++;
    };
  };
  // remove observations that are not in the primary band:
  int                           idx, num;
  idx = num = 0;
  while (idx<observations_.size())
  {
    SgVlbiObservation          *obs=observations_.at(idx);
    if (!obs->isAttr(SgVlbiObservation::Attr_CONTAINS_ALL_DATA))
    {
      observationByKey_.remove(obs->getKey());
      observations_.removeAt(idx);
      idx--;
      num++;
      delete obs;
    };
    idx++;
  };
  if (num)
    logger->write(SgLogger::INF, SgLogger::IO_DBH, className() +
      "::getDataFromDbhImages(): " + QString("").setNum(num) + 
      " second-band-only observation" + (num==1?" ":"s ") + "were removed");
  //
  if (cppsSoft_ == CPPS_UNKNOWN)
    cppsSoft_ = CPPS_HOPS;
  //
  // perform self check:
  isOk = isOk && selfCheck(guiExpected, masterfileSuffixes);
  //
  SgMJD                         finisEpoch(SgMJD::currentMJD());
  logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() +
    ": getDataFromDbhImages(): the session " + getName() + "  has been read from DBH database" + 
    ", elapsed time: " + QString("").sprintf("%.2f", (finisEpoch - startEpoch)*86400000.0) + " ms");
  //
  return isOk;
};



//
bool SgVlbiSession::getDataFromDbhImage(SgDbhImage *image, bool have2getAll, bool isStandardSession,
  bool have2LoadImmatureSession, bool guiExpected)
{
  SgMJD                         startEpoch(SgMJD::currentMJD());
  bool isOk = true;
  if (!image)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() +
        "::getDataFromDbhImage(): the image is NULL");
    return false;
  };
  //
  if (!image->numberOfObservations())
  {
    logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::getDataFromDbhImage(): the image does not contain observations");
    return false;
  };
  //
  const QString                 dbhName(image->fileName());
  logger->write(SgLogger::INF, SgLogger::IO_DBH, className() +
    "::getDataFromDbhImage(): starting parsing DBH image " + image->sessionID() );
  //
  // check info part:
  int         importingVersion=image->fileVersion();
  if (name_ == "unnamed") // first pass, get some info from the dbh image
  {
    setName(image->sessionID());
    if (name_.mid(8, 1)=="S") // right now manually switch to the primary band:
    {
      name_.replace(8, 1, "X");
      logger->write(SgLogger::INF, SgLogger::IO_DBH, className() +
        "::getDataFromDbhImage(): session name has been switched from " + image->sessionID() + 
        " to " + name_);
    };
    networkSuffix_ = name_.mid(9, 1);
    description_ = image->sessionDescription();
  };
  
  // extract the name of the band: get it from the name of the session, e.g., "$10JUL29SE"
  QString str = image->sessionID();
  QString bandKey = str.mid(8, 1);
  
  // check is it already imported:
  SgVlbiBand                   *band=NULL;
  int                           i=0;
  while (i<bands_.size() && band==NULL)
  {
    if (bands_.at(i)->getKey() == bandKey)
      band = bands_.at(i);
    i++;
  };
  // if it is a new band, insert it to the list:
  if (band == NULL)
    band = new SgVlbiBand;
  else // something going wrong, notify user:
  {
    logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
      "::getDataFromDbhImage(): the band " + bandKey + " already exists in the session; clearing..");
    delete band;
    band = new SgVlbiBand;
  };
  band->setKey(bandKey);
  bands_.append(band);
  bandByKey_.insert(bandKey, band);
  
  // run format verification:
  // first, lookup parameters:
  for (int i=0; i<numOfDxT; i++)
  {
    if (!(dbhDxTable[i].d_=image->lookupDescriptor(dbhDxTable[i].lCode_)))
      dbhDxTable[i].isPresent_ = false;
    else
      dbhDxTable[i].isPresent_ = true;
  };

  // then, check content of the dbh image:
  for (int i=0; i<numOfDxT; i++)
  {
//    if (!dbhDxTable[i].isPresent_ && dbhDxTable[i].isMandatory_ && false &&
    if (!dbhDxTable[i].isPresent_ && dbhDxTable[i].isMandatory_ && !have2LoadImmatureSession &&
        (dbhDxTable[i].expectedVersion_<=importingVersion))
    {
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::getDataFromDbhImage(): cannot find lcode '" + dbhDxTable[i].lCode_ +
        "' in the image; skipping the image of " + dbhName);
      return false;
    }
    else if (!dbhDxTable[i].isPresent_ && !dbhDxTable[i].isMandatory_ &&
             (dbhDxTable[i].expectedVersion_<=importingVersion))
      logger->write(SgLogger::INF, SgLogger::IO_DBH, className() +
        "::getDataFromDbhImage(): cannot find lcode '" + dbhDxTable[i].lCode_ + "' in the image of " +
        dbhName);
    // check types:
    if (dbhDxTable[i].isPresent_ && dbhDxTable[i].type_ != dbhDxTable[i].d_->type())
    {
      if (dbhDxTable[i].isMandatory_)
      {
        logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() +
          "::getDataFromDbhImage(): type missmatch for LCODE '" + dbhDxTable[i].lCode_ +
          "': expected " + SgDbhDatumDescriptor::typeName(dbhDxTable[i].type_) + ", got " + 
          dbhDxTable[i].d_->typeName() + "; skipping the image of " + dbhName);
        return false;
      }
      else
      {
        dbhDxTable[i].isPresent_ = false;
        logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
          "::getDataFromDbhImage(): type missmatch for LCODE '" + dbhDxTable[i].lCode_ +
          "': expected " + SgDbhDatumDescriptor::typeName(dbhDxTable[i].type_) + ", got " + 
          dbhDxTable[i].d_->typeName() + "; skipping use of the LCODE");
      };
    };
  };

  // import observations:
  QString                               station1Name, station2Name, sourceName, baselineName;
  QString                               scanName, scanId, obsKey;
  SgMJD                                 epoch;
  SgVlbiObservable                     *o=NULL;
  SgVlbiStationInfo                    *station1Info, *station2Info;
  SgVlbiStationInfo                    *bandStation1Info, *bandStation2Info;
  SgVlbiSourceInfo                     *sourceInfo, *bandSourceInfo;
  SgVlbiBaselineInfo                   *baselineInfo, *bandBaselineInfo;
  int                                   year, month, day, hour, minute;
  double                                second;
  SgVlbiObservation                    *obs=NULL;
  SgVlbiAuxObservation                 *auxObs=NULL, *auxObsE=NULL;
  QMap<QString, SgVlbiAuxObservation*> *auxObsByScan=NULL;
  SgMeteoData                           meteo1, meteo2;
  double                                cableCorrection1=0.0, cableCorrection2=0.0;
  double                                refFreq=0.0;
  double                                azimuthAngle1=0.0, azimuthAngle2=0.0;
  double                                azimuthAngleRate1=0.0, azimuthAngleRate2=0.0;
  double                                elevationAngle1=0.0, elevationAngle2=0.0;
  double                                elevationAngleRate1=0.0, elevationAngleRate2=0.0;
  double                                parallacticAngle1=0.0, parallacticAngle2=0.0;
  double                                calcNdryContribDel1, calcNdryContribDel2;
  double                                calcNwetContribDel1, calcNwetContribDel2;
  double                                calcNdryContribRat1, calcNdryContribRat2;
  double                                calcNwetContribRat1, calcNwetContribRat2;
  double                                calcNgradNPartDel1, calcNgradEPartDel1;
  double                                calcNgradNPartDel2, calcNgradEPartDel2;
  double                                calcNgradNPartRat1, calcNgradEPartRat1;
  double                                calcNgradNPartRat2, calcNgradEPartRat2;
  double                                calcNdryPartDel1, calcNdryPartDel2;
  double                                calcNdryPartRat1, calcNdryPartRat2;
  double                                calcNwetPartDel1, calcNwetPartDel2;
  double                                calcNwetPartRat1, calcNwetPartRat2;
  double                                calcUnPhaseCal1, calcUnPhaseCal2;
  double                                calcAxisOffsetDel1, calcAxisOffsetDel2;
  double                                calcAxisOffsetRat1, calcAxisOffsetRat2;
  double                                partAxisOffsetDel1, partAxisOffsetDel2;
  double                                partAxisOffsetRat1, partAxisOffsetRat2;
  double                                calcOcnLdDelH1, calcOcnLdDelH2, calcOcnLdDelV1, calcOcnLdDelV2;
  //
  calcNdryContribDel1 = calcNdryContribDel2 = calcNwetContribDel1 = calcNwetContribDel2 =
    calcNdryContribRat1 = calcNdryContribRat2 = calcNwetContribRat1 = calcNwetContribRat2 =
    calcNgradNPartDel1 = calcNgradEPartDel1 = calcNgradNPartDel2 = calcNgradEPartDel2 =
    calcNgradNPartRat1 = calcNgradEPartRat1 = calcNgradNPartRat2 = calcNgradEPartRat2 = 0.0;
  calcNdryPartDel1 = calcNdryPartDel2 = calcNdryPartRat1 = calcNdryPartRat2 = 0.0;
  calcNwetPartDel1 = calcNwetPartDel2 = calcNwetPartRat1 = calcNwetPartRat2 = 0.0;
  calcUnPhaseCal1 = calcUnPhaseCal2 = calcAxisOffsetDel1 = calcAxisOffsetDel2 = 0.0;
  calcAxisOffsetRat1 = calcAxisOffsetRat2 = 0.0;
  calcOcnLdDelH1 = calcOcnLdDelH2 = calcOcnLdDelV1 = calcOcnLdDelV2 = 0.0;
 
  // temporary (for pcal evaluating):
//  double                                pCal4GrpDelay1=2.0, pCal4GrpDelay2=2.0;
//  double                                pCalSigma4GrpDelay1=1.0, pCalSigma4GrpDelay2=1.0;
//  SgVector                              pCal1ByChan(16), pCal2ByChan(16);
//  SgVector                              pCalOffset1ByChan(16), pCalOffset2ByChan(16);

  int                                   numOfChannels=0;
  int                                   maxNumOfChannels=0;
  double                                f;
  bool                                  isSbdSigmaNan, isSbdSigmaInf;
  bool                                  isGrdSigmaNan, isGrdSigmaInf;
  bool                                  isPhrSigmaNan, isPhrSigmaInf;
  if (hasCalByFrq_ && hasRefFreqByChannel_)
  {
    if (dCalByFrq_->dim3() != dRefFreqByChannel_->dim1()) // complain:
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::getDataFromDbhImage(): the lcodes '" + rCalByFrq_.lCode_ +
        "' and '" + rRefFreqByChannel_.lCode_ + "' have incompatible dimensions"
        + QString("").sprintf(": %d vs %d", dCalByFrq_->dim3(), dRefFreqByChannel_->dim1()));
    maxNumOfChannels = std::min(dCalByFrq_->dim3(), dRefFreqByChannel_->dim1());
    logger->write(SgLogger::DBG, SgLogger::IO_DBH, className() + 
      "::getDataFromDbhImage(): the session has maximum " + 
      QString("").sprintf(" %d channels", maxNumOfChannels));
  };
  numOfChannels = maxNumOfChannels;
  // eot
  // set up correlator type, we use it in the loop:
  if (hasCorrelatorType_)
    band->setCorrelatorType(image->getStr(dCorrelatorType_, 0,0));
  else
    band->setCorrelatorType("Mk4"); // sort of default
  if (getCorrelatorType() != "UNKN")
    setCorrelatorType(band->getCorrelatorType());
  //
  // run the loop:
  for (int obsIdx=0; obsIdx<image->numberOfObservations(); obsIdx++)
  {
    // ========================= observation ==========================
    year   = image->getI2(dUtcTag_, 0, 0, 0, obsIdx);
    month  = image->getI2(dUtcTag_, 1, 0, 0, obsIdx);
    day    = image->getI2(dUtcTag_, 2, 0, 0, obsIdx);
    hour   = image->getI2(dUtcTag_, 3, 0, 0, obsIdx);
    minute = image->getI2(dUtcTag_, 4, 0, 0, obsIdx);
    second = image->getR8(dSecTag_, 0, 0, 0, obsIdx);
    epoch.setUpEpoch(year, month, day, hour, minute, second);
    
    // stations and source names:
    station1Name = image->getStr(dBaselineName_, 0, 0, obsIdx);
    station2Name = image->getStr(dBaselineName_, 1, 0, obsIdx);
    sourceName = image->getStr(dSourceName_, 0, 0, obsIdx);
    baselineName = station1Name + ":" + station2Name;
    // scanId and scanName:
    scanId = epoch.toString(SgMJD::F_INTERNAL) + "@" + sourceName;
    if (isStandardSession && hasScanName_)
      scanName = image->getStr(dScanName_, 0, 0, obsIdx);
    else if (isStandardSession && hasRunCode_)
      scanName = image->getStr(dRunCode_, 0, 0, obsIdx);
    else
      scanName.sprintf("%03d-%02d:%02d:%04.1f@%s",
        epoch.calcDayOfYear(), epoch.calcHour(), epoch.calcMin(), epoch.calcSec(), 
        qPrintable(sourceName));
    //
    // pick up or create an observation:
    obsKey.sprintf("%s", 
      qPrintable(epoch.toString(SgMJD::F_INTERNAL) + "-" + baselineName + "@" + sourceName));
    if (observationByKey_.contains(obsKey))
      obs = observationByKey_.value(obsKey);
    else
    {
      obs = new SgVlbiObservation(this);
      obs->setMJD(epoch);
      obs->setScanName(scanName);
      obs->setScanId(scanId);
      obs->setKey(obsKey);
      obs->setMediaIdx(observations_.size());
      observations_.append(obs);
      observationByKey_.insert(obsKey, obs);
    };
    o = new SgVlbiObservable(obs, band);
    o->setMediaIdx(obsIdx);
    obs->addObservable(band->getKey(), o);
    obs->setupActiveObservable(bandKey);

    // station #1:
    if (stationsByName_.contains(station1Name))
      station1Info = stationsByName_.value(station1Name);
    else // new station, add it to the container and register its index:
    {
      station1Info = new SgVlbiStationInfo(stationsByName_.size(), station1Name);
      stationsByName_.insert(station1Info->getKey(), station1Info);
      stationsByIdx_.insert(station1Info->getIdx(), station1Info);
    };
    // station #2:
    if (stationsByName_.contains(station2Name))
      station2Info = stationsByName_.value(station2Name);
    else // new station, add it to the container and register its index:
    {
      station2Info = new SgVlbiStationInfo(stationsByName_.size(), station2Name);
      stationsByName_.insert(station2Info->getKey(), station2Info);
      stationsByIdx_.insert(station2Info->getIdx(), station2Info);
    };
    // source:
    if (sourcesByName_.contains(sourceName))
      sourceInfo = sourcesByName_.value(sourceName);
    else // new source, add it to the container and register its index:
    {
      sourceInfo = new SgVlbiSourceInfo(sourcesByName_.size(), sourceName);
      sourcesByName_.insert(sourceInfo->getKey(), sourceInfo);
      sourcesByIdx_.insert(sourceInfo->getIdx(), sourceInfo);
    };
    // baseline:
    if (baselinesByName_.contains(baselineName))
      baselineInfo = baselinesByName_.value(baselineName);
    else // new baseline, add it to the container and register its index:
    {
      baselineInfo = new SgVlbiBaselineInfo(baselinesByName_.size(), baselineName);
      baselinesByName_.insert(baselineInfo->getKey(), baselineInfo);
      baselinesByIdx_.insert(baselineInfo->getIdx(), baselineInfo);
    };
    // 4band:
    // band's station #1:
    if (band->stationsByName().contains(station1Name))
      bandStation1Info = band->stationsByName().value(station1Name);
    else // new station, add it to the container:
    {
      bandStation1Info = new SgVlbiStationInfo(station1Info->getIdx(), station1Name);
      band->stationsByName().insert(bandStation1Info->getKey(), bandStation1Info);
      band->stationsByIdx().insert(bandStation1Info->getIdx(), bandStation1Info);
    };
    // band's station #2:
    if (band->stationsByName().contains(station2Name))
      bandStation2Info = band->stationsByName().value(station2Name);
    else // new station, add it to the container:
    {
      bandStation2Info = new SgVlbiStationInfo(station2Info->getIdx(), station2Name);
      band->stationsByName().insert(bandStation2Info->getKey(), bandStation2Info);
      band->stationsByIdx().insert(bandStation2Info->getIdx(), bandStation2Info);
    };
    // band's source:
    if (band->sourcesByName().contains(sourceName))
      bandSourceInfo = band->sourcesByName().value(sourceName);
    else // new source, add it to the container:
    {
      bandSourceInfo = new SgVlbiSourceInfo(sourceInfo->getIdx(), sourceName);
      band->sourcesByName().insert(bandSourceInfo->getKey(), bandSourceInfo);
      band->sourcesByIdx().insert(bandSourceInfo->getIdx(), bandSourceInfo);
    };
    // band's baselines:
    if (band->baselinesByName().contains(baselineName))
      bandBaselineInfo = band->baselinesByName().value(baselineName);
    else // new baseline, add it to the container and register its index:
    {
      bandBaselineInfo = new SgVlbiBaselineInfo(baselineInfo->getIdx(), baselineName);
      band->baselinesByName().insert(bandBaselineInfo->getKey(), bandBaselineInfo);
      band->baselinesByIdx().insert(bandBaselineInfo->getIdx(), bandBaselineInfo);
    };
    
    obs->setStation1Idx(station1Info->getIdx());
    obs->setStation2Idx(station2Info->getIdx());
    obs->setSourceIdx(sourceInfo->getIdx());
    obs->setBaselineIdx(baselineInfo->getIdx());
    //
    // plus additional info:
    if (hasFourFitFileName_)
      o->setFourfitOutputFName(image->getStr(dFourFitFileName_, 0, 0, obsIdx));
    //
    // collect observables:
    // single band delay:
    if (dSbDelay_)
      o->sbDelay().setValue(1.0e-6*(image->getR8(dSbDelay_, 0, 0, 0, obsIdx) + 
                                    image->getR8(dSbDelay_, 1, 0, 0, obsIdx)   ));
    // group delay:
    if (dObservedDelay_)
      o->grDelay().setValue(1.0e-6*(image->getR8(dObservedDelay_, 0, 0, 0, obsIdx) + 
                                    image->getR8(dObservedDelay_, 1, 0, 0, obsIdx)  ));
    // delay rate:
    if (dDelayRate_)
      o->phDRate().setValue(image->getR8(dDelayRate_,      0, 0, 0, obsIdx));
    //
    // workaround the DBH "feature":
    isSbdSigmaNan = isSbdSigmaInf = isGrdSigmaNan = isGrdSigmaInf = isPhrSigmaNan = isPhrSigmaInf 
      = false;
    // single band delay sigma:
    if (dSbSigmaDelay_)
      f = image->getR8(dSbSigmaDelay_, 0, 0, 0, obsIdx);
    else
      f = 1.0;
    if (isinf(f) != 0)
    {
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::getDataFromDbhImage(): the observation #" + QString("").setNum(obsIdx) +
        " at the baseline <" + baselineName + "> observing " + sourceName + " source at " +
        epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) +
        " contains wrong sigma (inf) for the single band delay", true);
        isSbdSigmaInf = true;
      f = 1.0;
      //      image->setR8(dSbSigmaDelay_, 0,0,0, obsIdx,  f);
    };
    if (isnan(f) != 0)
    {
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::getDataFromDbhImage(): the observation #" + QString("").setNum(obsIdx) +
        " at the baseline <" + baselineName + "> observing " + sourceName + " source at " +
        epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) +
        " contains wrong sigma (nan) for the single band delay", true);
      isSbdSigmaNan = true;
      f = 1.0;
      //      image->setR8(dSbSigmaDelay_, 0,0,0, obsIdx,  f);
    };
    o->sbDelay().setSigma(1.0e-6*f);
    //
    // group delay sigma:
    if (dObservedDelaySigma_)
      f = image->getR8(dObservedDelaySigma_, 0, 0, 0, obsIdx);
    else
      f = 1.0;
    if (isinf(f) != 0)
    {
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::getDataFromDbhImage(): the observation #" + QString("").setNum(obsIdx) +
        " at the baseline <" + baselineName + "> observing " + sourceName + " source at " +
        epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) +
        " contains wrong sigma (inf) for the group delay", true);
      isGrdSigmaInf = true;
      f = 1.0e-6;
      //      image->setR8(dObservedDelaySigma_, 0,0,0, obsIdx,  f);
    };
    if (isnan(f) != 0)
    {
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::getDataFromDbhImage(): the observation #" + QString("").setNum(obsIdx) +
        " at the baseline <" + baselineName + "> observing " + sourceName + " source at " +
        epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) +
        " contains wrong sigma (nan) for the group delay", true);
      isGrdSigmaNan = true;
      f = 1.0e-6;
      //      image->setR8(dObservedDelaySigma_, 0,0,0, obsIdx,  f);
    };
    o->grDelay().setSigma(f);
    //
    // delay rate sigma:
    if (dSigmaDelayRate_)
      f = image->getR8(dSigmaDelayRate_, 0, 0, 0, obsIdx);
    else
      f = 1.0;
    if (isinf(f) != 0)
    {
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::getDataFromDbhImage(): the observation #" + QString("").setNum(obsIdx) +
        " at the baseline <" + baselineName + "> observing " + sourceName + " source at " +
        epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) +
        " contains wrong sigma (inf) for the group delay", true);
      isPhrSigmaInf = true;
      f = 1.0e-6;
      //      image->setR8(dSigmaDelayRate_, 0,0,0, obsIdx,  f);
    };
    if (isnan(f) != 0)
    {
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::getDataFromDbhImage(): the observation #" + QString("").setNum(obsIdx) +
        " at the baseline <" + baselineName + "> observing " + sourceName + " source at " +
        epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) +
        " contains wrong sigma (nan) for the group delay", true);
      isPhrSigmaNan = true;
      f = 1.0e-6;
      //      image->setR8(dSigmaDelayRate_, 0,0,0, obsIdx,  f);
    };
    o->phDRate().setSigma(f);
    //
    if (guiExpected &&
        (isSbdSigmaNan || isSbdSigmaInf ||
         isGrdSigmaNan || isGrdSigmaInf ||
         isPhrSigmaNan || isPhrSigmaInf  ))
    {
      QString                   s1(""), s2(""), s3("");
      s1 = (isSbdSigmaNan || isGrdSigmaNan || isPhrSigmaNan)?"(nan)":"(inf)";
      if (isSbdSigmaNan || isSbdSigmaInf)
        s2 = "single band delay, ";
      if (isGrdSigmaNan || isGrdSigmaInf)
        s2+= "group delay, ";
      if (isPhrSigmaNan || isPhrSigmaInf)
        s2+= "delay rate, ";
      s2 = s2.left(s2.size() - 2);
      if (o->getFourfitOutputFName().size())
        s3 = "\n\tfourfit file name: " + obs->getScanName().trimmed() + "/" + o->getFourfitOutputFName();
      
      QMessageBox::warning(NULL, "Warning: wrong value",
        "The observation #" + QString("").setNum(obsIdx) + " contains wrong sigma " + s1 + " for " + s2 +
        ".\n\nThe observation identities:\n" + "\t[" + baselineName + "] @" + sourceName + 
        "\n\tepoch: " + epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) + s3 +
        "\n\nBatch SOLVE will likely fail to deal with this session, report to the correlator.");
    };
    //
    // --------------------------------------------------------------------------------------------------
    if (have2getAll)
    {
      // it is a primary band:
      obs->addAttr(SgVlbiObservation::Attr_CONTAINS_ALL_DATA);
      //
      // theo consensus delay:
      if (hasConsensusDelay_)
        obs->setCalcConsensusDelay(1.0e-6* (image->getR8(dConsensusDelay_, 0, 0, 0, obsIdx) + 
                                            image->getR8(dConsensusDelay_, 1, 0, 0, obsIdx)) );
      // theo consensus rate:
      if (hasConsensusRate_)
        obs->setCalcConsensusRate(image->getR8(dConsensusRate_, 0, 0, 0, obsIdx));

      if (hasConsBendingDelay_)
      {
        obs->setCalcConsBendingDelay(image->getR8(dConsBendingDelay_, 0, 0, 0, obsIdx));
        obs->setCalcConsBendingRate (image->getR8(dConsBendingDelay_, 1, 0, 0, obsIdx));
      };
      if (hasSun1BendingContrib_)
      {
        obs->setCalcConsBendingSunDelay(image->getR8(dSun1BendingContrib_, 0, 0, 0, obsIdx));
        obs->setCalcConsBendingSunRate (image->getR8(dSun1BendingContrib_, 1, 0, 0, obsIdx));
      };
      if (hasSun2BendingContrib_)
      {
        obs->setCalcConsBendingSunHigherDelay(image->getR8(dSun2BendingContrib_, 0, 0, 0, obsIdx));
        obs->setCalcConsBendingSunHigherRate (image->getR8(dSun2BendingContrib_, 1, 0, 0, obsIdx));
      };
      //
      if (hasMslmIonContrib_)
      {
        obs->setCalcGpsIonoCorrectionDelay(image->getR8(dMslmIonContrib_, 0, 0, 0, obsIdx));
        obs->setCalcGpsIonoCorrectionRate (image->getR8(dMslmIonContrib_, 1, 0, 0, obsIdx));
      };
      //
      //
      // contributions:
      if (hasOloadContrib_)
      {
        obs->setCalcOceanTideDelay(image->getR8(dOloadContrib_, 0, 0, 0, obsIdx));
        obs->setCalcOceanTideRate (image->getR8(dOloadContrib_, 1, 0, 0, obsIdx));
      };
      calcOcnLdDelH1 = calcOcnLdDelH2 = calcOcnLdDelV1 = calcOcnLdDelV2 = 0.0;
      if (hasOloadHorzContrib_ && hasOloadVertContrib_)
      {
        calcOcnLdDelH1 =-image->getR8(dOloadHorzContrib_, 0, 0, 0, obsIdx);
        calcOcnLdDelH2 = image->getR8(dOloadHorzContrib_, 1, 0, 0, obsIdx);
        calcOcnLdDelV1 =-image->getR8(dOloadVertContrib_, 0, 0, 0, obsIdx);
        calcOcnLdDelV2 = image->getR8(dOloadVertContrib_, 1, 0, 0, obsIdx);
      };
      if (hasUt1Ortho_)
      {
        obs->setCalcHiFyUt1Delay(image->getR8(dUt1Ortho_, 0, 0, 0, obsIdx));
        obs->setCalcHiFyUt1Rate (image->getR8(dUt1Ortho_, 1, 0, 0, obsIdx));
      };
      if (hasWobOrtho_)
      {
        obs->setCalcHiFyPxyDelay(image->getR8(dWobOrtho_, 0, 0, 0, obsIdx));
        obs->setCalcHiFyPxyRate (image->getR8(dWobOrtho_, 1, 0, 0, obsIdx));
      };
      if (hasPtdContrib_)
      {
        obs->setCalcPoleTideDelay(image->getR8(dPtdContrib_, 0, 0, 0, obsIdx));
        obs->setCalcPoleTideRate (image->getR8(dPtdContrib_, 1, 0, 0, obsIdx));
      };
      if (hasEtdContrib_)
      {
        obs->setCalcEarthTideDelay(image->getR8(dEtdContrib_, 0, 0, 0, obsIdx));
        obs->setCalcEarthTideRate (image->getR8(dEtdContrib_, 1, 0, 0, obsIdx));
      };
      if (hasOptlContib_)
      {
        obs->setCalcOceanPoleTideLdDelay(image->getR8(dOptlContib_, 0, 0, 0, obsIdx));
        obs->setCalcOceanPoleTideLdRate (image->getR8(dOptlContib_, 1, 0, 0, obsIdx));
      };
      if (hasWobXContrib_)
      {
        obs->setCalcPxDelay(image->getR8(dWobXContrib_, 0, 0, 0, obsIdx));
        obs->setCalcPxRate (image->getR8(dWobXContrib_, 1, 0, 0, obsIdx));
      };
      if (hasWobYContrib_)
      {
        obs->setCalcPyDelay(image->getR8(dWobYContrib_, 0, 0, 0, obsIdx));
        obs->setCalcPyRate (image->getR8(dWobYContrib_, 1, 0, 0, obsIdx));
      }
      if (hasWobNutatContrib_)
        obs->setCalcWobNutatContrib(image->getR8(dWobNutatContrib_, 0, 0, 0, obsIdx));
      if (hasWobLibra_)
      {
        obs->setCalcHiFyPxyLibrationDelay(image->getR8(dWobLibra_, 0, 0, 0, obsIdx));
        obs->setCalcHiFyPxyLibrationRate (image->getR8(dWobLibra_, 1, 0, 0, obsIdx));
      };
      if (hasUt1Libra_)
      {
        obs->setCalcHiFyUt1LibrationDelay(image->getR8(dUt1Libra_, 0, 0, 0, obsIdx));
        obs->setCalcHiFyUt1LibrationRate (image->getR8(dUt1Libra_, 1, 0, 0, obsIdx));
      };
      if (hasNut06xys_)
      {
        obs->setCalcCipXv(image->getR8(dNut06xys_, 0, 0, 0, obsIdx));
        obs->setCalcCipYv(image->getR8(dNut06xys_, 1, 0, 0, obsIdx));
        obs->setCalcCipSv(image->getR8(dNut06xys_, 2, 0, 0, obsIdx));
      };
      if (hasNutWahr_)
      {
        obs->setCalcNutWahr_dPsiV(image->getR8(dNutWahr_, 0, 0, 0, obsIdx));
        obs->setCalcNutWahr_dEpsV(image->getR8(dNutWahr_, 1, 0, 0, obsIdx));
      };
      if (hasNut2006_)
      {
        obs->setCalcNut2006_dPsiV(image->getR8(dNut2006_, 0, 0, 0, obsIdx));
        obs->setCalcNut2006_dEpsV(image->getR8(dNut2006_, 1, 0, 0, obsIdx));
      };
      if (hasOceOld_)
      {
        obs->setCalcOceanTideOldDelay(image->getR8(dOceOld_, 0, 0, 0, obsIdx));
        obs->setCalcOceanTideOldRate (image->getR8(dOceOld_, 1, 0, 0, obsIdx));
      };
      if (hasPtdOld_)
      {
        obs->setCalcPoleTideOldDelay(image->getR8(dPtdOld_, 0, 0, 0, obsIdx));
        obs->setCalcPoleTideOldRate (image->getR8(dPtdOld_, 1, 0, 0, obsIdx));
      };
      if (hasFeedCorr_)
      {
        obs->setCalcFeedCorrDelay(image->getR8(dFeedCorr_, 0, 0, 0, obsIdx));
        obs->setCalcFeedCorrRate (image->getR8(dFeedCorr_, 1, 0, 0, obsIdx));
      };
      if (hasTiltRemvr_)
      {
        obs->setCalcTiltRemvrDelay(image->getR8(dTiltRemvr_, 0, 0, 0, obsIdx));
        obs->setCalcTiltRemvrRate (image->getR8(dTiltRemvr_, 1, 0, 0, obsIdx));
      };
      if (hasNdryCont_)
      {
        calcNdryContribDel1 =-image->getR8(dNdryCont_, 0, 0, 0, obsIdx);
        calcNdryContribDel2 = image->getR8(dNdryCont_, 1, 0, 0, obsIdx);
        calcNdryContribRat1 =-image->getR8(dNdryCont_, 0, 1, 0, obsIdx);
        calcNdryContribRat2 = image->getR8(dNdryCont_, 1, 1, 0, obsIdx);
      };
      if (hasNwetCont_)
      {
        calcNwetContribDel1 =-image->getR8(dNwetCont_, 0, 0, 0, obsIdx);
        calcNwetContribDel2 = image->getR8(dNwetCont_, 1, 0, 0, obsIdx);
        calcNwetContribRat1 =-image->getR8(dNwetCont_, 0, 1, 0, obsIdx);
        calcNwetContribRat2 = image->getR8(dNwetCont_, 1, 1, 0, obsIdx);
      };
      if (hasNgradParts_)
      {
        calcNgradNPartDel1 = image->getR8(dNgradParts_, 0, 0, 0, obsIdx);
        calcNgradEPartDel1 = image->getR8(dNgradParts_, 0, 1, 0, obsIdx);
        calcNgradNPartDel2 = image->getR8(dNgradParts_, 1, 0, 0, obsIdx);
        calcNgradEPartDel2 = image->getR8(dNgradParts_, 1, 1, 0, obsIdx);
        calcNgradNPartRat1 = image->getR8(dNgradParts_, 0, 0, 1, obsIdx);
        calcNgradEPartRat1 = image->getR8(dNgradParts_, 0, 1, 1, obsIdx);
        calcNgradNPartRat2 = image->getR8(dNgradParts_, 1, 0, 1, obsIdx);
        calcNgradEPartRat2 = image->getR8(dNgradParts_, 1, 1, 1, obsIdx);
      };
      if (hasUnPhaseCal_)
      {
        calcUnPhaseCal1 =-image->getR8(dUnPhaseCal_, 0, 0, 0, obsIdx);
        calcUnPhaseCal2 = image->getR8(dUnPhaseCal_, 1, 0, 0, obsIdx);
      };
      if (hasAxOffsetCont_)
      {
        calcAxisOffsetDel1 =-image->getR8(dAxOffsetCont_, 0, 0, 0, obsIdx);
        calcAxisOffsetDel2 = image->getR8(dAxOffsetCont_, 1, 0, 0, obsIdx);
        calcAxisOffsetRat1 =-image->getR8(dAxOffsetCont_, 0, 1, 0, obsIdx);
        calcAxisOffsetRat2 = image->getR8(dAxOffsetCont_, 1, 1, 0, obsIdx);
      };
      //
      // partials:
      // dTau by dR stations:
      if (hasSitPart_)
      {
        double rx, ry, rz;
        // delay:
        rx = image->getR8(dSitPart_, 0, 0, 0, obsIdx);
        ry = image->getR8(dSitPart_, 1, 0, 0, obsIdx);
        rz = image->getR8(dSitPart_, 2, 0, 0, obsIdx);
        obs->setDdel_dR_1(Sg3dVector(rx, ry, rz));
        rx = image->getR8(dSitPart_, 0, 1, 0, obsIdx);
        ry = image->getR8(dSitPart_, 1, 1, 0, obsIdx);
        rz = image->getR8(dSitPart_, 2, 1, 0, obsIdx);
        obs->setDdel_dR_2(Sg3dVector(rx, ry, rz));
        // rate:
        rx = image->getR8(dSitPart_, 0, 0, 1, obsIdx);
        ry = image->getR8(dSitPart_, 1, 0, 1, obsIdx);
        rz = image->getR8(dSitPart_, 2, 0, 1, obsIdx);
        obs->setDrat_dR_1(Sg3dVector(rx, ry, rz));
        rx = image->getR8(dSitPart_, 0, 1, 1, obsIdx);
        ry = image->getR8(dSitPart_, 1, 1, 1, obsIdx);
        rz = image->getR8(dSitPart_, 2, 1, 1, obsIdx);
        obs->setDrat_dR_2(Sg3dVector(rx, ry, rz));
      };
      // sources
      if (hasStarPart_)
      {
        obs->setDdel_dRA( image->getR8(dStarPart_, 0, 0, 0, obsIdx) );
        obs->setDdel_dDN( image->getR8(dStarPart_, 1, 0, 0, obsIdx) );
        obs->setDrat_dRA( image->getR8(dStarPart_, 0, 1, 0, obsIdx) );
        obs->setDrat_dDN( image->getR8(dStarPart_, 1, 1, 0, obsIdx) );
      };
      // dUT1:
      if (hasUT1Part_)
      {
        obs->setDdel_dUT1   (-image->getR8(dUT1Part_, 0, 0, 0, obsIdx)*86400.0 );
//      obs->setDTau_dUT1dot(-image->getR8(dUT1Part_, 1, 0, 0, obsIdx)*86400.0 );
        obs->setDrat_dUT1   (-image->getR8(dUT1Part_, 1, 0, 0, obsIdx)*86400.0 );
      };
      // Polar motion:
      if (hasWobblePart_)
      {
        obs->setDdel_dPx( image->getR8(dWobblePart_, 0, 0, 0, obsIdx) );
        obs->setDdel_dPy( image->getR8(dWobblePart_, 1, 0, 0, obsIdx) );
        obs->setDrat_dPx( image->getR8(dWobblePart_, 0, 1, 0, obsIdx) );
        obs->setDrat_dPy( image->getR8(dWobblePart_, 1, 1, 0, obsIdx) );
      };
      // Angles of nutation:
      if (hasNut06xyp_)
      {
        obs->setDdel_dCipX( image->getR8(dNut06xyp_, 0, 0, 0, obsIdx) );
        obs->setDdel_dCipY( image->getR8(dNut06xyp_, 1, 0, 0, obsIdx) );
        obs->setDrat_dCipX( image->getR8(dNut06xyp_, 0, 1, 0, obsIdx) );
        obs->setDrat_dCipY( image->getR8(dNut06xyp_, 1, 1, 0, obsIdx) );
      }
      else if (hasNutationPart2K_)
      {
        obs->setDdel_dCipX( image->getR8(dNutationPart2K_, 0, 0, 0, obsIdx) );
        obs->setDdel_dCipY( image->getR8(dNutationPart2K_, 1, 0, 0, obsIdx) );
        obs->setDrat_dCipX( image->getR8(dNutationPart2K_, 0, 1, 0, obsIdx) );
        obs->setDrat_dCipY( image->getR8(dNutationPart2K_, 1, 1, 0, obsIdx) );
      }
      else if (hasNutationPart_)
      {
        obs->setDdel_dCipX( image->getR8(dNutationPart_, 0, 0, 0, obsIdx) );
        obs->setDdel_dCipY( image->getR8(dNutationPart_, 1, 0, 0, obsIdx) );
        obs->setDrat_dCipX( image->getR8(dNutationPart_, 0, 1, 0, obsIdx) );
        obs->setDrat_dCipY( image->getR8(dNutationPart_, 1, 1, 0, obsIdx) );
      };
    };
    //
    // ambiguity spacing:
    if (dGrpAmbiguity_)
      o->grDelay().setAmbiguitySpacing(image->getR8(dGrpAmbiguity_, 0, 0, 0, obsIdx));
    // ambiguity resolution:
    if (hasNAmbig_)
      o->grDelay().setNumOfAmbiguities(image->getI2(dNAmbig_, 0, 0, 0, obsIdx));
    if (hasNAmbigSecBand_ && obs->passiveObses().size()>0)
      obs->passiveObses().at(0)->grDelay().setNumOfAmbiguities(
                                                        image->getI2(dNAmbigSecBand_, 0, 0, 0, obsIdx));
    if (hasGrpAmbiguitySecBand_ && obs->passiveObses().size()>0) // just check and complain:
    {
      double                    s=image->getR8(dGrpAmbiguitySecBand_, 0, 0, 0, obsIdx);
      if (s != obs->passiveObses().at(0)->grDelay().getAmbiguitySpacing())
        logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() +
          "::getDataFromDbhImage(): secondary band ambiguity spacing, " + 
          QString("").setNum(obs->passiveObses().at(0)->grDelay().getAmbiguitySpacing()) +
          ", is different from value of GRPAMB_S LCode, " +
          QString("").setNum(s) + "; the difference is " + 
          QString("").setNum((obs->passiveObses().at(0)->grDelay().getAmbiguitySpacing() - s)*1.0E9) +
          "ns", true);
    };
    //
    //
    int                         qFactor;
    bool                        isOk;
    str = image->getStr(dQualityCode_, 0, 0, obsIdx);
    qFactor = str.toInt(&isOk);
    if (!isOk)
    {
      qFactor = -1;
      o->setErrorCode(str.simplified());
    }
    else if (hasFrngErr_)
      o->setErrorCode(image->getStr(dFrngErr_, 0, 0, obsIdx).simplified());
    else
      o->setErrorCode("");
    o->setQualityFactor(qFactor);
    
    if (hasQualityCodeSecBand_ && obs->passiveObses().size()>0)
    {
      str = image->getStr(dQualityCodeSecBand_, 0, 0, obsIdx);
      if (str != "  ")
      {
        qFactor = str.toInt(&isOk);
        if (!isOk)
          qFactor = -1;
        obs->passiveObses().at(0)->setQualityFactor(qFactor);
      };
    };
    //
    //
    if (dDelayUFlag_)
      o->grDelay().setUnweightFlag(image->getI2(dDelayUFlag_, 0, 0, 0, obsIdx));
    //
    if (config_->getUseSolveObsSuppresionFlags() && 
        have2getAll &&  // some second band versions could have wrong DELFLAGs, e.g., 99DEC14SA_V004
        (o->grDelay().getUnweightFlag()==1 || o->grDelay().getUnweightFlag()==2) &&
        importingVersion>=4) // otherwice, where the flag came from?
    {
      o->propagateAttr(SgVlbiMeasurement::Attr_NOT_VALID);
    };
    //
    if (dRateUFlag_)
      o->phDRate().setUnweightFlag(image->getI2(dRateUFlag_, 0, 0, 0, obsIdx));
    if (dReferenceFrequency_)
      o->setReferenceFrequency((refFreq=image->getR8(dReferenceFrequency_, 0, 0, 0, obsIdx)));
    if (hasNumOfChannels_)
      o->setNumOfChannels((numOfChannels=image->getI2(dNumOfChannels_, 0, 0, 0, obsIdx)));
    if (dCorrCoef_)
      o->setCorrCoeff(image->getR8(dCorrCoef_, 0, 0, 0, obsIdx));
    if (dSnr_)
      o->setSnr(image->getR8(dSnr_, 0, 0, 0, obsIdx));
    if (dTotalPhase_)
      o->setTotalPhase(image->getR8(dTotalPhase_, 0, 0, 0, obsIdx)*DEG2RAD);
    // check for ionospheric correction, if it is already calculated, get it:
    if (hasIonCorr_ && hasIonRms_)
    {
      o->grDelay().setIonoValue(image->getR8(dIonCorr_, 0, 0, 0, obsIdx));
      o->phDRate().setIonoValue(image->getR8(dIonCorr_, 1, 0, 0, obsIdx));
      o->grDelay().setIonoSigma(image->getR8(dIonRms_,  0, 0, 0, obsIdx));
      o->phDRate().setIonoSigma(image->getR8(dIonRms_,  1, 0, 0, obsIdx));
    };
    //
    // collect data necessary to evaluate the effective frequencies (for ionospheric correction):
    o->allocateChannelsSetupStorages(numOfChannels);
    SgVector&                   numOfAccPeriodsByChan_USB = *o->numOfAccPeriodsByChan_USB();
    SgVector&                   numOfAccPeriodsByChan_LSB = *o->numOfAccPeriodsByChan_LSB();
    SgVector&                   numOfSamplesByChan_USB =    *o->numOfSamplesByChan_USB();
    SgVector&                   numOfSamplesByChan_LSB =    *o->numOfSamplesByChan_LSB();
    SgVector&                   refFreqByChan =             *o->refFreqByChan();
    SgVector&                   fringeAmplitudeByChan =     *o->fringeAmplitudeByChan();
    SgMatrix&                   phaseCalData_1 =            *o->phaseCalData_1ByChan();
    SgMatrix&                   phaseCalData_2 =            *o->phaseCalData_2ByChan();
    double                      effFreq4GR=0.0, effFreq4PH=0.0, effFreq4RT=0.0;
    //
    for (int i=0; i<numOfChannels; i++)
    {
      if (dNumOfAccPeriods_)
      {
        numOfAccPeriodsByChan_LSB.setElement(i, image->getI2(dNumOfAccPeriods_, 0, i, 0, obsIdx));
        numOfAccPeriodsByChan_USB.setElement(i, image->getI2(dNumOfAccPeriods_, 1, i, 0, obsIdx));
      }
      if (hasNumOfSamples_)
      {
        numOfSamplesByChan_LSB.setElement(i, image->getR8(dNumOfSamples_, 0, i, 0, obsIdx));
        numOfSamplesByChan_USB.setElement(i, image->getR8(dNumOfSamples_, 1, i, 0, obsIdx));
      };
      if (dRefFreqByChannel_)
        refFreqByChan.setElement(i, image->getR8(dRefFreqByChannel_, i, 0, 0, obsIdx));  // MHz
      if (dAmpByFrq_)
        fringeAmplitudeByChan.setElement(i, image->getR8(dAmpByFrq_, 0, i, 0, obsIdx));
      if (dPhaseCalOffset_)
      {
        phaseCalData_1.setElement (SgVlbiObservable::PCCI_OFFSET,i,
                                        image->getI2(dPhaseCalOffset_, 0, i, 0, obsIdx)*DEG2RAD/100.0);
        phaseCalData_2.setElement (SgVlbiObservable::PCCI_OFFSET,i,
                                        image->getI2(dPhaseCalOffset_, 1, i, 0, obsIdx)*DEG2RAD/100.0);
      };
      if (hasCalByFrq_)
      {
        phaseCalData_1.setElement (SgVlbiObservable::PCCI_AMPLITUDE,i,
                                        image->getI2(dCalByFrq_, 0, 0, i, obsIdx));
        phaseCalData_2.setElement (SgVlbiObservable::PCCI_AMPLITUDE,i,
                                        image->getI2(dCalByFrq_, 0, 1, i, obsIdx));
        phaseCalData_1.setElement (SgVlbiObservable::PCCI_PHASE,i,
                                        image->getI2(dCalByFrq_, 1, 0, i, obsIdx)*DEG2RAD/100.0);
        phaseCalData_2.setElement (SgVlbiObservable::PCCI_PHASE,i,
                                        image->getI2(dCalByFrq_, 1, 1, i, obsIdx)*DEG2RAD/100.0);
        phaseCalData_1.setElement (SgVlbiObservable::PCCI_FREQUENCY,i,
                                        image->getI2(dCalByFrq_, 2, 0, i, obsIdx));
        phaseCalData_2.setElement (SgVlbiObservable::PCCI_FREQUENCY,i,
                                        image->getI2(dCalByFrq_, 2, 1, i, obsIdx));
      };
    };
    if (hasCalByFrq_)
      o->calcPhaseCalDelay();
/*
    if (dSampleRate_)
      sampleRate = image->getR8(dSampleRate_, 0, 0, 0, obsIdx);
    else if (dRecSetup_)
      sampleRate = image->getI2(dRecSetup_, 0, 0, 0, obsIdx)*1000.0;
*/
    if (maxNumOfChannels && numOfChannels)
      evaluateEffectiveFreqs(numOfAccPeriodsByChan_USB, numOfAccPeriodsByChan_LSB, refFreqByChan,
        fringeAmplitudeByChan, numOfSamplesByChan_USB, numOfSamplesByChan_LSB,
        *o->channelBandwidth(), refFreq, numOfChannels,
//        effFreq4GR, effFreq4PH, effFreq4RT, band->getCorrelatorType(), o->strId());
        effFreq4GR, effFreq4PH, effFreq4RT, band->getCorrelatorType(), o);
    else
      effFreq4GR = effFreq4PH = effFreq4RT = refFreq;
    o->grDelay().setEffFreq(effFreq4GR);
    o->phDelay().setEffFreq(effFreq4PH);
    o->phDRate().setEffFreq(effFreq4RT);
    // o->releaseChannelsSetupStorages();
    // end of channel dependent data
    //
    //
    // for ionospheric corrections:
    if (have2getAll && hasEffFreqsSecBand_)
    {
      double                    d=image->getR8(dEffFreqsSecBand_,  0,0,0, obsIdx);
      o->grDelay().setQ2( d*d/(effFreq4GR*effFreq4GR - d*d) );
      // evaluate the ionospheric correction for the aux band if the value of the correction for
      // the prime band and the effective frequencies are known:
      if (hasIonCorr_ && hasIonRms_ && obs->passiveObses().size()>0)
        obs->passiveObses().at(0)->grDelay().setIonoValue(
          o->grDelay().getIonoValue()*effFreq4GR*effFreq4GR/d/d);
    };
    //
    // per band statistics:
    band->incNumTotal(DT_DELAY);
    band->incNumTotal(DT_RATE);
    bandStation1Info->incNumTotal(DT_DELAY);
    bandStation1Info->incNumTotal(DT_RATE);
    bandStation2Info->incNumTotal(DT_DELAY);
    bandStation2Info->incNumTotal(DT_RATE);
    bandSourceInfo->incNumTotal(DT_DELAY);
    bandSourceInfo->incNumTotal(DT_RATE);
    bandBaselineInfo->incNumTotal(DT_DELAY);
    bandBaselineInfo->incNumTotal(DT_RATE);
    //
    // Aux observations:
    if (have2getAll)
    {
      // =========================   auxiliary observation ==========================
      //
      // auxiliary observations (meteo, cables, etc.):
      // should be: baseline- and band- independent; could be missed in the file
      //
      if (hasCableData_)
      {
        cableCorrection1 = image->getR8(dCableCalibration_, 0, 0, 0, obsIdx);
        cableCorrection2 = image->getR8(dCableCalibration_, 1, 0, 0, obsIdx);
      };
      if (hasMeteoTemperatureData_)
      {
        meteo1.setTemperature(image->getR8(dMeteoTemperature_, 0, 0, 0, obsIdx));
        meteo2.setTemperature(image->getR8(dMeteoTemperature_, 1, 0, 0, obsIdx));
      };
      if (hasMeteoPressureData_)
      {
        meteo1.setPressure(image->getR8(dMeteoPressure_, 0, 0, 0, obsIdx));
        meteo2.setPressure(image->getR8(dMeteoPressure_, 1, 0, 0, obsIdx));
      };
      if (hasMeteoRelHumidityData_)
      {
        meteo1.setRelativeHumidity(image->getR8(dMeteoHumidity_, 0, 0, 0, obsIdx));
        meteo2.setRelativeHumidity(image->getR8(dMeteoHumidity_, 1, 0, 0, obsIdx));
      };
      if (hasAzimuthData_)
      {
        azimuthAngle1     = image->getR8(dAzimuths_, 0, 0, 0, obsIdx);
        azimuthAngle2     = image->getR8(dAzimuths_, 1, 0, 0, obsIdx);
        azimuthAngleRate1 = image->getR8(dAzimuths_, 0, 1, 0, obsIdx);
        azimuthAngleRate2 = image->getR8(dAzimuths_, 1, 1, 0, obsIdx);
      };
      if (hasElevationData_)
      {
        elevationAngle1     = image->getR8(dElevations_, 0, 0, 0, obsIdx);
        elevationAngle2     = image->getR8(dElevations_, 1, 0, 0, obsIdx);
        elevationAngleRate1 = image->getR8(dElevations_, 0, 1, 0, obsIdx);
        elevationAngleRate2 = image->getR8(dElevations_, 1, 1, 0, obsIdx);
      };
      if (hasParAngle_)
      {
        parallacticAngle1 = image->getR8(dParAngle_, 0, 0, 0, obsIdx)*DEG2RAD;
        parallacticAngle2 = image->getR8(dParAngle_, 1, 0, 0, obsIdx)*DEG2RAD;
      };
      partAxisOffsetDel1 = partAxisOffsetDel2 = 0.0;
      partAxisOffsetRat1 = partAxisOffsetRat2 = 0.0;
      if (hasAxsOfsPart_)
      {
        partAxisOffsetDel1 =-image->getR8(dAxsOfsPart_, 0, 0, 0, obsIdx);
        partAxisOffsetDel2 = image->getR8(dAxsOfsPart_, 1, 0, 0, obsIdx);
        partAxisOffsetRat1 =-image->getR8(dAxsOfsPart_, 0, 1, 0, obsIdx);
        partAxisOffsetRat2 = image->getR8(dAxsOfsPart_, 1, 1, 0, obsIdx);
      };
      if (hasNdryPart_)
      {
        calcNdryPartDel1 =-image->getR8(dNdryPart_, 0, 0, 0, obsIdx);
        calcNdryPartDel2 = image->getR8(dNdryPart_, 1, 0, 0, obsIdx);
        calcNdryPartRat1 =-image->getR8(dNdryPart_, 0, 1, 0, obsIdx);
        calcNdryPartRat2 = image->getR8(dNdryPart_, 1, 1, 0, obsIdx);
      };
      if (hasNwetPart_)
      {
        calcNwetPartDel1 =-image->getR8(dNwetPart_, 0, 0, 0, obsIdx);
        calcNwetPartDel2 = image->getR8(dNwetPart_, 1, 0, 0, obsIdx);
        calcNwetPartRat1 =-image->getR8(dNwetPart_, 0, 1, 0, obsIdx);
        calcNwetPartRat2 = image->getR8(dNwetPart_, 1, 1, 0, obsIdx);
      };

      //
      // the first station:
      auxObsByScan = station1Info->auxObservationByScanId();
      if (!auxObsByScan->contains(scanId)) // new scan, insert data:
      {
        auxObs = new SgVlbiAuxObservation;
        auxObs->setMJD(epoch);
        auxObs->setCableCalibration( cableCorrection1);           // does not alter the sign
        auxObs->setCalcNdryCont4Delay( calcNdryContribDel1);      // the sign has been altered
        auxObs->setCalcNdryCont4Rate ( calcNdryContribRat1);      // the sign has been altered
        auxObs->setCalcNwetCont4Delay( calcNwetContribDel1);      // the sign has been altered
        auxObs->setCalcNwetCont4Rate ( calcNwetContribRat1);      // the sign has been altered
        auxObs->setCalcUnPhaseCal( calcUnPhaseCal1);              // the sign has been altered
        auxObs->setCalcAxisOffset4Delay(calcAxisOffsetDel1);      // ?? the sign has been changed for Stn#2?
        auxObs->setCalcAxisOffset4Rate (calcAxisOffsetRat1);      // ?? the sign has been changed for Stn#2?
        auxObs->setCalcOLoadHorz4Delay(calcOcnLdDelH1);           // the sign has been altered
        auxObs->setCalcOLoadVert4Delay(calcOcnLdDelV1);           // the sign has been altered
        auxObs->setDdel_dAxsOfs (partAxisOffsetDel1);             // ?? the sign has been changed for Stn#2?
        auxObs->setDrat_dAxsOfs (partAxisOffsetRat1);             // ?? the sign has been changed for Stn#2?
        auxObs->setDdel_dTzdGrdN(calcNgradNPartDel1);             // not flipped???
        auxObs->setDdel_dTzdGrdE(calcNgradEPartDel1);             // not flipped???
        auxObs->setDrat_dTzdGrdN(calcNgradNPartRat1);             // not flipped???
        auxObs->setDrat_dTzdGrdE(calcNgradEPartRat1);             // not flipped???
        auxObs->setDdel_dTzdDry(calcNdryPartDel1);                // the sign has been altered
        auxObs->setDrat_dTzdDry(calcNdryPartRat1);                // the sign has been altered
        auxObs->setDdel_dTzdWet(calcNwetPartDel1);                // the sign has been altered
        auxObs->setDrat_dTzdWet(calcNwetPartRat1);                // the sign has been altered

        if (!hasCableData_)
          auxObs->addAttr(SgVlbiAuxObservation::Attr_CABLE_CAL_BAD);
        if (hasMeteoTemperatureData_ || hasMeteoPressureData_ || hasMeteoRelHumidityData_)
        {
          auxObs->setMeteoData(meteo1);
          auxObs->meteoData().delAttr(SgMeteoData::Attr_ARTIFICIAL_DATA);
        };
        if (hasAzimuthData_)
        {
          auxObs->setAzimuthAngle(azimuthAngle1);
          auxObs->setAzimuthAngleRate(azimuthAngleRate1);
        };
        if (hasElevationData_)
        {
          auxObs->setElevationAngle(elevationAngle1);
          auxObs->setElevationAngleRate(elevationAngleRate1);
        };
        if (hasParAngle_)
          auxObs->setParallacticAngle(parallacticAngle1);
        // add the observation to the list:
        auxObsByScan->insert(scanId, auxObs);
      }
      else // check it, data should be identical:
      {
        auxObsE = auxObsByScan->value(scanId);
        //
        // checking:
        // let a user know about unconsistency in the database:
        if (!auxObsE->isAttr(SgVlbiAuxObservation::Attr_CABLE_CAL_BAD) &&
          hasCableData_ && auxObsE->getCableCalibration()!=cableCorrection1)
          logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
            "::getDataFromDbhImage(): got diff value for cable cal; epoch " +
            epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) + 
            QString().sprintf(" (was: %.6g, became: %.6g, diff: %.6g); idx=%d", 
            auxObsE->getCableCalibration(), cableCorrection1, 
            auxObsE->getCableCalibration()-cableCorrection1, obsIdx) + 
            ", station1 " + station1Name + " from " + station1Info->getKey() + ":" +
            station2Info->getKey() + " @ " + sourceInfo->getKey());
        // check meteoparameters:
        if (!auxObsE->meteoData().isAttr(SgMeteoData::Attr_ARTIFICIAL_DATA))
        {
          // check temperature:
          if (hasMeteoTemperatureData_ &&
            fabs(auxObsE->getMeteoData().getTemperature()-meteo1.getTemperature())>1.0e-12)
              logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
                "::getDataFromDbhImage(): got diff value for meteo.T; epoch " +
                epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) + 
                QString().sprintf(" (was: %.6g, became: %.6g, diff: %.6g); idx=%d", 
                auxObsE->getMeteoData().getTemperature(), meteo1.getTemperature(), 
                auxObsE->getMeteoData().getTemperature()-meteo1.getTemperature(), obsIdx) + 
                ", station1: " + station1Name + " from " + station1Info->getKey() + ":" +
                station2Info->getKey() + " @ " + sourceInfo->getKey());
          // check pressure:        
          if (hasMeteoPressureData_ && 
            fabs(auxObsE->getMeteoData().getPressure()-meteo1.getPressure())>1.0e-12)
              logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
                "::getDataFromDbhImage(): got diff value for meteo.P; epoch " +
                epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) + 
                QString().sprintf(" (was: %.6g, became: %.6g, diff: %.6g); idx=%d", 
                auxObsE->getMeteoData().getPressure(), meteo1.getPressure(), 
                auxObsE->getMeteoData().getPressure()-meteo1.getPressure(), obsIdx) + 
                ", station1 " + station1Name + " from " + station1Info->getKey() + ":" +
                station2Info->getKey() + " @ " + sourceInfo->getKey());
          // check relative humidity:
          if (hasMeteoRelHumidityData_ && 
            fabs(auxObsE->getMeteoData().getRelativeHumidity()-meteo1.getRelativeHumidity())>1.0e-12)
              logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
                "::getDataFromDbhImage(): got diff value for meteo.R; epoch " +
                epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) + 
                QString().sprintf(" (was: %.6g, became: %.6g, diff: %.6g); idx=%d", 
                auxObsE->getMeteoData().getRelativeHumidity(), meteo1.getRelativeHumidity(), 
                auxObsE->getMeteoData().getRelativeHumidity()-meteo1.getRelativeHumidity(), obsIdx) + 
                ", station1 " + station1Name + " from " + station1Info->getKey() + ":" +
                station2Info->getKey() + " @ " + sourceInfo->getKey());
        };
        // check azimuth:
        if (hasAzimuthData_ && fabs(auxObsE->getAzimuthAngle()-azimuthAngle1)>1.0e-12)
          logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
            "::getDataFromDbhImage(): got diff value for azimuth; epoch " +
            epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) + 
            QString().sprintf(" (was: %.6g, became: %.6g, diff: %.6g mas); idx=%d", 
            auxObsE->getAzimuthAngle(), azimuthAngle1, 
            (auxObsE->getAzimuthAngle()-azimuthAngle1)*RAD2MAS, obsIdx) + 
            ", station1 " + station1Name + " from " + station1Info->getKey() + ":" +
            station2Info->getKey() + " @ " + sourceInfo->getKey());
        // check elevation:
        if (hasElevationData_ && fabs(auxObsE->getElevationAngle()-elevationAngle1)>1.0e-12)
          logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
            "::getDataFromDbhImage(): got diff value for elevation; epoch " +
            epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) + 
            QString().sprintf(" (was: %.6g, became: %.6g, diff: %.6g mas); idx=%d", 
            auxObsE->getElevationAngle(), elevationAngle1, 
            (auxObsE->getElevationAngle()-elevationAngle1)*RAD2MAS, obsIdx) + 
            ", station1 " + station1Name + " from " + station1Info->getKey() + ":" +
            station2Info->getKey() + " @ " + sourceInfo->getKey());
        if (hasParAngle_ && fabs(auxObsE->getParallacticAngle()-parallacticAngle1)>1.0e-12)
          logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
            "::getDataFromDbhImage(): got diff value for parallactic angle; epoch " +
            epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) + 
            QString().sprintf(" (was: %.6g, became: %.6g, diff: %.6g mas); idx=%d", 
            auxObsE->getParallacticAngle(), parallacticAngle1, 
            (auxObsE->getParallacticAngle()-parallacticAngle1)*RAD2MAS, obsIdx) + 
            ", station1 " + station1Name + " from " + station1Info->getKey() + ":" +
            station2Info->getKey() + " @ " + sourceInfo->getKey());
        // check oload data:
        if (hasOloadHorzContrib_ && fabs(auxObsE->getCalcOLoadHorz4Delay()-calcOcnLdDelH1)>1.0e-15)
          logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
            "::getDataFromDbhImage(): got diff value for horizontal ocean loading; epoch " +
            epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) + 
            QString().sprintf(" (was: %.6g, became: %.6g, diff: %.6g ps); idx=%d", 
            auxObsE->getCalcOLoadHorz4Delay(), calcOcnLdDelH1, 
            (auxObsE->getCalcOLoadHorz4Delay()-calcOcnLdDelH1)*1.0e12, obsIdx) + 
            ", station1 " + station1Name + " from " + station1Info->getKey() + ":" +
            station2Info->getKey() + " @ " + sourceInfo->getKey());
        if (hasOloadVertContrib_ && fabs(auxObsE->getCalcOLoadVert4Delay()-calcOcnLdDelV1)>1.0e-15)
          logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
            "::getDataFromDbhImage(): got diff value for vertical ocean loading; epoch " +
            epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) + 
            QString().sprintf(" (was: %.6g, became: %.6g, diff: %.6g ps); idx=%d", 
            auxObsE->getCalcOLoadVert4Delay(), calcOcnLdDelV1, 
            (auxObsE->getCalcOLoadVert4Delay()-calcOcnLdDelV1)*1.0e12, obsIdx) + 
            ", station1 " + station1Name + " from " + station1Info->getKey() + ":" +
            station2Info->getKey() + " @ " + sourceInfo->getKey());
        //
        // updating:
        // if previous file did not contain cable cals (e.g., S-band), and now we got them:
        if (auxObsE->isAttr(SgVlbiAuxObservation::Attr_CABLE_CAL_BAD) && hasCableData_)
        {
          auxObsE->setCableCalibration(cableCorrection1);
          auxObsE->delAttr(SgVlbiAuxObservation::Attr_CABLE_CAL_BAD);
        };
        // also, set up meteo, if it is available:
        if (auxObsE->meteoData().isAttr(SgMeteoData::Attr_ARTIFICIAL_DATA) && 
          (hasMeteoTemperatureData_ || hasMeteoPressureData_ || hasMeteoRelHumidityData_) )
        {
          auxObsE->setMeteoData(meteo1);
          auxObsE->meteoData().delAttr(SgMeteoData::Attr_ARTIFICIAL_DATA);
        };
        // set up azimuth & elevation angles, if they were not set yet:
        if (hasAzimuthData_)
        {
          auxObsE->setAzimuthAngle(azimuthAngle1);
          auxObsE->setAzimuthAngleRate(azimuthAngleRate1);
        };
        if (hasElevationData_)
        {
          auxObsE->setElevationAngle(elevationAngle1);
          auxObsE->setElevationAngleRate(elevationAngleRate1);
        };
        if (hasParAngle_)
          auxObsE->setParallacticAngle(parallacticAngle1);
        //
        /*
        if (hasNgradParts_)
        {
          auxObsE->setDdel_dTzdGrdN(calcNgradNPart1);
          auxObsE->setDdel_dTzdGrdE(calcNgradEPart1);
        };
        */
      };
      //
      // the second station:
      auxObsByScan = station2Info->auxObservationByScanId();
      if (!auxObsByScan->contains(scanId)) // new scan, insert data:
      {
        auxObs = new SgVlbiAuxObservation;
        auxObs->setMJD(epoch);
        auxObs->setCableCalibration(cableCorrection2);
        auxObs->setCalcNdryCont4Delay(calcNdryContribDel2);
        auxObs->setCalcNdryCont4Rate (calcNdryContribRat2);
        auxObs->setCalcNwetCont4Delay(calcNwetContribDel2);
        auxObs->setCalcNwetCont4Rate (calcNwetContribRat2);
        auxObs->setCalcUnPhaseCal(calcUnPhaseCal2);
        auxObs->setCalcAxisOffset4Delay(calcAxisOffsetDel2);
        auxObs->setCalcAxisOffset4Rate (calcAxisOffsetRat2);
        auxObs->setCalcOLoadHorz4Delay(calcOcnLdDelH2);
        auxObs->setCalcOLoadVert4Delay(calcOcnLdDelV2);
        auxObs->setDdel_dAxsOfs ( partAxisOffsetDel2);                          // ?? sign ?
        auxObs->setDrat_dAxsOfs ( partAxisOffsetRat2);                          // ?? sign ?
        auxObs->setDdel_dTzdGrdN(calcNgradNPartDel2);
        auxObs->setDdel_dTzdGrdE(calcNgradEPartDel2);
        auxObs->setDrat_dTzdGrdN(calcNgradNPartRat2);
        auxObs->setDrat_dTzdGrdE(calcNgradEPartRat2);
        auxObs->setDdel_dTzdDry(calcNdryPartDel2);                // the sign has been altered
        auxObs->setDrat_dTzdDry(calcNdryPartRat2);                // the sign has been altered
        auxObs->setDdel_dTzdWet(calcNwetPartDel2);                // the sign has been altered
        auxObs->setDrat_dTzdWet(calcNwetPartRat2);                // the sign has been altered
        if (!hasCableData_)
          auxObs->addAttr(SgVlbiAuxObservation::Attr_CABLE_CAL_BAD);
        if (hasMeteoTemperatureData_ || hasMeteoPressureData_ || hasMeteoRelHumidityData_)
        {
          auxObs->setMeteoData(meteo2);
          auxObs->meteoData().delAttr(SgMeteoData::Attr_ARTIFICIAL_DATA);
        };
        if (hasAzimuthData_)
        {
          auxObs->setAzimuthAngle(azimuthAngle2);
          auxObs->setAzimuthAngleRate(azimuthAngleRate2);
        };
        if (hasElevationData_)
        {
          auxObs->setElevationAngle(elevationAngle2);
          auxObs->setElevationAngleRate(elevationAngleRate2);
        };
        if (hasParAngle_)
          auxObs->setParallacticAngle(parallacticAngle2);
        // add the observation to the list:
        auxObsByScan->insert(scanId, auxObs);
      }
      else // check it, data should be identical:
      {
        auxObsE = auxObsByScan->value(scanId);
        //
        // checking:
        // let a user know about unconsistency of the databases:
        if (!auxObsE->isAttr(SgVlbiAuxObservation::Attr_CABLE_CAL_BAD) && 
          hasCableData_ && auxObsE->getCableCalibration()!=cableCorrection2)
          logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
            "::getDataFromDbhImage(): got diff value for cable cal; epoch " +
            epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) + 
            QString().sprintf(" (was: %.6g, became: %.6g, diff: %.6g); idx=%d", 
            auxObsE->getCableCalibration(), cableCorrection2, 
            auxObsE->getCableCalibration()-cableCorrection2, obsIdx) + 
            ", station2 " + station2Name + " from " + station1Info->getKey() + ":" +
            station2Info->getKey() + " @ " + sourceInfo->getKey());
        // check meteoparameters:
        if (!auxObsE->meteoData().isAttr(SgMeteoData::Attr_ARTIFICIAL_DATA))
        {
          // check temperature:
          if (hasMeteoTemperatureData_ && 
            fabs(auxObsE->getMeteoData().getTemperature()-meteo2.getTemperature())>1.0e-12)
              logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
                "::getDataFromDbhImage(): got diff value for meteo.T; epoch " +
                epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) + 
                QString().sprintf(" (was: %.6g, became: %.6g, diff: %.6g); idx=%d", 
                auxObsE->getMeteoData().getTemperature(), meteo2.getTemperature(), 
                auxObsE->getMeteoData().getTemperature()-meteo2.getTemperature(), obsIdx) + 
                ", station2 " + station2Name + " from " + station1Info->getKey() + ":" +
                station2Info->getKey() + " @ " + sourceInfo->getKey());
          // check pressure:
          if (hasMeteoPressureData_ && 
            fabs(auxObsE->getMeteoData().getPressure()-meteo2.getPressure())>1.0e-12)
              logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
                "::getDataFromDbhImage(): got diff value for meteo.P; epoch " +
                epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) + 
                QString().sprintf(" (was: %.6g, became: %.6g, diff: %.6g); idx=%d", 
                auxObsE->getMeteoData().getPressure(), meteo2.getPressure(), 
                auxObsE->getMeteoData().getPressure()-meteo2.getPressure(), obsIdx) + 
                ", station2 " + station2Name + " from " + station1Info->getKey() + ":" +
                station2Info->getKey() + " @ " + sourceInfo->getKey());
          // check relative humidity:
          if (hasMeteoRelHumidityData_ && 
            fabs(auxObsE->getMeteoData().getRelativeHumidity()-meteo2.getRelativeHumidity())>1.0e-12)
              logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
                "::getDataFromDbhImage(): got diff value for meteo.R; epoch " +
                epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) + 
                QString().sprintf(" (was: %.6g, became: %.6g, diff: %.6g); idx=%d", 
                auxObsE->getMeteoData().getRelativeHumidity(), meteo2.getRelativeHumidity(), 
                auxObsE->getMeteoData().getRelativeHumidity()-meteo2.getRelativeHumidity(), obsIdx) + 
                ", station2 " + station2Name + " from " + station1Info->getKey() + ":" +
                station2Info->getKey() + " @ " + sourceInfo->getKey());
        };
        // check azimuth:
        if (hasAzimuthData_ && fabs(auxObsE->getAzimuthAngle()-azimuthAngle2)>1.0e-12)
          logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
            "::getDataFromDbhImage(): got diff value for azimuth; epoch " +
            epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) + 
            QString().sprintf(" (was: %.6g, became: %.6g, diff: %.6g mas); idx=%d", 
            auxObsE->getAzimuthAngle(), azimuthAngle2, 
            (auxObsE->getAzimuthAngle()-azimuthAngle2)*RAD2MAS, obsIdx) + 
            ", station2 " + station2Name + " from " + station1Info->getKey() + ":" +
            station2Info->getKey() + " @ " + sourceInfo->getKey());
        // check elevation:
        if (hasElevationData_ && fabs(auxObsE->getElevationAngle()-elevationAngle2)>1.0e-12)
          logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
            "::getDataFromDbhImage(): got diff value for elevation; epoch " +
            epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) + 
            QString().sprintf(" (was: %.6g, became: %.6g, diff: %.6g mas); idx=%d", 
            auxObsE->getElevationAngle(), elevationAngle2, 
            (auxObsE->getElevationAngle()-elevationAngle2)*RAD2MAS, obsIdx) + 
            ", station2 " + station2Name + " from " + station1Info->getKey() + ":" +
            station2Info->getKey() + " @ " + sourceInfo->getKey());
        // check parallactic angle:
        if (hasParAngle_ && fabs(auxObsE->getParallacticAngle()-parallacticAngle2)>1.0e-12)
          logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
            "::getDataFromDbhImage(): got diff value for parallactic angle; epoch " +
            epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) + 
            QString().sprintf(" (was: %.6g, became: %.6g, diff: %.6g mas); idx=%d", 
            auxObsE->getParallacticAngle(), parallacticAngle2, 
            (auxObsE->getParallacticAngle()-parallacticAngle2)*RAD2MAS, obsIdx) + 
            ", station2 " + station2Name + " from " + station1Info->getKey() + ":" +
            station2Info->getKey() + " @ " + sourceInfo->getKey());
        // check oload data:
        if (hasOloadHorzContrib_ && fabs(auxObsE->getCalcOLoadHorz4Delay()-calcOcnLdDelH2)>1.0e-15)
          logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
            "::getDataFromDbhImage(): got diff value for horizontal ocean loading; epoch " +
            epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) + 
            QString().sprintf(" (was: %.6g, became: %.6g, diff: %.6g ps); idx=%d", 
            auxObsE->getCalcOLoadHorz4Delay(), calcOcnLdDelH2, 
            (auxObsE->getCalcOLoadHorz4Delay()-calcOcnLdDelH2)*1.0e12, obsIdx) + 
            ", station2 " + station2Name + " from " + station1Info->getKey() + ":" +
            station2Info->getKey() + " @ " + sourceInfo->getKey());
        if (hasOloadVertContrib_ && fabs(auxObsE->getCalcOLoadVert4Delay()-calcOcnLdDelV2)>1.0e-15)
          logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
            "::getDataFromDbhImage(): got diff value for vertical ocean loading; epoch " +
            epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) + 
            QString().sprintf(" (was: %.6g, became: %.6g, diff: %.6g ps); idx=%d", 
            auxObsE->getCalcOLoadVert4Delay(), calcOcnLdDelV2, 
            (auxObsE->getCalcOLoadVert4Delay()-calcOcnLdDelV2)*1.0e12, obsIdx) + 
            ", station2 " + station2Name + " from " + station1Info->getKey() + ":" +
            station2Info->getKey() + " @ " + sourceInfo->getKey());
        //
        // updating:
        // if previous file did not contain cable cals (e.g., S-band), and now we got them:
        if (auxObsE->isAttr(SgVlbiAuxObservation::Attr_CABLE_CAL_BAD) && hasCableData_)
        {
          auxObsE->setCableCalibration(cableCorrection2);
          auxObsE->delAttr(SgVlbiAuxObservation::Attr_CABLE_CAL_BAD);
        };
        // also, set up meteo, if it is available:
        if (auxObsE->meteoData().isAttr(SgMeteoData::Attr_ARTIFICIAL_DATA) && 
          (hasMeteoTemperatureData_ || hasMeteoPressureData_ || hasMeteoRelHumidityData_) )
        {
          auxObsE->setMeteoData(meteo2);
          auxObsE->meteoData().delAttr(SgMeteoData::Attr_ARTIFICIAL_DATA);
        };
        // set up azimuth & elevation angles, if they were not setuped yet:
        //      if (auxObsE->getAzimuthAngle()<-9.0 && hasAzimuthData_)
        if (hasAzimuthData_)
        {
          auxObsE->setAzimuthAngle(azimuthAngle2);
          auxObsE->setAzimuthAngleRate(azimuthAngleRate2);
        };
        if (hasElevationData_)
        {
          auxObsE->setElevationAngle(elevationAngle2);
          auxObsE->setElevationAngleRate(elevationAngleRate2);
        };
        if (hasParAngle_)
          auxObsE->setParallacticAngle(parallacticAngle2);
        //
        /*
        if (hasNgradParts_)
        {
          auxObsE->setDdel_dTzdGrdN(calcNgradNPart2);
          auxObsE->setDdel_dTzdGrdE(calcNgradEPart2);
        };
        */
      };
      // cummulative statistics (do we need it?):
      station1Info->incNumTotal(DT_DELAY);
      station1Info->incNumTotal(DT_RATE);
      station2Info->incNumTotal(DT_DELAY);
      station2Info->incNumTotal(DT_RATE);
      sourceInfo->incNumTotal(DT_DELAY);
      sourceInfo->incNumTotal(DT_RATE);
      baselineInfo->incNumTotal(DT_DELAY);
      baselineInfo->incNumTotal(DT_RATE);
    }; // end of "have2getAll"
  };
  //
  //
  // the mk3 databases have order {LSB, USB}. 
  // here we specify in what order the data will be stored during export into vgosDb format:
  setSidebandOrder(SO_USB_LSB);
  //
  // here we will check DBH header for conistency with observations and
  // absorb some aux info (e.g., cable signs)
  //
  // first, check reported numbers:
  int                            dbhNumOfStations=0, dbhNumOfSources=0;
  // stations:
  if (hasNSites_)
  {
    dbhNumOfStations = image->getI2(dNSites_, 0,0,0);
    if (dbhNumOfStations!=stationsByName_.size())
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::getDataFromDbhImage(): the actual number of stations " +
        QString().sprintf("(%d) is not equal to the reported in DBH file (%d)",
          stationsByName_.size(), dbhNumOfStations));
  }
  else
    logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
      "::getDataFromDbhImage(): cannot find lcode '" + rNSites_.lCode_ + "' in the image");
  // sources:
  if (hasNStars_)
  {
    dbhNumOfSources = image->getI2(dNStars_, 0,0,0);
    if (dbhNumOfSources!=sourcesByName_.size())
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::getDataFromDbhImage(): the actual number of radio sources " +
        QString().sprintf("(%d) is not equal to the reported in DBH file (%d)",
          sourcesByName_.size(), dbhNumOfSources));
  }
  else
    logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
      "::getDataFromDbhImage(): cannot find lcode '" + rNStars_.lCode_ + "' in the image");
  // observations:
  /*
  if (hasNObs_ && have2getAll)
  {
    dbhNumOfObservations = image->getI2(dNObs_, 0,0,0);
    if (dbhNumOfObservations!=band->observables().size())
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        ": getDataFromDbhImage(): the actual number of observations " +
        QString().sprintf("(%d) is not equal to the reported in DBH file (%d)",
          band->observables().size(), dbhNumOfObservations));
  }
  else
    logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
      ": getDataFromDbhImage(): cannot find lcode '" + rNObs_.lCode_ + "' in the image");
  */
  //
  //
  //
  if (have2getAll)
  {
    if (hasCalcVersionValue_)
      calcInfo_.setDversion(image->getR8(dCalcVersionValue_, 0,0,0));
    // Preparation:
    //
    // make a setup of local clocks and zenith delays:
    for (StationsByName_it it = stationsByName_.begin(); it!=stationsByName_.end(); ++it)
    {
      station1Info = it.value();
      station1Info->setPcClocks(parametersDescriptor_->getClock0());
      station1Info->setPcZenith(parametersDescriptor_->getZenith());
    };
    //
    // PWL contraints setup:
    //
    bool                        hasUniqStnClcConfigSn(false), hasUniqStnZenConfigSn(false);
    bool                        hasUniqStnClcConfigSe(false), hasUniqStnZenConfigSe(false);
    double                      scale4ClcApriori, scale4ZenApriori;
    double                      oldClcPwlApriori, oldZenPwlApriori;
    double                      newClcPwlApriori, newZenPwlApriori;
    scale4ClcApriori = 36.0*24.0;
    scale4ZenApriori = 1.0e-12*24.0*vLight*100.0;
    oldClcPwlApriori = parametersDescriptor_->getClock0().getPwlAPriori();
    oldZenPwlApriori = parametersDescriptor_->getZenith().getPwlAPriori();
    //
    if (hasClockConstr_ && hasCcSites_ && dClockConstr_->dim1()==dCcSites_->dim2())
      hasUniqStnClcConfigSe = true;
    else if (hasClockConstr_ && hasSiteNames_ && dClockConstr_->dim1()==dSiteNames_->dim2())
      hasUniqStnClcConfigSn = true;
    else if (hasClockConstr_ && dClockConstr_->dim1()==1 &&
      fabs(oldClcPwlApriori/scale4ClcApriori - image->getR8(dClockConstr_, i,0,0)) > 1.0e-3)
    {
      newClcPwlApriori = image->getR8(dClockConstr_, 0,0,0)*scale4ClcApriori;
      SgParameterCfg            pc=parametersDescriptor_->getClock0();
      pc.setPwlAPriori(newClcPwlApriori);
      parametersDescriptor_->setClock0(pc);
      logger->write(SgLogger::INF, SgLogger::IO_DBH, className() +
        "::getDataFromDbhImage(): changed clocks PWL constraints from " + 
        QString("").setNum(oldClcPwlApriori) + " to " + QString("").setNum(newClcPwlApriori));
    };
    //
    if (hasAtmConstr_ && hasAcSites_ && dAtmConstr_->dim1()==dAcSites_->dim2())
      hasUniqStnZenConfigSe = true;
    else if (hasAtmConstr_ && hasSiteNames_ && dAtmConstr_->dim1()==dSiteNames_->dim2())
      hasUniqStnZenConfigSn = true;
    else if (hasAtmConstr_ && dAtmConstr_->dim1()==1 &&
      fabs(oldZenPwlApriori/scale4ZenApriori - image->getR8(dAtmConstr_, i,0,0)) > 1.0e-3)
    {
      newZenPwlApriori = image->getR8(dAtmConstr_, 0,0,0)*scale4ZenApriori;
      SgParameterCfg            pc=parametersDescriptor_->getZenith();
      pc.setPwlAPriori(newZenPwlApriori);
      parametersDescriptor_->setZenith(pc);
      logger->write(SgLogger::INF, SgLogger::IO_DBH, className() +
        "::getDataFromDbhImage(): changed zenith PWL constraints from " + 
        QString("").setNum(oldZenPwlApriori) + " to " + QString("").setNum(newZenPwlApriori));
    };
    //
    // intervals:
    // clocks:
    if (hasClockIntrv_)
    {
      if (dClockIntrv_->dim1()==1 && dClockIntrv_->dim2()==1 && dClockIntrv_->dim3()==1)
      {
        double                  ci=image->getR8(dClockIntrv_, 0,0,0)/24.0;
        SgParameterCfg          pc=parametersDescriptor_->getClock0();
        double                  oldCi=pc.getPwlStep();
        if (fabs(oldCi - ci) > 1.0e-3)
        {
          pc.setPwlStep(ci);
          parametersDescriptor_->setClock0(pc);
          logger->write(SgLogger::INF, SgLogger::IO_DBH, className() +
            "::getDataFromDbhImage(): changed clocks PWL interval from " + 
            QString("").setNum(oldCi*24.0) + " to " + QString("").setNum(ci*24.0) + " hr(s)");
        };
      }
      else
        logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
          "::getDataFromDbhImage(): got a multidimensional CLK_INTV: " + 
          QString("").sprintf("(%d:%d:%d)", 
            dClockIntrv_->dim1(), dClockIntrv_->dim2(), dClockIntrv_->dim3()));
    };
    //
    // zenith delays:
    if (hasAtmIntrv_)
    {
      if (dAtmIntrv_->dim1()==1 && dAtmIntrv_->dim2()==1 && dAtmIntrv_->dim3()==1)
      {
        double                  ci=image->getR8(dAtmIntrv_, 0,0,0)/24.0;
        SgParameterCfg          pc=parametersDescriptor_->getZenith();
        double                  oldCi=pc.getPwlStep();
        if (fabs(oldCi - ci) > 1.0e-3)
        {
          pc.setPwlStep(ci);
          parametersDescriptor_->setZenith(pc);
          logger->write(SgLogger::INF, SgLogger::IO_DBH, className() +
            "::getDataFromDbhImage(): changed zenith delay PWL interval from " + 
            QString("").setNum(oldCi*24.0) + " to " + QString("").setNum(ci*24.0) + " hr(s)");
        };
      }
      else
        logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
          "::getDataFromDbhImage(): got a multidimensional ATM_INTV: " + 
          QString("").sprintf("(%d:%d:%d)", dAtmIntrv_->dim1(), dAtmIntrv_->dim2(), dAtmIntrv_->dim3()));
    };
    //
    //
    //
    // collect info about sources:
    if (hasStarNames_)
    {
      if (dStarNames_->dim2()!=sourcesByName_.size())
        logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
          "::getDataFromDbhImage(): the dimension of the '" + rStarNames_.lCode_ + "' lCode " +
          QString().sprintf("(%d) is not equal to the actual number of sources (%d)",
            dStarNames_->dim2(), sourcesByName_.size()));
      for (int i=0; i<dStarNames_->dim2(); i++)
      {
        sourceName = image->getStr(dStarNames_, i,0);
        if (sourcesByName_.contains(sourceName))
        {
          sourceInfo = sourcesByName_.value(sourceName);
          bandSourceInfo = band->sourcesByName().value(sourceName);
          double                          ra(0.0), dn(0.0);
          // coordinates:
          if (hasStar2000_)
          {
            ra = image->getR8(dStar2000_, 0,i,0);
            dn = image->getR8(dStar2000_, 1,i,0);
            sourceInfo->setRA(ra);
            sourceInfo->setDN(dn);
            bandSourceInfo->setRA(ra);
            bandSourceInfo->setDN(dn);
          }
          else
            logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
              "::getDataFromDbhImage(): cannot find lcode '" + rStar2000_.lCode_ + "' in the image");
        }
        else // complain:
          logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
            "::getDataFromDbhImage(): cannot find source '" + sourceName + "' in the list of sources");
      };
    }
    else
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::getDataFromDbhImage(): cannot find lcode '" + rStarNames_.lCode_ + "' in the image");
    //
    //
    // collect info about stations:
    //
    if (hasSiteNames_)
    {
      if (dSiteNames_->dim2() != stationsByName_.size())
        logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
          "::getDataFromDbhImage(): the dimension of the '" + rSiteNames_.lCode_ + "' lCode " +
          QString().sprintf("(%d) is not equal to the actual number of stations (%d)",
          dSiteNames_->dim2(), stationsByName_.size()));
      // clear info's station list:
      calcInfo_.stations().clear();
      for (int i=0; i<dSiteNames_->dim2(); i++)
      {
        station1Name = image->getStr(dSiteNames_, i,0);
        if (stationsByName_.contains(station1Name))
        {
          station1Info = stationsByName_.value(station1Name);
          SgVlbiStationInfo::MountingType mnt = SgVlbiStationInfo::MT_UNKN;
          double                          axisOffset = 0.0;
          Sg3dVector                      r(v3Zero);
          // axis offset type:
          switch (hasAxisTypes_?image->getI2(dAxisTypes_, i,0,0):0)
          {
          case 1:
            mnt = SgVlbiStationInfo::MT_EQUA;
            break;
          case 2:
            mnt = SgVlbiStationInfo::MT_X_YN;
            break;
          case 3:
            mnt = SgVlbiStationInfo::MT_AZEL;
            break;
          case 4:
            mnt = SgVlbiStationInfo::MT_RICHMOND;
            break;
          case 5:
            mnt = SgVlbiStationInfo::MT_X_YE;
            break;
          default:
            logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
              "::getDataFromDbhImage(): got unknown value for the lCode '" + rAxisTypes_.lCode_ + "'");
            break;
          };
          station1Info->setMntType(mnt);
          logger->write(SgLogger::DBG, SgLogger::IO_DBH, className() +
            "::getDataFromDbhImage(): set up mounting type [" + sMntTypes[mnt] + "] for the antenna " + 
            station1Info->getKey());
          // axis offset:
          if (hasAxisOffsets_)
          {
            axisOffset = image->getR8(dAxisOffsets_, i,0,0);
            station1Info->setAxisOffset(axisOffset);
            logger->write(SgLogger::DBG, SgLogger::IO_DBH, className() +
              "::getDataFromDbhImage(): the value of axiss offset for the antenna " +
              station1Info->getKey() + " has been set up to " + QString("").setNum(axisOffset));
          }
          else
            logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
              "::getDataFromDbhImage(): cannot find lcode '" + rAxisOffsets_.lCode_ + "' in the image");
          // coordinates:
          if (hasSiteRecs_)
          {
            r(X_AXIS) = image->getR8(dSiteRecs_, 0,i,0);
            r(Y_AXIS) = image->getR8(dSiteRecs_, 1,i,0);
            r(Z_AXIS) = image->getR8(dSiteRecs_, 2,i,0);
            station1Info->setR(r);
            calcInfo_.stations().append(station1Info);
          }
          else
            logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
              "::getDataFromDbhImage(): cannot find lcode '" + rSiteRecs_.lCode_ + "' in the image");
          //
          // Pick up the ocean loading coefficients:
          if (hasOloadAmpVert_ && hasOloadPhsVert_ && hasOloadAmpHorz_ && hasOloadPhsHorz_)
          {
            for (int iWave=0; iWave<11; iWave++)
            {
              station1Info->setOLoadAmplitude(iWave, 0, image->getR8(dOloadAmpVert_, iWave,i,0));
              station1Info->setOLoadAmplitude(iWave, 1, image->getR8(dOloadAmpHorz_, iWave,i,0));
              station1Info->setOLoadAmplitude(iWave, 2, image->getR8(dOloadAmpHorz_, iWave,i,1));
              station1Info->setOLoadPhase(iWave, 0, image->getR8(dOloadPhsVert_, iWave,i,0));
              station1Info->setOLoadPhase(iWave, 1, image->getR8(dOloadPhsHorz_, iWave,i,0));
              station1Info->setOLoadPhase(iWave, 2, image->getR8(dOloadPhsHorz_, iWave,i,1));
            };
            logger->write(SgLogger::DBG, SgLogger::IO_DBH, className() +
              "::getDataFromDbhImage(): set up ocean loading coefficients for the station " +
              station1Info->getKey());
          };
          if (hasTectPlateNames_)
          {
            station1Info->setTectonicPlateName(image->getStr(dTectPlateNames_, i,0));
            logger->write(SgLogger::DBG, SgLogger::IO_DBH, className() +
              "::getDataFromDbhImage(): tectonic plate name was set to [" + 
              station1Info->getTectonicPlateName() + "] for the station " + station1Info->getKey());
          };
          //
          if (hasUniqStnClcConfigSn &&
            fabs(oldClcPwlApriori/scale4ClcApriori - image->getR8(dClockConstr_, i,0,0)) > 1.0e-3)
          {
            newClcPwlApriori = image->getR8(dClockConstr_, i,0,0)*scale4ClcApriori;
            station1Info->pcClocks()->setPwlAPriori(newClcPwlApriori);
            station1Info->addAttr(SgVlbiStationInfo::Attr_USE_LOCAL_CLOCKS);
            logger->write(SgLogger::INF, SgLogger::IO_DBH, className() +
              "::getDataFromDbhImage(): set a unique clock setup for station '" + station1Name + 
              "' and changed constraints from " + QString("").setNum(oldClcPwlApriori) + " to " + 
              QString("").setNum(newClcPwlApriori));
          };
          if (hasUniqStnZenConfigSn &&
            fabs(oldZenPwlApriori/scale4ZenApriori - image->getR8(dAtmConstr_, i,0,0)) > 1.0e-3)
          {
            newZenPwlApriori = image->getR8(dAtmConstr_, i,0,0)*scale4ZenApriori;
            station1Info->pcZenith()->setPwlAPriori(newZenPwlApriori);
            station1Info->addAttr(SgVlbiStationInfo::Attr_USE_LOCAL_ZENITH);
            logger->write(SgLogger::INF, SgLogger::IO_DBH, className() +
              "::getDataFromDbhImage(): set a unique zenith setup for station '" + station1Name + 
              "' and changed constraints from " + QString("").setNum(oldZenPwlApriori) + " to " + 
              QString("").setNum(newZenPwlApriori));
          };
        //
        }
        else // complain:
          logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
            "::getDataFromDbhImage(): cannot find station '" + station1Name + 
            "' in the list of stations");
      };
    }
    else
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::getDataFromDbhImage(): cannot find lcode '" + rSiteNames_.lCode_ + "' in the image");
    //
    // constraints, can be represented by theirs own namelists:
    if (hasUniqStnClcConfigSe)
      for (int i=0; i<dCcSites_->dim2(); i++)
      {
        station1Name = image->getStr(dCcSites_, i,0);
        if (stationsByName_.contains(station1Name))
        {
          station1Info = stationsByName_.value(station1Name);
          if (fabs(oldClcPwlApriori/scale4ClcApriori - image->getR8(dClockConstr_, i,0,0)) > 1.0e-3)
          {
            newClcPwlApriori = image->getR8(dClockConstr_, i,0,0)*scale4ClcApriori;
            station1Info->pcClocks()->setPwlAPriori(newClcPwlApriori);
            station1Info->addAttr(SgVlbiStationInfo::Attr_USE_LOCAL_CLOCKS);
            logger->write(SgLogger::INF, SgLogger::IO_DBH, className() +
              "::getDataFromDbhImage(): set a unique clock setup for station '" + station1Name + 
              "' and changed constraints from " + QString("").setNum(oldClcPwlApriori/scale4ClcApriori) +
              " to " + QString("").setNum(newClcPwlApriori/scale4ClcApriori) );
          };
        };
      };
    if (hasUniqStnZenConfigSe)
      for (int i=0; i<dAcSites_->dim2(); i++)
      {
        station1Name = image->getStr(dAcSites_, i,0);
        if (stationsByName_.contains(station1Name))
        {
          station1Info = stationsByName_.value(station1Name);
          if (fabs(oldZenPwlApriori/scale4ZenApriori - image->getR8(dAtmConstr_, i,0,0)) > 1.0e-3)
          {
            newZenPwlApriori = image->getR8(dAtmConstr_, i,0,0)*scale4ZenApriori;
            station1Info->pcZenith()->setPwlAPriori(newZenPwlApriori);
            station1Info->addAttr(SgVlbiStationInfo::Attr_USE_LOCAL_ZENITH);
            logger->write(SgLogger::INF, SgLogger::IO_DBH, className() +
              "::getDataFromDbhImage(): set a unique zenith setup for station '" + station1Name + 
              "' and changed constraints from " + QString("").setNum(oldZenPwlApriori/scale4ZenApriori) +
              " to " + QString("").setNum(newZenPwlApriori/scale4ZenApriori));
          };
        };
      };
    //
    //
    // baselines selection status:
    if (hasBaselinesSelStatus_ && hasSiteNames_)
    {
      int                         bitArray;
      SgVlbiBaselineInfo         *bi, *biR;
      for (int i=0; i<dSiteNames_->dim2(); i++)
      {
        station1Name = image->getStr(dSiteNames_, i,0);
        if (stationsByName_.contains(station1Name))
        {
          for (int j=0; j<dSiteNames_->dim2(); j++)
          {
            station2Name = image->getStr(dSiteNames_, j,0);
            if (stationsByName_.contains(station2Name))
            {
              bitArray = image->getI2(dBaselinesSelStatus_, dSiteNames_->dim2()*j + i,0,0);
              if (baselinesByName_.contains(station1Name + ":" + station2Name))
                bi = baselinesByName_.value(station1Name + ":" + station2Name);
              else
                bi = NULL;
              if (baselinesByName_.contains (station2Name + ":" + station1Name))
                biR = baselinesByName_.value(station2Name + ":" + station1Name);
              else
                biR = NULL;
              // check for deselect flag:
              if (!(bitArray & (1<<1)))
              {
                if (bi)
                  bi ->addAttr(SgVlbiBaselineInfo::Attr_NOT_VALID);
                if (biR)
                  biR->addAttr(SgVlbiBaselineInfo::Attr_NOT_VALID);
              };
              // check for baseline clocks flag:
              if ((bitArray & (1<<3)))
              {
                if (bi)
                  bi ->addAttr(SgVlbiBaselineInfo::Attr_ESTIMATE_CLOCKS);
                if (biR)
                  biR->addAttr(SgVlbiBaselineInfo::Attr_ESTIMATE_CLOCKS);
              };
            };
          };
        };
      };
    };
    //
    // reweightinig sigmas:
    if (hasError_Bl_ && hasError_K_)
    {
      SgVlbiBaselineInfo         *bi;
      for (int i=0; i<dError_Bl_->dim3(); i++)
      {
        station1Name = image->getStr(dError_Bl_, 0,i);
        station2Name = image->getStr(dError_Bl_, 1,i);
        if (baselinesByName_.contains(station1Name + ":" + station2Name))
        {
          bi = baselinesByName_.value(station1Name + ":" + station2Name);
          bi->setSigma2add(DT_DELAY, image->getR8(dError_K_,  0,i,0));
          bi->setSigma2add(DT_RATE,  image->getR8(dError_K_,  1,i,0));
        }
        else
          logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
            "::getDataFromDbhImage(): cannot find baseline '" + station1Name + ":" + station2Name + 
            "' in the map of baselines");
      };
    }
    else
    {
      for (BaselinesByName_it ij=baselinesByName_.begin(); ij!=baselinesByName_.end(); ++ij)
      {
        ij.value()->setSigma2add(DT_DELAY, config_->getInitAuxSigma4Delay());
        ij.value()->setSigma2add(DT_RATE,  config_->getInitAuxSigma4Rate());
      };
    };
    //
    // sources selection status:
    if (hasSourcesSelStatus_ && hasStarNames_)
    {
      int                         bitArray;
      SgVlbiSourceInfo           *si;
      for (int i=0; i<dStarNames_->dim2(); i++)
      {
        sourceName = image->getStr(dStarNames_, i,0);
        if (sourcesByName_.contains(sourceName))
        {
          si = sourcesByName_.value(sourceName);
          bitArray = image->getI2(dSourcesSelStatus_, i,0,0);
          // check for deselect flag:
          if (!(bitArray & (1<<0)))
            si ->addAttr(SgVlbiSourceInfo::Attr_NOT_VALID);
        };
      };
    };
    //
    // cable calibration signs:
    if (hasCableStations_ && hasCableSigns_)
    {
      QMap<QString, double>      signsByName;
      for (int i=0; i<dCableStations_->dim2(); i++)
      {
        QString                 stnName=image->getStr(dCableStations_, i, 0);
        QString                 stnSign=image->getStr(dCableSigns_, 0, 0).mid(2*i, 2);
        if (stnName.simplified().length()>1)
        {
          if (signsByName.contains(stnName))
            logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
              "::getDataFromDbhImage(): got duplicate station " + stnName + 
              " in the list of cable calibration signs");
          else
          {
            double                  d;
            if (stnSign.contains("-"))
              d = -1.0;
            else if (stnSign.contains("+"))
              d =  1.0;
            else 
              d =  0.0;
            signsByName[stnName] = d;
          };
        };
      };
      // apply the signs to the stations:
      for (StationsByName_it it=stationsByName_.begin(); it!=stationsByName_.end(); ++it)
      {
        QString                 stnName=it.value()->getKey();
        if (signsByName.contains(stnName))
        {
          // store a sign in separate place (may be we will need it later):
          it.value()->setCableCalMultiplierDBCal(signsByName[stnName]);
          logger->write(SgLogger::DBG, SgLogger::IO_DBH, className() +
            "::getDataFromDbhImage(): according to dbcal the station " + stnName + 
            (signsByName[stnName]!=0?(" has a sign `" + 
              QString(signsByName[stnName]==1?"+":"-") + 
                "' of cable calibration"):" has no cable calibration sign"));
        }
        else // no cable sign ==> no cable data at all. 
        {
          logger->write(SgLogger::INF, SgLogger::IO_DBH, className() +
            "::getDataFromDbhImage(): the station " + stnName + " has no cable cal sign");
          it.value()->addAttr(SgVlbiStationInfo::Attr_IGNORE_CABLE_CAL);
        };
      };
    };
    // a priori clock model (if available):
    if (hasAcmNames_ && hasAcmOffsets_ && hasAcmRates_)   // ACM stuff
    {
      int                     n=4;
      bool                    isOk=true;
      if (hasAcmNumber_)
        n = image->getI2(dAcmNumber_, 0,0,0);
      // check the dimensions:

      if (!(dAcmNames_->dim2()==n || (dAcmNames_->dim2()==4 && n<=4)))
      {
        isOk = false;
        logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
          "::getDataFromDbhImage(): get unexpected dimension of " + dAcmNames_->getLCode() + ": " +
          QString("").setNum(dAcmNames_->dim2()) + "; have to be " + QString("").setNum(n));
      };
      if (!(dAcmOffsets_->dim1()==n || (dAcmOffsets_->dim1()==4 && n<=4)))
      {
        isOk = false;
        logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
          "::getDataFromDbhImage(): get unexpected dimension of " + dAcmOffsets_->getLCode() + ": " +
          QString("").setNum(dAcmOffsets_->dim1()) + "; have to be " + QString("").setNum(n));
      };
      if (!(dAcmRates_->dim1()==n || (dAcmRates_->dim1()==4 && n<=4)))
      {
        isOk = false;
        logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
          "::getDataFromDbhImage(): get unexpected dimension of " + dAcmRates_->getLCode() + ": " +
          QString("").setNum(dAcmRates_->dim1()) + "; have to be " + QString("").setNum(n));
      };
      if (hasAcmEpochs_ && !(dAcmEpochs_->dim2()==n || (dAcmEpochs_->dim2()==4 && n<=4)))
      {
        isOk = false;
        logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
          "::getDataFromDbhImage(): get unexpected dimension of " + dAcmEpochs_->getLCode() + ": " +
          QString("").setNum(dAcmEpochs_->dim2()) + "; have to be " + QString("").setNum(n));
      };
      // 
      if (isOk)
        for (int i=0; i<n; i++)
        {
          QString             stnName=image->getStr(dAcmNames_, i, 0);
          if (stnName.simplified().size())
          {
            logger->write(SgLogger::DBG, SgLogger::IO_DBH, className() +
              "::getDataFromDbhImage(): processing ACM for [" + stnName + "] station");
            if (stationsByName_.contains(stnName))
            {
              station1Info = stationsByName_.value(stnName);
              // the ACM with non-zero epochs will go to a clock break model:
              if (hasAcmEpochs_ && image->getI2(dAcmEpochs_,  0,i,0))
              {
                year   = image->getI2(dAcmEpochs_,  0,i,0);
                month  = image->getI2(dAcmEpochs_,  1,i,0);
                day    = image->getI2(dAcmEpochs_,  2,i,0);
                hour   = image->getI2(dAcmEpochs_,  3,i,0);
                minute = image->getI2(dAcmEpochs_,  4,i,0);
                second = image->getI2(dAcmEpochs_,  5,i,0);
                // add a static clock break:
                SgParameterBreak     
                               *clockBreak=
                                new SgParameterBreak(SgMJD(year, month, day, hour, minute, second), 
                                                      image->getR8(dAcmOffsets_, i,0,0)*1.0e9,   // ns
                                                      image->getR8(dAcmRates_  , i,0,0)*1.0e12); // ps/s
                clockBreak->delAttr(SgParameterBreak::Attr_DYNAMIC);
                if (station1Info->clockBreaks().addBreak(clockBreak))
                  logger->write(SgLogger::INF, SgLogger::IO_DBH, className() +
                    "::getDataFromDbhImage(): a priori clock model was added to the station " + 
                    station1Info->getKey() + " as a static clock break that occurred on " + 
                    clockBreak->toString());
                else
                  logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
                    "::getDataFromDbhImage(): adding new clock break at " + station1Info->getKey() +
                    " station on " + clockBreak->toString() + " failed");
              }
              // otherwice, add it as a permanent shift:
              else
              {
                station1Info->setNeed2useAPrioriClocks(true);
                station1Info->setAPrioriClockTerm_0(image->getR8(dAcmOffsets_, i,0,0));
                station1Info->setAPrioriClockTerm_1(image->getR8(dAcmRates_,   i,0,0));
              };
            }
            else
              logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
                "::getDataFromDbhImage(): the station [" + stnName + "] was not found in the map");
          };
        }
      else
        logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
          "::getDataFromDbhImage(): a priori clock model was found but the dimensions looks strange");
    }
    else 
      logger->write(SgLogger::DBG, SgLogger::IO_DBH, className() +
        "::getDataFromDbhImage(): no any a priori clock model found");
    // flags for cable calibrations (if available):
    if (hasCalList_ && hasCalSites_ && hasCalFlags_)
    {
      bool                        isCableCalMatched(false);
      int                         cableCalFlagIdx(-1);
      for (int i=0; i<dCalList_->dim2(); i++)
        if (image->getStr(dCalList_, i,0) == "CABL DEL")
        {
          isCableCalMatched = true;
          cableCalFlagIdx = i;
        };
      if (isCableCalMatched)
      {
        QString         str;
        // use order of stations from CALSITES list:
        for (int i=0; i<dCalSites_->dim2(); i++)
        {
          str = image->getStr(dCalSites_, i,0);
          if  (stationsByName_.contains(str))
          {
            SgVlbiStationInfo    *si=stationsByName_.value(str);
            // just alternate cable cal flag:
            int                   nFlag=image->getI2(dCalFlags_, i,0,0,-1);
            if (nFlag & 1<<cableCalFlagIdx)
              si->delAttr(SgVlbiStationInfo::Attr_IGNORE_CABLE_CAL);
            else
              si->addAttr(SgVlbiStationInfo::Attr_IGNORE_CABLE_CAL);
          }
          else
            logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() +
              "::getDataFromDbhImage(): cannot find " + str + 
              " (an entry from CALSITES LCode) in the list of stations");
        };
      }
      else
        logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
          "::getDataFromDbhImage(): there is no \"CABL DEL\" flag in the LCode CAL LIST");
    };
    // set up a reference clock site, if available:
    QString               clockSiteName("");
    if (dClockSite_)
    {
      clockSiteName = image->getStr(dClockSite_, 0, 0);
      for (StationsByName_it it=band->stationsByName().begin(); it!=band->stationsByName().end(); ++it)
      {
        if (clockSiteName == it.value()->getKey())
        {
          it.value()->addAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS);
          logger->write(SgLogger::DBG, SgLogger::IO_DBH, className() +
            "::getDataFromDbhImage(): Reference Clock attribute has been assigned to the station " +
            it.value()->getKey() + "; band: " + band->getKey());
        };
      };
    };
    // reference clock site finished.
    // clock break:
    if (hasClockBreakStatus_ && image->getStr(dClockBreakStatus_, 0,0) == "YE")
    { 
      int numOfClockBreaks = image->getI2(dClkBrkNum_,   0,0,0);
      if (numOfClockBreaks != dClkBrkEpochs_->dim1())
      {
        logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() +
          "::getDataFromDbhImage(): declared number of clock breaks (" + 
          QString("").setNum(numOfClockBreaks) + ") is not equal to BRK_EPOC dimension " +
          QString("").setNum(dClkBrkEpochs_->dim1()));
        numOfClockBreaks = std::min(numOfClockBreaks, (int)(dClkBrkEpochs_->dim1()));
      };
      if (numOfClockBreaks != dClkBrkNames_->dim2())
      {
        logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() +
          "::getDataFromDbhImage(): declared number of clock breaks (" + 
          QString("").setNum(numOfClockBreaks) + ") is not equal to BRK_SNAM dimension " +
          QString("").setNum(dClkBrkNames_->dim2()));
        numOfClockBreaks = std::min(numOfClockBreaks, (int)(dClkBrkNames_->dim2()));
      };
      QString                   stnName("");
      for (int i=0; i<numOfClockBreaks; i++)
      {
        double                  t;
        t = image->getR8(dClkBrkEpochs_,  i,0,0) - 2400000.5; // JD => MJD
        t = round(t*DAY2SEC*10.0)/10.0/DAY2SEC; // round it up to 0.1sec
        SgMJD                   tBreak(t);
        stnName = image->getStr(dClkBrkNames_,  i,0);
        SgVlbiStationInfo      *stn2Correct=NULL;
        if (stationsByName_.contains(stnName))
          stn2Correct = stationsByName_.value(stnName);
        // databases does not know where the clock break occurred, made it session-wide and dynamic:
        if (stn2Correct) 
        {
          SgParameterBreak     *clockBreak=new SgParameterBreak(tBreak, 0.0);
          clockBreak->addAttr(SgParameterBreak::Attr_DYNAMIC);
          if (stn2Correct->clockBreaks().addBreak(clockBreak))
            logger->write(SgLogger::INF, SgLogger::IO_DBH, className() +
              "::getDataFromDbhImage(): a clock break at station " + stn2Correct->getKey() + 
              " that occurred on " + tBreak.toString() + " has been added to the session");
          else
            logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
              "::getDataFromDbhImage(): adding new clock break at " + stn2Correct->getKey() +
              " station on " + tBreak.toString() + " failed");
        };
        if (band->stationsByName().contains(stnName))
          stn2Correct = band->stationsByName().value(stnName);
        if (false && stn2Correct) // that's for future
        {
          SgParameterBreak         *clockBreak=new SgParameterBreak(SgMJD(t), 0.0);
          if (stn2Correct->clockBreaks().addBreak(clockBreak))
            logger->write(SgLogger::INF, SgLogger::IO_DBH, className() +
              "::getDataFromDbhImage(): a clock break at station " + stn2Correct->getKey() + 
              " that occurred on " + tBreak.toString() + " has been added to the band");
          else
            logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
              "::getDataFromDbhImage(): adding new clock break at " + stn2Correct->getKey() +
              " station on " + tBreak.toString() + " failed");
        };
      };
    };
    // end of clock breaks.
    // check for leap seconds:
    if (hasTai_2_Utc_)
    {
      double ls=image->getR8(dTai_2_Utc_, 1,0,0);
      if (fabs(getLeapSeconds()) < 1.0e-6) // practically, == 0.0
      {
        setLeapSeconds(ls);
        logger->write(SgLogger::DBG, SgLogger::IO_DBH, className() +
          "::getDataFromDbhImage(): a leap seconds LCode has been detected; " + QString().setNum(ls) + 
          " leap seconds were assigned");
      }
      else if (fabs(getLeapSeconds() - ls) > 1.0e-6) // just check and complain
        logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
          "::getDataFromDbhImage(): a leap seconds from the database (" + QString().setNum(ls) + 
          ") is not the same as in the session (" + QString().setNum(getLeapSeconds()) + "). Ignored.");
    };
    //
    // pick up ERP table for interpolation (if exists):
    if (hasFut1_inf_ && hasFut1_pts_)
    {
      double                      t0=image->getR8(dFut1_inf_, 0,0,0);
      double                      dt=image->getR8(dFut1_inf_, 1,0,0);
      int                         numOfPts=image->getR8(dFut1_inf_, 2,0,0);
      if (t0 > 2390000.0)
        t0 -= 2400000.5;
      if (dFut1_pts_->dim1()<numOfPts)
        numOfPts = dFut1_pts_->dim1();
      if (args4Ut1Interpolation_)
        delete args4Ut1Interpolation_;
      args4Ut1Interpolation_ = new SgVector(numOfPts);
      if (tabs4Ut1Interpolation_)
        delete tabs4Ut1Interpolation_;
      tabs4Ut1Interpolation_ = new SgMatrix(numOfPts, 1);
      for (int i=0; i<numOfPts; i++)
      {
        args4Ut1Interpolation_->setElement(i, t0 + dt*i);
        // LCODE contains TAI-UT1, revert the sign:
        tabs4Ut1Interpolation_->setElement(i, 0, -image->getR8(dFut1_pts_, i,0,0));
      };
      hasUt1Interpolation_ = true;
      logger->write(SgLogger::DBG, SgLogger::IO_DBH, className() +
        "::getDataFromDbhImage(): loaded " + QString().setNum(numOfPts) + 
        " records for UT1 interpolation");
    }
    else
    {
      if (args4Ut1Interpolation_)
      {
        delete args4Ut1Interpolation_;
        args4Ut1Interpolation_ = NULL;
      };
      if (tabs4Ut1Interpolation_)
      {
        delete tabs4Ut1Interpolation_;
        tabs4Ut1Interpolation_ = NULL;
      };
      hasUt1Interpolation_ = false;
    };
    if (hasFwob_inf_ && hasFwob_pts_)
    {
      double                      t0=image->getR8(dFwob_inf_, 0,0,0);
      double                      dt=image->getR8(dFwob_inf_, 1,0,0);
      int                         numOfPts=image->getR8(dFwob_inf_, 2,0,0);
      if (t0 > 2390000.0)
        t0 -= 2400000.5;
      if (dFwob_pts_->dim2()<numOfPts)
        numOfPts = dFwob_pts_->dim2();
      if (args4PxyInterpolation_)
        delete args4PxyInterpolation_;
      args4PxyInterpolation_ = new SgVector(numOfPts);
      if (tabs4PxyInterpolation_)
        delete tabs4PxyInterpolation_;
      tabs4PxyInterpolation_ = new SgMatrix(numOfPts, 2);
      for (int i=0; i<numOfPts; i++)
      {
        args4PxyInterpolation_->setElement(i, t0 + dt*i);
        tabs4PxyInterpolation_->setElement(i, 0,   image->getR8(dFwob_pts_, 0,i,0));
        tabs4PxyInterpolation_->setElement(i, 1,   image->getR8(dFwob_pts_, 1,i,0));
      };
      hasPxyInterpolation_ = true;
      logger->write(SgLogger::DBG, SgLogger::IO_DBH, className() +
        "::getDataFromDbhImage(): loaded " + QString().setNum(numOfPts) + 
        " records for polar motion interpolation");
    }
    else
    {
      if (args4PxyInterpolation_)
      {
        delete args4PxyInterpolation_;
        args4PxyInterpolation_ = NULL;
      };
      if (tabs4PxyInterpolation_)
      {
        delete tabs4PxyInterpolation_;
        tabs4PxyInterpolation_ = NULL;
      };
      hasPxyInterpolation_ = false;
    };
    if (hasTidalUt1_)
    {
      QString ut1Type("");
      int                         n=image->getI2(dTidalUt1_, 0,0,0);
      switch (n)
      {
        case 1: 
          tabsUt1Type_ = SgTidalUt1::CT_FULL;
          ut1Type = "Seriesfull of tides";
        break;
        case -1: // UT1R
          tabsUt1Type_ = SgTidalUt1::CT_SHORT_TERMS_REMOVED;
          ut1Type = "Only short period tidal terms are removed";
        break;
        case -2: // UT1S
          tabsUt1Type_ = SgTidalUt1::CT_ALL_TERMS_REMOVED;
          ut1Type = "All tidal terms are removed";
        break;
        default:
          logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
            "::getDataFromDbhImage(): got undocumented value for the LCode TIDALUT1: " + 
            QString().setNum(n) + "; the UT1 type has been reverted to default");
        break;
      };
      logger->write(SgLogger::DBG, SgLogger::IO_DBH, className() +
        "::getDataFromDbhImage(): the type of the UT1 inner series has been set to " + ut1Type);
    };
    //
    //
    // if aux.obs were loaded, set up the flag:
    if (hasCableData_ || hasMeteoTemperatureData_ || hasMeteoPressureData_ || hasMeteoRelHumidityData_)
      addAttr(Attr_HAS_AUX_OBS);
    if (hasIonCorr_ && hasIonRms_)
    {
      band->addAttr(SgVlbiBand::Attr_HAS_IONO_SOLUTION);
      addAttr(Attr_HAS_IONO_CORR);
//      addAttr(Attr_FF_ION_C_CALCULATED);
    };
    //
    // checks for available "contributions":
    if (hasOloadContrib_)
      hasOceanTideContrib_ = true;
    if (hasPtdContrib_)
      hasPoleTideContrib_ = true;
    if (hasEtdContrib_)
      hasEarthTideContrib_ = true;
    if (hasWobXContrib_)
      hasWobblePxContrib_ = true;
    if (hasWobYContrib_)
      hasWobblePyContrib_ = true;
    if (hasWobNutatContrib_)
      hasWobbleNutContrib_ = true;
    if (hasWobOrtho_)
      hasWobbleHighFreqContrib_ = true;
    if (hasUt1Ortho_)
      hasUt1HighFreqContrib_ = true;
    if (hasFeedCorr_)
      hasFeedCorrContrib_ = true;
    if (hasTiltRemvr_)
      hasTiltRemvrContrib_ = true;
    if (hasWobLibra_)
      hasPxyLibrationContrib_ = true;
    if (hasUt1Libra_)
      hasUt1LibrationContrib_ = true;
    if (hasOptlContib_)
      hasOceanPoleTideContrib_ = true;
    if (hasMslmIonContrib_)
      hasGpsIonoContrib_ = true;
    if (hasPtdOld_)
      hasPoleTideOldContrib_ = true;
    if (hasOceOld_)
      hasOceanTideOldContrib_ = true;
    if (hasNdryCont_)
      hasNdryContrib_ = true;
    if (hasNwetCont_)
      hasNwetContrib_ = true;
    if (hasUnPhaseCal_)
      hasUnPhaseCalContrib_ = true;
    if (hasNgradParts_)
      hasGradPartials_ = true;
    if (hasNutationPart2K_ || hasNut06xyp_)
      hasCipPartials_ = true;
    else
      hasCipPartials_ = false;
    //
    // check for calibration set up:
    if (hasObcFlags_ && hasObcList_)
    {
      int                       flags=image->getI2(dObcFlags_, 0,0,0);
      QList<QString>            calList;
      for (int i=0; i<dObcList_->dim2(); i++)
        calList << image->getStr(dObcList_, i,0);
      applyObsCalibrationSetup(flags, calList);
      calList.clear();
    };
  };
  //--

  band->setTCreation(image->fileCreationEpoch());
  band->setInputFileName(image->fileName());
  band->setInputFileVersion(image->fileVersion());
  //  if (hasCalcVersionValue_)
  //    band->setCalcVersionValue(image->getR8(dCalcVersionValue_, 0,0,0));
  //
  // quick'n'dirty:
  int                           idx(0);
  o = observations_.at(idx)->observable(bandKey);
  while (!o && idx<observations_.size())
    o = observations_.at(idx++)->observable(bandKey);
  if (o)
    band->setFrequency(o->getReferenceFrequency());
  //
  // pick up history:
  band->history().importDbhHistoryBlock(image);
  //
  // ok, this is a primary band:
  if (have2getAll)
  {
    primaryBand_ = band;
    primaryBand_->addAttr(SgVlbiBand::Attr_PRIMARY);

    if (hasError_Bl_ && hasError_K_)
      addAttr(Attr_HAS_WEIGHTS);

    if (hasConsensusDelay_)
      addAttr(Attr_HAS_CALC_DATA);
  };

  SgMJD                         finisEpoch(SgMJD::currentMJD());
  logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() +
    "::getDataFromDbhImage(): data have been read from DBH database \"" + image->fileName() +
    "\", elapsed time: " + QString("").sprintf("%.2f", (finisEpoch - startEpoch)*86400000.0) + " ms");

  return isOk;
};



//
bool SgVlbiSession::putDataIntoDbhImage(SgDbhImage *image)
{
  SgMJD                         startEpoch(SgMJD::currentMJD());
  bool                          isOk(true);
  bool                          isFormatActual;
  bool                          needIonFrqOutput;
  bool                          needEccOutput;
  bool                          need2CreateStnDepndntCals;
  bool                          need2CreateFlybyCals;
  bool                          need2CreateObcCals;
  bool                          need2MoveSecBandInfo(false);
  int                           numOfReferenceClockStations;
  int                           numOfClockBreaks;
  int                           numOfAcms;
  int                           numOfStations;
  int                           cableCalFlagIdx;
  int                           numOfBaselineClocks;
  QMultiMap<SgMJD, QString>     cBreakByEpoch;
  //

  cableCalFlagIdx = 0;
  
  // write first part into history block (to mimic SOLVE behavior):
  makeHistoryIntro(contemporaryHistory_);
  //
  // Now, make output.
  // first, lookup parameters:
  for (int i=0; i<numOfDxT; i++)
    dbhDxTable[i].isPresent_ = (dbhDxTable[i].d_=image->lookupDescriptor(dbhDxTable[i].lCode_)) != NULL;
  // figure out what we have to export:
  needIonFrqOutput    = !(hasGrIonFrq_ && hasPhIonFrq_);
  needEccOutput       = !(hasEccCoords_ && hasEccNames_ && hasEccTypes_);
  need2MoveSecBandInfo= !(hasNAmbigSecBand_ && hasQualityCodeSecBand_ && hasEffFreqsSecBand_ &&
                          hasRateObsSecBand_ && hasSigmaRateObsSecBand_ && hasDelayObsSecBand_ &&
                          hasSigmaDelayObsSecBand_ && hasPhaseObsSecBand_ && hasSigmaPhaseObsSecBand_ &&
                          hasTotalPhaseSecBand_ && hasRefFreqSecBand_ && hasSnrSecBand_ &&
                          hasCorrCoefSecBand_ && hasSBDelaySecBand_ && hasSigmaSBDelaySecBand_ &&
                          hasGrpAmbiguitySecBand_ && hasFourFitFileNameSecBand_);
  need2MoveSecBandInfo = true; // move it anyway!
  // but if we have only one band, nothing to move:
  need2MoveSecBandInfo = need2MoveSecBandInfo && bands_.size()>1;
  need2CreateStnDepndntCals = !(hasCalList_ && hasCalSites_ && hasCalFlags_);
  need2CreateFlybyCals =      !(hasFclList_ && hasFclFlags_);
//need2CreateObcCals =        !(hasObcList_ && hasObcFlags_);
  need2CreateObcCals = true;
  // adjust dimensions:
  if (hasCalSites_)
  {
    rCalSites_.dimensions_[0] = dCalSites_->dim1();
    rCalSites_.dimensions_[1] = dCalSites_->dim2();
    rCalSites_.dimensions_[2] = dCalSites_->dim3();
  };
  if (hasCalFlags_)
  {
    rCalFlags_.dimensions_[0] = dCalFlags_->dim1();
    rCalFlags_.dimensions_[1] = dCalFlags_->dim2();
    rCalFlags_.dimensions_[2] = dCalFlags_->dim3();
  };

  // form lists of stations/sources/baselines:
  QList<SgVlbiStationInfo*>     stations;
  QList<SgVlbiSourceInfo*>      sources;
  QList<SgVlbiBaselineInfo*>    baselines;
  // keep the same order as in the DBH file:
  for (int i=0; i<dSiteNames_->dim2(); i++)
  {
    QString stationName = image->getStr(dSiteNames_, i,0);
    if (stationsByName_.contains(stationName))
      stations.append(stationsByName_.value(stationName));
  };
  numOfStations = stations.size();
  for (int i=0; i<dStarNames_->dim2(); i++)
  {
    QString sourceName = image->getStr(dStarNames_, i,0);
    if (sourcesByName_.contains(sourceName))
      sources.append(sourcesByName_.value(sourceName));
  };
  for (BaselinesByName_it it=baselinesByName_.begin(); it!=baselinesByName_.end(); ++it)
    baselines.append(it.value());

  // calc numbers of clock-reference stations and clock breaks:
  numOfReferenceClockStations = 0;
  numOfClockBreaks = 0;
  numOfAcms = 0;
  for (int i=0; i<stations.size(); i++)
  {
    SgVlbiStationInfo          *si=stations.at(i);
    if (si->isAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS))
      numOfReferenceClockStations++;
    if (si->clockBreaks().size())
    {
      si->calcCBEpochs4Export(*si->auxObservationByScanId());
      for (int j=0; j<si->clockBreaks().size(); j++)
      {
        SgParameterBreak       *pb=si->clockBreaks().at(j);
        // session-wide static clock breaks are going to ACM:
        if (pb->isAttr(SgParameterBreak::Attr_DYNAMIC))
          cBreakByEpoch.insert(pb->getEpoch4Export(), si->getKey());
        else
          numOfAcms++;
      };
    };
    if (si->getNeed2useAPrioriClocks())
      numOfAcms++;
  };
  logger->write(SgLogger::DBG, SgLogger::IO_DBH, className() +
    "::putDataIntoDbhImage(): the number of ACMs is " + QString("").setNum(numOfAcms));
  //
  // check for clock break flags at the each band:
  for (int iBand=0; iBand<bands_.size(); iBand++)
  {
    SgVlbiBand               *band=bands_.at(iBand);
    for (StationsByName_it it=band->stationsByName().begin(); it!=band->stationsByName().end(); ++it)
    {
      SgVlbiStationInfo *si=it.value();
      if (si->clockBreaks().size())
      {
        if (stationsByName_.contains(si->getKey()))
          si->calcCBEpochs4Export( *stationsByName_.value(si->getKey())->auxObservationByScanId() );
        for (int j=0; j<si->clockBreaks().size(); j++)
        {
          SgMJD             cbEpoch=si->clockBreaks().at(j)->getEpoch4Export();
          if (!cBreakByEpoch.contains(cbEpoch))
            cBreakByEpoch.insert(cbEpoch, si->getKey());
          else if (si->getKey() != cBreakByEpoch.find(cbEpoch).value()) 
            cBreakByEpoch.insert(cbEpoch, si->getKey());
        };
      };
    };
  };
  numOfClockBreaks = cBreakByEpoch.size();
  
  if (!need2CreateStnDepndntCals) // checking, continue:
  {
    bool                        have2rewriteSDC=false;
    // dimensions, if they are not equal, reset the three LCodes
    if (rCalSites_.dimensions_[1] != stationsByName_.size())
    {
      have2rewriteSDC = true;
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::putDataIntoDbhImage(): rCalSites_.dimensions_[1] != stationsByName_.size(), " +
        QString("").setNum(rCalSites_.dimensions_[1]) + "!=" + 
        QString("").setNum(stationsByName_.size()));
    };
    if (rCalFlags_.dimensions_[0] != stationsByName_.size())
    {
      have2rewriteSDC = true;
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::putDataIntoDbhImage(): rCalFlags_.dimensions_[0] != stationsByName_.size(), " +
        QString("").setNum(rCalFlags_.dimensions_[0]) + "!=" + 
        QString("").setNum(stationsByName_.size()));
    };
    bool                        isCableCalMatched=false;
    for (int i=0; i<rCalList_.dimensions_[1]; i++)
      if (image->getStr(dCalList_, i,0) == "CABL DEL")
      {
        isCableCalMatched = true;
        cableCalFlagIdx = i;
      };
    if (!isCableCalMatched)
      have2rewriteSDC = true;
    if (have2rewriteSDC)
    { 
      // complain!
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::putDataIntoDbhImage(): looks like we have to reset station dependent calibration flags, "
        "LCodes: CALSITES, CAL LIST and CAL FLGS");
      need2CreateStnDepndntCals = true;
    };
  };
  // figure out how many baselines have clock offset:
  numOfBaselineClocks = 0;
  for (int i=0; i<baselines.size(); i++)
    if (baselines.at(i)->isAttr(SgVlbiBaselineInfo::Attr_ESTIMATE_CLOCKS))
      numOfBaselineClocks++;
  //
  // end of checking
  //
  // adjust dimensions of some descriptors:
  //
  rError_K_ .dimensions_[1] = baselinesByName_.size();
  rError_Bl_.dimensions_[2] = baselinesByName_.size();
  rBaselinesSelStatus_.dimensions_[0] = numOfStations*numOfStations;
  rSourcesSelStatus_.dimensions_[0] = sources.size();
  rBlDepClocks_.dimensions_[1] = numOfBaselineClocks;
  //
  if (numOfAcms)
  {
    rAcmNames_  .dimensions_[1] = useOldAcm?4:numOfAcms;
    rAcmOffsets_.dimensions_[0] = useOldAcm?4:numOfAcms;
    rAcmRates_  .dimensions_[0] = useOldAcm?4:numOfAcms;
    rAcmEpochs_ .dimensions_[1] = useOldAcm?4:numOfAcms;
  };
  if (useOldAcm && 4<numOfAcms)
    logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
      "::putDataIntoDbhImage(): number of stations with a priori clock models (" + 
      QString("").setNum(numOfAcms) + ") is greater than 4, some parameters will not be stored");
  //
  if (numOfReferenceClockStations>0)
    rRefClockSites_.dimensions_[1] = numOfReferenceClockStations;
  if (numOfClockBreaks)
  {
    rClkBrkEpochs_.dimensions_[0] = numOfClockBreaks;
    rClkBrkFlags_.dimensions_[0]  = numOfClockBreaks;
    rClkBrkNames_.dimensions_[1]  = numOfClockBreaks;
  };
  // clock and zenith delay constraints:
  rAtmConstr_.dimensions_[0]    = numOfStations;
  rClockConstr_.dimensions_[0]  = numOfStations;
 
  //
  //
  // form the list of descriptors which have to be modified:
  QList<DbhDescriptorX*>        listOfDescriptors;
  //
  // TOC #1 (header):
  listOfDescriptors.append(&rNObs_);
  listOfDescriptors.append(&rN4Obs_);
  listOfDescriptors.append(&rNRefClocks_);
  // ionosphere correction:
  listOfDescriptors.append(&rIonCorr_);
  listOfDescriptors.append(&rIonRms_);
  // number of ambiguities:
  listOfDescriptors.append(&rNAmbig_);
  // troposphere and clocks attributes:
  listOfDescriptors.append(&rAtmConstr_);
  listOfDescriptors.append(&rAtmIntrv_);
  listOfDescriptors.append(&rClockConstr_);
  listOfDescriptors.append(&rClockIntrv_);
  listOfDescriptors.append(&rBatchCnt_);
  listOfDescriptors.append(&rSolData_);
  listOfDescriptors.append(&rSupMet_);
  listOfDescriptors.append(&rAtmPartFlag_);
  if (numOfBaselineClocks > 0)
    listOfDescriptors.append(&rBlDepClocks_);
  listOfDescriptors.append(&rError_K_);
  listOfDescriptors.append(&rError_Bl_);
  listOfDescriptors.append(&rBaselinesSelStatus_);
  listOfDescriptors.append(&rSourcesSelStatus_);
  listOfDescriptors.append(&rIonBits_);
  listOfDescriptors.append(&rUserAction4Suppression_);
  if (numOfReferenceClockStations)
  {
    listOfDescriptors.append(&rRefClockSites_);
    listOfDescriptors.append(&rClockSite_);
  };
  listOfDescriptors.append(&rEl_Cut_);
  listOfDescriptors.append(&rAtmCflg_);
  listOfDescriptors.append(&rClkCflg_);
  if (numOfAcms)
  {
    listOfDescriptors.append(&rAcmNames_);
    listOfDescriptors.append(&rAcmOffsets_);
    listOfDescriptors.append(&rAcmRates_);
    listOfDescriptors.append(&rAcmEpochs_);
    listOfDescriptors.append(&rAcmNumber_);
  };
  if (numOfClockBreaks)
  {
    listOfDescriptors.append(&rClkBrkEpochs_);
    listOfDescriptors.append(&rClkBrkNum_);
    listOfDescriptors.append(&rClkBrkFlags_);
    listOfDescriptors.append(&rClkBrkNames_);
    addAttr(Attr_HAS_CLOCK_BREAKS);
  }
  else if (isAttr(Attr_HAS_CLOCK_BREAKS))
    delAttr(Attr_HAS_CLOCK_BREAKS);
  listOfDescriptors.append(&rClockBreakStatus_);
  // store eccentricities in the database (if they do not there yet):
  if (needEccOutput)
  {
    rEccCoords_.dimensions_[1] = stationsByName_.size();
    rEccNames_.dimensions_ [1] = stationsByName_.size();
    rEccTypes_.dimensions_ [0] = stationsByName_.size();
    listOfDescriptors.append(&rEccCoords_);
    listOfDescriptors.append(&rEccNames_);
    listOfDescriptors.append(&rEccTypes_);
  };
  if (need2CreateStnDepndntCals)
  {
    rCalSites_.dimensions_ [1] = stationsByName_.size();
    rCalFlags_.dimensions_ [0] = stationsByName_.size();
    // just to be sure:
    rCalList_. dimensions_ [0] = 4;
    rCalList_. dimensions_ [1] = 6;
    rCalList_. dimensions_ [2] = 1;
    listOfDescriptors.append(&rCalList_);
    listOfDescriptors.append(&rCalSites_);
    listOfDescriptors.append(&rCalFlags_);
  };
  if (need2CreateFlybyCals)
  {
    rFclFlags_.dimensions_ [0] = 7;
    rFclFlags_.dimensions_ [1] = stationsByName_.size();
    rFclFlags_.dimensions_ [2] = 1;
    // just to be sure:
    rFclList_. dimensions_ [0] = 4;
    rFclList_. dimensions_ [1] = 8;
    rFclList_. dimensions_ [2] = 1;
    listOfDescriptors.append(&rFclList_);
    listOfDescriptors.append(&rFclFlags_);
  };
  if (need2CreateObcCals)
  {
    rObcFlags_.dimensions_ [0] = 1;
    rObcFlags_.dimensions_ [1] = 1;
    rObcFlags_.dimensions_ [2] = 1;
    // just to be sure:
    rObcList_. dimensions_ [0] = 4;
    rObcList_. dimensions_ [1] = calcInfo_.getDversion()<11.0 ? 12 : 14;
    rObcList_. dimensions_ [2] = 1;
    listOfDescriptors.append(&rObcList_);
    listOfDescriptors.append(&rObcFlags_);
  };
  //
  //
  // TOC #2:
  listOfDescriptors.append(&rIonCode_);
  if (needIonFrqOutput)
  {
    listOfDescriptors.append(&rGrIonFrq_);
    listOfDescriptors.append(&rPhIonFrq_);
  };
  if (need2MoveSecBandInfo)
  {
    listOfDescriptors.append(&rNAmbigSecBand_);
    listOfDescriptors.append(&rQualityCodeSecBand_);
    listOfDescriptors.append(&rEffFreqsSecBand_);
    listOfDescriptors.append(&rRateObsSecBand_);
    listOfDescriptors.append(&rSigmaRateObsSecBand_);
    listOfDescriptors.append(&rDelayObsSecBand_);
    listOfDescriptors.append(&rSigmaDelayObsSecBand_);
    listOfDescriptors.append(&rPhaseObsSecBand_);
    listOfDescriptors.append(&rSigmaPhaseObsSecBand_);
    listOfDescriptors.append(&rTotalPhaseSecBand_);
    listOfDescriptors.append(&rRefFreqSecBand_);
    listOfDescriptors.append(&rSnrSecBand_);
    listOfDescriptors.append(&rCorrCoefSecBand_);
    listOfDescriptors.append(&rSBDelaySecBand_);
    listOfDescriptors.append(&rSigmaSBDelaySecBand_);
    listOfDescriptors.append(&rGrpAmbiguitySecBand_);
    listOfDescriptors.append(&rFourFitFileNameSecBand_);
    listOfDescriptors.append(&rPhAmbig_S_);
  };

  //
  //
  // check for their presence:
  isFormatActual = true;
  for (int i=0; i<listOfDescriptors.size(); i++)
  {
    SgDbhDatumDescriptor *d;
    if (!(d=image->lookupDescriptor(listOfDescriptors.at(i)->lCode_)))
      isFormatActual = listOfDescriptors.at(i)->isPresent_ = false;
    else if ( d->dim1() != listOfDescriptors.at(i)->dimensions_[0] ||
              d->dim2() != listOfDescriptors.at(i)->dimensions_[1] ||
              d->dim3() != listOfDescriptors.at(i)->dimensions_[2])
    {
      isFormatActual = false;
      listOfDescriptors.at(i)->isPresent_ = true;
      listOfDescriptors.at(i)->isDimensionsFixed_ = false;
    }
    else
    {
      listOfDescriptors.at(i)->isPresent_ = true;
      listOfDescriptors.at(i)->isDimensionsFixed_ = true;
    };
  };
  // modify the format:
  if (!isFormatActual)
  {
    // delete LCODEs that changed dimensions:
    image->startFormatModifying();
    for (int i=0; i<listOfDescriptors.size(); i++)
    {
      DbhDescriptorX*   rec=listOfDescriptors.at(i);
      if (rec->isPresent_ && !rec->isDimensionsFixed_)
      {
        SgDbhDatumDescriptor   *d=new SgDbhDatumDescriptor(rec->lCode_, rec->description_);
        image->delDescriptor(d);
        rec->isPresent_ = false;
        delete d;
      };
    };
    image->finisFormatModifying();
    // add new LCODEs:
    image->startFormatModifying();
    for (int i=0; i<listOfDescriptors.size(); i++)
    {
      DbhDescriptorX*   rec=listOfDescriptors.at(i);
      if (!rec->isPresent_)
      {
        SgDbhDatumDescriptor   *d=new SgDbhDatumDescriptor(rec->lCode_, rec->description_);
        d->setType(rec->type_);
        d->setDimensions(rec->dimensions_[0], rec->dimensions_[1], rec->dimensions_[2]);
        d->setNTc(rec->numOfTc_);
        image->addDescriptor(d);
        delete d;
      };
    };
    image->finisFormatModifying();
  };
  // at last, look up for actual descriptors:
  for (int i=0; i<listOfDescriptors.size(); i++)
    listOfDescriptors.at(i)->d_ = image->lookupDescriptor(listOfDescriptors.at(i)->lCode_);

  //
  // update the image:
  //
  // first, deal with the header (TOC#1):
  //
  //-----------------------------------------------------------------------------------------------------
  //
  // general info:
  image->setI2 (dNObs_,         0,0,0,  -1, primaryBand_->observables().size());
  image->setI2 (dNRefClocks_,   0,0,0,  -1, numOfReferenceClockStations);
  //
  // here, "7" is "G-Gxs combination":
  image->setI2 (dSolData_,      0,0,0,  -1, 7);
  // SUPMET Codes (we do not need it, just to keep SOLVE happy):
  // PARAMETER  ( SUPMET__PRE98 = 501 ) ! pre-98 method of observ. suppression
  // PARAMETER  ( SUPMET__PRE91 = 502 ) ! pre-91 method of observ. suppression
  // PARAMETER  ( SUPMET__COMB1 = 503 ) ! combination method of suppression
  // PARAMETER  ( SUPMET__SNGBA = 504 ) ! single band method of suppression
  // PARAMETER  ( SUPMET__META  = 505 ) ! advanced Meta-Solve strategey
  image->setI2 (dSupMet_,       0,0,0,  -1, 503);
  //
  // no choice:
  image->setI2 (dAtmPartFlag_,  0,0,0,  -1, 7);
  //
  // set up observation calibrations:
  int                           flag;
  QList<QString>                calList;
  formObsCalibrationSetup(flag, calList);
  image->setI2 (dObcFlags_,     0,0,0,  -1, flag);
  //  
  image->setJ4 (dN4Obs_,        0,0,0,  -1, primaryBand_->observables().size());
  image->setR8 (dClockIntrv_,   0,0,0,  -1, parametersDescriptor_->getClock0().getPwlStep()*24.0);
  image->setR8 (dAtmIntrv_,     0,0,0,  -1, parametersDescriptor_->getZenith().getPwlStep()*24.0);
  // hardcoded, we do not have elevation cut off implementation:
  image->setStr(dEl_Cut_,       0,0,    -1, "ALL      0.00000");
  image->setStr(dAtmCflg_,      0,0,    -1, "YE");
  image->setStr(dClkCflg_,      0,0,    -1, "YE");
  image->setStr(dBatchCnt_,     0,0,    -1, "YEYE");

  if (need2CreateStnDepndntCals)
  {
    image->setStr(dCalList_,    0,0,    -1, sCalList[0]);
    image->setStr(dCalList_,    1,0,    -1, sCalList[1]);
    image->setStr(dCalList_,    2,0,    -1, sCalList[2]);
    image->setStr(dCalList_,    3,0,    -1, sCalList[3]);
    image->setStr(dCalList_,    4,0,    -1, sCalList[4]);
    image->setStr(dCalList_,    5,0,    -1, sCalList[5]);
  };
  if (need2CreateFlybyCals)
    for (int i=0; i<8; i++)
      image->setStr(dFclList_,  i,0,    -1, sFclList[i]);

  if (need2CreateObcCals)
    for (int i=0; i<calList.size(); i++)
      image->setStr(dObcList_,  i,0,    -1, calList.at(i));
  calList.clear();
  //
  //-----------------------------------------------------------------------------------------------------
  //
  // stations:
  // init a priori clock model:
  if (numOfAcms)
  {
    image->setI2 (dAcmNumber_,  0,0,0, -1,  numOfAcms);
    for (int i=0; i<(useOldAcm?4:numOfAcms); i++)
    {
      image->setStr(dAcmNames_,   i,0,   -1, QString("        "));
      image->setR8 (dAcmOffsets_, i,0,0, -1, 0.0);
      image->setR8 (dAcmRates_,   i,0,0, -1, 0.0);
      image->setI2 (dAcmEpochs_,  0,i,0, -1, 0.0);
      image->setI2 (dAcmEpochs_,  1,i,0, -1, 0.0);
      image->setI2 (dAcmEpochs_,  2,i,0, -1, 0.0);
      image->setI2 (dAcmEpochs_,  3,i,0, -1, 0.0);
      image->setI2 (dAcmEpochs_,  4,i,0, -1, 0.0);
      image->setI2 (dAcmEpochs_,  5,i,0, -1, 0.0);
    };
  };
  //
  QString                       sEccTypes("");
  int                           maxObsAtRefClck=0, clkSiteIdx=-1, rfIdx=0, acmIdx=0;
  for (int i=0; i<stations.size(); i++)
  {
    SgVlbiStationInfo          *si=stations.at(i);
    //
    // a priori clock model parameters:
    if (si->getNeed2useAPrioriClocks())
    {
      image->setStr(dAcmNames_,   acmIdx,0,   -1, si->getKey());
      image->setR8 (dAcmOffsets_, acmIdx,0,0, -1, si->getAPrioriClockTerm_0());
      image->setR8 (dAcmRates_,   acmIdx,0,0, -1, si->getAPrioriClockTerm_1());
      if (!useOldAcm || (useOldAcm&&acmIdx<4))
        acmIdx++;
    };
    for (int j=0; j<si->clockBreaks().size(); j++)
    {
      SgParameterBreak       *pb=si->clockBreaks().at(j);
      if (!pb->isAttr(SgParameterBreak::Attr_DYNAMIC))
      {
        int                     year, month, day, hour, minute;
        double                  second;
        SgMJD::MJD_reverse(pb->getDate(), pb->getTime(), year, month, day, hour, minute, second);
        image->setStr(dAcmNames_,   acmIdx,0,   -1, si->getKey());
        image->setR8 (dAcmOffsets_, acmIdx,0,0, -1, pb->getA0()*1.0e-9);   // ns
        image->setR8 (dAcmRates_,   acmIdx,0,0, -1, pb->getA1()*1.0e-12);  // ps/s
        image->setI2 (dAcmEpochs_,  0,acmIdx,0, -1, year);
        image->setI2 (dAcmEpochs_,  1,acmIdx,0, -1, month);
        image->setI2 (dAcmEpochs_,  2,acmIdx,0, -1, day);
        image->setI2 (dAcmEpochs_,  3,acmIdx,0, -1, hour);
        image->setI2 (dAcmEpochs_,  4,acmIdx,0, -1, minute);
        image->setI2 (dAcmEpochs_,  5,acmIdx,0, -1, round(second));
        if (!useOldAcm || (useOldAcm&&acmIdx<4))
          acmIdx++;
      };
    };
    //
    // reference clock stations:
    if (si->isAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS) && numOfReferenceClockStations)
    {
      if (maxObsAtRefClck < si->numTotal(DT_DELAY))
      {
        maxObsAtRefClck = si->numTotal(DT_DELAY);
        clkSiteIdx = i;
      };
      image->setStr(dRefClockSites_, rfIdx++,0, -1, si->getKey());
    };
    //
    // Eccentricities:
    if (needEccOutput && si->eccRec())
    {
      image->setStr(dEccNames_, i,0, -1, 
        QString("").sprintf("%-8s%0d", qPrintable(si->getKey()), si->getCdpNumber()) );
      sEccTypes += si->eccRec()->getEccType()==SgEccRec::ET_XYZ ? "XY" : "NE";
      image->setR8 (dEccCoords_, 0,i,0,  -1, si->eccRec()->getDR().at(X_AXIS));
      image->setR8 (dEccCoords_, 1,i,0,  -1, si->eccRec()->getDR().at(Y_AXIS));
      image->setR8 (dEccCoords_, 2,i,0,  -1, si->eccRec()->getDR().at(Z_AXIS));
    };
    //
    // station calibrations flags (if they did not exist in the database yet):
    if (need2CreateStnDepndntCals)
    {
      // it is simple, we use just cable cal here:
      image->setStr(dCalSites_, i,0,  -1, si->getKey());
      image->setI2 (dCalFlags_, i,0,0,-1, si->isAttr(SgVlbiStationInfo::Attr_IGNORE_CABLE_CAL)?0:1);
    };
    // flyby:
    if (need2CreateFlybyCals)  // it is simple too, we have NMF only:
      image->setI2 (dFclFlags_, 0,i,0,-1, 1<<(8-1));
    //
    // clocks constraints per station (new or missed CC_SITES LCODE):
    if (!hasCcSites_)
      image->setR8 (dClockConstr_,  i,0,0,  -1,
        (si->isAttr(SgVlbiStationInfo::Attr_USE_LOCAL_CLOCKS) ?
          si->getPcClocks().getPwlAPriori() : parametersDescriptor_->getClock0().getPwlAPriori())     
            /864.0);
    // zenith delays constraints per station (new or missed AC_SITES LCODE):
    if (!hasAcSites_)
      image->setR8 (dAtmConstr_,    i,0,0,  -1,
        (si->isAttr(SgVlbiStationInfo::Attr_USE_LOCAL_ZENITH) ? 
          si->getPcZenith().getPwlAPriori() : parametersDescriptor_->getZenith().getPwlAPriori())
            /(1.0e-12*24.0*vLight*100.0));
  };
  // constraints, continue:
  if (hasCcSites_)
    for (int i=0; i<dCcSites_->dim2(); i++)
    {
      QString                   name("");
      name = image->getStr(dCcSites_, i,0);
      if (stationsByName_.contains(name))
      {
        SgVlbiStationInfo      *si=stationsByName_.value(name);
        image->setR8 (dClockConstr_,  i,0,0,  -1,
          (si->isAttr(SgVlbiStationInfo::Attr_USE_LOCAL_CLOCKS) ?
            si->getPcClocks().getPwlAPriori() : parametersDescriptor_->getClock0().getPwlAPriori())
              /864.0);
      }
      else
        logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() +
          "::putDataIntoDbhImage(): cannot find station '" + name + "' from CC_SITES in the map");
    };
  if (hasAcSites_)
    for (int i=0; i<dAcSites_->dim2(); i++)
    {
      QString                   name("");
      name = image->getStr(dCcSites_, i,0);
      if (stationsByName_.contains(name))
      {
        SgVlbiStationInfo      *si=stationsByName_.value(name);
        image->setR8 (dAtmConstr_,    i,0,0,  -1,
          (si->isAttr(SgVlbiStationInfo::Attr_USE_LOCAL_ZENITH) ? 
            si->getPcZenith().getPwlAPriori() : parametersDescriptor_->getZenith().getPwlAPriori())
              /(1.0e-12*24.0*vLight*100.0));
      }
      else
        logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() +
          "::putDataIntoDbhImage(): cannot find station '" + name + "' from AC_SITES in the map");
    };
  //
  //
  if (clkSiteIdx>-1)
    image->setStr(dClockSite_, 0,0, -1, stations.at(clkSiteIdx)->getKey());
  if (needEccOutput)// actually, the dimensions [N, 1, 1] for A2 in DBH means one string 2xN chars length
    image->setStr(dEccTypes_, 0,0, -1, sEccTypes); 
  //
  // clock breaks:
  if (numOfClockBreaks)
  { 
    contemporaryHistory_.addHistoryRecord("Detected " + QString().setNum(numOfClockBreaks) + 
      " clock break" + (numOfClockBreaks==1?"":"s"), SgMJD::currentMJD().toUtc());
    image->setI2 (dClkBrkNum_,   0,0,0, -1,  numOfClockBreaks);
    image->setStr(dClockBreakStatus_, 0,0, -1, "YE");
    int               cbIdx=0;
    for (QMultiMap<SgMJD, QString>::iterator it=cBreakByEpoch.begin();
      it!=cBreakByEpoch.end(); ++it)
    {
      QString cbName=it.value();
      SgMJD   cbEpoch=it.key();
      image->setR8 (dClkBrkEpochs_,  cbIdx,0,0, -1,  cbEpoch.toDouble() + 2400000.5); // MJD => JD
      image->setI2 (dClkBrkFlags_,   cbIdx,0,0, -1,  7);
      image->setStr(dClkBrkNames_,   cbIdx,0,   -1,  cbName);
      cbIdx++;
      logger->write(SgLogger::DBG, SgLogger::IO_DBH, className() +
        "::putDataIntoDbhImage(): registered a clock break epoch " +
        cbEpoch.toString() + " for the station " + cbName);
      // it's a little bit more informative then current SOLVE:
      contemporaryHistory_.addHistoryRecord("A clock break marker has been set for " + cbName +  " at " +
        cbEpoch.toString(), SgMJD::currentMJD().toUtc());
    };
  }
  else
    image->setStr(dClockBreakStatus_, 0,0, -1, "NO");
  //
  // station dependent calibrations:
  if (!need2CreateStnDepndntCals)
  {
    QString         str;
    // use order of stations from CALSITES list:
    for (int i=0; i<rCalSites_.dimensions_[1]; i++)
    {
      str = image->getStr(dCalSites_, i,0);
      if (stationsByName_.contains(str))
      {
        SgVlbiStationInfo    *si=stationsByName_.value(str);
        // just alternate cable cal flag:
        int                   nFlag=image->getI2(dCalFlags_, i,0,0,-1);
        if (si->isAttr(SgVlbiStationInfo::Attr_IGNORE_CABLE_CAL))
          nFlag &= ~(1<<cableCalFlagIdx);
        else
          nFlag |= 1<<cableCalFlagIdx;
        image->setI2 (dCalFlags_, i,0,0,-1, nFlag);
      }
      else
        logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() +
          "::putDataIntoDbhImage(): cannot find " + str + 
          " (an entry from CALSITES LCode) in the list of stations");
    };
  };
  //
  //-----------------------------------------------------------------------------------------------------
  //
  // sources:
  for (int i=0; i<sources.size(); i++)
  {
    SgVlbiSourceInfo           *si=sources.at(i);
    int                         bitArray;
    bitArray = 0;
    // first bit (participating in the solution):
    if (!si->isAttr(SgVlbiSourceInfo::Attr_NOT_VALID) && si->numProcessed(DT_DELAY)>0)
    {
      bitArray |= (1<<0);
    };
    // second bit:
    bitArray |= (1<<1);
    // third and fourth bits (estimation of right ascension and declination):
    if (parametersDescriptor_ &&
        parametersDescriptor_->getSrcCoo().getPMode() != SgParameterCfg::PM_NONE &&
        si->isAttr(SgVlbiSourceInfo::Attr_ESTIMATE_COO) &&
        si->numProcessed(DT_DELAY)>0)
    {
      bitArray |= (1<<2);
      bitArray |= (1<<3);
    };
    image->setI2(dSourcesSelStatus_,  i,0,0, -1, bitArray);
  };
  //
  //-----------------------------------------------------------------------------------------------------
  //
  // baselines:
  int j=0;
  for (int i=0; i<baselines.size(); i++)
  {
    QString                     st1Name, st2Name;
    SgVlbiBaselineInfo         *bi=baselines.at(i);
    st1Name = bi->getKey().left(8);
    st2Name = bi->getKey().right(8);
    image->setR8 (dError_K_,      0,i,0, -1, bi->getSigma2add(DT_DELAY));
    image->setR8 (dError_K_,      1,i,0, -1, bi->getSigma2add(DT_RATE));
    image->setStr(dError_Bl_,     0,i,   -1, st1Name);
    image->setStr(dError_Bl_,     1,i,   -1, st2Name);
    if (bi->isAttr(SgVlbiBaselineInfo::Attr_ESTIMATE_CLOCKS))
    {
      image->setStr(dBlDepClocks_,  j,0,   -1, st1Name + st2Name);
      j++;
    };
  };
  // baselines too:
  for (int i=0; i<numOfStations; i++)
  {
    for (int j=0; j<numOfStations; j++)
    {
      QString                   st1Name(stations.at(i)->getKey()), st2Name(stations.at(j)->getKey());
      int                       bitArray;
      bitArray = 0;      
      SgVlbiBaselineInfo       *bi, *biR;

      if (baselinesByName_.contains(st1Name + ":" + st2Name))
        bi = baselinesByName_.value(st1Name + ":" + st2Name);
      else
        bi = NULL;
      if (baselinesByName_.contains (st2Name + ":" + st1Name))
        biR = baselinesByName_.value(st2Name + ":" + st1Name);
      else
        biR = NULL;
      // set the first bit:
      if (bi && bi->numTotal(DT_DELAY))
      {
        bitArray |= (1<<0);
        bitArray |= (1<<2);
      };
      //
      // bi and biR can be NULLs!
      //
      // set the second and third bits:
      SgVlbiStationInfo       *s1i=NULL, *s2i=NULL;
      if (stationsByName_.contains(st1Name))
        s1i = stationsByName_.value(st1Name);
      if (stationsByName_.contains(st2Name))
        s2i = stationsByName_.value(st2Name);
      if ( ((bi  && !bi ->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID)) ||
            (biR && !biR->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID)))
          && (s1i && !s1i->isAttr(SgVlbiStationInfo::Attr_NOT_VALID))
          && (s2i && !s2i->isAttr(SgVlbiStationInfo::Attr_NOT_VALID))  )
      {
        bitArray |= (1<<1);
      };
      // set the fourth and fifth bits:
      if ((bi  && 
           bi ->isAttr(SgVlbiBaselineInfo::Attr_ESTIMATE_CLOCKS) && 
          !bi ->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID)) ||
          (biR && 
           biR->isAttr(SgVlbiBaselineInfo::Attr_ESTIMATE_CLOCKS) &&
          !biR->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID))  )
      {
        bitArray |= (1<<3);
        //        bitArray |= (1<<4);
      };
      image->setI2(dBaselinesSelStatus_,  (numOfStations*j + i),0,0, -1, bitArray);
    };
  };
  //--------------------------   TOC #>1:
  //
  // then, update the observations:
  // update data:
  QString                       pbKey=primaryBand_->getKey();
  QString                       sbKey(""), str("");
  bool                          has2ndBand;
  has2ndBand = bands_.size() > 1;
  if (has2ndBand)
  {
    int                         bandIdx=0;
    sbKey = bands_.at(bandIdx++)->getKey();
    while (sbKey==pbKey && bandIdx<bands_.size())  // pick up a first non-prime band:
      sbKey = bands_.at(bandIdx++)->getKey();
  };
  for (int iObs=0; iObs<observations_.size(); iObs++)
  {
    SgVlbiObservation          *obs=observations_.at(iObs);
    SgVlbiObservable           *pbObs=obs->observable(pbKey);
    SgVlbiObservable           *sbObs=has2ndBand?obs->observable(sbKey):NULL;
    if (pbObs) // can be NULL
    {
      int                       idx(pbObs->getMediaIdx());
      int                       ionCode(has2ndBand?-1:0);
      int                       ionBits(has2ndBand?1<<1:0);        //    No matching group data for GION
      
      // ionospheric corrections
      if (isAttr(Attr_FF_ION_C_CALCULATED))
      {
        image->setR8(dIonCorr_, 0,0,0, idx, pbObs->grDelay().getIonoValue());
        image->setR8(dIonCorr_, 1,0,0, idx, pbObs->phDRate().getIonoValue());
        image->setR8(dIonRms_,  0,0,0, idx, pbObs->grDelay().getIonoSigma());
        image->setR8(dIonRms_,  1,0,0, idx, pbObs->phDRate().getIonoSigma());
       };
      // ambiguity resolution:
      if (isAttr(Attr_FF_AMBIGS_RESOLVED))
        image->setI2(dNAmbig_,  0,0,0, idx, pbObs->grDelay().getNumOfAmbiguities());
      //
      if (needIonFrqOutput)
      {
        image->setR8(dGrIonFrq_, 0,0,0, idx, pbObs->grDelay().getEffFreq());
        image->setR8(dPhIonFrq_, 0,0,0, idx, pbObs->phDelay().getEffFreq());
      };
      //
      //
      if (pbObs->grDelay().isAttr(SgVlbiMeasurement::Attr_NOT_VALID) && 
          pbObs->grDelay().getUnweightFlag()==0)
      {
        pbObs->grDelay().setUnweightFlag(1);
        image->setI2(dDelayUFlag_,  0,0,0, idx, pbObs->grDelay().getUnweightFlag());
      };
      //
      if (has2ndBand && !sbObs && pbObs->grDelay().getUnweightFlag()==0)
      {
        pbObs->grDelay().setUnweightFlag(8);
        image->setI2(dDelayUFlag_,  0,0,0, idx, pbObs->grDelay().getUnweightFlag());
      };
      //
      if (!obs->isAttr(SgObservation::Attr_PROCESSED) && pbObs->grDelay().getUnweightFlag()==0)
      {
        pbObs->grDelay().setUnweightFlag(2);
        image->setI2(dDelayUFlag_,  0,0,0, idx, pbObs->grDelay().getUnweightFlag());
      };
      //
      if (!pbObs->grDelay().isAttr(SgVlbiMeasurement::Attr_NOT_VALID) && 
           obs->isAttr(SgObservation::Attr_PROCESSED) &&
           pbObs->grDelay().getUnweightFlag()!=0)
      {
        pbObs->grDelay().setUnweightFlag(0);
        image->setI2(dDelayUFlag_,  0,0,0, idx, pbObs->grDelay().getUnweightFlag());
      };
      //
      // just mimic SOLVE:
      if (need2MoveSecBandInfo)
        image->setJ4(dPhAmbig_S_,  0,0,0, idx, 1840672182);
      //
      //
      if (sbObs) // can be NULL too
      {
        ionCode = 0;
        str.sprintf("%2d", sbObs->getQualityFactor()>=0?sbObs->getQualityFactor():0);
        image->setI2 (dNAmbigSecBand_,          0,0,0, idx, sbObs->grDelay().getNumOfAmbiguities());
        image->setStr(dQualityCodeSecBand_,     0,0,   idx, str);
        image->setR8 (dEffFreqsSecBand_,        0,0,0, idx, sbObs->grDelay().getEffFreq());
        image->setR8 (dEffFreqsSecBand_,        1,0,0, idx, sbObs->phDelay().getEffFreq());
        image->setR8 (dEffFreqsSecBand_,        2,0,0, idx, sbObs->phDRate().getEffFreq());
        image->setR8 (dRateObsSecBand_,         0,0,0, idx, sbObs->phDRate().getValue());
        image->setR8 (dSigmaRateObsSecBand_,    0,0,0, idx, sbObs->phDRate().getSigma());
        image->setR8 (dDelayObsSecBand_,        0,0,0, idx,(sbObs->grDelay().getValue() +
                                                                    sbObs->grDelay().ambiguity())*1.0e6);
        image->setR8 (dSigmaDelayObsSecBand_,   0,0,0, idx, sbObs->grDelay().getSigma());
        image->setR8 (dPhaseObsSecBand_,        0,0,0, idx, 0.0);
        image->setR8 (dSigmaPhaseObsSecBand_,   0,0,0, idx, 0.0);
        image->setR8 (dTotalPhaseSecBand_,      0,0,0, idx, sbObs->getTotalPhase()/DEG2RAD); // ->degree
        image->setR8 (dRefFreqSecBand_,         0,0,0, idx, sbObs->getReferenceFrequency());
        image->setR8 (dSnrSecBand_,             0,0,0, idx, sbObs->getSnr());
        image->setR8 (dCorrCoefSecBand_,        0,0,0, idx, sbObs->getCorrCoeff());
        image->setR8 (dSBDelaySecBand_,         0,0,0, idx, sbObs->sbDelay().getValue()*1.0e6);
        image->setR8 (dSigmaSBDelaySecBand_,    0,0,0, idx, sbObs->sbDelay().getSigma()*1.0e6);
        image->setR8 (dGrpAmbiguitySecBand_,    0,0,0, idx, sbObs->grDelay().getAmbiguitySpacing());
        image->setStr(dFourFitFileNameSecBand_, 0,0,   idx, sbObs->getFourfitOutputFName());
      
        // set up ION Bits:
        int                     qCodeSb=sbObs->getQualityFactor();
        if (qCodeSb>7 && pbObs->grDelay().isAttr(SgVlbiMeasurement::Attr_PROCESSED))
          ionBits = 1 << 0;
        else if (qCodeSb < 1)
        {
          ionBits = 1 << 5;
          // adjust DELUFLAG for observations with QF==0 at the opposite band:
          if ( !pbObs->grDelay().isAttr(SgVlbiMeasurement::Attr_PROCESSED) && 
                pbObs->grDelay().getUnweightFlag()==0)
          {
            pbObs->grDelay().setUnweightFlag(4);
            image->setI2(dDelayUFlag_,  0,0,0, idx, pbObs->grDelay().getUnweightFlag());
          };
        }
        else
          ionBits = 1 << 3;
      };
      //
      if (pbObs->grDelay().getUnweightFlag()==0 && (sbObs || !has2ndBand))
        image->setI2(dUserAction4Suppression_,  0,0,0, idx, -32768);
      else if (pbObs->grDelay().getUnweightFlag()==1)
        image->setI2(dUserAction4Suppression_,  0,0,0, idx, -32767);
      else
        image->setI2(dUserAction4Suppression_,  0,0,0, idx, -32763);
      //
      image->setI2(dIonCode_, 0,0,0, idx, ionCode);
      image->setI2(dIonBits_, 0,0,0, idx, ionBits);
    };
  };
  //
  //
  image->prepare2save(); //
  //
  makeHistory(contemporaryHistory_);
  contemporaryHistory_.export2DbhHistoryBlock(image);
  //

  SgMJD                         finisEpoch(SgMJD::currentMJD());
  logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() +
    "::putDataIntoDbhImage(): data have been stored in DBH database \"" + image->fileName() +
    "\", elapsed time: " + QString("").sprintf("%.2f", (finisEpoch - startEpoch)*86400000.0) + " ms");

  return isOk;
};



//
void SgVlbiSession::makeHistoryIntro(SgVlbiHistory& history)
{
  SgVlbiHistory                 hst;
  //
  // make a copy f already collected records:
  if (history.size())
  {
    for (int i=0; i<history.size(); i++)
      hst.append(new SgVlbiHistoryRecord(*history.at(i)));
    history.clear();
  };
    
  // collect the history records that were generated automatically:
  for (int i=0; i<primaryBand_->history().size(); i++)
  {
    SgVlbiHistoryRecord        *rec=primaryBand_->history().at(i);
    if (rec->getVersion() == primaryBand_->getInputFileVersion() + 1 && !rec->getIsEditable())
      history.addHistoryRecord(rec->getText(), rec->getEpoch());
  };
  //
  // add the copies back: 
  if (hst.size())
  {
    for (int i=0; i<hst.size(); i++)
      history.append(new SgVlbiHistoryRecord(*hst.at(i)));
    hst.clear();
  };

  //
  SgIdentities                 *ids=reporter_->identities();
  history.addHistoryRecord("Analyzed by " + ids->getUserName() + " (hereafter " +
    ids->getUserDefaultInitials() + ") at " + ids->getAcFullName() + ". Contact info: <" + 
    ids->getUserEmailAddress() + ">", SgMJD::currentMJD().toUtc());
  //    ids->getUserDefaultInitials() + ") at " + ids->getAcFullName() + ".");
  //  history.addHistoryRecord("Contact info: <" + ids->getUserEmailAddress() + ">");
  //
  //
  // collect the history records that were added by the user:
  for (int i=0; i<primaryBand_->history().size(); i++)
  {
    SgVlbiHistoryRecord        *rec=primaryBand_->history().at(i);
    // that was added by user, keep content only:
    if (rec->getIsEditable())
      history.addHistoryRecord(ids->getUserDefaultInitials() + "> " + rec->getText(), 
        SgMJD::currentMJD().toUtc());
  };
  //
  //
  // made standard report about the solution:
  if (tFinis_-tStart_ < 3.0/24.0 &&
      parametersDescriptor_->getPolusUT1Mode() != SgParameterCfg::PM_NONE) // it looks like INT session
  {
    //"INT12###/GSFC F-solve update. -SLB-"
    history.addHistoryRecord(officialName_ + "/" + ids->getAcAbbrevName() + " nuSolve update. -" +
      ids->getUserDefaultInitials() + "-", SgMJD::currentMJD().toUtc());
    //"Delays only, 6 parameters, ##.### psec fit,"
    history.addHistoryRecord("Delays only, " + QString("").setNum(reporter_->getNumOfUnknowns()) +
      " parameters, " + QString("").sprintf("%.3f", primaryBand_->wrms(DT_DELAY)*1.0E12) +
      " psec fit,", SgMJD::currentMJD().toUtc());
    //"## obs, ##.## microsec UT1 formal error."
    SgParameter   *p=NULL;
    if (reporter_->parByName()->contains("EOP: dUT1, ms"))
      p = reporter_->parByName()->value("EOP: dUT1, ms");
    history.addHistoryRecord(QString("").setNum(reporter_->getNumOfObservations()) +  " obs, " + 
      QString("").sprintf("%.2f", (p ? p->getSigma()*DAY2SEC*1.0e6 : 999.9)) + 
      " microsec UT1 formal error.", SgMJD::currentMJD().toUtc());
  }
  else // regular session:
  {
    //"r45*: GSFC analysis (SB)"
    history.addHistoryRecord(officialName_ + ": " + ids->getAcAbbrevName() + " analysis (" +
      ids->getUserDefaultInitials() + ")", SgMJD::currentMJD().toUtc());
    //"G-Gxs solution, 60/60 clocks/atms, 2/40 constraints, 24-hr gradients."
    QString     str("G-Gxs solution");
    if (parametersDescriptor_->getClock0Mode() == SgParameterCfg::PM_PWL && 
        parametersDescriptor_->getZenithMode() == SgParameterCfg::PM_PWL  )
    {
      double      clc_int, wzd_int, grd_int, clc_cnt, atm_cnt;
      clc_int = parametersDescriptor_->getClock0().getPwlStep()*24.0*60.0;
      wzd_int = parametersDescriptor_->getZenith().getPwlStep()*24.0*60.0;
      grd_int = parametersDescriptor_->getAtmGrad().getPwlStep();
      if (grd_int > tFinis_-tStart_)
        grd_int = tFinis_-tStart_;
      grd_int *= 24.0*60.0;
      grd_int = round(grd_int)/60.0; // this parameter is in hours
      clc_cnt = parametersDescriptor_->getClock0().getPwlAPriori()
                  /parametersDescriptor_->getClock0().getScale()/86400.0*1.0e14;
      atm_cnt = parametersDescriptor_->getZenith().getPwlAPriori()
                  /parametersDescriptor_->getZenith().getScale()/vLight/24.0*1.0e12;
      str +=
        QString("").sprintf(", %4g/%4g clocks/atms, %4g/%4g constraints",
                            clc_int, wzd_int, clc_cnt, atm_cnt);
      if (parametersDescriptor_->getAtmGradMode() == SgParameterCfg::PM_PWL)
        str += QString("").sprintf(", %4g-hr gradients", grd_int);
    };
    history.addHistoryRecord(str + ".", SgMJD::currentMJD().toUtc());
    //"Solve fit is * psec."
    history.addHistoryRecord("Solve fit is " + 
      QString("").sprintf("%.2f", primaryBand_->wrms(DT_DELAY)*1.0E12) + " psec.", 
      SgMJD::currentMJD().toUtc());
  };
  // history.addHistoryRecord(" ");
  // end of header part of history.
};



//
void SgVlbiSession::makeHistory(SgVlbiHistory& history)
{
  QString                       str;
  
  // Fly-bies:
  history.addHistoryRecord("The following flyby options were used in the solution:", 
    SgMJD::currentMJD().toUtc());
  // stations positions and velocities:
  history.addHistoryRecord("Station positions flyby file: " +
    (config_->getUseExtAPrioriSitesPositions() ? 
      config_->getExtAPrioriSitesPositionsFileName() : "NONE"), SgMJD::currentMJD().toUtc());
  history.addHistoryRecord("Station velocities flyby file: " +
    (config_->getUseExtAPrioriSitesVelocities() ? 
      config_->getExtAPrioriSitesVelocitiesFileName() : "NONE"), SgMJD::currentMJD().toUtc());
  // sources positions:
  history.addHistoryRecord("Source flyby file: " +
    (config_->getUseExtAPrioriSourcesPositions() ? 
      config_->getExtAPrioriSourcesPositionsFileName() : "NONE"), SgMJD::currentMJD().toUtc());
  // stations axis offsets:
  history.addHistoryRecord("Station axis offset flyby file: " +
    (config_->getUseExtAPrioriAxisOffsets() ? 
      config_->getExtAPrioriAxisOffsetsFileName() : "NONE"), SgMJD::currentMJD().toUtc());
  // EOP high freqs:
  history.addHistoryRecord("EOP high frequency variations flyby file: " +
    (config_->getUseExtAPrioriHiFyErp() ? 
      config_->getExtAPrioriHiFyErpFileName() : "NONE"), SgMJD::currentMJD().toUtc());
  // stations mean gradients:
  history.addHistoryRecord("Station mean gradients flyby file: " +
    (config_->getUseExtAPrioriMeanGradients() ? 
      config_->getExtAPrioriMeanGradientsFileName() : "NONE"), SgMJD::currentMJD().toUtc());
  // 
  history.addHistoryRecord("Nutation series flyby file :  NONE", SgMJD::currentMJD().toUtc());
  history.addHistoryRecord("Nutation offset flyby file :  NONE", SgMJD::currentMJD().toUtc());
  history.addHistoryRecord("Earth rotation flyby file  :  " +
    (config_->getUseExtAPrioriErp() ? 
      config_->getExtAPrioriErpFileName() : "NONE"), SgMJD::currentMJD().toUtc());
  history.addHistoryRecord("Tectonic plate motion file :  NONE", SgMJD::currentMJD().toUtc());
  //
  if (isAttr(Attr_FF_AMBIGS_RESOLVED))
    history.addHistoryRecord("Group delay ambiguities were resolved.", 
    SgMJD::currentMJD().toUtc());
  if (isAttr(Attr_FF_OUTLIERS_PROCESSED))
    history.addHistoryRecord("Group delay outliers were processed and editing info has been updated.", 
      SgMJD::currentMJD().toUtc());
  history.addHistoryRecord("Troposphere and cable cal status for standard solution have been stored.", 
    SgMJD::currentMJD().toUtc());
  history.addHistoryRecord("Observation dependent contribution status has been stored.", 
    SgMJD::currentMJD().toUtc());
  // reweighting:
  if (isAttr(Attr_FF_WEIGHTS_CORRECTED))
  {
    history.addHistoryRecord("Reweighting updated for:", SgMJD::currentMJD().toUtc());
    SgVlbiBaselineInfo         *bi=NULL;
    int                         n;
    n = 0;
    str = "";
    for (BaselinesByName_it it=baselinesByName_.begin(); it!=baselinesByName_.end(); ++it)
    {
      bi = it.value();
      if (bi && !bi->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID))
      {
        if (n==10)
        {
          history.addHistoryRecord(str, SgMJD::currentMJD().toUtc());
          n = 0;
          str = "";
        };
        str += bi->getKey().mid(0,2) + "-" + bi->getKey().mid(9,2) + " ";
        n++;
      };
    };
    if (n>0)
      history.addHistoryRecord(str, SgMJD::currentMJD().toUtc());
  };
  // solution type:
  history.addHistoryRecord("Solution data configuration: group delays with ionosphere corrections", 
    SgMJD::currentMJD().toUtc());
  // these are saved by default:
  history.addHistoryRecord("Clock and atmosphere constraint information saved.", 
    SgMJD::currentMJD().toUtc());
  history.addHistoryRecord("Batchmode clock parameterization saved.", 
    SgMJD::currentMJD().toUtc());
  history.addHistoryRecord("Batchmode atmosphere parameterization saved.", 
    SgMJD::currentMJD().toUtc());

  // ionosphere corrections:
  if (isAttr(Attr_FF_ION_C_CALCULATED))
    history.addHistoryRecord("Ionosphere corrections were evaluated for group delays.", 
      SgMJD::currentMJD().toUtc());

  if (primaryBand_->getInputFileVersion()==3)
  {
    // never saw them:
    history.addHistoryRecord("Barometer calibration and barometer height offset not available.", 
      SgMJD::currentMJD().toUtc());
    // no cutoffs, just zeros there:
    history.addHistoryRecord("Elevation cutoff data stored in database.", 
      SgMJD::currentMJD().toUtc());
  };
  // eccentricities:
  if (isAttr(Attr_FF_ECC_UPDATED))
    history.addHistoryRecord("Eccentricity data store in data base header.", 
      SgMJD::currentMJD().toUtc());
};
/*=====================================================================================================*/









/*=====================================================================================================*/
//
// constants:
//


DbhDescriptorX dbhDxTable[] =
{
  //lCode_, description_, d_, isPresent_, type_
  //dimensions_[3], numOfTc_, expectedVersion_, isDimensionsFixed_, isMandatory_
  //
  // First, TOC#0 (== the header):
  // Version #1:
  // R8:
  // I2:
  {"# SITES ", "Number of sites.",                  NULL, true, SgDbhDatumDescriptor::T_I2,  //   0
    { 1, 1, 1}, 0,   1, true,  false},
  {"# STARS ", "Number of radio sources.",          NULL, true, SgDbhDatumDescriptor::T_I2,  //   1
    { 1, 1, 1}, 0,   1, true,  false},
  {"NUMB OBS", "Number of observations in file.",   NULL, true, SgDbhDatumDescriptor::T_I2,  //   2
    { 1, 1, 1}, 0,   1, true,  false},
  // A2:
  {"SITNAMES", "Site names array.",                 NULL, true, SgDbhDatumDescriptor::T_A2,  //   3
    { 4, 6, 1}, 0,   1, false, true },
  {"STRNAMES",  "Source names array",               NULL, true, SgDbhDatumDescriptor::T_A2,  //   4
    { 4,71, 1}, 0,   1, false, true },
  // Version #2:
  // R8:
  {"AXISOFFS", "Axis offsets (m).",                 NULL, true, SgDbhDatumDescriptor::T_R8,  //   5
    { 6, 1, 1}, 0,   2, false, true },
  {"STAR2000", "J2000 source RAs, decs (rd,rd).",   NULL, true, SgDbhDatumDescriptor::T_R8,  //   6
    { 2,71, 1}, 0,   2, false, true },
  {"SITERECS", "Site cartesian coords (m).",        NULL, true, SgDbhDatumDescriptor::T_R8,  //   7
    { 3, 6, 1}, 0,   2, false, true },
  {"SITHOCAM", "Horz ocean loading ampltudes (m)",  NULL, true, SgDbhDatumDescriptor::T_R8,  //   8
    {11, 2, 6}, 0,   2, false, false},
  {"SITHOCPH", "Horz ocean loading phases (rad).",  NULL, true, SgDbhDatumDescriptor::T_R8,  //   9
    {11, 2, 6}, 0,   2, false, false},
  {"SITOCAMP", "Vert ocean loading ampltudes (m)",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  10
    {11, 6, 1}, 0,   2, false, false},
  {"SITOCPHS", "Vert ocean loading phases (rad).",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  11
    {11, 6, 1}, 0,   2, false, false},
  // I2:
  {"AXISTYPS", "Axis type (1-eq,2-xy,3-azel,4,5)",  NULL, true, SgDbhDatumDescriptor::T_I2,  //  12
    { 6, 1, 1}, 0,   2, false, true },
  // Version #3:
  // A2:
  {"CBL SIGN", "Signs of cable cal application",    NULL, true, SgDbhDatumDescriptor::T_A2,  //  13
    { 6, 1, 1}, 0,   3, false, false},
  {"CBL STAT", "Stations for cable sign",           NULL, true, SgDbhDatumDescriptor::T_A2,  //  14
    { 4, 6, 1}, 0,   3, false, false},
  // Version #4:
  // R8:
  {"ATM_CNST", "Atmosphere constraint. ps/hr",      NULL, true, SgDbhDatumDescriptor::T_R8,  //  15
    { 1, 1, 1}, 0,   4, false, false},
  {"ATM_INTV", "Batchmode atmos interval - hours",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  16
    { 1, 1, 1}, 0,   4, true,  false},
  {"CLK_CNST", "Clock constraint-Parts in 1.e14",   NULL, true, SgDbhDatumDescriptor::T_R8,  //  17
    { 1, 1, 1}, 0,   4, false, false},
  {"CLK_INTV", "Batchmode clock interval - hours",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  18
    { 1, 1, 1}, 0,   4, true,  false},
  {"CLOOFACM", "A priori clock offset (sec)",       NULL, true, SgDbhDatumDescriptor::T_R8,  //  19
    { 4, 1, 1}, 0,   4, false, false},
  {"CLODRACM", "A priori clock drift (sec/sec)",    NULL, true, SgDbhDatumDescriptor::T_R8,  //  20
    { 4, 1, 1}, 0,   4, false, false},
  {"ERROR K ", "Formal error constants",            NULL, true, SgDbhDatumDescriptor::T_R8,  //  21
    { 2,15, 1}, 0,   4, false, false},
  {"ECCCOORD", "Eccentricity coords. (m)     ",     NULL, true, SgDbhDatumDescriptor::T_R8,  //  22
    { 3, 6, 1}, 0,   4, false, false},
  // I2:
  {"# CLK_RF", "# of clock reference stations",     NULL, true, SgDbhDatumDescriptor::T_I2,  //  23
    { 1, 1, 1}, 0,   4, true,  false},
  {"BASLSTAT", "Baselines selection status",        NULL, true, SgDbhDatumDescriptor::T_I2,  //  24
    {36, 1, 1}, 0,   4, false, false},
  {"SOURSTAT", "Sources selection status",          NULL, true, SgDbhDatumDescriptor::T_I2,  //  25
    {71, 1, 1}, 0,   4, false, false},
  // A2:
  {"BLDEPCKS", "Bl-dependent clock list",           NULL, true, SgDbhDatumDescriptor::T_A2,  //  26
    { 8, 3, 1}, 0,   4, false, false},
  {"CLK_SITE", "Batchmode clock reference site  ",  NULL, true, SgDbhDatumDescriptor::T_A2,  //  27
    { 4, 1, 1}, 0,   4, false, false},
  {"CLK_SITS", "List of clock reference stations",  NULL, true, SgDbhDatumDescriptor::T_A2,  //  28
    { 4, 1, 1}, 0,   4, false, false},
  {"CLKBREAK", "Status of clock break existence ",  NULL, true, SgDbhDatumDescriptor::T_A2,  //  29
    { 1, 1, 1}, 0,   4, true,  false},
  {"ECCNAMES", "Ecc. site and monument names.",     NULL, true, SgDbhDatumDescriptor::T_A2,  //  30
    { 9, 6, 1}, 0,   4, false, false},
  {"ECCTYPES", "Eccentricy types - XY or NE.",      NULL, true, SgDbhDatumDescriptor::T_A2,  //  31
    { 6, 1, 1}, 0,   4, false, false},
  {"EL-CUT  ", "Solution elevation cutoff data. ",  NULL, true, SgDbhDatumDescriptor::T_A2,  //  32
    { 8, 1, 1}, 0,   4, true, false},
  {"ERROR BL", "B.L.names for formal errors",       NULL, true, SgDbhDatumDescriptor::T_A2,  //  33
    { 4, 2,15}, 0,   4, false, false},
  // J4:
  {"NUM4 OBS", "Number of observations (I*4)    ",  NULL, true, SgDbhDatumDescriptor::T_J4,  //  34
    { 1, 1, 1}, 0,   4, true,  false},

  //
  //
  // Other TOCs (== observ.record):
  // Version #1:
  // R8:
  {"#SAMPLES", "# of samples by sideband and cha",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  35
    { 2,16, 1}, 1,   1, true,  false},
  {"AMPBYFRQ", "Amp(0-1), phs by chan(-180to180)",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  36
    { 2,16, 1}, 1,   1, true,  false},
  {"COHERCOR", "Corr coeff (0 --> 1)............",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  37
    { 1, 1, 1}, 1,   1, true,  true },
  {"DEL OBSV", "Observed delay us in 2 parts....",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  38
    { 2, 1, 1}, 1,   1, true,  true },
  {"DELSIGMA", "Delay err (sec).................",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  39
    { 1, 1, 1}, 1,   1, true,  true },
  {"GPDLAMBG", "Group delay ambiguity (sec).....",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  40
    { 1, 1, 1}, 1,   1, true,  true },
  {"RAT OBSV", "Observd delay rate (sec per sec)",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  41
    { 1, 1, 1}, 1,   1, true,  true },
  {"RATSIGMA", "Rate formal error(sec per sec)..",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  42
    { 1, 1, 1}, 1,   1, true,  true },
  {"RECSETUP", "Samp rate(kHz),Frames/PP,PP/AP..",  NULL, true, SgDbhDatumDescriptor::T_I2,  //  43
    { 1, 1, 1}, 1,   1, true,  false},
  {"REF FREQ", "Freq to which phase is referred.",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  44
    { 1, 1, 1}, 1,   1, true,  true },
  {"RFREQ   ", "RF freq by channel (MHz)........",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  45
    {16, 1, 1}, 1,   1, true,  true },
  {"SAMPLRAT", "Sample rate (Hz)................",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  46
    { 1, 1, 1}, 1,   1, true,  false},
  {"SB DELAY", "Single band delay (microsec)....",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  47
    { 1, 1, 1}, 1,   1, true,  true },
  {"SB SIGMA", "SB delay error microseconds.....",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  48
    { 1, 1, 1}, 1,   1, true,  true },
  {"SEC TAG ", "Seconds part of UTC TAG.........",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  49
    { 1, 1, 1}, 1,   1, true,  true },
  {"SNRATIO ", "Signal to noise  ratio..........",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  50
    { 1, 1, 1}, 1,   1, true,  true },
  // I2:
  {"#CHANELS", "No. of U-L pairs in integration.",  NULL, true, SgDbhDatumDescriptor::T_I2,  //  51
    { 1, 1, 1}, 1,   1, true,  true },
  {"CALBYFRQ", "PC amp,phs,frq by sta,channel...",  NULL, true, SgDbhDatumDescriptor::T_I2,  //  52
    { 3, 2,16}, 1,   1, true,  false},
  {"DELUFLAG", "Delay unweight flag             ",  NULL, true, SgDbhDatumDescriptor::T_I2,  //  53
    { 1, 1, 1}, 1,   1, true,  false},
  {"NO.OF AP", "# of AP by sideband and channel.",  NULL, true, SgDbhDatumDescriptor::T_I2,  //  54
    { 2,16, 1}, 1,   1, true,  true },
  {"PHCALOFF", "Phase cal offset (-18000/18000).",  NULL, true, SgDbhDatumDescriptor::T_I2,  //  55
    { 2,16, 1}, 1,   1, true,  false},
  {"RATUFLAG", "Delay rate unweight flag........",  NULL, true, SgDbhDatumDescriptor::T_I2,  //  56
    { 1, 1, 1}, 1,   1, true,  false},
  {"UTC TAG ", "Epoch UTC YMDHM.................",  NULL, true, SgDbhDatumDescriptor::T_I2,  //  57
    { 5, 1, 1}, 1,   1, true,  true },
  // A2:
  {"BASELINE", "Ref and rem site names..........",  NULL, true, SgDbhDatumDescriptor::T_A2,  //  58
    { 4, 2, 1}, 1,   1, true,  true },
  {"QUALCODE", "FRNGE quality  index 0 --> 9....",  NULL, true, SgDbhDatumDescriptor::T_A2,  //  59
    { 1, 1, 1}, 1,   1, true,  true },
  {"STAR ID ", "Radio source name...............",  NULL, true, SgDbhDatumDescriptor::T_A2,  //  60
    { 4, 1, 1}, 1,   1, true,  true },

  // Version #2:
  // R8:
  {"AZ-THEO ", "Azimuth array definition        ",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  61
    { 2, 2, 1}, 1,   2, true,  true },
  {"CON CONT", "Consensus bending contrib. (sec)",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  62
    { 2, 1, 1}, 1,   2, true,  false },
  {"CONSNDEL", "Consensus theo. delay (microsec)",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  63
    { 2, 1, 1}, 1,   2, true,  false },//true },
  {"EL-THEO ", "Elevation array definition      ",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  64
    { 2, 2, 1}, 1,   2, true,  true },
  {"NUT PART", "Nutation partial derive. def.   ",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  65
    { 2, 2, 1}, 1,   2, true,  false}, 
  {"NUT2KXYP", "IAU2000A Nutation X,Y Partials  ",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  66
    { 2, 2, 1}, 1,   2, true,  false},
  {"OCE CONT", "Ocean loading contributions def.",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  67
    { 2, 1, 1}, 1,   2, true,  true },
  {"SIT PART", "Site partial derivative def.    ",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  68
    { 3, 2, 2}, 1,   2, true,  true },
  {"STR PART", "Star partial derivatives def.   ",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  69
    { 2, 2, 1}, 1,   2, true,  true },
  {"UT1 PART", "UT1 partial derivatives def.    ",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  70
    { 2, 2, 1}, 1,   2, true,  true },
  {"WOB PART", "Wobble partial derivatives def. ",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  71
    { 2, 2, 1}, 1,   2, true,  true },
  // I2:
  // A2:

  // Version #3:
  // R8:
  {"ATM PRES", "Pressure in mb at site          ",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  72
    { 2, 2, 1}, 1,   3, true,  false},
  {"CABL DEL", "Cable calibration data          ",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  73
    { 2, 2, 1}, 1,   3, true,  false},
  {"REL.HUM.", "Rel.Hum. at local WX st (50%=.5)",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  74
    { 2, 2, 1}, 1,   3, true,  false},
  {"TEMP C  ", "Temp in C at local WX station   ",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  75
    { 2, 2, 1}, 1,   3, true,  false},
  // I2:
  // A2:

  // Version #4:
  // R8:
  {"GRIONFRQ", "Effective ion.frq for gr.del MHz",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  76
    { 1, 1, 1}, 1,   4, true,  false},
  {"ION CORR", "Ion correction. Add to theo. sec",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  77
    { 2, 1, 1}, 1,   4, true,  false},
  {"IONRMS  ", "Ion correction to sigma. sec    ",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  78
    { 2, 1, 1}, 1,   4, true,  false},
  {"PHIONFRQ", "Effective ion.frq for ph.del MHz",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  79
    { 1, 1, 1}, 1,   4, true,  false},
  // I2:
  {"# AMBIG ", "No. of ambiguities",                NULL, true, SgDbhDatumDescriptor::T_I2,  //  80
    { 1, 1, 1}, 1,   4, true,  false},
  // A2:

  // +...
  // to be re-sorted:
  {"ATM_CFLG", "Atmosphere constraint use flag  ",  NULL, true, SgDbhDatumDescriptor::T_A2,  //  81
    { 1, 1, 1}, 0,   4, true,  false},
  {"CLK_CFLG", "Clock constraint use flag.      ",  NULL, true, SgDbhDatumDescriptor::T_A2,  //  82
    { 1, 1, 1}, 0,   4, true,  false},
  {"BRK_EPOC", "Batchmode clock break epochs",      NULL, true, SgDbhDatumDescriptor::T_R8,  //  83
    { 2, 1, 1}, 0,   4, false, false},
  {"BRK_NUMB", "Number of batchmode clock breaks",  NULL, true, SgDbhDatumDescriptor::T_I2,  //  84
    { 1, 1, 1}, 0,   4, true,  false},
  {"BRK_FLAG", "Batchmode clock break flags",       NULL, true, SgDbhDatumDescriptor::T_I2,  //  85
    { 2, 1, 1}, 0,   4, false, false},
  {"BRK_SNAM", "Batchmode clock break stations",    NULL, true, SgDbhDatumDescriptor::T_A2,  //  86
    { 4, 2, 1}, 0,   4, false, false},
  {"ION CODE", "Ion corr code. -1=no good, 0=OK ",  NULL, true, SgDbhDatumDescriptor::T_I2,  //  87
    { 1, 1, 1}, 1,   4, true,  false},
  {"#GAMBG_S", "No. of ambiguities for S-band",     NULL, true, SgDbhDatumDescriptor::T_I2,  //  88
    { 1, 1, 1}, 1,   4, true,  false},
  {"QCODE XS", "Quality code, opposite band",       NULL, true, SgDbhDatumDescriptor::T_A2,  //  89
    { 1, 1, 1}, 1,   4, true,  false},

  {"EFF FREQ", "Effective freqs, opposite band  ",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  90
    { 3, 1, 1}, 1,   4, true,  false},
  {"RTOBS XS", "Rate observable, opposite band  ",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  91
    { 1, 1, 1}, 1,   4, true,  false},
  {"RTERR XS", "Rate formal err, opposite band  ",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  92
    { 1, 1, 1}, 1,   4, true,  false},
  {"DLOBS XS", "Delay observable, opposite band ",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  93
    { 1, 1, 1}, 1,   4, true,  false},
  {"DLERR XS", "Delay formal err, opposite band ",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  94
    { 1, 1, 1}, 1,   4, true,  false},
  {"DPHAS XS", "Phase observable, opposite band ",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  95
    { 1, 1, 1}, 1,   4, true,  false},
  {"DPHER XS", "Phase formal err, opposite band ",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  96
    { 1, 1, 1}, 1,   4, true,  false},
  {"TOTPHA_S", "S-band Total phase (in degrees) ",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  97
    { 1, 1, 1}, 1,   4, true,  false},
  {"REF FR_S", "Fr to w.phase is referred S-band",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  98
    { 1, 1, 1}, 1,   4, true,  false},
  {"SNR_S   ", "Signal to noise ratio for S-band",  NULL, true, SgDbhDatumDescriptor::T_R8,  //  99
    { 1, 1, 1}, 1,   4, true,  false},
  {"COHCOR_S", "Corr coeff (0 --> 1) for S-band ",  NULL, true, SgDbhDatumDescriptor::T_R8,  // 100
    { 1, 1, 1}, 1,   4, true,  false},
  {"SB DEL_S", "Single band delay for S-band    ",  NULL, true, SgDbhDatumDescriptor::T_R8,  // 101
    { 1, 1, 1}, 1,   4, true,  false},
  {"SB SIG_S", "SB delay error S-band, mks      ",  NULL, true, SgDbhDatumDescriptor::T_R8,  // 102
    { 1, 1, 1}, 1,   4, true,  false},
  {"GRPAMB_S", "Group delay ambiguity for S-band",  NULL, true, SgDbhDatumDescriptor::T_R8,  // 103
    { 1, 1, 1}, 1,   4, true,  false},
  {"TOTPHASE", "Total phase degrees mod 360.....",  NULL, true, SgDbhDatumDescriptor::T_R8,  // 104
    { 1, 1, 1}, 1,   1, true, true},
  
  //addons:
  //lCode_, description_, d_, isPresent_, type_
  //dimensions_[3], numOfTc_, expectedVersion_, isDimensionsFixed_, isMandatory_
  {"BATCHCNT", "Batchmode parms use flag.",         NULL, true, SgDbhDatumDescriptor::T_A2,  // 105
    { 2, 1, 1}, 0,   4, true,  false},
  {"CORRTYPE", "Correlator type: MK3/MK4/K4 etc.",  NULL, true, SgDbhDatumDescriptor::T_A2,  // 106
    { 4, 1, 1}, 0,   1, true,  false},
  {"STAT_ACM", "Stations with a priori clock mod",  NULL, true, SgDbhDatumDescriptor::T_A2,  // 107
    { 4, 4, 1}, 0,   4, false, false},
  {"CALC VER", "CALC version number",               NULL, true, SgDbhDatumDescriptor::T_R8,  // 108
    { 1, 1, 1}, 0,   2, true,  false},
  {"TECTPLNM", "4-char tectonic plate names.",      NULL, true, SgDbhDatumDescriptor::T_A2,  // 109
    { 2, 6, 1}, 0,   2, false, false},
  {"UT1ORTHO", "ORTHO_EOP Tidal UT1 contribution",  NULL, true, SgDbhDatumDescriptor::T_R8,  // 110
    { 2, 1, 1}, 1,   2, true,  false},
  {"WOBORTHO", "ORTHO_EOP tidal wobble contribtn",  NULL, true, SgDbhDatumDescriptor::T_R8,  // 111
    { 2, 1, 1}, 1,   2, true,  false},

  //contributions:
  //lCode_, description_, d_, isPresent_, type_
  //dimensions_[3], numOfTc_, expectedVersion_, isDimensionsFixed_, isMandatory_
  {"PTD CONT", "Pole tide contributions def.",      NULL, true, SgDbhDatumDescriptor::T_R8,  // 112
    { 2, 1, 1}, 1,   2, true,  false},
  {"ETD CONT", "Earth tide contributions def.",     NULL, true, SgDbhDatumDescriptor::T_R8,  // 113
    { 2, 1, 1}, 1,   2, true,  false},
  {"WOBXCONT", "X Wobble contribution definition",  NULL, true, SgDbhDatumDescriptor::T_R8,  // 114
    { 2, 1, 1}, 1,   2, true,  false},
  {"WOBYCONT", "Y Wobble contribution definition",  NULL, true, SgDbhDatumDescriptor::T_R8,  // 115
    { 2, 1, 1}, 1,   2, true,  false},
  {"WOBNUTAT", "Short period nutation wobble con",  NULL, true, SgDbhDatumDescriptor::T_R8,  // 116
    { 2, 1, 1}, 1,   2, true,  false},
  {"FEED.COR", "Feedhorn corr. in CORFIL scheme",   NULL, true, SgDbhDatumDescriptor::T_R8,  // 117
    { 2, 1, 1}, 1,   2, true,  false},
  {"TILTRMVR", "Axis Tilt Contribution Remover",    NULL, true, SgDbhDatumDescriptor::T_R8,  // 118
    { 2, 1, 1}, 1,   2, true,  false},
  {"NDRYCONT", "Nhmf (dry) atm. contribution",      NULL, true, SgDbhDatumDescriptor::T_R8,  // 119
    { 2, 2, 1}, 1,   2, true,  false},
  {"NWETCONT", "Whmf (wet) atm. contribution",      NULL, true, SgDbhDatumDescriptor::T_R8,  // 120
    { 2, 2, 1}, 1,   2, true,  false},
  {"NGRADPAR", "Niell dry atm. gradient partials",  NULL, true, SgDbhDatumDescriptor::T_R8,  // 121
    { 2, 2, 2}, 1,   2, true,  false},
  {"UNPHASCL", "UnPhaseCal effect - group&rate",    NULL, true, SgDbhDatumDescriptor::T_R8,  // 122
    { 2, 2, 1}, 1,   2, true,  false},
  {"AXO CONT", "New Axis Offset Contributions",     NULL, true, SgDbhDatumDescriptor::T_R8,  // 123
    { 2, 2, 1}, 1,   2, true,  false},

  //contributions' flags:
  //lCode_, description_, d_, isPresent_, type_
  //dimensions_[3], numOfTc_, expectedVersion_, isDimensionsFixed_, isMandatory_
  {"CALSITES", "List of sites for standard cal",    NULL, true, SgDbhDatumDescriptor::T_A2,  // 124
    { 4, 1, 1}, 0,   4, false,  false},
  {"CAL LIST", "Key to the standard cal config",    NULL, true, SgDbhDatumDescriptor::T_A2,  // 125
    { 4, 6, 1}, 0,   4, false,  false},
  {"CAL FLGS", "Standard cal configuration",        NULL, true, SgDbhDatumDescriptor::T_I2,  // 126
    { 1, 1, 1}, 0,   4, false,  false},
  // partials:
  {"AXO PART", "Axis Offset partial deriv. def.",   NULL, true, SgDbhDatumDescriptor::T_R8,  // 127
    { 2, 2, 1}, 1,   2, true,  false},
  //
  {"TAI- UTC", "FJD,TAI-UTC (sec),rate off.(sec)",  NULL, true, SgDbhDatumDescriptor::T_R8,  // 128
    { 3, 1, 1}, 0,   2, true,  false},
  {"FOURFFIL", "Fourfit output filename.",          NULL, true, SgDbhDatumDescriptor::T_A2,  // 129
    { 8, 1, 1}, 1,   1, true,  false},
  {"FOURFFXS", "Fourfit output filename S-band",    NULL, true, SgDbhDatumDescriptor::T_A2,  // 130
    { 8, 1, 1}, 2,   4, true,  false},
  {"ION_BITS", "ICORR for full ion tracking.",      NULL, true, SgDbhDatumDescriptor::T_I2,  // 131
    { 1, 1, 1}, 1,   4, true,  false},
  {"UACSUP  ", "User action for suppression",       NULL, true, SgDbhDatumDescriptor::T_I2,  // 132
    { 1, 1, 1}, 1,   4, true,  false},
  //
  {"FUT1 INF", "Final Value TAI-UT1 array descrp",  NULL, true, SgDbhDatumDescriptor::T_R8,  // 133
    { 4, 1, 1}, 0,   2, false, false},
  {"FUT1 PTS", "Final Value TAI-UT1 data points.",  NULL, true, SgDbhDatumDescriptor::T_R8,  // 134
    {15, 1, 1}, 0,   2, false, false},
  {"FWOB INF", "Final Value wobble array descr.",   NULL, true, SgDbhDatumDescriptor::T_R8,  // 135
    { 3, 1, 1}, 0,   2, false, false},
  {"FWOBX&YT", "Final wobble X,Y component value",  NULL, true, SgDbhDatumDescriptor::T_R8,  // 136
    { 2,15, 1}, 0,   2, false, false},
  {"TIDALUT1", "Flag for tidal terms in UT1 sers",  NULL, true, SgDbhDatumDescriptor::T_I2,  // 137
    { 1, 1, 1}, 0,   2, true, false},
  {"#PAMBG_S", "# phase ambiguities for S-band",    NULL, true, SgDbhDatumDescriptor::T_J4,  // 138
    { 1, 1, 1}, 1,   4, true, false},
  //
  //lCode_, description_, d_, isPresent_, type_
  //dimensions_[3], numOfTc_, expectedVersion_, isDimensionsFixed_, isMandatory_
  {"SOL_DATA", "Stnd sol data. IDATYP from SOCOM",  NULL, true, SgDbhDatumDescriptor::T_I2,  // 139
    { 1, 1, 1}, 0,   4, true, false},
  {"SUPMET  ", "Suppression method",                NULL, true, SgDbhDatumDescriptor::T_I2,  // 140
    { 1, 1, 1}, 0,   4, true, false},
  {"BLDEPCKS", "Bl-dependent clock list",           NULL, true, SgDbhDatumDescriptor::T_A2,  // 141
    { 8, 6, 1}, 0,   4, false, false},
  {"PRT FLAG", "Atmosphere partial to be applied",  NULL, true, SgDbhDatumDescriptor::T_I2,  // 142
    { 1, 1, 1}, 0,   4, true, false},
  {"FCL LIST", "Key to the standard flcal config",  NULL, true, SgDbhDatumDescriptor::T_A2,  // 143
    { 4, 8, 1}, 0,   4, false, false},
  {"FCL FLGS", "Standard flcal configuration",      NULL, true, SgDbhDatumDescriptor::T_I2,  // 144
    { 7, 2, 1}, 0,   4, false, false},
  {"OBCLLIST", "Key to standard contribs config",   NULL, true, SgDbhDatumDescriptor::T_A2,  // 145
    { 4,12, 1}, 0,   4, false, false},
  {"OBCLFLGS", "Standard contribs configuration",   NULL, true, SgDbhDatumDescriptor::T_I2,  // 146
    { 1, 1, 1}, 0,   4, true,  false},
  {"SCANNAME", "Scan name ......................",  NULL, true, SgDbhDatumDescriptor::T_A2,  // 147
    { 5, 1, 1}, 1,   1, true,  false},
  {"RUN CODE", "Run,code, e.g., \"329-1300\"......",NULL, true, SgDbhDatumDescriptor::T_A2,  // 148
    { 4, 1, 1}, 1,   1, true,  false},
  {"OCE HORZ", "Site-dep ocean cont - horizontal",  NULL, true, SgDbhDatumDescriptor::T_R8,  // 149
    { 2, 2, 1}, 1,   2, true,  false },
  {"OCE VERT", "Site-dep ocean cont - vertical",    NULL, true, SgDbhDatumDescriptor::T_R8,  // 150
    { 2, 2, 1}, 1,   2, true,  false },

  {"WOBLIBRA", "High freq libration wobble contr",  NULL, true, SgDbhDatumDescriptor::T_R8,  // 151
    { 2, 1, 1}, 1,   2, true,  false },
  {"UT1LIBRA", "UT1 Libration contribution.",       NULL, true, SgDbhDatumDescriptor::T_R8,  // 152
    { 2, 1, 1}, 1,   2, true,  false },
  {"NUT06XYS", "2000/2006 Nut/Prec X,Y,S & Rates",  NULL, true, SgDbhDatumDescriptor::T_R8,  // 153
    { 3, 2, 1}, 1,   2, true,  false },
  {"NUT06XYP", "2000/2006 Nut/Prec X,Y Partials",   NULL, true, SgDbhDatumDescriptor::T_R8,  // 154
    { 2, 2, 1}, 1,   2, true,  false },
  {"NUT WAHR", "Wahr nut vals  - Dpsi,Deps&rates",  NULL, true, SgDbhDatumDescriptor::T_R8,  // 155
    { 2, 2, 1}, 1,   2, true,  false },
  {"NUT2006A", "2000/2006 Nut/Prec psi/epsilon",    NULL, true, SgDbhDatumDescriptor::T_R8,  // 156
    { 2, 2, 1}, 1,   2, true,  false },
  {"OPTLCONT", "Ocean Pole Tide Load Contributn",   NULL, true, SgDbhDatumDescriptor::T_R8,  // 157
    { 2, 1, 1}, 1,   2, true,  false },
  {"AC_SITES", "Site list for atmos constraints",   NULL, true, SgDbhDatumDescriptor::T_A2,  // 158
    { 4, 1, 1}, 0,   4, false, false},
  {"CC_SITES", "Site list for clocks constraints",  NULL, true, SgDbhDatumDescriptor::T_A2,  // 159
    { 4, 1, 1}, 0,   4, false, false},
  //
  {"OCE_OLD ", "Old Ocean loading contribution.",   NULL, true, SgDbhDatumDescriptor::T_R8,  // 160
    { 2, 1, 1}, 1,   2, true,  false},
  {"PTOLDCON", "Old Pole Tide Restorer Contrib.",   NULL, true, SgDbhDatumDescriptor::T_R8,  // 161
    { 2, 1, 1}, 1,   2, true,  false},
  {"FRNGERR ", "Fourfit error flag blank=OK.....",  NULL, true, SgDbhDatumDescriptor::T_A2,  // 162
    { 1, 1, 1}, 1,   1, true,  false},
  //
  {"UTC_ACM ", "Epochs YMDHMS f/ apriori clk mod",  NULL, false,SgDbhDatumDescriptor::T_I2,  // 163
    { 6, 1, 1}, 0,   5, true,  false},
  {"NUM_ACM ", "# of stations w/ apriori clk mod",  NULL, false,SgDbhDatumDescriptor::T_I2,  // 164
    { 1, 1, 1}, 0,   5, true,  false},

  {"PARANGLE", "Feedhorn rot. angle, ref and rem",  NULL, false,SgDbhDatumDescriptor::T_R8,  // 165
    { 2, 1, 1}, 1,   2, true,  false},
  //
  //
  {"CONSNRAT", "Consensus theo. rate (sec/sec)  ",  NULL, true, SgDbhDatumDescriptor::T_R8,  // 166
    { 1, 1, 1}, 1,   2, true,  true },
  {"NDRYPART", "Nhmf2 dry partial deriv. def.   ",  NULL, true, SgDbhDatumDescriptor::T_R8,  // 167
    { 2, 2, 1}, 1,   2, true,  true },
  {"NWETPART", "Whmf2 wet partial deriv. def.   ",  NULL, true, SgDbhDatumDescriptor::T_R8,  // 168
    { 2, 2, 1}, 1,   2, true,  true },
  {"SUN CONT", "Consensus bending contrib. (sec)",  NULL, true, SgDbhDatumDescriptor::T_R8,  // 169
    { 2, 1, 1}, 1,   2, true,  true },
  {"SUN2CONT", "High order bending contrib.(sec)",  NULL, true, SgDbhDatumDescriptor::T_R8,  // 170
    { 2, 1, 1}, 1,   2, true,  true },

  {"MSLM ION", "MSLM/JPL GPS Iono Correctn (sec)",  NULL, false,SgDbhDatumDescriptor::T_R8,  // 171
    { 2, 1, 1}, 1,   4, true,  false},

  //lCode_, description_, d_, isPresent_, type_
  //dimensions_[3], numOfTc_, expectedVersion_, isDimensionsFixed_, isMandatory_
  //
  //  //
  //  {"GROBSDEL", "Observed group delay in sec.....",  NULL, true, SgDbhDatumDescriptor::T_R8,  // 151
  //    { 1, 1, 1}, 1,   1, true,  false },

  // DO WE NEED THIS?
  //
  //MCALSTAT [  2,  1,  1]   4  335 I2 Mode calibrations status
  //"MCALSTAT" (aka "Mode calibrations status") [2,1,1] =
  //(
  //   ((0, 0))
  //);
  //                           + :
  //"MCALNAMS" (aka "Mode calibrations names") [4,15,1] =
  //(
  //   ("SOURSTRU", "SPURPCAL", "USERMCAL", "        ", "        ", "        ", "        ", "        ",
  //    "        ", "        ", "        ", "        ", "        ", "        ", "        ")
  //);
  //
  //134. AUTO_SUP [  1,  1,  1]   4    2 J4 Automatic suppression status
  //135. USER_SUP [  1,  1,  1]   4    3 J4 User observation suppression sts
  //136. USER_REC [  1,  1,  1]   4    4 J4 User observation recovery status
};

// number of the records:
const int numOfDxT = sizeof(dbhDxTable)/sizeof(DbhDescriptorX);

// individual records:
DbhDescriptorX         &rNSites_                = dbhDxTable[  0];
DbhDescriptorX         &rNStars_                = dbhDxTable[  1];
DbhDescriptorX         &rNObs_                  = dbhDxTable[  2];
DbhDescriptorX         &rSiteNames_             = dbhDxTable[  3];
DbhDescriptorX         &rStarNames_             = dbhDxTable[  4];
DbhDescriptorX         &rAxisOffsets_           = dbhDxTable[  5];
DbhDescriptorX         &rStar2000_              = dbhDxTable[  6];
DbhDescriptorX         &rSiteRecs_              = dbhDxTable[  7];
DbhDescriptorX         &rOloadAmpHorz_          = dbhDxTable[  8];
DbhDescriptorX         &rOloadPhsHorz_          = dbhDxTable[  9];
DbhDescriptorX         &rOloadAmpVert_          = dbhDxTable[ 10];
DbhDescriptorX         &rOloadPhsVert_          = dbhDxTable[ 11];
DbhDescriptorX         &rAxisTypes_             = dbhDxTable[ 12];
DbhDescriptorX         &rCableSigns_            = dbhDxTable[ 13];
DbhDescriptorX         &rCableStations_         = dbhDxTable[ 14];
DbhDescriptorX         &rAtmConstr_             = dbhDxTable[ 15];
DbhDescriptorX         &rAtmIntrv_              = dbhDxTable[ 16];
DbhDescriptorX         &rClockConstr_           = dbhDxTable[ 17];
DbhDescriptorX         &rClockIntrv_            = dbhDxTable[ 18];
DbhDescriptorX         &rAcmOffsets_            = dbhDxTable[ 19];
DbhDescriptorX         &rAcmRates_              = dbhDxTable[ 20];
DbhDescriptorX         &rError_K_               = dbhDxTable[ 21];
DbhDescriptorX         &rEccCoords_             = dbhDxTable[ 22];
DbhDescriptorX         &rNRefClocks_            = dbhDxTable[ 23];
DbhDescriptorX         &rBaselinesSelStatus_    = dbhDxTable[ 24];
DbhDescriptorX         &rSourcesSelStatus_      = dbhDxTable[ 25];
DbhDescriptorX         &rBaselinesDepClocks_    = dbhDxTable[ 26];
DbhDescriptorX         &rClockSite_             = dbhDxTable[ 27];
DbhDescriptorX         &rRefClockSites_         = dbhDxTable[ 28];
DbhDescriptorX         &rClockBreakStatus_      = dbhDxTable[ 29];
DbhDescriptorX         &rEccNames_              = dbhDxTable[ 30];
DbhDescriptorX         &rEccTypes_              = dbhDxTable[ 31];
DbhDescriptorX         &rEl_Cut_                = dbhDxTable[ 32];
DbhDescriptorX         &rError_Bl_              = dbhDxTable[ 33];
DbhDescriptorX         &rN4Obs_                 = dbhDxTable[ 34];
DbhDescriptorX         &rNumOfSamples_          = dbhDxTable[ 35];
DbhDescriptorX         &rAmpByFrq_              = dbhDxTable[ 36];
DbhDescriptorX         &rCorrCoef_              = dbhDxTable[ 37];
DbhDescriptorX         &rObservedDelay_         = dbhDxTable[ 38];
DbhDescriptorX         &rObservedDelaySigma_    = dbhDxTable[ 39];
DbhDescriptorX         &rGrpAmbiguity_          = dbhDxTable[ 40];
DbhDescriptorX         &rDelayRate_             = dbhDxTable[ 41];
DbhDescriptorX         &rSigmaDelayRate_        = dbhDxTable[ 42];
DbhDescriptorX         &rRecSetup_              = dbhDxTable[ 43];
DbhDescriptorX         &rReferenceFrequency_    = dbhDxTable[ 44];
DbhDescriptorX         &rRefFreqByChannel_      = dbhDxTable[ 45];
DbhDescriptorX         &rSampleRate_            = dbhDxTable[ 46];
DbhDescriptorX         &rSbDelay_               = dbhDxTable[ 47];
DbhDescriptorX         &rSbSigmaDelay_          = dbhDxTable[ 48];
DbhDescriptorX         &rSecTag_                = dbhDxTable[ 49];
DbhDescriptorX         &rSnr_                   = dbhDxTable[ 50];
DbhDescriptorX         &rNumOfChannels_         = dbhDxTable[ 51];
DbhDescriptorX         &rCalByFrq_              = dbhDxTable[ 52];
DbhDescriptorX         &rDelayUFlag_            = dbhDxTable[ 53];
DbhDescriptorX         &rNumOfAccPeriods_       = dbhDxTable[ 54];
DbhDescriptorX         &rPhaseCalOffset_        = dbhDxTable[ 55];
DbhDescriptorX         &rRateUFlag_             = dbhDxTable[ 56];
DbhDescriptorX         &rUtcTag_                = dbhDxTable[ 57];
DbhDescriptorX         &rBaselineName_          = dbhDxTable[ 58];
DbhDescriptorX         &rQualityCode_           = dbhDxTable[ 59];
DbhDescriptorX         &rSourceName_            = dbhDxTable[ 60];
DbhDescriptorX         &rAzimuths_              = dbhDxTable[ 61];
DbhDescriptorX         &rConsBendingDelay_      = dbhDxTable[ 62];
DbhDescriptorX         &rConsensusDelay_        = dbhDxTable[ 63];
DbhDescriptorX         &rElevations_            = dbhDxTable[ 64];
DbhDescriptorX         &rNutationPart_          = dbhDxTable[ 65];
DbhDescriptorX         &rNutationPart2K_        = dbhDxTable[ 66];
DbhDescriptorX         &rOloadContrib_          = dbhDxTable[ 67];
DbhDescriptorX         &rSitPart_               = dbhDxTable[ 68];
DbhDescriptorX         &rStarPart_              = dbhDxTable[ 69];
DbhDescriptorX         &rUT1Part_               = dbhDxTable[ 70];
DbhDescriptorX         &rWobblePart_            = dbhDxTable[ 71];
DbhDescriptorX         &rMeteoPressure_         = dbhDxTable[ 72];
DbhDescriptorX         &rCableCalibration_      = dbhDxTable[ 73];
DbhDescriptorX         &rMeteoHumidity_         = dbhDxTable[ 74];
DbhDescriptorX         &rMeteoTemperature_      = dbhDxTable[ 75];
DbhDescriptorX         &rGrIonFrq_              = dbhDxTable[ 76];
DbhDescriptorX         &rIonCorr_               = dbhDxTable[ 77];
DbhDescriptorX         &rIonRms_                = dbhDxTable[ 78];
DbhDescriptorX         &rPhIonFrq_              = dbhDxTable[ 79];
DbhDescriptorX         &rNAmbig_                = dbhDxTable[ 80];
DbhDescriptorX         &rAtmCflg_               = dbhDxTable[ 81];
DbhDescriptorX         &rClkCflg_               = dbhDxTable[ 82];
DbhDescriptorX         &rClkBrkEpochs_          = dbhDxTable[ 83];
DbhDescriptorX         &rClkBrkNum_             = dbhDxTable[ 84];
DbhDescriptorX         &rClkBrkFlags_           = dbhDxTable[ 85];
DbhDescriptorX         &rClkBrkNames_           = dbhDxTable[ 86];
DbhDescriptorX         &rIonCode_               = dbhDxTable[ 87];
DbhDescriptorX         &rNAmbigSecBand_         = dbhDxTable[ 88];
DbhDescriptorX         &rQualityCodeSecBand_    = dbhDxTable[ 89];
DbhDescriptorX         &rEffFreqsSecBand_       = dbhDxTable[ 90];
DbhDescriptorX         &rRateObsSecBand_        = dbhDxTable[ 91];
DbhDescriptorX         &rSigmaRateObsSecBand_   = dbhDxTable[ 92];
DbhDescriptorX         &rDelayObsSecBand_       = dbhDxTable[ 93];
DbhDescriptorX         &rSigmaDelayObsSecBand_  = dbhDxTable[ 94];
DbhDescriptorX         &rPhaseObsSecBand_       = dbhDxTable[ 95];
DbhDescriptorX         &rSigmaPhaseObsSecBand_  = dbhDxTable[ 96];
DbhDescriptorX         &rTotalPhaseSecBand_     = dbhDxTable[ 97];
DbhDescriptorX         &rRefFreqSecBand_        = dbhDxTable[ 98];
DbhDescriptorX         &rSnrSecBand_            = dbhDxTable[ 99];
DbhDescriptorX         &rCorrCoefSecBand_       = dbhDxTable[100];
DbhDescriptorX         &rSBDelaySecBand_        = dbhDxTable[101];
DbhDescriptorX         &rSigmaSBDelaySecBand_   = dbhDxTable[102];
DbhDescriptorX         &rGrpAmbiguitySecBand_   = dbhDxTable[103];
DbhDescriptorX         &rTotalPhase_            = dbhDxTable[104];
DbhDescriptorX         &rBatchCnt_              = dbhDxTable[105];
DbhDescriptorX         &rCorrelatorType_        = dbhDxTable[106];
DbhDescriptorX         &rAcmNames_              = dbhDxTable[107];
DbhDescriptorX         &rCalcVersionValue_      = dbhDxTable[108];
DbhDescriptorX         &rTectPlateNames_        = dbhDxTable[109];
DbhDescriptorX         &rUt1Ortho_              = dbhDxTable[110];
DbhDescriptorX         &rWobOrtho_              = dbhDxTable[111];
DbhDescriptorX         &rPtdContrib_            = dbhDxTable[112];
DbhDescriptorX         &rEtdContrib_            = dbhDxTable[113];
DbhDescriptorX         &rWobXContrib_           = dbhDxTable[114];
DbhDescriptorX         &rWobYContrib_           = dbhDxTable[115];
DbhDescriptorX         &rWobNutatContrib_       = dbhDxTable[116];
DbhDescriptorX         &rFeedCorr_              = dbhDxTable[117];
DbhDescriptorX         &rTiltRemvr_             = dbhDxTable[118];
DbhDescriptorX         &rNdryCont_              = dbhDxTable[119];
DbhDescriptorX         &rNwetCont_              = dbhDxTable[120];
DbhDescriptorX         &rNgradParts_            = dbhDxTable[121];
DbhDescriptorX         &rUnPhaseCal_            = dbhDxTable[122];
DbhDescriptorX         &rAxOffsetCont_          = dbhDxTable[123];
DbhDescriptorX         &rCalSites_              = dbhDxTable[124];
DbhDescriptorX         &rCalList_               = dbhDxTable[125];
DbhDescriptorX         &rCalFlags_              = dbhDxTable[126];
DbhDescriptorX         &rAxsOfsPart_            = dbhDxTable[127];
DbhDescriptorX         &rTai_2_Utc_             = dbhDxTable[128];
DbhDescriptorX         &rFourFitFileName_       = dbhDxTable[129];
DbhDescriptorX         &rFourFitFileNameSecBand_= dbhDxTable[130];
DbhDescriptorX         &rIonBits_               = dbhDxTable[131];
DbhDescriptorX         &rUserAction4Suppression_= dbhDxTable[132];
DbhDescriptorX         &rFut1_inf_              = dbhDxTable[133];
DbhDescriptorX         &rFut1_pts_              = dbhDxTable[134];
DbhDescriptorX         &rFwob_inf_              = dbhDxTable[135];
DbhDescriptorX         &rFwob_pts_              = dbhDxTable[136];
DbhDescriptorX         &rTidalUt1_              = dbhDxTable[137];
DbhDescriptorX         &rPhAmbig_S_             = dbhDxTable[138];
DbhDescriptorX         &rSolData_               = dbhDxTable[139];
DbhDescriptorX         &rSupMet_                = dbhDxTable[140];
DbhDescriptorX         &rBlDepClocks_           = dbhDxTable[141];
DbhDescriptorX         &rAtmPartFlag_           = dbhDxTable[142];
DbhDescriptorX         &rFclList_               = dbhDxTable[143];
DbhDescriptorX         &rFclFlags_              = dbhDxTable[144];
DbhDescriptorX         &rObcList_               = dbhDxTable[145];
DbhDescriptorX         &rObcFlags_              = dbhDxTable[146];
DbhDescriptorX         &rScanName_              = dbhDxTable[147];
DbhDescriptorX         &rRunCode_               = dbhDxTable[148];
DbhDescriptorX         &rOloadHorzContrib_      = dbhDxTable[149];
DbhDescriptorX         &rOloadVertContrib_      = dbhDxTable[150];
DbhDescriptorX         &rWobLibra_              = dbhDxTable[151];
DbhDescriptorX         &rUt1Libra_              = dbhDxTable[152];
DbhDescriptorX         &rNut06xys_              = dbhDxTable[153];
DbhDescriptorX         &rNut06xyp_              = dbhDxTable[154];
DbhDescriptorX         &rNutWahr_               = dbhDxTable[155];
DbhDescriptorX         &rNut2006_               = dbhDxTable[156];
DbhDescriptorX         &rOptlContib_            = dbhDxTable[157];
DbhDescriptorX         &rAcSites_               = dbhDxTable[158];
DbhDescriptorX         &rCcSites_               = dbhDxTable[159];
DbhDescriptorX         &rOceOld_                = dbhDxTable[160];
DbhDescriptorX         &rPtdOld_                = dbhDxTable[161];
DbhDescriptorX         &rFrngErr_               = dbhDxTable[162];
DbhDescriptorX         &rAcmEpochs_             = dbhDxTable[163];
DbhDescriptorX         &rAcmNumber_             = dbhDxTable[164];
DbhDescriptorX         &rParAngle_              = dbhDxTable[165];
DbhDescriptorX         &rConsensusRate_         = dbhDxTable[166];
DbhDescriptorX         &rNdryPart_              = dbhDxTable[167];
DbhDescriptorX         &rNwetPart_              = dbhDxTable[168];
DbhDescriptorX         &rSun1BendingContrib_    = dbhDxTable[169];
DbhDescriptorX         &rSun2BendingContrib_    = dbhDxTable[170];

DbhDescriptorX         &rMslmIonContrib_        = dbhDxTable[171];



// individual descriptors:
SgDbhDatumDescriptor  *&dNSites_                = rNSites_.d_;
bool                   &hasNSites_              = rNSites_.isPresent_;
SgDbhDatumDescriptor  *&dNStars_                = rNStars_.d_;
bool                   &hasNStars_              = rNStars_.isPresent_;
SgDbhDatumDescriptor  *&dNObs_                  = rNObs_.d_;
bool                   &hasNObs_                = rNObs_.isPresent_;
SgDbhDatumDescriptor  *&dSiteNames_             = rSiteNames_.d_;
bool                   &hasSiteNames_           = rSiteNames_.isPresent_;
SgDbhDatumDescriptor  *&dStarNames_             = rStarNames_.d_;
bool                   &hasStarNames_           = rStarNames_.isPresent_;
SgDbhDatumDescriptor  *&dAxisOffsets_           = rAxisOffsets_.d_;
bool                   &hasAxisOffsets_         = rAxisOffsets_.isPresent_;
SgDbhDatumDescriptor  *&dStar2000_              = rStar2000_.d_;
bool                   &hasStar2000_            = rStar2000_.isPresent_;
SgDbhDatumDescriptor  *&dSiteRecs_              = rSiteRecs_.d_;
bool                   &hasSiteRecs_            = rSiteRecs_.isPresent_;
SgDbhDatumDescriptor  *&dOloadAmpHorz_          = rOloadAmpHorz_.d_;
bool                   &hasOloadAmpHorz_        = rOloadAmpHorz_.isPresent_;
SgDbhDatumDescriptor  *&dOloadPhsHorz_          = rOloadPhsHorz_.d_;
bool                   &hasOloadPhsHorz_        = rOloadPhsHorz_.isPresent_;
SgDbhDatumDescriptor  *&dOloadAmpVert_          = rOloadAmpVert_.d_;
bool                   &hasOloadAmpVert_        = rOloadAmpVert_.isPresent_;
SgDbhDatumDescriptor  *&dOloadPhsVert_          = rOloadPhsVert_.d_;
bool                   &hasOloadPhsVert_        = rOloadPhsVert_.isPresent_;
SgDbhDatumDescriptor  *&dAxisTypes_             = rAxisTypes_.d_;
bool                   &hasAxisTypes_           = rAxisTypes_.isPresent_;
SgDbhDatumDescriptor  *&dCableSigns_            = rCableSigns_.d_;
bool                   &hasCableSigns_          = rCableSigns_.isPresent_;
SgDbhDatumDescriptor  *&dCableStations_         = rCableStations_.d_;
bool                   &hasCableStations_       = rCableStations_.isPresent_;
SgDbhDatumDescriptor  *&dAtmConstr_             = rAtmConstr_.d_;
bool                   &hasAtmConstr_           = rAtmConstr_.isPresent_;
SgDbhDatumDescriptor  *&dAtmIntrv_              = rAtmIntrv_.d_;
bool                   &hasAtmIntrv_            = rAtmIntrv_.isPresent_;
SgDbhDatumDescriptor  *&dClockConstr_           = rClockConstr_.d_;
bool                   &hasClockConstr_         = rClockConstr_.isPresent_;
SgDbhDatumDescriptor  *&dClockIntrv_            = rClockIntrv_.d_;
bool                   &hasClockIntrv_          = rClockIntrv_.isPresent_;
SgDbhDatumDescriptor  *&dAcmOffsets_            = rAcmOffsets_.d_;
bool                   &hasAcmOffsets_          = rAcmOffsets_.isPresent_;
SgDbhDatumDescriptor  *&dAcmRates_              = rAcmRates_.d_;
bool                   &hasAcmRates_            = rAcmRates_.isPresent_;
SgDbhDatumDescriptor  *&dError_K_               = rError_K_.d_;
bool                   &hasError_K_             = rError_K_.isPresent_;
SgDbhDatumDescriptor  *&dEccCoords_             = rEccCoords_.d_;
bool                   &hasEccCoords_           = rEccCoords_.isPresent_;
SgDbhDatumDescriptor  *&dNRefClocks_            = rNRefClocks_.d_;
bool                   &hasNRefClocks_          = rNRefClocks_.isPresent_;
SgDbhDatumDescriptor  *&dBaselinesSelStatus_    = rBaselinesSelStatus_.d_;
bool                   &hasBaselinesSelStatus_  = rBaselinesSelStatus_.isPresent_;
SgDbhDatumDescriptor  *&dSourcesSelStatus_      = rSourcesSelStatus_.d_;
bool                   &hasSourcesSelStatus_    = rSourcesSelStatus_.isPresent_;
SgDbhDatumDescriptor  *&dBaselinesDepClocks_    = rBaselinesDepClocks_.d_;
bool                   &hasBaselinesDepClocks_  = rBaselinesDepClocks_.isPresent_;
SgDbhDatumDescriptor  *&dClockSite_             = rClockSite_.d_;
SgDbhDatumDescriptor  *&dRefClockSites_         = rRefClockSites_.d_;
bool                   &hasRefClockSites_       = rRefClockSites_.isPresent_;
SgDbhDatumDescriptor  *&dClockBreakStatus_      = rClockBreakStatus_.d_;
bool                   &hasClockBreakStatus_    = rClockBreakStatus_.isPresent_;
SgDbhDatumDescriptor  *&dEccNames_              = rEccNames_.d_;
bool                   &hasEccNames_            = rEccNames_.isPresent_;
SgDbhDatumDescriptor  *&dEccTypes_              = rEccTypes_.d_;
bool                   &hasEccTypes_            = rEccTypes_.isPresent_;
SgDbhDatumDescriptor  *&dEl_Cut_                = rEl_Cut_.d_;
bool                   &hasEl_Cut_              = rEl_Cut_.isPresent_;
SgDbhDatumDescriptor  *&dError_Bl_              = rError_Bl_.d_;
bool                   &hasError_Bl_            = rError_Bl_.isPresent_;
SgDbhDatumDescriptor  *&dN4Obs_                 = rN4Obs_.d_;
bool                   &hasN4Obs_               = rN4Obs_.isPresent_;
SgDbhDatumDescriptor  *&dNumOfSamples_          = rNumOfSamples_.d_;
bool                   &hasNumOfSamples_        = rNumOfSamples_.isPresent_;
SgDbhDatumDescriptor  *&dAmpByFrq_              = rAmpByFrq_.d_;
SgDbhDatumDescriptor  *&dCorrCoef_              = rCorrCoef_.d_;
SgDbhDatumDescriptor  *&dObservedDelay_         = rObservedDelay_.d_;
SgDbhDatumDescriptor  *&dObservedDelaySigma_    = rObservedDelaySigma_.d_;
SgDbhDatumDescriptor  *&dGrpAmbiguity_          = rGrpAmbiguity_.d_;
SgDbhDatumDescriptor  *&dDelayRate_             = rDelayRate_.d_;
SgDbhDatumDescriptor  *&dSigmaDelayRate_        = rSigmaDelayRate_.d_;
SgDbhDatumDescriptor  *&dRecSetup_              = rRecSetup_.d_;
SgDbhDatumDescriptor  *&dReferenceFrequency_    = rReferenceFrequency_.d_;
SgDbhDatumDescriptor  *&dRefFreqByChannel_      = rRefFreqByChannel_.d_;
bool                   &hasRefFreqByChannel_    = rRefFreqByChannel_.isPresent_;
SgDbhDatumDescriptor  *&dSampleRate_            = rSampleRate_.d_;
SgDbhDatumDescriptor  *&dSbDelay_               = rSbDelay_.d_;
SgDbhDatumDescriptor  *&dSbSigmaDelay_          = rSbSigmaDelay_.d_;
SgDbhDatumDescriptor  *&dSecTag_                = rSecTag_.d_;
SgDbhDatumDescriptor  *&dSnr_                   = rSnr_.d_;
SgDbhDatumDescriptor  *&dNumOfChannels_         = rNumOfChannels_.d_;
bool                   &hasNumOfChannels_       = rNumOfChannels_.isPresent_;
SgDbhDatumDescriptor  *&dCalByFrq_              = rCalByFrq_.d_;
bool                   &hasCalByFrq_            = rCalByFrq_.isPresent_;
SgDbhDatumDescriptor  *&dDelayUFlag_            = rDelayUFlag_.d_;
SgDbhDatumDescriptor  *&dNumOfAccPeriods_       = rNumOfAccPeriods_.d_;
SgDbhDatumDescriptor  *&dPhaseCalOffset_        = rPhaseCalOffset_.d_;
bool                   &hasPhaseCalOffset_      = rPhaseCalOffset_.isPresent_;
SgDbhDatumDescriptor  *&dRateUFlag_             = rRateUFlag_.d_;
SgDbhDatumDescriptor  *&dUtcTag_                = rUtcTag_.d_;
SgDbhDatumDescriptor  *&dBaselineName_          = rBaselineName_.d_;
SgDbhDatumDescriptor  *&dQualityCode_           = rQualityCode_.d_;
SgDbhDatumDescriptor  *&dSourceName_            = rSourceName_.d_;
SgDbhDatumDescriptor  *&dElevations_            = rElevations_.d_;
bool                   &hasElevationData_       = rElevations_.isPresent_;
SgDbhDatumDescriptor  *&dConsBendingDelay_      = rConsBendingDelay_.d_;
bool                   &hasConsBendingDelay_    = rConsBendingDelay_.isPresent_;
SgDbhDatumDescriptor  *&dConsensusDelay_        = rConsensusDelay_.d_;
bool                   &hasConsensusDelay_      = rConsensusDelay_.isPresent_;
SgDbhDatumDescriptor  *&dAzimuths_              = rAzimuths_.d_;
bool                   &hasAzimuthData_         = rAzimuths_.isPresent_;
SgDbhDatumDescriptor  *&dNutationPart_          = rNutationPart_.d_;
bool                   &hasNutationPart_        = rNutationPart_.isPresent_;
SgDbhDatumDescriptor  *&dNutationPart2K_        = rNutationPart2K_.d_;
bool                   &hasNutationPart2K_      = rNutationPart2K_.isPresent_;
SgDbhDatumDescriptor  *&dOloadContrib_          = rOloadContrib_.d_;
bool                   &hasOloadContrib_        = rOloadContrib_.isPresent_;
SgDbhDatumDescriptor  *&dSitPart_               = rSitPart_.d_;
bool                   &hasSitPart_             = rSitPart_.isPresent_;
SgDbhDatumDescriptor  *&dStarPart_              = rStarPart_.d_;
bool                   &hasStarPart_            = rStarPart_.isPresent_;
SgDbhDatumDescriptor  *&dUT1Part_               = rUT1Part_.d_;
bool                   &hasUT1Part_             = rUT1Part_.isPresent_;
SgDbhDatumDescriptor  *&dWobblePart_            = rWobblePart_.d_;
bool                   &hasWobblePart_          = rWobblePart_.isPresent_;
SgDbhDatumDescriptor  *&dMeteoPressure_         = rMeteoPressure_.d_;
bool                   &hasMeteoPressureData_   = rMeteoPressure_.isPresent_;
SgDbhDatumDescriptor  *&dCableCalibration_      = rCableCalibration_.d_;
bool                   &hasCableData_           = rCableCalibration_.isPresent_;
SgDbhDatumDescriptor  *&dMeteoHumidity_         = rMeteoHumidity_.d_;
bool                   &hasMeteoRelHumidityData_= rMeteoHumidity_.isPresent_;
SgDbhDatumDescriptor  *&dMeteoTemperature_      = rMeteoTemperature_.d_;
bool                   &hasMeteoTemperatureData_= rMeteoTemperature_.isPresent_;
SgDbhDatumDescriptor  *&dNAmbig_                = rNAmbig_.d_;
bool                   &hasNAmbig_              = rNAmbig_.isPresent_;
SgDbhDatumDescriptor  *&dGrIonFrq_              = rGrIonFrq_.d_;
bool                   &hasGrIonFrq_            = rGrIonFrq_.isPresent_;
SgDbhDatumDescriptor  *&dIonCorr_               = rIonCorr_.d_;
bool                   &hasIonCorr_             = rIonCorr_.isPresent_;
SgDbhDatumDescriptor  *&dIonRms_                = rIonRms_.d_;
bool                   &hasIonRms_              = rIonRms_.isPresent_;
SgDbhDatumDescriptor  *&dPhIonFrq_              = rPhIonFrq_.d_;
bool                   &hasPhIonFrq_            = rPhIonFrq_.isPresent_;
SgDbhDatumDescriptor  *&dAtmCflg_               = rAtmCflg_.d_;
bool                   &hasAtmCflg_             = rAtmCflg_.isPresent_;
SgDbhDatumDescriptor  *&dClkCflg_               = rClkCflg_.d_;
bool                   &hasClkCflg_             = rClkCflg_.isPresent_;
SgDbhDatumDescriptor  *&dClkBrkEpochs_          = rClkBrkEpochs_.d_;
bool                   &hasClkBrkEpochs_        = rClkBrkEpochs_.isPresent_;
SgDbhDatumDescriptor  *&dClkBrkNum_             = rClkBrkNum_.d_;
bool                   &hasClkBrkNum_           = rClkBrkNum_.isPresent_;
SgDbhDatumDescriptor  *&dClkBrkFlags_           = rClkBrkFlags_.d_;
bool                   &hasClkBrkFlags_         = rClkBrkFlags_.isPresent_;
SgDbhDatumDescriptor  *&dClkBrkNames_           = rClkBrkNames_.d_;
bool                   &hasClkBrkNames_         = rClkBrkNames_.isPresent_;
SgDbhDatumDescriptor  *&dIonCode_               = rIonCode_.d_;
SgDbhDatumDescriptor  *&dNAmbigSecBand_         = rNAmbigSecBand_.d_;
bool                   &hasNAmbigSecBand_       = rNAmbigSecBand_.isPresent_;
SgDbhDatumDescriptor  *&dQualityCodeSecBand_    = rQualityCodeSecBand_.d_;
bool                   &hasQualityCodeSecBand_  = rQualityCodeSecBand_.isPresent_;
SgDbhDatumDescriptor  *&dEffFreqsSecBand_       = rEffFreqsSecBand_.d_;
bool                   &hasEffFreqsSecBand_     = rEffFreqsSecBand_.isPresent_;
SgDbhDatumDescriptor  *&dRateObsSecBand_        = rRateObsSecBand_.d_;
bool                   &hasRateObsSecBand_      = rRateObsSecBand_.isPresent_;
SgDbhDatumDescriptor  *&dSigmaRateObsSecBand_   = rSigmaRateObsSecBand_.d_;
bool                   &hasSigmaRateObsSecBand_ = rSigmaRateObsSecBand_.isPresent_;
SgDbhDatumDescriptor  *&dDelayObsSecBand_       = rDelayObsSecBand_.d_;
bool                   &hasDelayObsSecBand_     = rDelayObsSecBand_.isPresent_;
SgDbhDatumDescriptor  *&dSigmaDelayObsSecBand_  = rSigmaDelayObsSecBand_.d_;
bool                   &hasSigmaDelayObsSecBand_= rSigmaDelayObsSecBand_.isPresent_;
SgDbhDatumDescriptor  *&dPhaseObsSecBand_       = rPhaseObsSecBand_.d_;
bool                   &hasPhaseObsSecBand_     = rPhaseObsSecBand_.isPresent_;
SgDbhDatumDescriptor  *&dSigmaPhaseObsSecBand_  = rSigmaPhaseObsSecBand_.d_;
bool                   &hasSigmaPhaseObsSecBand_= rSigmaPhaseObsSecBand_.isPresent_;
SgDbhDatumDescriptor  *&dTotalPhaseSecBand_     = rTotalPhaseSecBand_.d_;
bool                   &hasTotalPhaseSecBand_   = rTotalPhaseSecBand_.isPresent_;
SgDbhDatumDescriptor  *&dRefFreqSecBand_        = rRefFreqSecBand_.d_;
bool                   &hasRefFreqSecBand_      = rRefFreqSecBand_.isPresent_;
SgDbhDatumDescriptor  *&dSnrSecBand_            = rSnrSecBand_.d_;
bool                   &hasSnrSecBand_          = rSnrSecBand_.isPresent_;
SgDbhDatumDescriptor  *&dCorrCoefSecBand_       = rCorrCoefSecBand_.d_;
bool                   &hasCorrCoefSecBand_     = rCorrCoefSecBand_.isPresent_;
SgDbhDatumDescriptor  *&dSBDelaySecBand_        = rSBDelaySecBand_.d_;
bool                   &hasSBDelaySecBand_      = rSBDelaySecBand_.isPresent_;
SgDbhDatumDescriptor  *&dSigmaSBDelaySecBand_   = rSigmaSBDelaySecBand_.d_;
bool                   &hasSigmaSBDelaySecBand_ = rSigmaSBDelaySecBand_.isPresent_;
SgDbhDatumDescriptor  *&dGrpAmbiguitySecBand_   = rGrpAmbiguitySecBand_.d_;
bool                   &hasGrpAmbiguitySecBand_ = rGrpAmbiguitySecBand_.isPresent_;
SgDbhDatumDescriptor  *&dTotalPhase_            = rTotalPhase_.d_;
bool                   &hasTotalPhase_          = rTotalPhase_.isPresent_;
SgDbhDatumDescriptor  *&dBatchCnt_              = rBatchCnt_.d_;
bool                   &hasBatchCnt_            = rBatchCnt_.isPresent_;
SgDbhDatumDescriptor  *&dCorrelatorType_        = rCorrelatorType_.d_;
bool                   &hasCorrelatorType_      = rCorrelatorType_.isPresent_;
SgDbhDatumDescriptor  *&dAcmNames_              = rAcmNames_.d_;
bool                   &hasAcmNames_            = rAcmNames_.isPresent_;
SgDbhDatumDescriptor  *&dCalcVersionValue_      = rCalcVersionValue_.d_;
bool                   &hasCalcVersionValue_    = rCalcVersionValue_.isPresent_;
SgDbhDatumDescriptor  *&dTectPlateNames_        = rTectPlateNames_.d_;
bool                   &hasTectPlateNames_      = rTectPlateNames_.isPresent_;
SgDbhDatumDescriptor  *&dUt1Ortho_              = rUt1Ortho_.d_;
bool                   &hasUt1Ortho_            = rUt1Ortho_.isPresent_;
SgDbhDatumDescriptor  *&dWobOrtho_              = rWobOrtho_.d_;
bool                   &hasWobOrtho_            = rWobOrtho_.isPresent_;
SgDbhDatumDescriptor  *&dPtdContrib_            = rPtdContrib_.d_;
bool                   &hasPtdContrib_          = rPtdContrib_.isPresent_;
SgDbhDatumDescriptor  *&dEtdContrib_            = rEtdContrib_.d_;
bool                   &hasEtdContrib_          = rEtdContrib_.isPresent_;
SgDbhDatumDescriptor  *&dWobXContrib_           = rWobXContrib_.d_;
bool                   &hasWobXContrib_         = rWobXContrib_.isPresent_;
SgDbhDatumDescriptor  *&dWobYContrib_           = rWobYContrib_.d_;
bool                   &hasWobYContrib_         = rWobYContrib_.isPresent_;
SgDbhDatumDescriptor  *&dWobNutatContrib_       = rWobNutatContrib_.d_;
bool                   &hasWobNutatContrib_     = rWobNutatContrib_.isPresent_;
SgDbhDatumDescriptor  *&dFeedCorr_              = rFeedCorr_.d_;
bool                   &hasFeedCorr_            = rFeedCorr_.isPresent_;
SgDbhDatumDescriptor  *&dTiltRemvr_             = rTiltRemvr_.d_;
bool                   &hasTiltRemvr_           = rTiltRemvr_.isPresent_;
SgDbhDatumDescriptor  *&dNdryCont_              = rNdryCont_.d_;
bool                   &hasNdryCont_            = rNdryCont_.isPresent_;
SgDbhDatumDescriptor  *&dNwetCont_              = rNwetCont_.d_;
bool                   &hasNwetCont_            = rNwetCont_.isPresent_;
SgDbhDatumDescriptor  *&dNgradParts_            = rNgradParts_.d_;
bool                   &hasNgradParts_          = rNgradParts_.isPresent_;
SgDbhDatumDescriptor  *&dUnPhaseCal_            = rUnPhaseCal_.d_;
bool                   &hasUnPhaseCal_          = rUnPhaseCal_.isPresent_;
SgDbhDatumDescriptor  *&dAxOffsetCont_          = rAxOffsetCont_.d_;
bool                   &hasAxOffsetCont_        = rAxOffsetCont_.isPresent_;
SgDbhDatumDescriptor  *&dCalSites_              = rCalSites_.d_;
bool                   &hasCalSites_            = rCalSites_.isPresent_;
SgDbhDatumDescriptor  *&dCalList_               = rCalList_.d_;
bool                   &hasCalList_             = rCalList_.isPresent_;
SgDbhDatumDescriptor  *&dCalFlags_              = rCalFlags_.d_;
bool                   &hasCalFlags_            = rCalFlags_.isPresent_;
SgDbhDatumDescriptor  *&dAxsOfsPart_            = rAxsOfsPart_.d_;
bool                   &hasAxsOfsPart_          = rAxsOfsPart_.isPresent_;
SgDbhDatumDescriptor  *&dTai_2_Utc_             = rTai_2_Utc_.d_;
bool                   &hasTai_2_Utc_           = rTai_2_Utc_.isPresent_;
SgDbhDatumDescriptor  *&dFourFitFileName_           = rFourFitFileName_.d_;
bool                   &hasFourFitFileName_         = rFourFitFileName_.isPresent_;
SgDbhDatumDescriptor  *&dFourFitFileNameSecBand_    = rFourFitFileNameSecBand_.d_;
bool                   &hasFourFitFileNameSecBand_  = rFourFitFileNameSecBand_.isPresent_;
SgDbhDatumDescriptor  *&dIonBits_                   = rIonBits_.d_;
bool                   &hasIonBits_                 = rIonBits_.isPresent_;
SgDbhDatumDescriptor  *&dUserAction4Suppression_    = rUserAction4Suppression_.d_;
bool                   &hasUserAction4Suppression_  = rUserAction4Suppression_.isPresent_;
SgDbhDatumDescriptor  *&dFut1_inf_              = rFut1_inf_.d_;
bool                   &hasFut1_inf_            = rFut1_inf_.isPresent_;
SgDbhDatumDescriptor  *&dFut1_pts_              = rFut1_pts_.d_;
bool                   &hasFut1_pts_            = rFut1_pts_.isPresent_;
SgDbhDatumDescriptor  *&dFwob_inf_              = rFwob_inf_.d_;
bool                   &hasFwob_inf_            = rFwob_inf_.isPresent_;
SgDbhDatumDescriptor  *&dFwob_pts_              = rFwob_pts_.d_;
bool                   &hasFwob_pts_            = rFwob_pts_.isPresent_;
SgDbhDatumDescriptor  *&dTidalUt1_              = rTidalUt1_.d_;
bool                   &hasTidalUt1_            = rTidalUt1_.isPresent_;
SgDbhDatumDescriptor  *&dPhAmbig_S_             = rPhAmbig_S_.d_;
bool                   &hasPhAmbig_S_           = rPhAmbig_S_.isPresent_;
SgDbhDatumDescriptor  *&dSolData_               = rSolData_.d_;
bool                   &hasSolData_             = rSolData_.isPresent_;
SgDbhDatumDescriptor  *&dSupMet_                = rSupMet_.d_;
bool                   &hasSupMet_              = rSupMet_.isPresent_;
SgDbhDatumDescriptor  *&dBlDepClocks_           = rBlDepClocks_.d_;
bool                   &hasBlDepClocks_         = rBlDepClocks_.isPresent_;
SgDbhDatumDescriptor  *&dAtmPartFlag_           = rAtmPartFlag_.d_;
bool                   &hasAtmPartFlag_         = rAtmPartFlag_.isPresent_;
SgDbhDatumDescriptor  *&dFclList_               = rFclList_.d_;
bool                   &hasFclList_             = rFclList_.isPresent_;
SgDbhDatumDescriptor  *&dFclFlags_              = rFclFlags_.d_;
bool                   &hasFclFlags_            = rFclFlags_.isPresent_;
SgDbhDatumDescriptor  *&dObcList_               = rObcList_.d_;
bool                   &hasObcList_             = rObcList_.isPresent_;
SgDbhDatumDescriptor  *&dObcFlags_              = rObcFlags_.d_;
bool                   &hasObcFlags_            = rObcFlags_.isPresent_;
SgDbhDatumDescriptor  *&dScanName_              = rScanName_.d_;
bool                   &hasScanName_            = rScanName_.isPresent_;
SgDbhDatumDescriptor  *&dRunCode_               = rRunCode_.d_;
bool                   &hasRunCode_             = rRunCode_.isPresent_;
SgDbhDatumDescriptor  *&dOloadHorzContrib_      = rOloadHorzContrib_.d_;
bool                   &hasOloadHorzContrib_    = rOloadHorzContrib_.isPresent_;
SgDbhDatumDescriptor  *&dOloadVertContrib_      = rOloadVertContrib_.d_;
bool                   &hasOloadVertContrib_    = rOloadVertContrib_.isPresent_;
SgDbhDatumDescriptor  *&dWobLibra_              = rWobLibra_.d_;
bool                   &hasWobLibra_            = rWobLibra_.isPresent_;
SgDbhDatumDescriptor  *&dUt1Libra_              = rUt1Libra_.d_;
bool                   &hasUt1Libra_            = rUt1Libra_.isPresent_;
SgDbhDatumDescriptor  *&dNut06xys_              = rNut06xys_.d_;
bool                   &hasNut06xys_            = rNut06xys_.isPresent_;
SgDbhDatumDescriptor  *&dNut06xyp_              = rNut06xyp_.d_;
bool                   &hasNut06xyp_            = rNut06xyp_.isPresent_;
SgDbhDatumDescriptor  *&dNutWahr_               = rNutWahr_.d_;
bool                   &hasNutWahr_             = rNutWahr_.isPresent_;
SgDbhDatumDescriptor  *&dNut2006_               = rNut2006_.d_;
bool                   &hasNut2006_             = rNut2006_.isPresent_;
SgDbhDatumDescriptor  *&dOptlContib_            = rOptlContib_.d_;
bool                   &hasOptlContib_          = rOptlContib_.isPresent_;
SgDbhDatumDescriptor  *&dAcSites_               = rAcSites_.d_;
bool                   &hasAcSites_             = rAcSites_.isPresent_;
SgDbhDatumDescriptor  *&dCcSites_               = rCcSites_.d_;
bool                   &hasCcSites_             = rCcSites_.isPresent_;
SgDbhDatumDescriptor  *&dOceOld_                = rOceOld_.d_;
bool                   &hasOceOld_              = rOceOld_.isPresent_;
SgDbhDatumDescriptor  *&dPtdOld_                = rPtdOld_.d_;
bool                   &hasPtdOld_              = rPtdOld_.isPresent_;
SgDbhDatumDescriptor  *&dFrngErr_               = rFrngErr_.d_;
bool                   &hasFrngErr_             = rFrngErr_.isPresent_;
SgDbhDatumDescriptor  *&dAcmEpochs_             = rAcmEpochs_.d_;
bool                   &hasAcmEpochs_           = rAcmEpochs_.isPresent_;
SgDbhDatumDescriptor  *&dAcmNumber_             = rAcmNumber_.d_;
bool                   &hasAcmNumber_           = rAcmNumber_.isPresent_;
SgDbhDatumDescriptor  *&dParAngle_              = rParAngle_.d_;
bool                   &hasParAngle_            = rParAngle_.isPresent_;
SgDbhDatumDescriptor  *&dConsensusRate_         = rConsensusRate_.d_;
bool                   &hasConsensusRate_       = rConsensusRate_.isPresent_;
SgDbhDatumDescriptor  *&dNdryPart_              = rNdryPart_.d_;
bool                   &hasNdryPart_            = rNdryPart_.isPresent_;
SgDbhDatumDescriptor  *&dNwetPart_              = rNwetPart_.d_;
bool                   &hasNwetPart_            = rNwetPart_.isPresent_;
SgDbhDatumDescriptor  *&dSun1BendingContrib_    = rSun1BendingContrib_.d_;
bool                   &hasSun1BendingContrib_  = rSun1BendingContrib_.isPresent_;
SgDbhDatumDescriptor  *&dSun2BendingContrib_    = rSun2BendingContrib_.d_;
bool                   &hasSun2BendingContrib_  = rSun2BendingContrib_.isPresent_;
SgDbhDatumDescriptor  *&dMslmIonContrib_        = rMslmIonContrib_.d_;
bool                   &hasMslmIonContrib_      = rMslmIonContrib_.isPresent_;


/*=====================================================================================================*/

/*=====================================================================================================*/
