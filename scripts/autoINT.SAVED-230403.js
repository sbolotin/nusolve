//
//    This file is a part of nuSolve. nuSolve is a part of CALC/SOLVE system
//    and is designed to perform data analyis of a geodetic VLBI session.
//    Copyright (C) 2021--2023 Sergei Bolotin.
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//
//

const   isTesting = true;

//
//
//
const   selfName = 'autoINT.js';
var     hasShortBln;
var     hasDeselectedBln;
var     deselectedBlnKey;
var     need2save;

var     isAutoGood = true;

//
//
function setSigma2add(sigma2add)
{
    var baselines = session.baselines;
    // session-wide:
    for (var i=0; i<baselines.length; i++)
        baselines[i].sigma2add = sigma2add;
    // per band:
    for (var i=0; i<session.bands.length; i++)
    {
        for (var j=0; j<session.bands[i].baselines.length; j++)
            session.bands[i].baselines[j].sigma2add = sigma2add;
    };
};




//
function checkStationNames(phrase)
{
    var count = 0;
    var stations = session.stations;
    for (var i=0; i<stations.length; i++)
    {
        var stn = stations[i];
        if (stn.key.indexOf(phrase) !== -1)
        {
            stn.setIsValid(false);
            logger.write(Log.Inf, Log.Preproc, selfName + ': checkStationNames(): station ' +
                stn.name + ' has been deselected');
            count++;
        };
    };
    logger.write(Log.Inf, Log.Preproc, selfName + ': checkStationNames(): ' + count +
        ' stations have been deselected');
};




//
function checkShortBaselines(limit)
{
    var baselines = session.baselines;
    for (var i=0; i<baselines.length; i++)
    {
        var bln = baselines[i];
        if (bln.length < limit)
        {
            bln.setIsValid(false);
            logger.write(Log.Wrn, Log.Preproc, selfName + ': checkShortBaselines(): the short baseline ' +
                bln.name + ' has been deselected; length ' + bln.length + ' < ' + limit + ' (m)');
            hasShortBln = true;
        };
    };
};




//
function reCheckNumOfGoodObs()
{
    var obs;
    var num;
    var thr = config.goodQualityCodeAtStartup - 1;

    var pBand = session.bands[session.primaryBandIdx];
    var primaryBandKey = pBand.key;
    var baselines = pBand.baselines;

    for (var j=0; j<baselines.length; j++)
    {
        var bln = baselines[j];
        print(' ++  bln ' + bln.name + ': NumObs= ' + bln.observations.length);
        num = 0;
        for (var i=0; i<bln.observations.length; i++)
        {
            obs = bln.observations[i];
            if (obs.isValid)
                num++;
        };
        if (num <= 8) // too few obs, try to pick up obs with a lower QC:
        {
            num = 0;
            for (var i=0; i<bln.observations.length; i++)
            {
                obs = bln.observations[i];
                if (!obs.isValid && thr <= obs.qualityFactor(primaryBandKey))
                {
                    obs.isValid = true;
                    num++;
                };
            };
            logger.write(Log.Inf, Log.Preproc, selfName + ': reCheckNumOfGoodObs: ' + num + ' obs restored with QC= ' + thr +
                ' for baseline ' + bln.name);
        };
    };
};



//
function reCheckShortBaselines(limit)
{
    const prevWrms = session.bands[config.activeBandIdx].wrms;

    logger.write(Log.Inf, Log.Preproc, selfName + ': reCheckShortBaselines(): begin');

    var baselines = session.baselines;
    for (var i=0; i<baselines.length; i++)
    {
        var bln = baselines[i];
        if (bln.length < limit && 7 < bln.length)
        {
            if (!bln.isValid)
            {
                logger.write(Log.Inf, Log.Preproc, selfName + ': reCheckShortBaselines(): checking baseline ' + bln.name);
                bln.setIsValid(true);
                session.process();
                if (2*prevWrms < session.bands[config.activeBandIdx].wrms)
                {
                    bln.setIsValid(false);
                    logger.write(Log.Wrn, Log.Preproc, selfName + ': reCheckShortBaselines(): looks like it is better not to use baseline ' +
                        bln.name + ': the wrms are increasing: ' +  (prevWrms*1.0e12).toFixed(2) + ' => ' +
                        (session.bands[config.activeBandIdx].wrms*1.0e12).toFixed(2) + ' (ps)');
                    session.process();
                }
                else
                {
                    logger.write(Log.Inf, Log.Preproc, selfName + ': reCheckShortBaselines(): looks like baseline ' +
                        bln.name + ' is ok, wrms: ' +  (prevWrms*1.0e12).toFixed(2) + ' => ' +
                        (session.bands[config.activeBandIdx].wrms*1.0e12).toFixed(2) + ' (ps)');
                };
            }
            else
                logger.write(Log.Wrn, Log.Preproc, selfName + ': reCheckShortBaselines(): Warning: the baseline ' +
                    bln.name + ' already is turned on');
        };
    };
};



