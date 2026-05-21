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
//    autoINT.js script
//    Version:   0.1.0
//    Released:  04/03/2023
//


//const   isTesting = true;

const   isTesting = false;

// a separate directory for per session logs:
const   prefixDir = "autoINT";
//const   prefixDir = "";


//
const   thresholdNumProcessedObs        =  10;
const   thresholdPartOfProcessedObs     = 0.5;
const   thresholdWRMS                   = 130; // ps
const   thresholdUT1adjustment          = 800; // microsec
const   thresholdUT1sigma               = 100; // microsec
const   thresholdSessionLength          =  25; // if session shorter than 25 minutes, do it yourself

//
//
//
const   selfName = 'autoINT.js';
var     hasShortBln;
var     hasDeselectedBln;
var     deselectedBlnKey;
var     need2save;

var     isAutoGood = true;

var     isVgos = false;

var     anomalies = [];
var     reasonOfFailure = [];


//
const stnKey2estimate = Array('NYALE13S');



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
            anomalies.push('Deselected station ' + stn.name);
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

    if (isVgos)
        thr = config.qualityCodeThreshold;

    var pBand = session.bands[session.primaryBandIdx];
    var primaryBandKey = pBand.key;
    var baselines = pBand.baselines;

    for (var j=0; j<baselines.length; j++)
    {
        var bln = baselines[j];
        if (isTesting)
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
                    anomalies.push('Deselected baseline ' + bln.name);
                }
                else
                {
                    logger.write(Log.Inf, Log.Preproc, selfName + ': reCheckShortBaselines(): looks like baseline ' +
                        bln.name + ' is ok, wrms: ' +  (prevWrms*1.0e12).toFixed(2) + ' => ' +
                        (session.bands[config.activeBandIdx].wrms*1.0e12).toFixed(2) + ' (ps)');
                    anomalies.push('Selected back baseline ' + bln.name);
                };
            }
            else
                logger.write(Log.Wrn, Log.Preproc, selfName + ': reCheckShortBaselines(): Warning: the baseline ' +
                    bln.name + ' already is turned on');
        };
    };
};





//
// checks is it a VGOS setup or not:
//
function checkIsVgos()
{
    isVgos = false;
    if (session.bands.length == 1 && 24 < session.bands[0].numOfChannels)
        isVgos = true;
};





//
// make initial set up of a session:
//
function makeSetup()
{
    // collect some info about the session:
    checkIsVgos();
    if (isVgos)
        logger.write(Log.Inf, Log.Preproc, selfName + ': makeSetup(): this is a VGOS session');
    else
        logger.write(Log.Inf, Log.Preproc, selfName + ': makeSetup(): this is an S/X session');

    // make the initial setup:
    logger.write(Log.Inf, Log.Preproc, selfName + ': makeSetup(): making initial setup.');
    session.zeroIono();
    session.resetAllEditings();
    logger.write(Log.Inf, Log.Preproc, selfName + ': makeSetup(): edit info has been reset');

    //
    reCheckNumOfGoodObs();

    //
    // set up a reference clock station:
    session.pickupReferenceClocksStation();
    // ->
    // ->    // set up a reference station station:
    // ->    session.pickupReferenceCoordinatesStation();
    // ->
    // turn off estimate coordinates flag for all stations:
    for (var i=0; i<session.stations.length; i++)
    {
        session.stations[i].estimateCoords = false;
        logger.write(Log.Inf, Log.Preproc, selfName + ': makeSetup(): coords off for ' + session.stations[i].key);
    };

    // scan correlator's report and search for applied manual phase calibrations:
    session.checkUseOfManualPhaseCals();
    //
    session.setNumOfClockPolynoms4Stations(2);
    //
    setSigma2add(100.0e-12); // 100ps, since we start with SBD
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

    logger.write(Log.Inf, Log.Preproc, selfName + ': check4BlnOutliersSBD(): begin.');

    if (0 < baselines.length)
    {
        if (isTesting)
            print(' ++ checking ' + baselines.length + ' basleines');
        for (var i=0; i<baselines.length; i++)
        {
            bln = baselines[i];
            if (maxWrms < bln.wrms)
            {
                maxWrms = bln.wrms;
                bln2rmKey = bln.key;
            };
        };
        if (isTesting)
            print(' ++ the candidate: ' + bln2rmKey + ' wrms= ' + maxWrms + '; totWrms = ' + prevWrms);
        if (1.5*prevWrms < maxWrms && 1.0e-9 < maxWrms) //
        {
            if (isTesting)
                print(' ++ the candidate: ' + bln2rmKey + ' ; ' + maxWrms + ' > 2* ' + prevWrms);
            logger.write(Log.Inf, Log.Preproc, selfName + ': check4BlnOutliersSBD(): the baseline ' + bln2rmKey + ' is deselected');
            bln = session.lookUpBaseline(bln2rmKey);
            bln.setIsValid(false);
            session.process();
            hasDeselectedBln = true;
            deselectedBlnKey = bln2rmKey;
            anomalies.push('Baseline ' + bln.name + ' has big SB delay residuals');
        };
    };
    logger.write(Log.Inf, Log.Preproc, selfName + ': check4BlnOutliersSBD(): end.');
};



//
//
//
//
function reCheckDeselectedBln()
{
    const prevWrms = session.bands[config.activeBandIdx].wrms;
    var bln;

    logger.write(Log.Inf, Log.Preproc, selfName + ': reCheckDeselectedBln(): begin.');

    bln = session.lookUpBaseline(deselectedBlnKey);
    bln.setIsValid(true);

    if (isTesting)
        print(' ++ veryfyng bln: ' + deselectedBlnKey);
    logger.write(Log.Inf, Log.Preproc, selfName + ': reCheckDeselectedBln(): verifying deselected ' + deselectedBlnKey + ' baseline');
    session.process();

    session.eliminateOutliersSimpleMode(config.activeBandIdx, 0.35*session.numOfObservations, 3.5, 1000.0e-9);
    session.scanAmbiguityMultipliers(config.activeBandIdx);
    session.process();
//    session.eliminateOutliersSimpleMode(config.activeBandIdx, 0.35*session.numOfObservations, 3.0,  100.0e-9);
//    session.scanAmbiguityMultipliers(config.activeBandIdx);
//    session.process();

    if (1.5*prevWrms < session.bands[config.activeBandIdx].wrms && 1.0e-9 < session.bands[config.activeBandIdx].wrms)
    {
        if (isTesting)
            print(' ++ still looks bad: last wrms = ' + session.bands[config.activeBandIdx].wrms*1.0e12 + ', prev wrms = ' + prevWrms*1.0e12 + ' ps');
        logger.write(Log.Inf, Log.Preproc, selfName + ': reCheckDeselectedBln(): ' + deselectedBlnKey + ' still looks bad');
        bln.setIsValid(false);
        session.process();
    }
    else
    {
        if (isTesting)
            print(' ++ looks ok: last wrms = ' + session.bands[config.activeBandIdx].wrms*1.0e12 + ', prev wrms = ' + prevWrms*1.0e12 + ' ps');
    };
    logger.write(Log.Inf, Log.Preproc, selfName + ': reCheckDeselectedBln(): end.');
};






//
// resolve ambiguities:
//
function try2resolveAmbiguities()
{
    var maxNumOfPasses  = 0.35*session.numOfObservations;       //
    var threshold       = 4.0;                                  // sigmas
    var bands           = session.bands;
    var numOfClockBreaks= 0;
    var totalNumOfCB    = 0;
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
    logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): initial cleaning: begin');
    session.eliminateLargeOutliers(config.activeBandIdx, maxNumOfPasses, 1.8);
    session.eliminateOutliersSimpleMode(config.activeBandIdx, maxNumOfPasses, 5.0, -1.0);
    session.eliminateOutliersSimpleMode(config.activeBandIdx, maxNumOfPasses, 4.0, 2000.0e-9);
    session.eliminateOutliersSimpleMode(config.activeBandIdx, maxNumOfPasses, 3.5, 1000.0e-9);
    session.eliminateOutliersSimpleMode(config.activeBandIdx, maxNumOfPasses, 3.0,  500.0e-9);
    logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): initial cleaning: done');

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
    for (var i=bands.length-1; -1<i; i--)
    {
        if (isTesting)
            print('Initial processing of the ' + bands[i].key + '-band:');
        logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): Processing the ' + bands[i].key + '-band:');
        logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): Scanning ambig.multipliers');
        logger.write(Log.Inf, Log.Preproc, '');

        config.activeBandIdx = i;
        //
        if (0 < totalNumOfCB) // there is a clock break, estimate first:
        {
            session.process();
        };
        //
        session.scanAmbiguityMultipliers(i);
        session.process();