//
// make initial set up of a session:
//
function makeSetup()
{
    logger.write(Log.Inf, Log.Preproc, selfName + ': makeSetup(): making initial setup.');
    session.zeroIono();
    session.resetAllEditings();
    logger.write(Log.Inf, Log.Preproc, selfName + ': makeSetup(): edit info has been reset');

    //
    reCheckNumOfGoodObs();

    //
    // set up a reference clock station:
    session.pickupReferenceClocksStation();
    // set up a reference station station:
    session.pickupReferenceCoordinatesStation();
    // scan correlator's report and search for applied manual phase calibrations:
    session.checkUseOfManualPhaseCals();
    //
    session.setNumOfClockPolynoms4Stations(2);
    //
    setSigma2add(5.0e-9); // 5ns, since we start with SBD
    //
    checkStationNames('BBC');
    //
    hasShortBln = false;
    hasDeselectedBln = false;
    checkShortBaselines(7); // turn off baselines with length < 7m (usually, tests of BBC on the same antenna)
    //?    session.calcIono(true); // <- only for SB delays

    config.have2ApplyNdryContrib = false;
    config.have2ApplyNwetContrib = false;
    config.flybyTropZenithMap = CFG.TZM_NMF;
};




//
//
//
function check4BlnOutliersSBD()
{
    const prevWrms = session.bands[config.activeBandIdx].wrms;
    var baselines = session.baselines;
    var maxWrms = 0;
    var bln;
    var bln2rmKey;

    if (0 < baselines.length)
    {
//      print(' ++ checking ' + baselines.length + ' basleines');
        for (var i=0; i<baselines.length; i++)
        {
            bln = baselines[i];
            if (maxWrms < bln.wrms)
            {
                maxWrms = bln.wrms;
                bln2rmKey = bln.key;
            };
        };
//      print(' ++ the candidate: ' + bln2rmKey + ' wrms= ' + maxWrms + '; totWrms = ' + prevWrms);
        if (1.5*prevWrms < maxWrms && 1.0e-9 < maxWrms) //
        {
            print(' ++ the candidate: ' + bln2rmKey + ' ; ' + maxWrms + ' > 2* ' + prevWrms);
            bln = session.lookUpBaseline(bln2rmKey);
            bln.setIsValid(false);
            session.process();
            hasDeselectedBln = true;
            deselectedBlnKey = bln2rmKey;
        };
    };
};



//
//
//
//
function reCheckDeselectedBln()
{
    const prevWrms = session.bands[config.activeBandIdx].wrms;
    var bln;

    bln = session.lookUpBaseline(deselectedBlnKey);
    bln.setIsValid(true);

    print(' ++ veryfyng bln: ' + deselectedBlnKey);
    session.process();

    session.eliminateOutliersSimpleMode(config.activeBandIdx, 0.35*session.numOfObservations, 3.5, 1000.0e-9);
    session.scanAmbiguityMultipliers(config.activeBandIdx);
    session.process();

    if (1.5*prevWrms < session.bands[config.activeBandIdx].wrms && 1.0e-9 < session.bands[config.activeBandIdx].wrms)
    {
        print(' ++ still looks bad: last wrms = ' + session.bands[config.activeBandIdx].wrms*1.0e12 + ', prev wrms = ' + prevWrms*1.0e12 + ' ps');
        bln.setIsValid(false);
        session.process();
    }
    else
        print(' ++ looks ok: last wrms = ' + session.bands[config.activeBandIdx].wrms*1.0e12 + ', prev wrms = ' + prevWrms*1.0e12 + ' ps');

};






//
// resolve ambiguities:
//
function try2resolveAmbiguities()
{
    var maxNumOfPasses  = 0.35*session.numOfObservations;       //
    var threshold       = 4.0;                                  // sigmas
    var bands           = session.bands;
    //
    logger.write(Log.Inf, Log.Preproc, '');
    logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): Begin of resolving ambiguities.');
    logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): Set up estimated parameters (clocks only):');
    logger.write(Log.Inf, Log.Preproc, '');

    parsDescript.unsetAllParameters();
    parsDescript.setMode4Parameter(Parameters.Clocks,   Parameters.EstimateLocal);

    logger.write(Log.Inf, Log.Preproc, '');
    logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): Obtaining a solution for SbDelays:');
    logger.write(Log.Inf, Log.Preproc, '');

    config.useDelayType = CFG.VD_SB_DELAY;      // SBD
    config.activeBandIdx = bands.length - 1;    // S-band
    session.process();

    logger.write(Log.Inf, Log.Preproc, '');
    logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): the solution obtained.');


    // check for SB outliers:
    check4BlnOutliersSBD();

    //
    // initially, remove everything that is outp of 7*sigma, no thresholds for absolute values:
    session.eliminateLargeOutliers(config.activeBandIdx, maxNumOfPasses, 1.8);
    session.eliminateOutliersSimpleMode(config.activeBandIdx, maxNumOfPasses, 5.0, -1.0);
    session.eliminateOutliersSimpleMode(config.activeBandIdx, maxNumOfPasses, 4.0, 2000.0e-9);
    session.eliminateOutliersSimpleMode(config.activeBandIdx, maxNumOfPasses, 3.5, 1000.0e-9);

    checkShortBaselines(500);
    session.process();

    // if have a short baseline try to turn it back:
    if (hasShortBln)
        reCheckShortBaselines(500);

/*
    if (1 < session.baselines.length)
    {
        config.opMode = CFG.OPM_BAND;
        session.process();
        session.eliminateLargeOutliers(config.activeBandIdx, maxNumOfPasses, 1.8);
        session.eliminateOutliersSimpleMode(config.activeBandIdx, maxNumOfPasses, 5.0, 30.0e-9);
    };
*/

    //
    logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): Switching to GrpDelays:');
    config.useDelayType = CFG.VD_GRP_DELAY;
    config.opMode       = CFG.OPM_BASELINE;
    setSigma2add(20.0e-12); // 20ps, we switched to GrpDelay

    //
    logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): Checking each band:');
    logger.write(Log.Inf, Log.Preproc, '');
//  for (var i=0; i<bands.length; i++)
    for (var i=bands.length-1; -1<i; i--)
    {
        logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): Processing the ' + bands[i].key + '-band:');
        logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): Scanning ambig.multipliers');
        logger.write(Log.Inf, Log.Preproc, '');

        config.activeBandIdx = i;
        session.scanAmbiguityMultipliers(i);
        session.process();

        session.eliminateOutliersSimpleMode(config.activeBandIdx, maxNumOfPasses, 3.5, 1000.0e-9);
        session.scanAmbiguityMultipliers(i);
        session.process();

        session.eliminateOutliersSimpleMode(config.activeBandIdx, maxNumOfPasses, 3.5, 1000.0e-9);