//      session.eliminateOutliersSimpleMode(config.activeBandIdx, maxNumOfPasses, 3.5,  400.0e-9);
        session.eliminateOutliersSimpleMode(config.activeBandIdx, maxNumOfPasses, 3.5, 1000.0e-9);
        session.process();
        session.scanAmbiguityMultipliers(i);
        session.process();

            session.eliminateLargeOutliers(config.activeBandIdx, maxNumOfPasses, 1.2);
            session.process();
            session.scanAmbiguityMultipliers(i);
            session.process();




        // check for clock breaks:
        if ((numOfClockBreaks=session.checkClockBreaks(i)))
        {
            logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): found ' +
                numOfClockBreaks + ' clock breaks in ' + bands[i].key + '-band:');
            session.process();
            totalNumOfCB += numOfClockBreaks;
            if (isTesting)
                print(' .. totalNumOfCB = ' + totalNumOfCB + ', band=' + bands[i].key);
            anomalies.push('A clock break is detected');
        };
        /*
        session.eliminateOutliersSimpleMode(config.activeBandIdx, maxNumOfPasses, 3.2, 100.0e-9);
        session.scanAmbiguityMultipliers(i);
        session.process();
        */

//      session.eliminateOutliersSimpleMode(config.activeBandIdx, maxNumOfPasses, 3.0,  0.5*bands[i].getGroupDelaysAmbigSpacing());
//        session.scanAmbiguityMultipliers(i);
//        session.process();

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


        logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): Running another solution');
        session.process();
        logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): Calling eliminateOutliersSimpleMode():');

        // remove 5*sigma or if absolute value are bigger than 0.8 of typical ambig spacing:
        session.eliminateLargeOutliers(config.activeBandIdx, maxNumOfPasses, 1.4);
        session.eliminateOutliersSimpleMode(i, maxNumOfPasses, threshold, bands[i].groupDelaysAmbigSpacing*0.4);

        /**/
        if (5.0e-9 < session.bands[config.activeBandIdx].wrms)
        {
            logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): too special case: begin');
            session.eliminateLargeOutliers(config.activeBandIdx, maxNumOfPasses, 1.2);
            session.eliminateOutliersSimpleMode(i, maxNumOfPasses, 3.2, bands[i].groupDelaysAmbigSpacing*0.5);
            session.eliminateLargeOutliers(config.activeBandIdx, maxNumOfPasses, 1.2);
            session.eliminateOutliersSimpleMode(i, maxNumOfPasses, 3.0, bands[i].groupDelaysAmbigSpacing*0.2);
            logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities():: too special case: end');

            session.scanAmbiguityMultipliers(i);
            session.process();
        };
        /**/


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
    //
    // check for presence of new stations, if found turn on estimation of their coordinates:
    for (var i=0; i<stnKey2estimate.length; i++)
    {
        var stn = session.lookUpStation(stnKey2estimate[i]);
        if (stn === null)
        {
        }
        else
        {
            stn.estimateCoords = true;
            logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): estimation of ' + stn.key + ' coordinates is turned on');
        };
    };

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

        if ((numOfClockBreaks=session.checkClockBreaks(i)))
        {
            logger.write(Log.Inf, Log.Preproc, selfName + ': try2resolveAmbiguities(): found ' +
                numOfClockBreaks + ' another clock breaks in ' + bands[i].key + '-band:');
            session.process();
            anomalies.push('A clock break is detected');
        };

        session.scanAmbiguityMultipliers(i);
        session.process();
        //->
//        session.eliminateOutliersSimpleMode(i, maxNumOfPasses, threshold, bands[i].groupDelaysAmbigSpacing*0.35);
        session.scanAmbiguityMultipliers(i);
        session.process();
        //->
//        session.eliminateOutliersSimpleMode(i, maxNumOfPasses, 3.0, bands[i].groupDelaysAmbigSpacing*0.30);
        session.scanAmbiguityMultipliers(i);
        session.process();
        //->
//        session.eliminateOutliersSimpleMode(i, maxNumOfPasses, 3.0, bands[i].groupDelaysAmbigSpacing*0.25);
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
            if (isTesting)
                print(' -- clkRefStn: ' + refClockStnKey);
        };
    };

    parsDescript.unsetAllParameters();
    parsDescript.setMode4Parameter(Parameters.Clocks,   Parameters.EstimateLocal);
    parsDescript.setMode4Parameter(Parameters.Zenith,   Parameters.EstimateLocal);
    parsDescript.setMode4Parameter(Parameters.PolusUt1, Parameters.EstimateLocal);
    parsDescript.setMode4Parameter(Parameters.StnCoo,   Parameters.EstimateLocal);
    parsDescript.setMode4Parameter(Parameters.Bl_Clk,   Parameters.EstimateLocal);
    config.useDelayType = CFG.VD_GRP_DELAY;
    config.activeBandIdx= session.primaryBandIdx;

    shortestBlnLength = -1;
    for (var i=0; i<baselines.length; i++)
    {
        if (baselines[i].key.indexOf(refClockStnKey) == -1)
            baselines[i].estimateClocks = true;
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
    };
    if (isTesting)
        print(' -- baseline ' + shortestBlnKey + ' is a candidate for deslection');
    session.process();
    //
    // check the solution:
    for (var i=0; i<baselines.length; i++)
    {
        if (baselines[i].estimateClocks)
        {
            if (150e-12 < Math.abs(baselines[i].clockOffset) &&                                     // 150ns, is it enough?
                7*baselines[i].clockOffsetStdDev < Math.abs(baselines[i].clockOffset))              // 7sigmas?
            {
                haveLargeClockOffset = true;
                logger.write(Log.Inf, Log.Preproc, selfName + ': checkSpecialCase01(): baseline ' + baselines[i].key + ' has large clock offset: ' +
                    baselines[i].clockOffset*1.0e9 + ' +- ' + baselines[i].clockOffsetStdDev*1.0e9 + ' ns');
                if (isTesting)
                    print(' -- baseline ' + baselines[i].key + ' has large clock offset: ' + baselines[i].clockOffset + ' +/- ' + baselines[i].clockOffsetStdDev);
            }
            else
            {
                logger.write(Log.Inf, Log.Preproc, selfName + ': checkSpecialCase01(): baseline ' + baselines[i].key + ' has not so large clock offset: ' +
                    baselines[i].clockOffset*1.0e9 + ' +- ' + baselines[i].clockOffsetStdDev*1.0e9 + ' ns');
                if (isTesting)
                    print(' -- baseline ' + baselines[i].key + ' has not so large clock offset: ' + baselines[i].clockOffset + ' +/- ' + baselines[i].clockOffsetStdDev);
            };
            // turn the clocks off:
            baselines[i].estimateClocks = false;
        };
    };
    // do it:
    if (haveLargeClockOffset)
    {
        var bln = session.lookUpBaseline(shortestBlnKey);
        bln.setIsValid(false);
        if (isTesting)
            print(' -- baseline ' + bln.key + ' is deslected');
        logger.write(Log.Inf, Log.Preproc, selfName + ': checkSpecialCase01(): baseline ' + bln.key + ' is deslected');
    };
    session.process();
    logger.write(Log.Inf, Log.Preproc, selfName + ': checkSpecialCase01(): end');
};