//        session.eliminateOutliersSimpleMode(config.activeBandIdx, maxNumOfPasses, 3.0,  bands[i].getGroupDelaysAmbigSpacing()/2.0);
        session.scanAmbiguityMultipliers(i);
        session.process();

        //
        if (hasDeselectedBln)
        {
            reCheckDeselectedBln();
            session.eliminateOutliersSimpleMode(config.activeBandIdx, maxNumOfPasses, 3.5, 1000.0e-9);
            session.scanAmbiguityMultipliers(i);
            session.process();
//            session.eliminateOutliersSimpleMode(config.activeBandIdx, maxNumOfPasses, 3.0,  bands[i].getGroupDelaysAmbigSpacing()/2.0);
            session.scanAmbiguityMultipliers(i);
            session.process();
        };


        logger.write(Log.Inf, Log.Preproc, '');
        logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): Running another solution');
        logger.write(Log.Inf, Log.Preproc, '');

        session.process();

        logger.write(Log.Inf, Log.Preproc, '');
        logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): Calling eliminateOutliersSimpleMode():');
        logger.write(Log.Inf, Log.Preproc, '');

        // remove 5*sigma or if absolute value are bigger than 0.8 of typical ambig spacing:
        session.eliminateLargeOutliers(config.activeBandIdx, maxNumOfPasses, 1.4);
        session.eliminateOutliersSimpleMode(i, maxNumOfPasses, threshold, bands[i].groupDelaysAmbigSpacing*0.4);

        logger.write(Log.Inf, Log.Preproc, '');
        logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): the band is done.');
        logger.write(Log.Inf, Log.Preproc, '');
    };
    //
    //
    // add parameters and repeat:
    //
    logger.write(Log.Inf, Log.Preproc, '');
    logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): Setting the number of polynomial terms for station clocks to 3:');
    session.setNumOfClockPolynoms4Stations(3);
    logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): Set up estimated parameters (plus zenith delays and baseline vector):');
    parsDescript.setMode4Parameter(Parameters.Zenith,    Parameters.EstimateLocal);
    parsDescript.setMode4Parameter(Parameters.Bl_Length, Parameters.EstimateLocal);
    logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): Obtaining an intermediate solution:');

    session.process();

    logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): a solution is obtained.');
    logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): Checking each band again:');
    logger.write(Log.Inf, Log.Preproc, '');

    threshold           = 4.0;          // sigmas
    for (var i=bands.length-1; -1<i; i--)
    {
        logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): Processing the ' + bands[i].key + '-band:');
        logger.write(Log.Inf, Log.Preproc, '');

        config.activeBandIdx = i;
        session.process();
        session.eliminateOutliersSimpleMode(i, maxNumOfPasses, threshold, bands[i].groupDelaysAmbigSpacing*0.3);


        session.scanAmbiguityMultipliers(i);
        session.process();
        session.scanAmbiguityMultipliers(i);
        session.process();
        session.scanAmbiguityMultipliers(i);
        session.process();

        logger.write(Log.Inf, Log.Preproc, '');

        session.process();
        logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): ' + bands[i].key + '-band: done.');
        logger.write(Log.Inf, Log.Preproc, '');
    };
    logger.write(Log.Inf, Log.Preproc, '');
    logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): finished.');
    logger.write(Log.Inf, Log.Preproc, '');
};





//
// check clock breaks:
//
function checkClockBreaks()
{
    var bands = session.bands;
    logger.write(Log.Inf, Log.Preproc, '');

    logger.write(Log.Inf, Log.Preproc, '');
    logger.write(Log.Inf, Log.Preproc, selfName + ': checkClockBreaks(): Begin of clock break checking.');
    logger.write(Log.Inf, Log.Preproc, '');

    config.activeBandIdx = session.primaryBandIdx;
    config.useDelayType  = CFG.VD_GRP_DELAY;
    logger.write(Log.Inf, Log.Preproc, '');

    logger.write(Log.Inf, Log.Preproc, '');
    logger.write(Log.Inf, Log.Preproc, selfName + ': checkClockBreaks(): Obtaining an intermediate solution:');
    logger.write(Log.Inf, Log.Preproc, '');

    session.process();

    logger.write(Log.Inf, Log.Preproc, '');
    logger.write(Log.Inf, Log.Preproc, selfName + ': checkClockBreaks(): done.');
    logger.write(Log.Inf, Log.Preproc, '');

    for (var i=0; i<bands.length; i++)
    {
        logger.write(Log.Inf, Log.Preproc, '');
        logger.write(Log.Inf, Log.Preproc, selfName + ': checkClockBreaks(): Processing the ' + bands[i].key + '-band:');
        logger.write(Log.Inf, Log.Preproc, '');
        session.checkClockBreaks(i);
        logger.write(Log.Inf, Log.Preproc, '');
        logger.write(Log.Inf, Log.Preproc, selfName + ': checkClockBreaks(): done.');
        logger.write(Log.Inf, Log.Preproc, '');
    };
    logger.write(Log.Inf, Log.Preproc, '');
    logger.write(Log.Inf, Log.Preproc, selfName + ': checkClockBreaks(): finished.');
    logger.write(Log.Inf, Log.Preproc, '');
};