//
// check cables:
//
function checkCableCals()
{
    var stations = session.stations;
    var wrmsByStation = Object;

    parsDescript.unsetAllParameters();
    parsDescript.setMode4Parameter(Parameters.Clocks,   Parameters.EstimateLocal);
    parsDescript.setMode4Parameter(Parameters.Zenith,   Parameters.EstimateLocal);
    parsDescript.setMode4Parameter(Parameters.PolusUt1, Parameters.EstimateLocal);
    parsDescript.setMode4Parameter(Parameters.StnCoo,   Parameters.EstimateLocal);
    parsDescript.setMode4Parameter(Parameters.Bl_Clk,   Parameters.EstimateLocal);
    config.useDelayType = CFG.VD_GRP_DELAY;
    config.activeBandIdx= session.primaryBandIdx;

    //
    session.process();
    for (var i=0; i<stations.length; i++)
    {
        wrmsByStation[stations[i].key] = stations[i].wrms;
//      print(' .. station ' + stations[i].key + ' => ' + wrmsByStation[stations[i].key]*1.0e12  + ' ps wrms');
    };
    //
    for (var i=0; i<stations.length; i++)
    {
        if (stations[i].useCableCal)
        {
            stations[i].useCableCal = false;
            session.process();
            if (1.1 < wrmsByStation[stations[i].key]/stations[i].wrms)
            {
                logger.write(Log.Inf, Log.Preproc, selfName + ': checkCableCals(): wrms of station ' + stations[i].key +
                    ' are better without cable cals');
                anomalies.push('Station ' + stations[i].name + ' has suspicious cable cals');
            }
            else
            {
                // restore:
                stations[i].useCableCal = true;
                session.process();

            };
        };
    };

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
    parsDescript.setMode4Parameter(Parameters.StnCoo,   Parameters.EstimateLocal);
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

    if (thresholdNumProcessedObs < primaryBand.numProcessed)
    {
        isAutoGood = true;
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
        reasonOfFailure.push('Too small number of used observations: ' +  primaryBand.numProcessed);
        logger.write(Log.Wrn, Log.Preproc, selfName + ': doReweightAndOutliers(): number of usable observations, ' +
            primaryBand.numProcessed + ', is too low, unable to analyze a session');
    };

    logger.write(Log.Inf, Log.Preproc, '');

    for (var i=0; i<7; i++)
        session.process();


    // check what we have:
    // tooo much observations are deselected:
    if (primaryBand.numProcessed/primaryBand.numUsable < thresholdPartOfProcessedObs)
    {
        reasonOfFailure.push('Too many good observations are deselected: ' +  (primaryBand.numUsable - primaryBand.numProcessed) + ' of ' + primaryBand.numUsable);
        isAutoGood = false;
    };
    // tooo large wrms:
    if (thresholdWRMS < primaryBand.wrms*1.0e12)
    {
        reasonOfFailure.push('Too large residuals: ' +  (primaryBand.wrms*1.0e12).toFixed(2) + ' ps');
        isAutoGood = false;
    };
    // tooo large dUT1 correction:
    if (thresholdUT1adjustment < Math.abs(session.dUt1Correction*86400*1.0e6))
    {
        reasonOfFailure.push('Too large UT1 corrections: ' + (session.dUt1Correction*86400*1.0e6).toFixed(2) + ' microsec');
        isAutoGood = false;
    };
    // tooo large UT1 sigma:
    if (thresholdUT1sigma < Math.abs(session.dUt1StdDev*86400*1.0e6))
    {
        reasonOfFailure.push('Too large UT1 std.dev: ' + (session.dUt1StdDev*86400*1.0e6).toFixed(2) + ' microsec');
        isAutoGood = false;
    };
    // tooo short interval:
    if ((session.tFinis - session.tStart)/1000.0/60 < thresholdSessionLength)
    {
        reasonOfFailure.push('Too short interval of used observations: ' + ((session.tFinis - session.tStart)/1000.0/60).toFixed(2) + ' min');
        isAutoGood = false;
    };


    //
    // trun off the flag:
    config.doWeightCorrection = false;
};




//
//
// make some outut:
//
function printInfo4Debugging()
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

    for (var i=0; i<anomalies.length; i++)
        print(' -- printInfo(): Anomalies              : ' + anomalies[i]);

    print(' -- printInfo(): Numbers                : ' + primaryBand.numTotal + '/' + primaryBand.numUsable + '/' + primaryBand.numProcessed +
        '  ' + (primaryBand.wrms*1.0e12).toFixed(2) + '    UT1= ' + (session.dUt1Value*86400*1.0e6).toFixed(2) +
        '  ' + (session.dUt1Correction*86400*1.0e6).toFixed(2) + '  ' + (session.dUt1StdDev*86400*1.0e6).toFixed(2) +
        '  ' + sIds + ' ' + session.correlatorName + ' ' + session.correlatorType + '   ' + ((session.tFinis - session.tStart)/1000.0).toFixed(2) +
        '  ' + ((session.tFinis - session.tStart)/1000.0/60).toFixed(2)
         );
};