//
//
// a baseline has significant clock offset, like ONSA13NE:ONSA13SW at 22SEP25VC:
function checkSpecialCase01()
{
    var baselines = session.baselines;
    var stations = session.stations;
    var refClockStnKey;
    var shortestBlnKey;
    var shortestBlnLength;
    var haveLargeClockOffset;


    logger.write(Log.Inf, Log.Preproc, selfName + ': checkSpecialCase01(): begin');

    haveLargeClockOffset = false;

    for (var i=0; i<stations.length; i++)
    {
        if (stations[i].referenceClocks)
        {
            refClockStnKey = stations[i].key;
            print(' -- clkRefStn: ' + refClockStnKey);
        };
    };

    parsDescript.unsetAllParameters();
    parsDescript.setMode4Parameter(Parameters.Clocks,   Parameters.EstimateLocal);
    parsDescript.setMode4Parameter(Parameters.Zenith,   Parameters.EstimateLocal);
    parsDescript.setMode4Parameter(Parameters.PolusUt1, Parameters.EstimateLocal);
    parsDescript.setMode4Parameter(Parameters.Bl_Clk,   Parameters.EstimateLocal);
    config.useDelayType = CFG.VD_GRP_DELAY;
    config.activeBandIdx= session.primaryBandIdx;

    shortestBlnLength = -1;
    for (var i=0; i<baselines.length; i++)
    {
        if (baselines[i].key.indexOf(refClockStnKey) == -1)
        {
            print(' -- got ' + baselines[i].key);
            baselines[i].estimateClocks = true;
        }
        else
            print(' -- ' + baselines[i].key + ' not the one');
        //
        if (shortestBlnLength < 0)
        {
            shortestBlnLength = baselines[i].length;
            shortestBlnKey = baselines[i].key;
        }
        else
        {
            if (baselines[i].length < shortestBlnLength)
            {
                shortestBlnLength = baselines[i].length;
                shortestBlnKey = baselines[i].key;
            };
        };
        print(' -- baseline ' + shortestBlnKey + ' is a candidate for deslection');

    };
    session.process();
    //
    // check the solution:
    for (var i=0; i<baselines.length; i++)
    {
        if (baselines[i].estimateClocks && 
            150e-12 < Math.abs(baselines[i].clockOffset) &&                                     // 150ns, is it enough?
            7*baselines[i].clockOffsetStdDev < Math.abs(baselines[i].clockOffset))              // 7sigmas?
        {
            haveLargeClockOffset = true;
            print(' -- baseline ' + baselines[i].key + ' has large clock offset: ' + baselines[i].clockOffset + ' +/- ' + baselines[i].clockOffsetStdDev);
        };
    };
    // do it:
    if (haveLargeClockOffset)
    {
        var bln = session.lookUpBaseline(shortestBlnKey);
        bln.setIsValid(false);
        print(' -- baseline ' + bln.key + ' is deslected');
        session.process();
    };
    logger.write(Log.Inf, Log.Preproc, selfName + ': checkSpecialCase01(): end');
};