function printInfo()
{
    var primaryBand = session.bands[session.primaryBandIdx];
    var sIds = "";
//  sIds = '=';
    for (var i=0; i<session.stations.length; i++)
    {
        sIds += session.stations[i].name;
        if (i < session.stations.length - 1)
            sIds += ':';
    };
//  sIds += '=';

    print('Session name           : ' + session.name);
    print('Session scheduled by   : ' + session.schedulerName);
    print('Session submitted by   : ' + session.submitterName);
    print('Session correlator name: ' + session.correlatorName);
    print('Session official name  : ' + session.officialName);
    print('Session description    : ' + session.description);
    print('Number of bands        : ' + session.numOfBands);
    print('Number of stations     : ' + session.numOfStations);
    print('Number of baselines    : ' + session.numOfBaselines);
    print('Number of sources      : ' + session.numOfSources);
    print('List of stations       : ' + sIds);
    print('Session created on     : ' + session.tCreation);
    print('Session started on     : ' + session.tStart);
    print('Session stopped on     : ' + session.tFinis);
    print('Session mean epoch     : ' + session.tMean);
    print('Session interval       : ' + ((session.tFinis - session.tStart)/1000.0/60).toFixed(2) + ' min');

    print('Number of total obs    : ' + primaryBand.numTotal);
    print('Number of usable obs   : ' + primaryBand.numUsable);
    print('Number of processed obs: ' + primaryBand.numProcessed);
    print('WRMS                   : ' + (primaryBand.wrms*1.0e12).toFixed(2) + ' (ps)');
    print('DoF                    : ' + primaryBand.dof);
    print('Chi^2/DoF              : ' + (primaryBand.chi2/primaryBand.dof).toFixed(4));

    print('dUT1 value             : ' + (session.dUt1Value     *86400*1.0e6).toFixed(2) + ' (microsec)');
    print('dUT1 adjustment        : ' + (session.dUt1Correction*86400*1.0e6).toFixed(2) + ' (microsec)');
    print('dUT1 std.dev           : ' + (session.dUt1StdDev    *86400*1.0e6).toFixed(2) + ' (microsec)');

    for (var i=0; i<anomalies.length; i++)
        print('Anomalies              : ' + anomalies[i]);
    for (var i=0; i<reasonOfFailure.length; i++)
        print('Failure                : ' + reasonOfFailure[i]);
};



function main()
{
    const       pwd = setup.pwd;

    need2save = false;
    can2save  = true;

    // check arguments:
    if (!args.length)
    {
        print('\nscript ' + selfName + ' usage:\n');
        print(selfName + ' input [save_database]');
        print('where arguments:');
        print('   input         -- a database name (with or without version part) or a wrapper file name');
        print('   save_database -- an optional argument, if it set to "yes", the processed database will be saved as a new version');
        logger.returnCode = 1;
        return logger.returnCode;
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
//======================================================
//      setup.path2VgosDbFiles     = pwd + '/vgosDb';
//======================================================
//      setup.path2SpoolFileOutput = pwd + sfx + '/Spools';
//      setup.path2ReportOutput    = pwd + sfx + '/Reports';
//      logger.dirName = pwd + sfx + '/Logs';

/*
        setup.path2SpoolFileOutput = prefixDir + '/Spools';
        setup.path2ReportOutput    = prefixDir + '/Reports';
*/
        logger.dirName             = prefixDir + '/Logs';
//      setup.have2KeepSpoolFileReports = true;
        //
        // set up logging:
        logger.fileName = handler.guessedSessionName + '.log';
        logger.have2store = true;
        logger.rmLogFile();
    };
    //
    //
    if (prefixDir.length)
        logger.dirName = setup.path2Home + '/' + prefixDir + '-Logs';
    //
    // set up logging:
    logger.fileName = handler.guessedSessionName + '.log';
    logger.have2store = true;
    logger.rmLogFile();


    logger.isMute = true;
    logger.write(Log.Inf, Log.Preproc, selfName + ': Processing ' + handler.fileName + ' database');

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
        //
        config.activeBandIdx = session.primaryBandIdx;
        //
        if (session.bands.length == 2)
        {
            logger.write(Log.Inf, Log.Preproc, selfName + ': calculation inosphere corrections:');
            session.calcIono();
        };
        session.process();
        //
        if (3 <= session.baselines.length)
            checkSpecialCase01();
        //
        logger.write(Log.Inf, Log.Preproc, selfName + ': verifying cable calibration corrections');
        checkCableCals();
        //
        logger.write(Log.Inf, Log.Preproc, selfName + ': begn of reweighting/outlier processing');
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
            logger.returnCode = 0;
            return logger.returnCode;
        }
        else
        {
            handler.generateReport();
            printInfo();
            logger.returnCode = 1;
            return logger.returnCode;
        };
        //
        logger.write(Log.Inf, Log.Preproc, selfName + ': the session is done: ' + handler.fileName);
    }
    else
    {
        logger.write(Log.Err, Log.Preproc, selfName + ': session reading failed: ' + handler.fileName);
        logger.returnCode = 1;
        return logger.returnCode;
    };
};










main();