//
// do reweighting and outliers:
//
function doReweightAndOutliers()
{
    var primaryBand = session.bands[session.primaryBandIdx];
    // make a full solution:
    logger.write(Log.Inf, Log.Preproc, '');
    logger.write(Log.Inf, Log.Preproc, selfName + ': doReweightAndOutliers(): Set up estimated parameters for full solution:');
    logger.write(Log.Inf, Log.Preproc, '');

    session.setNumOfClockPolynoms4Stations(3);
    parsDescript.unsetAllParameters();
    parsDescript.setMode4Parameter(Parameters.Clocks,   Parameters.EstimateLocal);
    parsDescript.setMode4Parameter(Parameters.Zenith,   Parameters.EstimateLocal);
    parsDescript.setMode4Parameter(Parameters.PolusUt1, Parameters.EstimateLocal);
//  parsDescript.setMode4Parameter(Parameters.Bl_Clk,   Parameters.EstimateLocal);

    logger.write(Log.Inf, Log.Preproc, '');
    logger.write(Log.Inf, Log.Preproc, selfName + ': doReweightAndOutliers(): done.');
    logger.write(Log.Inf, Log.Preproc, selfName + ': doReweightAndOutliers(): Configure the solution:');
    logger.write(Log.Inf, Log.Preproc, '');

    config.useDelayType = CFG.VD_GRP_DELAY;
    config.activeBandIdx= session.primaryBandIdx;
    config.opThreshold  = 3.0;
    config.opMode       = CFG.OPM_BASELINE;
    config.opAction     = CFG.OPA_RESTORE;

    logger.write(Log.Inf, Log.Preproc, '');
    logger.write(Log.Inf, Log.Preproc, selfName + ': doReweightAndOutliers(): done.');
    logger.write(Log.Inf, Log.Preproc, selfName + ': doReweightAndOutliers(): Reweighting (first run):');
    logger.write(Log.Inf, Log.Preproc, '');

    session.doReWeighting();

    logger.write(Log.Inf, Log.Preproc, '');
    logger.write(Log.Inf, Log.Preproc, selfName + ': doReweightAndOutliers(): done.');
    logger.write(Log.Inf, Log.Preproc, selfName + ': doReweightAndOutliers(): Begin of reweighting/outlier processing iteration:');
    logger.write(Log.Inf, Log.Preproc, '');

    var numOfRestored = 0;
    var numOfEliminated = 0;
    var roiCounter = 0;
    var mdfData=0;
    do
    {
        if ( (numOfRestored = session.restoreOutliers(session.primaryBandIdx)) )
            session.doReWeighting();
        if ( (numOfEliminated = session.eliminateOutliers(session.primaryBandIdx)) )
            session.doReWeighting();
        mdfData += numOfRestored + numOfEliminated;
        roiCounter++;
    }
    while ((numOfRestored + numOfEliminated) && (10 < primaryBand.numProcessed) && (roiCounter < 100));

    logger.write(Log.Inf, Log.Preproc, '');

    if (10 < primaryBand.numProcessed)
    {
    isAutoGood

        if (roiCounter < 100)
            logger.write(Log.Inf, Log.Preproc, selfName + ': doReweightAndOutliers(): End of reweighting/outlier processing iteration. Counter=' +
                roiCounter + ', number of modified observations: ' + mdfData);
        else
            logger.write(Log.Wrn, Log.Preproc, selfName + ': doReweightAndOutliers(): Reweighting/outlier iteration counter=' +
                roiCounter + ' is too big, number of modified observations: ' + mdfData);
    }
    else
    {
        isAutoGood = false;
        logger.write(Log.Wrn, Log.Preproc, selfName + ': doReweightAndOutliers(): number of usable observations, ' +
            primaryBand.numProcessed + ', is too low, unable to analyze a session');
    };

    logger.write(Log.Inf, Log.Preproc, '');

    session.process();
    session.process();
    session.process();
    session.process();
    session.process();

    //
    // trun off the flag:
    config.doWeightCorrection = false;
};


//
//
// make some outut:
//
function printInfo()
{
    var sIds;
    sIds = '=';
    for (var i=0; i<session.stations.length; i++)
    {
        sIds += session.stations[i].name;
        if (i < session.stations.length - 1)
            sIds += ':';
    };
    sIds += '=';


    var primaryBand = session.bands[session.primaryBandIdx];
    print(' -- printInfo(): Session name           : ' + session.name);
    print(' -- printInfo(): Session scheduled by   : ' + session.schedulerName);
    print(' -- printInfo(): Session submitted by   : ' + session.submitterName);
    print(' -- printInfo(): Session correlator name: ' + session.correlatorName);
    print(' -- printInfo(): Session official name  : ' + session.officialName);
    print(' -- printInfo(): Session sessionCode    : ' + session.sessionCode);
    print(' -- printInfo(): Session description    : ' + session.description);
    print(' -- printInfo(): Session suffix         : ' + session.networkSuffix);
    print(' -- printInfo(): Session networkID      : ' + session.networkID);
    print(' -- printInfo(): Session #bands         : ' + session.numOfBands);
    print(' -- printInfo(): Session #stations      : ' + session.numOfStations);
    print(' -- printInfo(): Session #baseline      : ' + session.numOfBaselines);
    print(' -- printInfo(): Session #sources       : ' + session.numOfSources);
    print(' -- printInfo(): Session #observs       : ' + session.numOfObservations);
    print(' -- printInfo(): Session created on     : ' + session.tCreation);
    print(' -- printInfo(): Session started on     : ' + session.tStart);
    print(' -- printInfo(): Session stopped on     : ' + session.tFinis);
    print(' -- printInfo(): Session mean epoch     : ' + session.tMean);

    print(' -- printInfo(): Primary band key       : ' + primaryBand.key);
    print(' -- printInfo(): Epoch of the first obs : ' + primaryBand.tFirst);
    print(' -- printInfo(): Epoch of the last obs  : ' + primaryBand.tLast);
    print(' -- printInfo(): Number of total obs    : ' + primaryBand.numTotal);
    print(' -- printInfo(): Number of usable obs   : ' + primaryBand.numUsable);
    print(' -- printInfo(): Number of used obs     : ' + primaryBand.numProcessed);
    print(' -- printInfo(): WRMS                   : ' + (primaryBand.wrms*1.0e12).toFixed(2) + ' (ps)');
    print(' -- printInfo(): DoF                    : ' + primaryBand.dof);
    print(' -- printInfo(): Chi^2/DoF              : ' + (primaryBand.chi2/primaryBand.dof).toFixed(4));

    print(' -- printInfo(): Numbers                : ' + primaryBand.numTotal + '/' + primaryBand.numUsable + '/' + primaryBand.numProcessed +
        '  ' + (primaryBand.wrms*1.0e12).toFixed(2) + '    UT1= ' + (session.dUt1Value*86400*1.0e6).toFixed(2) +
        '  ' + (session.dUt1Correction*86400*1.0e6).toFixed(2) + '  ' + (session.dUt1StdDev*86400*1.0e6).toFixed(2) +
        '  ' + sIds + ' ' + session.correlatorName + ' ' + session.correlatorType + '   ' + ((session.tFinis - session.tStart)/1000.0).toFixed(2) +
        '  ' + ((session.tFinis - session.tStart)/1000.0/60).toFixed(2)
         );
};




function main()
{
    const       pwd = setup.pwd;
    const       sfx = "/o-aut";

    need2save = false;

    // check arguments:
    if (!args.length)
    {
        print('\nscript ' + selfName + ' usage:\n');
        print(selfName + ' input [save_database]');
        print('where arguments:');
        print('   input         -- a database name (with or without version part) or a wrapper file name');
        print('   save_database -- an optional argument, if it set to "yes", the processed database will be saved as a new version');
        return;
    };

    if (1 < args.length)
    {
        if (args[1] == 'yes')
            need2save = true;
    };


    handler.fileName  = args[0];
    handler.inputType = 'VDB';
    //
    //
    //
    if (isTesting)
    {
//      setup.path2VgosDbFiles     = pwd + '/vgosDb';
        setup.path2SpoolFileOutput = pwd + sfx + '/Spools';
        setup.path2ReportOutput    = pwd + sfx + '/Reports';
        //setup.have2KeepSpoolFileReports = true;
        //
        // set up logging:
        logger.dirName = pwd + sfx + '/Logs';
        logger.fileName = handler.guessedSessionName + '.log';
        logger.have2store = true;
        logger.rmLogFile();
    };

    logger.isMute = true;
    logger.write(Log.Inf, Log.Preproc, selfName + ': Starting processing ' + handler.fileName + ' file');

    handler.importSession();
    //
    //
    if (session.isOk)
    {
        //
        makeSetup();
        //
        try2resolveAmbiguities();
        //
//      checkClockBreaks();
        //
        config.activeBandIdx = session.primaryBandIdx;
        logger.write(Log.Inf, Log.Preproc, selfName + ': calculation inosphere corrections:');
        session.calcIono();
        session.process();
        //
        if (3 <= session.baselines.length)
            checkSpecialCase01();


        logger.write(Log.Inf, Log.Preproc, selfName + ': obtaining another solution: done');
        //
        doReweightAndOutliers();
        //
        if (isAutoGood)
        {
            if (need2save)
                handler.saveResults();
            //
            handler.generateReport();
            //
            printInfo();
        };
        //
        logger.write(Log.Inf, Log.Preproc, selfName + ': the session is done: ' + handler.fileName);
    }
    else
        logger.write(Log.Err, Log.Preproc, selfName + ': session reading failed: ' + handler.fileName);
};










main();




//for(var key in config) print(key + ' => ' + config[key]);







